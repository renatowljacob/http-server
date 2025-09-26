#include "base_arena.h"
#include "base_core.h"
#include "base_string.h"

String get_status_code(i32 sc)
{
    switch (sc)
    {
        case 200: return SEXPR("200 OK");
        case 400: return SEXPR("400 Bad Request");
        case 404: return SEXPR("404 Not Found");
        case 413: return SEXPR("413 Content Too Large");
        case 500: return SEXPR("500 Internal Server Error");
        default: return SEXPR("501 Not Implemented");
    }
}

// TODO: hash the string to get content-type
String get_content_type(u64 hash)
{
    switch (hash)
    {
        // case CSS: return "text/css";
        // case GIF: return "image/gif";
        // case HTML: return "text/html";
        // case JPEG: return "image/jpeg";
        // case JPG: return "image/jpg";
        // case JS: return "application/javascript";
        // case PNG: return "image/png";
        // case TXT: return "text/plain";
        // case WASM: return "application/wasm";
        // case WEBP: return "image/webp";
        default: return SEXPR("application/octet-stream");
    }
}

u16 handle_request(i32 fd, char *req)
{
    u16 status_code = 0;

    // Supposing the function returns a status code that means an error
    if ((status_code = parse_request_target(req)))
    {
        goto cleanup;
    }

    String file = get_file(req);
    i32 fildes = open(file, O_RDONLY);
    if (fildes == -1)
    {
        status_code = errno == ENOENT and 404 or 500;
        goto cleanup;
    }

    struct stat metadata;
    if (fstat(fildes, &metadata) == -1)
    {
        status_code = 500;
        goto cleanup;
    }

    StringSlice extension = string_strrchr(file, '.');
    if (extension == nil_string)
    {
        // do something;
    }
    extension.value = extension.value[1];

    u64 hash = hash_this_value(extension.value, extension.len);
    String content_type(get_content_type(hash));

    i64 bytes_to_write = metadata.st_size;
    while (bytes_to_write > 0)
    {
        i64 bytes_written = sendfile(fd, fildes, NULL, (size_t)metadata.st_size);
        if (bytes_written == -1)
        {
            status_code = 500;
            goto cleanup;
        }
    }

cleanup:
    if (fildes != -1) close(fildes);
    return status_code;
}

void send_response(Arena *arena, i32 fd, u16 status_code)
{
    // Define a variable for the response size
    String *response = arena_alloc(arena, 999);
    StringBuilder sb = string_builder_alloc(arena, 999);

    // Idk, maybe just pass the arguments to dprintf directly :P
    if (status_code >= 200 && status_code <= 299)
    {
        *response = string_builder_build(
            &sb,
            "%s %s\r\nContent-Type: %s\r\n\r\n",
            // protocol,
            // status code description
            // content type
        );
    }
    else
    {
        *response = string_builder_build(
            &sb,
            "%s %s\r\n\r\n",
            // protocol,
            // status code description
        );
    }

    if (dprintf(fd, "%s", response->value) == -1)
    {
        dprintf(fd, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
    }
}