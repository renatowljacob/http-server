#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>

#define DEFAULT_FILE "index.html"

#define HTTP_URI_SCHEME "http://"
#define HTTPS_URI_SCHEME "https://"

#define HTTP_VERSION_SCHEME "HTTP/X.X"
#define HTTP_VERSION_SCHEME_SIZE sizeof(HTTP_VERSION_SCHEME)

#define METHODS_SIZE 8

#define STR_TUPLE(str) { sizeof(str) - 1, (str) }

typedef struct {
    size_t len;
    char *data;
} HTTP_str_t;

typedef struct {
    HTTP_str_t header;
    int32_t (*function)(char *, char *, size_t *);
} HTTP_header_t;

// HTTP methods
typedef enum {
    CONNECT,
    DELETE,
    GET,
    HEAD,
    OPTIONS,
    POST,
    PUT,
    TRACE
} HTTP_methods_e;

// HTTP versions
typedef enum {
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0,
    HTTP_3_0,
} HTTP_versions_e;

/**
 * @brief Parse request-target from request message
 * @param request Pointer to request
 * @return Pointer to request-target initial character if valid, NULL otherwise
 *
 * Parse filepath from a request-target only in origin-form
 */
char *parse_request(char *request);

int32_t parse_field_line(char *request, char *field_name, char* value);

/**
 * @brief Get method from request.
 * @param request Pointer to request initial character. If a valid method is
 * found, it will be mutated to point to the last method character.
 */
int32_t parse_method(char *request);

int32_t parse_protocol(char *request);

int32_t parse_request_target(char *request, char *buf, size_t *buf_size);

int32_t parse_request_target_absolute_form(char *request, char *buf, size_t *buf_size);

int32_t parse_request_target_origin_form(char *request, char *buf, size_t *buf_size);

#endif
