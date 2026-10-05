#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define RWLJ_IMPLEMENTATION
#include "rwlj.h"

#define DEFAULT_PORT      8080
#define DEFAULT_ADDRESS   INADDR_ANY
#define MAX_REQUESTS      64
#define MAX_REQUEST_SIZE  rwlj_kb(64)
#define MAX_RESPONSE_SIZE MAX_REQUEST_SIZE
#define ARENA_SIZE        rwlj_mb(10)

#define HTTP_SCHEME  "http://"
#define HTTPS_SCHEME "https://"

// TODO: Turn this into a map
// clang-format off
#define X(callback) \
    callback(OCTET_STREAM, "",      "application/octet-stream"), \
    callback(HTML,         "html",  "text/html"), \
    callback(CSS,          "css",   "text/css"), \
    callback(JS,           "js",    "application/javascript"), \
    callback(TXT,          "txt",   "text/plain"), \
    callback(JPG,          "jpg",   "image/jpg"), \
    callback(JPEG,         "jpeg",  "image/jpeg"), \
    callback(PNG,          "png",   "image/png"), \
    callback(SVG,          "svg",   "image/svg+xml"), \
    callback(ICO,          "ico",   "image/vnd.microsoft.icon"), \
    callback(GIF,          "gif",   "image/gif"), \
    callback(WASM,         "wasm",  "application/wasm")
// clang-format on

#define MAKE_ENUM(param1, param2, param3)    rwlj_concat_(CONTENT_TYPE, param1)
#define MAKE_STRINGS(param1, param2, param3) { STRING(param2), STRING(param3) }
#define ENUM_TABLE                           X(MAKE_ENUM)
#define STRING_TABLE                         X(MAKE_STRINGS)

enum Content_Type {
    ENUM_TABLE,
    CONTENT_TYPE_COUNT,
};
typedef isize Content_Type;

typedef struct Content_Type_KV {
    rwljString key;
    rwljString value;
} Content_Type_KV;

Content_Type_KV content_types[CONTENT_TYPE_COUNT] = {
    STRING_TABLE,
};

#undef X
#undef MAKE_ENUM
#undef MAKE_STRINGS
#undef ENUM_TABLE
#undef STRING_TABLE

// clang-format off
#define X(callback) \
    callback(GET), \
    callback(POST), \
    callback(PUT), \
    callback(DELETE), \
    callback(HEAD), \
    callback(OPTIONS), \
    callback(TRACE), \
    callback(CONNECT)
// clang-format on

#define MAKE_ENUM(param)    rwlj_concat_(METHOD, param)
#define MAKE_STRINGS(param) STRING(#param)
#define ENUM_TABLE          X(MAKE_ENUM)
#define STRING_TABLE        X(MAKE_STRINGS)

enum Method {
    METHOD_NIL = -1,
    ENUM_TABLE,
    METHOD_COUNT,
};
typedef isize Method;
rwljString methods[METHOD_COUNT] = {
    STRING_TABLE,
};

#undef X
#undef MAKE_ENUM
#undef MAKE_STRINGS
#undef ENUM_TABLE
#undef STRING_TABLE

// clang-format off
#define X(callback) \
    callback(HTTP1,    "HTTP/1.0"), \
    callback(HTTP1_1,  "HTTP/1.1"), \
    callback(HTTP2,    "HTTP/2.0"), \
    callback(HTTP3,    "HTTP/3.0")
// clang-format on

#define MAKE_ENUM(param1, param2)    rwlj_concat_(PROTOCOL, param1)
#define MAKE_STRINGS(param1, param2) STRING(param2)
#define ENUM_TABLE                   X(MAKE_ENUM)
#define STRING_TABLE                 X(MAKE_STRINGS)

enum Protocol {
    PROTOCOL_NIL = -1,
    ENUM_TABLE,
    PROTOCOL_COUNT,
};
typedef isize Protocol;
rwljString protocols[PROTOCOL_COUNT] = {
    STRING_TABLE,
};

#undef X
#undef MAKE_ENUM
#undef MAKE_STRINGS
#undef ENUM_TABLE
#undef STRING_TABLE

enum URI_Kind {
    URI_KIND_AUTHORITY_FORM,
    URI_KIND_ABSOLUTE_FORM,
    URI_KIND_ORIGIN_FORM,
    URI_KIND_ASTERISK_FORM,
    URI_KIND_COUNT
};
typedef isize URI_Kind;

