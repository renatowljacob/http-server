#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"

// Mirrors methods enum in parser.h
char *METHODS[] = {
    "CONNECT",
    "DELETE",
    "GET",
    "HEAD",
    "OPTIONS",
    "POST",
    "PUT",
    "TRACE"
};

// TODO: Come up with better error handling (everything returns -1)

methods parse_method(char *request)
{
    bool is_equal;
    char *end, *method;

    for (int32_t i = 0; i < METHODS_NUMBER; i++)
    {
        is_equal = true;
        end = request;
        method = METHODS[i];

        for (;
            *method != '\0' && *end != ' ';
            end++, method++
        )
        {
            if (*end != *method)
            {
                is_equal = false;
                break;
            }
        }

        if (is_equal)
        {
            request = end;
            return i;
        }
    }

    return -1;
}

HTTP_versions parse_protocol(char *request)
{
    if (!strncmp("HTTP/1.0", request, HTTP_VERSION_SCHEME_SIZE - 1))
    {
        return HTTP_1_0;
    }
    else if (!strncmp("HTTP/1.1", request, HTTP_VERSION_SCHEME_SIZE - 1))
    {
        return HTTP_1_1;
    }
    else if (!strncmp("HTTP/2.0", request, HTTP_VERSION_SCHEME_SIZE - 1))
    {
        return HTTP_2_0;
    }
    else if (!strncmp("HTTP/3.0", request, HTTP_VERSION_SCHEME_SIZE - 1))
    {
        return HTTP_3_0;
    }

    request += HTTP_VERSION_SCHEME_SIZE - 1;

    return -1;
}

char *parse_request(char *request)
{
    char *request_target_buf;
    size_t request_target_buf_size = 0;
    HTTP_versions version;
    methods method = parse_method(request);

    // We don't support CONNECT
    if (method == -1 || method == CONNECT)
    {
        return NULL;
    }

    request++;
    if (*request != ' ')
    {
        return NULL;
    }
    request++;

    if (parse_request_target(
        request,
        request_target_buf,
        &request_target_buf_size
    ) == -1)
    {
        return NULL;
    }
    request++;
    assert(request_target_buf != NULL);

    version = parse_protocol(request);
    switch (version)
    {
        case HTTP_1_0: break; // Downgrade
        default: break; // Proceed normally
    }
    request++;

    return request_target_buf;
}

int32_t parse_request_target(char *request, char *buf, size_t *buf_size)
{
    // Absolute URIs must start with a scheme, thus starting character must be
    // a 'h'
    switch (*request)
    {
        case '*':
            *buf_size = sizeof("*");
            buf = malloc(sizeof(buf_size));
            return 0;
        case '/':
            return parse_request_target_origin_form(request, buf, buf_size);
        case 'h':
            return parse_request_target_absolute_form(request, buf, buf_size);
        default:
            return -1;
    }
}

// TODO: Get host from request target and ignore Host header field
int32_t parse_request_target_absolute_form(char *request, char *buf, size_t *buf_size)
{
    char *host_start, *host_end;
    char *scheme;
    size_t _buf_size = *buf_size;
    size_t scheme_size = 0;

    // Get scheme and determine the start of host information in request target
    if (!strncmp(request, HTTP_SCHEME, sizeof(HTTP_SCHEME) - 1))
    {
        scheme = HTTP_SCHEME;
    }
    else if (!strncmp(request, HTTPS_SCHEME, sizeof(HTTPS_SCHEME) - 1))
    {
        scheme = HTTPS_SCHEME;
    }
    else
    {
        return NULL;
    }
    scheme_size = sizeof(scheme) - 1;
    request += scheme_size;
    _buf_size += scheme_size;
    host_start = request;

    // Get end of host information
    while (*request != '/' && *request != ' ')
    {
        if (*request == '\0')
        {
            return -1;
        }

        request++;
        _buf_size++;
    }
    host_end = request - 1;

    // Since what's left of the URI is only the path and queries portions, we
    // can parse the rest as origin-form
    *buf_size = _buf_size;
    if (parse_request_target_origin_form(request, buf, buf_size) == -1)
    {
        return -1;
    }

    return 0;
}

// TODO: deduplicate this code (pass a buf as a pointer)
int32_t parse_request_target_origin_form(char *request, char *buf, size_t *buf_size)
{
    size_t _buf_size = *buf_size;

    // Get the end of filepath
    while (*request != ' ')
    {
        if (*request == '\0')
        {
            return -1;
        }
        request++;
        buf_size++;
    }

    buf = malloc(_buf_size);
    if (buf == NULL)
    {
        return -1;
    }
    memcpy(buf, request - _buf_size, _buf_size);
    buf[_buf_size] = '\0';
    *buf_size = _buf_size;

    return 0;
}
