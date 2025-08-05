#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>

#include "http.h"
#include "main.h"

extern State state;

char *get_status_code_desc(int32_t code)
{
    switch (code)
    {
        case 200: return "200 OK";
        case 400: return "400 Bad Request";
        case 404: return "404 Not Found";
        case 413: return "413 Content Too Large";
        case 500: return "500 Internal Server Error";
        default: return "501 Not Implemented";
    }
}

char *get_content_type(char ext[4])
{
    switch (*(int32_t *)ext)
    {
        case CSS: return "text/css";
        case GIF: return "image/gif";
        case HTML: return "text/html";
        case JPEG: return "image/jpeg";
        case JPG: return "image/jpg";
        case JS: return "application/javascript";
        case PNG: return "image/png";
        case TXT: return "text/plain";
        case WASM: return "application/wasm";
        case WEBP: return "image/webp";
        default: return "application/octet-stream";
    }
}

int32_t handle_request(int32_t sfd, char *request)
{
    int32_t return_code = 0;

    char *file = parse_request(request);
    if (file == NULL)
    {
        fprintf(stderr, "Malformed header.\n");

        state.status_code = 400;
        send_response(sfd);

        return_code = -1;
        goto cleanup;
    }

    // Jump preceding slash
    file++;

    int32_t fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        if (errno == ENOENT)
        {
            state.status_code = 404;
        }
        else
        {
            state.status_code = 500;
        }

        perror(__func__);
        send_response(sfd);

        return_code = -1;
        goto cleanup;
    }

    struct stat metadata;
    if (fstat(fd, &metadata) == -1)
    {
        perror(__func__);

        state.status_code = 500;
        send_response(sfd);

        return_code = -1;
        goto cleanup;
    }

    char *path_ext = strrchr(file, '.');
    assert(path_ext != NULL);
    ++path_ext;

    char ext[4] = {0};
    uint64_t extlen = strlen(path_ext);

    if (extlen <= 4)
    {
        strncpy(ext, path_ext, extlen);
    }

    state.content_type = get_content_type(ext);
    send_response(sfd);

    // Retry if sendfile() returns fewer bytes than expected
    int64_t bytes_to_write = metadata.st_size;
    while (bytes_to_write > 0)
    {
        int64_t bytes_written = sendfile(sfd, fd, NULL, (size_t)metadata.st_size);
        if (bytes_written == -1)
        {
            perror(__func__);

            state.status_code = 500;
            send_response(sfd);

            return_code = -1;
            goto cleanup;
        }
        bytes_to_write -= bytes_written;
    }

cleanup:
    close(fd);

    if (file != NULL)
    {
        free(file);
    }

    return return_code;
}

void send_response(int32_t sfd)
{
    char response_buf[MAX_MESSAGE_BYTES];
    int32_t response_buflen = 0;

    // TODO: Handle special statuses
    // switch (header.status_code)
    // {
    //     case 204:
    //     case 304:
    //         header->content_type = "";
    //         break;
    // }

    if (state.status_code >= 200 && state.status_code <= 299)
    {
        response_buflen = snprintf(
            response_buf,
            MAX_MESSAGE_BYTES,
            "%s %s\r\nContent-Type: %s\r\n\r\n",
            state.protocol,
            get_status_code_desc(state.status_code),
            state.content_type
        );
    }
    else
    {
        response_buflen = snprintf(
            response_buf,
            MAX_MESSAGE_BYTES,
            "%s %s\r\n\r\n",
            state.protocol,
            get_status_code_desc(state.status_code)
        );
    }

    // TODO: Find a better way to handle this. Hardcoding the message is
    // kinda cringe
    if (write(sfd, response_buf, (size_t)response_buflen) == -1)
    {
        perror(__func__);
        write(sfd, "HTTP/1.1 500 Internal Server Error\r\n\r\n", 31);
    }
}