typedef struct URI {
    URI_Kind kind;
    rwljString scheme;
    rwljString authority;
    rwljString path;
    rwljString query;
    rwljString fragment;
} URI;

typedef struct File_Data {
    char *os_path; // OS API interop
    rwljString stem;
    rwljString extension;
} File_Data;

// clang-format off
#define X(callback)                                                            \
    callback(200, OK),                                                         \
    callback(400, BAD_REQUEST),                                                \
    callback(404, RESOURCE_NOT_FOUND),                                         \
    callback(414, URI_TOO_LONG),                                               \
    callback(500, INTERNAL_SERVER_ERROR),                                      \
    callback(501, NOT_IMPLEMENTED),                                            \
    callback(505, HTTP_VERSION_NOT_SUPPORTED)
// clang-format on

#define MAKE_ENUM(param1, param2) rwlj_concat_(STATUS_CODE, param2) = param1
#define ENUM_TABLE                X(MAKE_ENUM)

enum Status_Code {
    ENUM_TABLE,
};
typedef isize Status_Code;

#undef X
#undef MAKE_ENUM
#undef ENUM_TABLE

typedef struct Exchange_Data {
    // Request
    Method method;
    isize port;
    // rwljString message;
    rwljString target;
    URI uri;

    // Response
    Content_Type content_type;
    isize status_code;
    rwljString response;
    rwljString status_message;
    File_Data file;

    // Exchange
    Protocol protocol;

    // Context - stuff to pass around
    rwljString_Builder string_builder;
} Exchange_Data;

typedef struct Header_KV {
    rwljString key;
    rwljString value;
} Header_KV;
GENERIC_ARRAY(Header_KV, Header_KV);

enum Newline {
    NEWLINE_INVALID = -1,
    NEWLINE_NIL,
    NEWLINE_VALID,
};
typedef isize Newline;

// Exit codes
enum Server_Error {
    SERVER_ERROR_CREATE_SERVER_SOCKET = 1,
    SERVER_ERROR_CONFIGURE_SERVER_SOCKET,
    SERVER_ERROR_BIND_SERVER_SOCKET,
    SERVER_ERROR_LISTEN_SERVER_SOCKET,
    SERVER_ERROR_CLOSE_SERVER_SOCKET,
};
typedef i32 Server_Error;

Newline is_newline(rwljString s);
Newline skip_newline(rwljString *r);
void advance_message(rwljString *r, isize offset);
rwljString get_status_message(Status_Code code);
void send_response_line(Exchange_Data *e, i32 connection_socket);
isize send_response(rwljString response, i32 connection_socket);

