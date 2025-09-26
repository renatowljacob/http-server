#include <string.h>

#include "base_core.h"
#include "base_string.h"
#include "parser.h"

#define IS_WS(_char) ((_char) == ' ' || (_char) == '\t')

String methods[] = {
    SEXPR("GET"),
    SEXPR("HEAD"),
    SEXPR("POST"),
    SEXPR("CONNECT"),
    SEXPR("DELETE"),
    SEXPR("PUT"),
    SEXPR("OPTIONS"),
};

String headers[] = {
    SEXPR("Host"),
    SEXPR("Content-Type"),
    SEXPR("Content-Length"),
    SEXPR("Transfer-Encoding"),
    SEXPR("Connection"),
    SEXPR("Date"),
    SEXPR("Server"),
    SEXPR("Last-Modified"),
    SEXPR("ETag"),
    SEXPR("Cache-Control"),
};

// TODO: Make these procedures return actually useful values

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
            String http_url = SEXPR("http://");
            String https_url = SEXPR("https://");

            if (!strncmp(r, http_url.value, http_url.len))
            {
                r+= http_url.len;
            }
            else if (!strncmp(r, https_url.value, https_url.len))
            {
                r+= https_url.len;
            }
            else
            {
                return -1;
            }

            StringSlice host = {
                .value = r
            };
            while (r[0] != ' ' && r[0] != '/')
            {
                r = r[1];
                host.len++;
            }

            // If character is whitespace, do not fallthrough
            if (r[0] == ' ')
            {
                break;
            }
        }
        // Origin-form
        case '/':
        {
            StringSlice resource = {
                .value = r,
            };
            while (r[0] != ' ')
            {
                r = r[1];
                resource.len++;
            }

            break;
        }
        // Asterisk-form
        case '*':
        {
            StringSlice resource = {
                .value = r,
                .len = 1
            };

            break;
        }
        // We don't support authority-form
        default: return -1;
    }

    // Parse whitespace
    r = r[1];
    if (r[0] != ' ' || r[1] == ' ')
    {
        return -1;
    }

    // Parse protocol version
    String protocol_name = SEXPR("HTTP/");
    String protocol_version = SEXPR("X.X");

    if (strncmp(r, protocol_name.value, protocol_name.len))
    {
        return -1;
    }
    r = r[protocol_name.len];

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
    r = r[protocol_version.len + 1];
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
        r = r[1];
    }
    else if (r[0] == '\r' && r[1] == '\n')
    {
        r = r[2];
    }
    else
    {
        return -1;
    }

    request = r;

    return 1;
}
