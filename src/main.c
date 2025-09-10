#include <asm-generic/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/bitypes.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "base_arena.h"
#include "base_core.h"
#include "http.h"
#include "main.h"

State state = {
    .content_type = NULL,
    .protocol = "HTTP/1.1",
    .status_code = 200,
};

int main(void)
{
    // Put this into a variable :P
    void *backing_buf = malloc(10 << 10);
    Arena arena = {0};
    arena_init(&arena, backing_buf, 10 << 10);

    i32 status_code = 0;

    i32 sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        perror(__func__);
        status_code = 1;
        goto cleanup;
    }

    i32 opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(DEFAULT_PORT),
        .sin_addr = {
            .s_addr = htonl(DEFAULT_S_ADDR)
        },
    };
    socklen_t saddr_size = sizeof(saddr);

    // Bind socket to an address
    if (bind(sfd, (struct sockaddr *)&saddr, saddr_size) == -1)
    {
        perror(__func__);
        status_code = 2;
        goto cleanup;
    };

    if (listen(sfd, MAX_REQUESTS) == -1)
    {
        perror(__func__);
        status_code = 3;
        goto cleanup;
    }
    printf("Listening on port %d.\n", DEFAULT_PORT);

    char request_buf[MAX_MESSAGE_BYTES];
    while (true)
    {
        // Block until a connection is made
        i32 request_sfd = accept(
            sfd,
            (struct sockaddr *)&saddr,
            &saddr_size
        );
        if (request_sfd == -1)
        {
            perror(__func__);
            state.status_code = 500;
            send_response(request_sfd);
            continue;
        }

        ssize_t request_size = read(
            request_sfd,
            request_buf,
            MAX_MESSAGE_BYTES
        );
        if (request_size >= MAX_MESSAGE_BYTES)
        {
            perror(__func__);
            state.status_code = 413;
            send_response(request_sfd);
            continue;
        }

        request_buf[request_size] = '\0';

        if (handle_request(request_sfd, request_buf) == -1)
        {
            fprintf(stderr, "Failed to handle request.\n");
        }

        close(request_sfd);
    }

cleanup:
    (void) close(sfd);
    arena_free(&arena);

    exit(status_code);
}
