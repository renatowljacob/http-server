#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"

// Mirrors methods enum in parser.h
HTTP_str_t methods[] = {
    STR_TUPLE("CONNECT"),
    STR_TUPLE("DELETE"),
    STR_TUPLE("GET"),
    STR_TUPLE("HEAD"),
    STR_TUPLE("OPTIONS"),
    STR_TUPLE("POST"),
    STR_TUPLE("PUT"),
    STR_TUPLE("TRACE"),
};

HTTP_header_t headers[] = {
    { STR_TUPLE("Cache-Control"), },
    { STR_TUPLE("Connection"), },
    { STR_TUPLE("Content-Length"), },
    { STR_TUPLE("Content-Type"), },
    { STR_TUPLE("Date"), },
    { STR_TUPLE("ETag"), },
    { STR_TUPLE("Host"), },
    { STR_TUPLE("Last-Modified"), },
    { STR_TUPLE("Server"), },
    { STR_TUPLE("Transfer-Encoding"), },
};

// TODO: Come up with better error handling (everything returns -1)

char *parse_request(char *request)
{
    char *request_target_buf;
    size_t request_target_buf_size = 0;
    HTTP_versions_e version;
    HTTP_methods_e method;

    // We don't support CONNECT
    method = parse_method(request);
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

    if (*request != '\n' || *request != '\r' && *(request + 1) != '\n')
    {
        return NULL;
    }

    char field_name[50], value[50];
    while (true)
    {
        memset(field_name, 0, sizeof(field_name) / sizeof(char));
        memset(value, 0, sizeof(value) / sizeof(char));

        if (parse_field_line(request, field_name, value) == -1)
        {
            return NULL;
        }

        for (int32_t i = 0; i < sizeof(headers) / sizeof(HTTP_header_t); i++)
        {
            if (!strncmp(headers[i].header.data, field_name, headers[i].header.len))
            {
                // Do something
                // headers[i].function();
            }
        }
    }

    return request_target_buf;
}

int32_t parse_field_line(char *request, char *field_name, char* value)
{
    for (
        int32_t i = 0;
        *request != '\0' && *request != '\r' && *request != '\n';
        i++, request++
    )
    {
        if (*request == ' ')
        {
            if (*(request + 1) == ':')
            {
                return -1;
            }
            else if (*(request - 1) == ':')
            {
                break;
            }
        }

        field_name[i] = *request;
    }

    for (
        int32_t i = 0;
        *request != '\0' && *request != '\r' && *request != '\n';
        i++, request++
    )
    {
        if (*request == ' '
            && (*(request + 1) == '\n' || *(request + 1) == '\r'))
        {
            continue;
        }

        value[i] = *request;
    }

    request += *request == '\r' ? 2 : 1;

    return 0;
}

int32_t parse_method(char *request)
{
    char *method;
    int32_t methodlen;

    for (int32_t i = 0; i < METHODS_SIZE; i++)
    {
        method = methods[i].data;
        methodlen = methods[i].len;

        if (!strncmp(method, request, methodlen))
        {
            request += methodlen;
            return i;
        }
    }

    return -1;
}

// NOTE: We should account in the future for cases where the protocol version
// the recipient supports is determined through configuration
int32_t parse_protocol(char *request)
{
    HTTP_versions_e version = HTTP_1_0;

    if (!strncmp("HTTP/1.1", request, HTTP_VERSION_SCHEME_SIZE - 1))
    {
        version = HTTP_1_1;
    }
    else if (!strncmp("HTTP/2.0", request, HTTP_VERSION_SCHEME_SIZE - 1))
    {
        version = HTTP_2_0;
    }
    else if (!strncmp("HTTP/3.0", request, HTTP_VERSION_SCHEME_SIZE - 1))
    {
        version = HTTP_3_0;
    }

    request += HTTP_VERSION_SCHEME_SIZE - 1;

    return version;
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
    if (!strncmp(request, HTTP_URI_SCHEME, sizeof(HTTP_URI_SCHEME) - 1))
    {
        scheme = HTTP_URI_SCHEME;
    }
    else if (!strncmp(request, HTTPS_URI_SCHEME, sizeof(HTTPS_URI_SCHEME) - 1))
    {
        scheme = HTTPS_URI_SCHEME;
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
        _buf_size++;
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
