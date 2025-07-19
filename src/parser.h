#ifndef PARSER_H
#define PARSER_H

#define DEFAULT_FILE "index.html"

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

/**
 * @brief Parse request-target from request message
 * @param request Pointer to request
 * @return Pointer to request-target initial character if valid, NULL otherwise
 *
 * Parse filepath from a request-target only in origin-form
 */
char *parse_request_target(char *request);

/**
 * @brief Get method from request.
 * @param request Pointer to request initial character. If a valid method is
 * found, it will be mutated to point to the last method character.
 */
methods parse_method(char *request);

#endif
