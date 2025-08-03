#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// TODO: ip(7) use other options
#define DEFAULT_S_ADDR INADDR_ANY
#define MAX_REQUESTS 64

typedef struct {
    char *content_type;
    char *protocol;
    int16_t status_code;
} State;

#endif
