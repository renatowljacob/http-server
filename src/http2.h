#include "base.h"

#define MAX_REQUEST_BYTES MB(2)
#define MAX_RESPONSE_BYTES KB(10)

typedef struct
{
    Arena *arena;
    u64 content_type;
    u16 status_code;
    u16 protocol;
} Request;

void send_response(i32 fd, Request *r);
u16 handle_request(i32 fd, String *req);
String get_content_type(u64 hash);
String get_status_code(i32 sc);