i32
main(void)
{
    rwljArena connection_arena = { 0 };
    rwlj_arena_init_static(&connection_arena, ARENA_SIZE);

    i32 server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        i32 err = errno;
        rwlj_debug_printf("Failed to create server socket: %s", strerror(err));

        exit(SERVER_ERROR_CREATE_SERVER_SOCKET);
    }
    rwlj_debug_print(STRING("Opened socket"));

    i32 opt = 1;
    if (setsockopt(
            server_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &opt,
            cast(socklen_t) rwlj_size_of(opt)
        ) == -1) {
        i32 err = errno;
        rwlj_debug_printf(
            "Failed to configure server socket: %s", strerror(err)
        );

        exit(SERVER_ERROR_CONFIGURE_SERVER_SOCKET);
    }
    rwlj_debug_print(STRING("Set socket"));

    struct sockaddr_in sockaddr = {
        .sin_family = AF_INET,
        .sin_port = htons(DEFAULT_PORT),
        .sin_addr = { .s_addr = htonl(DEFAULT_ADDRESS) },
    };
    socklen_t socklen = rwlj_size_of(sockaddr);
    if (bind(server_fd, cast(struct sockaddr *) & sockaddr, socklen) == -1) {
        i32 err = errno;
        rwlj_debug_printf(
            "Failed to bind server socket to address: %s", strerror(err)
        );

        exit(SERVER_ERROR_BIND_SERVER_SOCKET);
    }
    rwlj_debug_print(STRING("Bound socket"));

    if (listen(server_fd, MAX_REQUESTS) == -1) {
        i32 err = errno;
        rwlj_debug_printf(
            "Failed to set server socket up for listening: %s", strerror(err)
        );

        exit(SERVER_ERROR_LISTEN_SERVER_SOCKET);
    }
    rwlj_debug_print(STRING("Listened to socket"));
    rwlj_eprintfln("Listening on port %d.", DEFAULT_PORT);

    rwljArena_Temp temp_connection_arena = { 0 };
    rwlj_arena_temp_init(&temp_connection_arena, &connection_arena);
    i32 connection_fd = -1;
    i32 file_fd = -1;
    while (true) {
        rwlj_arena_temp_free_all(&temp_connection_arena);

        if (file_fd != -1 && close(file_fd) == -1) {
            i32 err = errno;
            rwlj_debug_printf("Failed to close file: %s", strerror(err));
        }

        if (connection_fd != -1) {
            if (close(connection_fd) == -1) {
                i32 err = errno;
                rwlj_debug_printf("Failed to close socket: %s", strerror(err));
            }
            rwlj_debug_print(STRING("Closed connection"));
        }

        if ((connection_fd = accept(
                 server_fd, cast(struct sockaddr *) & sockaddr, &socklen
             )) == -1) {
            i32 err = errno;
            rwlj_debug_printf(
                "Failed to create connection socket: %s", strerror(err)
            );

            break;
        }
        rwlj_debug_print(STRING("Accepted connection"));

        rwljSlice_U8 message_buf = {
            rwlj_arena_alloc(temp_connection_arena.arena, MAX_REQUEST_SIZE),
            MAX_REQUEST_SIZE
        };
        isize message_size = read(
            connection_fd, cast(void *) message_buf.data, MAX_REQUEST_SIZE
        );
        if (message_size == -1) {
            // TODO: Also send diagnostics to log file
            i32 err = errno;
            rwlj_debug_printf(
                "Failed to read connection socket: %s", strerror(err)
            );
            continue;
        }
        rwljString message = rwlj_string(message_buf.data, 0, message_size);

        // Ignore empty lines preceding request-line
        while (skip_newline(&message)) {
            rwlj_no_op();
        }

        // Init exchange
        Exchange_Data exchange = { 0 };
        exchange.file.os_path = "index.html";
        exchange.file.stem = STRING("index");
        exchange.file.extension = STRING("html");
        exchange.content_type = CONTENT_TYPE_HTML;
        exchange.method = METHOD_NIL;
        exchange.protocol = PROTOCOL_HTTP1;
        rwlj_string_builder_init(
            &exchange.string_builder, MAX_RESPONSE_SIZE, &connection_arena
        );

        // PARSE REQUEST-LINE

        // Parse method
        for (isize i = 0; i < METHOD_COUNT; i += 1) {
            rwljString method = methods[i];
            if (rwlj_string_are_equal(
                    method, rwlj_string(message.data, 0, method.len)
                )) {
                advance_message(&message, method.len);
                exchange.method = i;

                break;
            }
        }
        if (exchange.method == METHOD_NIL) {
            exchange.status_code = STATUS_CODE_NOT_IMPLEMENTED;
            send_response_line(&exchange, connection_fd);

            continue;
        }

        if (message.data[0] != ' ') {
            exchange.status_code = STATUS_CODE_BAD_REQUEST;
            send_response_line(&exchange, connection_fd);

            continue;
        }
        advance_message(&message, 1);

        // TODO: Reconstruct target URI from Host

        // TODO: This might be a naive form of parsing. The target might be
        // invalid
        // Parse request-target
        {
            isize len = 0;

            if (message.data[0] == '*') {
                if (exchange.method != METHOD_OPTIONS) {
                    exchange.status_code = STATUS_CODE_RESOURCE_NOT_FOUND;
                    send_response_line(&exchange, connection_fd);

                    continue;
                }

                exchange.uri.kind = URI_KIND_ASTERISK_FORM;
                len += 1;
                exchange.uri.path = rwlj_string(message.data, 0, len);

                goto target_parsing_end;
            }

            if (message.data[0] == '/') {
                exchange.uri.kind = URI_KIND_ORIGIN_FORM;
            } else {
                // Parse URI scheme

                bool is_http = false;
                bool is_https = false;

                rwljString http_scheme = STRING(HTTP_SCHEME);
                rwljString https_scheme = STRING(HTTPS_SCHEME);

                // TODO: Track protocol version throughout connection (and guess
                // through heuristics)

                // TODO: If Host is also absent, assume it's a HTTP/1.0 request
                if ((is_http = rwlj_string_are_equal(
                         rwlj_string(message.data, 0, http_scheme.len),
                         http_scheme
                     )) ||
                    (is_https = rwlj_string_are_equal(
                         rwlj_string(message.data, 0, https_scheme.len),
                         https_scheme
                     ))) {
                    exchange.uri.kind = URI_KIND_ABSOLUTE_FORM;
                    exchange.uri.scheme = is_http ? http_scheme : https_scheme;
                    len += exchange.uri.scheme.len;
                }
            }

            if (exchange.uri.kind == URI_KIND_AUTHORITY_FORM &&
                exchange.method != METHOD_CONNECT) {
                exchange.status_code = STATUS_CODE_BAD_REQUEST;
                send_response_line(&exchange, connection_fd);

                continue;
            }

            // Parse URI authority
            isize start = len;
            bool seen_colon = false;
            while (!rwlj_is_space(message.data[len]) &&
                   message.data[len] != '/' && len < message.len) {
                if (exchange.uri.kind == URI_KIND_AUTHORITY_FORM) {
                    if (message.data[len] == ':') {
                        seen_colon = true;
                    }
                    if (seen_colon && rwlj_is_digit(message.data[len])) {
                        exchange.port *= 10;
                        exchange.port += message.data[len] - '0';
                    }
                }
                len += 1;
            }
            exchange.uri.authority.len = len - start;
            exchange.uri.authority.data =
                exchange.uri.authority.len > 0 ? (&message.data[start]) : NULL;

            // Parse URI path
            start = len;
            while (!rwlj_is_space(message.data[len]) &&
                   message.data[len] != '?' && len < message.len) {
                // Skip encoded quotations
                if (message.data[len] == '%' &&
                    (message.data[len + 1] == '3' &&
                     message.data[len + 2] == 'F' &&
                     message.data[len + 3] == '?')) {
                    len += 3;
                }
                len += 1;
            }
            exchange.uri.path.len = len - start;
            exchange.uri.path.data =
                exchange.uri.path.len > 0 ? (&message.data[start]) : NULL;

            // Parse URI query
            start = len;
            while (!rwlj_is_space(message.data[len]) &&
                   message.data[len] != '#' && len < message.len) {
                len += 1;
            }
            exchange.uri.query.len = len - start;
            exchange.uri.query.data =
                exchange.uri.query.len > 0 ? (&message.data[start]) : NULL;

            // Parse URI fragment
            start = len;
            while (!rwlj_is_space(message.data[len]) && len < message.len) {
                len += 1;
            }
            exchange.uri.fragment.len = len - start;
            exchange.uri.fragment.data =
                exchange.uri.fragment.len > 0 ? (&message.data[start]) : NULL;

            // TODO: Respond with "414 URI Too Long"?

            exchange.target = rwlj_string(message.data, 0, len);
            advance_message(&message, len);

        target_parsing_end:
            rwlj_no_op();
        }

        // Parse path for extension, skip if points to index.html
        if (!rwlj_string_are_equal(exchange.uri.path, STRING("/")) &&
            (exchange.uri.kind == URI_KIND_ORIGIN_FORM ||
             exchange.uri.kind == URI_KIND_ABSOLUTE_FORM)) {
            rwljArena_Temp temp_arena = { 0 };
            rwlj_arena_temp_init(&temp_arena, &connection_arena);

            // Skip leading directory slash
            rwljString filepath =
                rwlj_string(exchange.uri.path.data, 1, exchange.uri.path.len);
            rwljSlice_String split =
                rwlj_string_split(filepath, STRING("/"), temp_arena.arena);

            rwljString basename =
                rwlj_slice_get_struct(rwljString, &split, split.len - 1);
            split = rwlj_string_split(basename, STRING("."), temp_arena.arena);

            // Extension
            exchange.file.extension =
                rwlj_slice_get_struct(rwljString, &split, split.len - 1);

            // Stem
            exchange.file.stem = rwlj_string(
                basename.data,
                0,
                basename.len - (exchange.file.extension.len + 1)
            );

            rwlj_arena_temp_free_all(&temp_arena);

            exchange.file.os_path =
                rwlj_cstring_from_string(filepath, &connection_arena);
        }

        if (message.data[0] != ' ') {
            exchange.status_code = STATUS_CODE_BAD_REQUEST;
            send_response_line(&exchange, connection_fd);

            continue;
        }
        advance_message(&message, 1);

        // Parse protocol
        for (isize i = 0; i < PROTOCOL_COUNT; i += 1) {
            rwljString protocol = protocols[i];
            if (rwlj_string_are_equal(
                    protocol, rwlj_string(message.data, 0, protocol.len)
                )) {
                advance_message(&message, protocol.len);
                exchange.protocol = i;

                break;
            }
        }
        if (exchange.protocol == PROTOCOL_NIL) {
            exchange.status_code = STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED;
            send_response_line(&exchange, connection_fd);

            continue;
        }

        // Skip newlines
        isize newlines = 0;
        Newline newline_kind = NEWLINE_NIL;
        for (isize i = 0; i < 2; i += 1) {
            newline_kind = skip_newline(&message);
            if (newline_kind == NEWLINE_NIL ||
                newline_kind == NEWLINE_INVALID) {
                break;
            }
            newlines += 1;
        }
        if (newline_kind == NEWLINE_INVALID || newlines == 0 || newlines == 2) {
            exchange.status_code = STATUS_CODE_BAD_REQUEST;
            send_response_line(&exchange, connection_fd);

            continue;
        }

        // Parse headers
        // TODO: Handle line folding (RCF 9112 - 5.2)
        rwljArray_Header_KV headers = { 0 };
        rwlj_array_init_dynamic(&headers, &connection_arena);

        newlines = 0;
        do {
            rwljString key = rwlj_string(message.data, 0, 0);
            while (message.data[key.len] != ':' && key.len < message.len) {
                if (rwlj_is_space(message.data[key.len])) {
                    exchange.status_code = STATUS_CODE_BAD_REQUEST;
                    send_response_line(&exchange, connection_fd);

                    goto connection_loop_continue;
                }
                key.len += 1;
            }
            advance_message(&message, key.len + 1);

            if (rwlj_is_space(message.data[0])) {
                advance_message(&message, 1);
            }

            rwljString value = rwlj_string(message.data, 0, 0);
            while (!is_newline(
                       rwlj_string(message.data, value.len, message.len)
                   ) &&
                   value.len < message.len) {
                value.len += 1;
            }
            advance_message(&message, value.len);

            rwlj_array_append(&headers, ((Header_KV){ key, value }));

            // End the loop if two or no newlines were found
            newlines = skip_newline(&message);
            newlines += skip_newline(&message);
        } while (newlines == 1);
        if (newlines == 0) {
            exchange.status_code = STATUS_CODE_BAD_REQUEST;
            send_response_line(&exchange, connection_fd);

            continue;
        }

        // TODO: Handle headers
#if false
        isize len = fields.len;
        while (len--) {
            rwljString key =
                rwlj_array_get_struct(Field_Line_KV, &fields, len).key;
            if (rwlj_string_are_equal(key, STRING("Host"))) {
                // Reconstruct origin-form URI

                if (request_data.uri.kind == URI_KIND_ABSOLUTE_FORM) {
                    break;
                }

                if (request_data.uri.kind == URI_KIND_AUTHORITY_FORM &&
                    request_data.uri.authority.len == 0) {
                }
            }

            if (rwlj_string_are_equal(key, STRING("Content-Type"))) {
                rwljString value =
                    rwlj_array_get_struct(Field_Line_KV, &fields, len).key;

                for (isize i = 0; i < rwlj_count_of(content_types); i += 1) {
                    rwljString content_type = content_types[i];
                    if (rwlj_string_are_equal(value, content_type)) {
                        request_data.content_type = i;
                        break;
                    }
                }
            }

            if (rwlj_string_are_equal(key, STRING("Content-Type"))) {
                continue;
            }
        }
#endif

        // TODO: Handle content
        {
        }

        switch (exchange.uri.kind) {
        case URI_KIND_ORIGIN_FORM:
        case URI_KIND_ABSOLUTE_FORM: {
            file_fd = open(exchange.file.os_path, O_RDONLY);
            if (file_fd == -1) {
                i32 err = errno;
                exchange.status_code = err == ENOENT
                                           ? STATUS_CODE_RESOURCE_NOT_FOUND
                                           : STATUS_CODE_INTERNAL_SERVER_ERROR;
                send_response_line(&exchange, connection_fd);
                rwlj_debug_printf("Failed to open file: %s", strerror(err));

                continue;
            }

            struct stat metadata = { 0 };
            if (fstat(file_fd, &metadata) == -1) {
                i32 err = errno;
                exchange.status_code = STATUS_CODE_INTERNAL_SERVER_ERROR;
                send_response_line(&exchange, connection_fd);
                rwlj_debug_printf(
                    "Failed to get file metadata: %s", strerror(err)
                );

                continue;
            }

            // Get content type (octet-stream by default)
            for (isize i = 0; i < rwlj_count_of(content_types); i += 1) {
                Content_Type_KV content_type = content_types[i];
                if (!rwlj_string_compare_insensitive(
                        exchange.file.extension, content_type.key
                    )) {
                    exchange.content_type = i;

                    break;
                }
            }

            rwlj_sbprintf(
                &exchange.string_builder,
                "%S %S\r\n",
                protocols[exchange.protocol],
                get_status_message(exchange.status_code)
            );
            rwlj_sbprintf(
                &exchange.string_builder,
                "Content-Type: %S\r\n\r\n",
                content_types[exchange.content_type].value
            );
            if (send_response(
                    rwlj_string_builder_to_string(&exchange.string_builder),
                    connection_fd
                ) == -1) {
                exchange.status_code = STATUS_CODE_INTERNAL_SERVER_ERROR;
                send_response_line(&exchange, connection_fd);

                continue;
            }
            rwlj_debug_print(get_status_message(exchange.status_code));

            // Send file
            isize bytes_to_write = metadata.st_size;
            while (bytes_to_write > 0) {
                isize bytes_written = sendfile(
                    connection_fd, file_fd, NULL, cast(usize) bytes_to_write
                );
                if (bytes_written == -1) {
                    i32 err = errno;
                    exchange.status_code = STATUS_CODE_INTERNAL_SERVER_ERROR;
                    send_response_line(&exchange, connection_fd);
                    rwlj_debug_printf(
                        "Failed to transfer file to connection socket: %s",
                        strerror(err)
                    );

                    break;
                }
                bytes_to_write -= bytes_written;
            }

            break;
        }
        case URI_KIND_ASTERISK_FORM: {
        }
        case URI_KIND_AUTHORITY_FORM: {
        }
        }

        // If it's not possible to use "continue" due to it being in a inner
        // loop
    connection_loop_continue:
        rwlj_no_op();
    }

    if (close(server_fd) == -1) {
        i32 err = errno;
        rwlj_debug_printf("Failed to close server socket: %s", strerror(err));

        exit(SERVER_ERROR_CLOSE_SERVER_SOCKET);
    }
    rwlj_debug_print(STRING("Closed socket"));

    return 0;
}

