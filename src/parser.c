#include <string.h>

#include "base_core.h"
#include "base_string.h"
#include "parser.h"

#define IS_WS(_char) ((_char) == ' ' || (_char) == '\t')

String methods[] = {
    S("GET"),
    S("HEAD"),
    S("POST"),
    S("CONNECT"),
    S("DELETE"),
    S("PUT"),
    S("OPTIONS"),
};

String headers[] = {
    S("Host"),
    S("Content-Type"),
    S("Content-Length"),
    S("Transfer-Encoding"),
    S("Connection"),
    S("Date"),
    S("Server"),
    S("Last-Modified"),
    S("ETag"),
    S("Cache-Control"),
};

i32 parse_request(char *request)
{
    if (parse_start_line(request) == -1)
    {
        return -1;
    }

    if (parse_CRLF(request) == -1)
    {
        return -1;
    }

    // NOTE: Any form of whitespace should be detected
    if (request[0] == ' ')
    {
        return -1;
    }

    // Parse headers
    for (i32 i = 0, n = SIZEOF_ARRAY(headers); i < n; i++)
    {
    }

    if (parse_CRLF(request) == -1)
    {
        return -1;
    }

    // Parse message body if any

    return 0;
}

i32 parse_start_line(char *request)
{
    char *r = request;

    // Parse method
    for (i32 i = 0, n = SIZEOF_ARRAY(methods); i < n; i++)
    {
        if (!strncmp(r, methods[i].elements, methods[i].len))
        {
            // Store header info somewhere
            r += methods[i].len + 1;
            break;
        }

        return -1;
    }

    // Parse whitespace
    if (r[0] != ' ')
    {
        // Malformed request
        return -1;
    }
    r++;

    // Parse request target
    switch (*r)
    {
        // Absolute-form
        case 'h':
        {
            StaticString http_url = INIT_STATIC_STRING("http://");
            StaticString https_url = INIT_STATIC_STRING("https://");

            if (!strncmp(r, http_url.elements, http_url.len))
            {
                r+= http_url.len;
            }
            else if (!strncmp(r, https_url.elements, https_url.len))
            {
                r+= https_url.len;
            }
            else
            {
                return -1;
            }

            CharSlice host = {
                .buf = request,
                .start = ++r,
            };
            while (r[1] != ' ' && r[1] != '/')
            {
                r++;
            }
            host.end = r++;

            // If character is whitespace, do not fallthrough
            if (r[0] == ' ')
            {
                break;
            }
        }
        // Origin-form
        case '/':
        {
            CharSlice resource = {
                .buf = request,
                .start = r
            };
            while (r[1] != ' ')
            {
                r++;
            }
            resource.end = r;

            break;
        }
        // Asterisk-form
        case '*':
        {
            CharSlice resource = {
                .buf = request,
                .start = r,
                .end = r
            };

            break;
        }
        // Do not support authority-form
        default: return -1;
    }

    // Parse whitespace
    r++;
    if (r[0] != ' ' || r[1] == ' ')
    {
        return -1;
    }

    // Parse protocol version
    StaticString protocol_name = INIT_STATIC_STRING("HTTP/");
    StaticString protocol_version = INIT_STATIC_STRING("X.X");

    if (strncmp(r, protocol_name.elements, protocol_name.len))
    {
        return -1;
    }
    r += protocol_name.len;

    if (!strncmp(r, "1.1", protocol_version.len))
    {
        // Do something
    }
    else if (!strncmp(r, "2.0", protocol_version.len))
    {
        // Do something
    }
    else if (!strncmp(r, "3.0", protocol_version.len))
    {
        // Do something
    }
    // Treat client version as 1.0
    else
    {
        // Do something
    }
    r += protocol_version.len + 1;

    request = r;

    return 0;
}

i32 parse_header(char *request)
{


    return 0;
}

static i32 parse_CRLF(char *request)
{
    char *r = request;

    // Parse end of line
    if (r[0] == '\n')
    {
        r++;
    }
    else if (r[0] == '\r' && r[1] == '\n')
    {
        r += 2;
    }
    else
    {
        return -1;
    }

    request = r;

    return 1;
}
