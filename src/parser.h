#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>
#define DEFAULT_FILE "index.html"

#define HTTP_SCHEME "http://"
#define HTTPS_SCHEME "https://"

#define HTTP_VERSION_SCHEME "HTTP/X.X"
#define HTTP_VERSION_SCHEME_SIZE sizeof(HTTP_VERSION_SCHEME)

#define MAX_METHOD_LENGTH 7
#define METHODS_NUMBER 8

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
} methods;

// HTTP versions
typedef enum {
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0,
    HTTP_3_0,
} HTTP_versions;

/**
 * @brief Get method from request.
 * @param request Pointer to request initial character. If a valid method is
 * found, it will be mutated to point to the last method character.
 */
methods parse_method(char *request);

HTTP_versions parse_protocol(char *request);

/**
 * @brief Parse request-target from request message
 * @param request Pointer to request
 * @return Pointer to request-target initial character if valid, NULL otherwise
 *
 * Parse filepath from a request-target only in origin-form
 */
char *parse_request(char *request);

int32_t parse_request_target(char *request, char *buf, size_t *buf_size);

int32_t parse_request_target_absolute_form(char *request, char *buf, size_t *buf_size);

int32_t parse_request_target_origin_form(char *request, char *buf, size_t *buf_size);

#endif
