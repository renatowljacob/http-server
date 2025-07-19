#include <stdbool.h>
#include <stdint.h>
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

methods parse_method(char *request)
{
    bool is_equal;
    char *end, *method;

    for (int32_t i = 0; i < METHODS_NUMBER; i++)
    {
        is_equal = true;
        end = request;
        method = METHODS[i];

        for (
            int32_t j = 0;
            j < MAX_METHOD_LENGTH && *end != ' ';
            j++, end++, method++
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
    char *start = request;
    char *end;
    bool has_dot = false;

    // Get the start of file path
    while (*start != ' ')
    {
        if (!start)
        {
            return NULL;
        }
        start++;
    }
    end = ++start;

    // Accept only origin-form
    if (*end != '/')
    {
        return NULL;
    }

    // Get the end of file path
    while (*end != ' ')
    {
        if (end == NULL)
        {
            return NULL;
        }
        else if (*end == '.')
        {
            has_dot = true;
        }
        end++;
    }

    // If file path does not have a dot, it must be treated as a directory,
    // otherwise end the string here
    if (!has_dot)
    {
        if (*(end - 1) != '/')
        {
            *end = '/';
            end++;
        }

        const size_t file_len = strlen(DEFAULT_FILE) + 1;

        if (strlen(end) < file_len)
        {
            return NULL;
        }
        memcpy(end, DEFAULT_FILE, file_len);
    }
    else
    {
        *end = '\0';
    }

    return start;
}
