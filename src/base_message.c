#include <stdarg.h>

#include "base_core.h"
#include "base_message.h"
#include "base_string.h"

MessageNode *
message_new(Arena *arena, u32 index, i32 status_code)
{
    MessageNode *msg = arena_alloc(arena, sizeof(MessageNode));
    if (msg == NULL)
    {
        return (MessageNode *)&nil_message;
    }

    msg->index = index;
    msg->status_code = status_code;

    return msg;
}

char *
message_alloc(
  Arena *arena,
  MessageNode *msg,
  size_t buf_capacity,
  char *fmt,
  ...
)
{
    va_list ap;

    StringBuilder sb = string_builder_alloc(arena, buf_capacity);
    if (sb.buf.value == NULL)
    {
        sb.buf = nil_string;
        return sb.buf.value;
    }

    va_start(ap, fmt);
    vstring_builder_build(&sb, fmt, ap);
    va_end(ap);

    msg->message = string_builder_return_str(arena, &sb);
    string_builder_free(arena, &sb);

    return msg->message.value;
}
