// TODO: Turn main into a arg parse function and transfer its contents to
// another function

#include "base_arena.h"
#include "base_core.h"
#include "base_string.h"
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_ERR_LEN 300
#define DEFAULT_PORT 8080
#define DEFAULT_S_ADDR INADDR_ANY

#define MAX_REQUESTS 32
#define MAX_MESSAGE_BYTES KB(10)

enum ERROR {
    SOCKET_CONN_ERR = 1,
    SOCKET_BIND_ERR,
    SOCKET_LISTEN_ERR,
};

struct Fildes {
    i32 fds[2];
    i8 count;
};

struct RequestState {
    u16 status_code;
};

struct ProgramState {
    i8 exit_code;
    struct Fildes fildes;
    Arena requests;
    Arena error_messages;
};

void fildes_cleanup(struct Fildes fildes)
{
    while (fildes.count > 0)
    {
        close(fildes.fds[--fildes.count]);
    }
}

int main(int argc, char *argv[])
{
    struct ProgramState state = {0};
    state.requests = arena_init(.size = MB(1));
    state.error_messages = arena_init();

    struct RequestState request_state = {0};

    StringBuilder sb = {0};
    string_builder_init(&state.error_messages, &sb, DEFAULT_ERR_LEN);

    state.fildes.fds[state.fildes.count++] = socket(AF_INET, SOCK_STREAM, 0);

    // Consider using a handler for getting the file descriptor
    if (state.fildes.fds[0] != -1)
    {
        // TODO: Turn this into a procedure
        String errmsg = string_builder_build(
            &state.error_messages,
            &sb,
            "%s %s",
            __FUNCTION__,
            __LINE__
        );
        perror(errmsg.string);
        state.exit_code = SOCKET_CONN_ERR;
        goto cleanup;
    }

    i32 opt = 1;
    setsockopt(
        state.fildes.fds[0],
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(DEFAULT_PORT),
        .sin_addr = {
            .s_addr = htonl(DEFAULT_S_ADDR)
        },
    };
    socklen_t saddr_size = sizeof(saddr);

    // Bind socket to some address
    if (bind(state.fildes.fds[0], (struct sockaddr *)&saddr, saddr_size) == -1)
    {
        String errmsg = string_builder_build(
            &state.error_messages,
            &sb,
            "%s %s",
            __FUNCTION__,
            __LINE__
        );
        perror(errmsg.string);
        state.exit_code = SOCKET_BIND_ERR;
        goto cleanup;
    }

    if (listen(state.fildes.fds[0], 0) == -1)
    {
        String errmsg = string_builder_build(
            &state.error_messages,
            &sb,
            "%s %s",
            __FUNCTION__,
            __LINE__
        );
        perror(errmsg.string);
        state.exit_code = SOCKET_LISTEN_ERR;
        goto cleanup;
    }
    printf("Listening on port %d.\n", DEFAULT_PORT);

    char *request_buf = arena_alloc(&state.requests, MAX_MESSAGE_BYTES);
    while (true)
    {
        state.fildes.fds[state.fildes.count++] = accept(
            state.fildes.fds[0],
            (struct sockaddr *)&saddr,
            &saddr_size
        );
        if (state.fildes.fds[1] == -1)
        {
            request_state.status_code = 500;

            // NOTE: I overwrite the message or I waste the first slot in
            // the arena. Should rewrite this... it uses a specific arena for
            // the messages anyway
            string_builder_reinit(&state.error_messages, &sb);
            String errmsg = string_builder_build(
                &state.error_messages,
                &sb,
                "%s %s %s",
                request_state.status_code,
                __FUNCTION__,
                __LINE__
            );
            perror(errmsg.string);

            // Send response here


            continue;
        }

        // NOTE: Multiple issues:
        //   - Wastes a whole buffer of 10KBs if not resized;
        //   - If resized, data will likely get split into two;
        //   - The previous offset will be updated, so we won't be able to
        //     join two or more buffers together;
        //   - Also MAX_MESSAGE_BYTES might be too small
        ssize_t request_size = read(
            state.fildes.fds[1],
            request_buf,
            MAX_MESSAGE_BYTES
        );
        if (request_size >= MAX_MESSAGE_BYTES)
        {
            request_state.status_code = 413;

            string_builder_reinit(&state.error_messages, &sb);
            String errmsg = string_builder_build(
                &state.error_messages,
                &sb,
                "%s %s %s",
                request_state.status_code,
                __FUNCTION__,
                __LINE__
            );
            perror(errmsg.string);
            // send_response(state.fildes.fds[1]);

            continue;
        }
        else if (request_size < 0)
        {
            request_state.status_code = 500;

            string_builder_reinit(&state.error_messages, &sb);
            String errmsg = string_builder_build(
                &state.error_messages,
                &sb,
                "%s %s %s",
                request_state.status_code,
                __FUNCTION__,
                __LINE__
            );
            perror(errmsg.string);
            // send_response(state.fildes.fds[1]);

            continue;
        }

        request_buf[request_size] = '\0';

        // Handle request here


        close(state.fildes.fds[1]);
    }

cleanup:
    fildes_cleanup(state.fildes);
    arena_free(&state.requests);
    arena_free(&state.error_messages);
}
