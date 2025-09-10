#include "base_core.h"

i32 parse_request(char *request);

i32 parse_start_line(char *request);

i32 parse_headers(char *request);

static i32 parse_CRLF(char *request);
