#ifndef HTTP_H
#define HTTP_H

#include <stdint.h>

#include "parser.h"

#define DEFAULT_PORT 8080
#define MAX_MESSAGE_BYTES 65536

// Convert 4-length strings to a 32bit integer
#define STR_TO_INT(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

typedef enum {
    CSS = STR_TO_INT('c', 's', 's', 0),
    GIF = STR_TO_INT('g', 'i', 'f', 0),
    HTML = STR_TO_INT('h', 't', 'm', 'l'),
    JPEG = STR_TO_INT('j', 'p', 'e', 'g'),
    JPG = STR_TO_INT('j', 'p', 'g', 0),
    JS = STR_TO_INT('j', 's', 0, 0),
    PNG = STR_TO_INT('p', 'n', 'g', 0),
    TXT = STR_TO_INT('t', 'x', 't', 0),
    WASM = STR_TO_INT('w', 'a', 's', 'm'),
    WEBP = STR_TO_INT('w', 'e', 'b', 'p'),
} Formats;

/**
 * @brief Get content-type string
 * @param code 4-length array of content-type format extension characters
 * @return Content-type field value
 *
 * It is important that the array is at most 4 chars long because it will be
 * encoded as int32_t
 */
char *get_content_type(char ext[4]);

void get_method(methods method);

/**
 * @brief Get status code description
 * @param code HTTP status code
 * @return HTTP status code description
 */
char *get_status_code_desc(int32_t code);

/**
 * @brief Handle client request
 * @param sfd Socket handle
 * @param request Pointer to request
 * @return 0 on success, -1 on failure
 */
int32_t handle_request(int32_t sfd, char *request);

/**
 * @brief Send response back to the client
 * @param sfd Socket handle
 */
void send_response(int32_t sfd);

#endif
