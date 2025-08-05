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

// TODO: Come up with better error handling (everything returns NULL)

methods parse_method(char *request)
{
    bool is_equal;
    char *end, *method;

    for (int32_t i = 0; i < METHODS_NUMBER; i++)
    {
        is_equal = true;
        end = request;
        method = METHODS[i];

        // NOTE: Think about the overflows!!!
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
            return (methods) i;
        }
    }

    return -1;
}

char *parse_request_target(char *request)
{
    // Absolute URIs must start with a scheme, hence check for 'h' in that
    // scenario
    switch (*request)
    {
        case '*': return malloc(sizeof("*"));
        case '/': return parse_request_target_origin_form(request);
        case 'h': return parse_request_target_absolute_form(request);
        default: return NULL;
    }
}

char *parse_request_target_origin_form(char *request)
{
    char *buf;
    size_t buf_size = 0;

    // Get the end of file path
    while (*request != ' ')
    {
        if (*request == '\0')
        {
            return NULL;
        }
        request++;
        buf_size++;
    }

    buf = malloc(buf_size);
    if (buf == NULL)
    {
        return NULL;
    }
    memcpy(buf, request - buf_size, buf_size);
    buf[buf_size] = '\0';

    return buf;
}

// TODO: Get host from request target and ignore Host header field
char *parse_request_target_absolute_form(char *request)
{
    char *buf;
    char *host_start, *host_end;
    char *scheme;
    size_t scheme_size = 0;
    size_t buf_size = 0;

    // NOTE: Consider doing all this in one loop (though it may get convoluted)

    // Get scheme and determine start of host information in request target
    if (strncmp(request, HTTP_SCHEME, sizeof(HTTP_SCHEME) - 1))
    {
        scheme = HTTP_SCHEME;
    }
    else if (strncmp(request, HTTPS_SCHEME, sizeof(HTTPS_SCHEME) - 1))
    {
        scheme = HTTPS_SCHEME;
    }
    else
    {
        return NULL;
    }
    scheme_size = sizeof(scheme) - 1;
    request += scheme_size;
    buf_size += scheme_size;
    host_start = request;

    // Get end of host information
    while (*request != '/' && *request != ' ')
    {
        if (*request == '\0')
        {
            return NULL;
        }

        request++;
        buf_size++;
    }
    host_end = request - 1;

    // Parse rest of URI
    while (*request != ' ')
    {
        if (*request == '\0')
        {
            return NULL;
        }

        request++;
        buf_size++;
    }

    buf = malloc(buf_size);
    if (buf == NULL)
    {
        return NULL;
    }
    memcpy(buf, request - buf_size, buf_size);
    buf[buf_size] = '\0';

    return buf;
}

char *parse_request(char *request)
{
    char *buf;
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

    buf = parse_request_target(request);
    if (buf == NULL)
    {
        return NULL;
    }

    return buf;
}