// Returns 2 for CRLF, 1 for LF, and 0 for no newline
Newline
is_newline(rwljString s)
{
    if (s.len >= 2 && s.data[0] == '\r' && s.data[1] == '\n') {
        return NEWLINE_VALID;
    }
    if (s.len >= 1 && (s.data[0] == '\n' || s.data[0] == '\r')) {
        return NEWLINE_INVALID;
    }

    return NEWLINE_NIL;
}

// Skips a newline, unless it's a bare CR, if possible
Newline
skip_newline(rwljString *s)
{
    Newline skipped = is_newline(*s);
    if (skipped == NEWLINE_VALID) {
        advance_message(s, 2);
    }

    return skipped;
}

// Advance request by offset with bounds-check
void
advance_message(rwljString *m, isize offset)
{
    m->data = rwlj_min(m->data + offset, m->data + m->len);
    m->len = rwlj_max(m->len - offset, 0);
}

rwljString
get_status_message(Status_Code code)
{
    switch (cast(enum Status_Code) code) {
    case 200:
        return STRING("200 OK");
    case 400:
        return STRING("400 Bad Request");
    case 404:
        return STRING("404 Resource Not Found");
    case 414:
        return STRING("414 URI Too Long");
    case 500:
        return STRING("500 Internal Server Error");
    case 501:
        return STRING("501 Not Implemented");
    case 505:
        return STRING("501 HTTP Version Not Supported");
    }

    return STRING("");
}

void
send_response_line(Exchange_Data *e, i32 connection_socket)
{
    rwlj_sbprintf(
        &e->string_builder,
        "%S %S\r\n",
        protocols[e->protocol],
        get_status_message(e->status_code)
    );
    send_response(
        rwlj_string_builder_to_string(&e->string_builder), connection_socket
    );
    rwlj_debug_print(get_status_message(e->status_code));
}

isize
send_response(rwljString response, i32 connection_socket)
{
    if (write(connection_socket, response.data, cast(usize) response.len) ==
        -1) {
        i32 err = errno;
        rwlj_debug_printf("Failed to write to socket: %s", strerror(err));

        rwljString s = STRING("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        write(connection_socket, s.data, cast(usize) s.len);

        return -1;
    }

    return 0;
}
