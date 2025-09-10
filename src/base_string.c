#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "base_string.h"

const String nil_string = {0, ""};

// Copying
String cstring_copy(Arena *arena, const char *s)
{
    size_t len = strlen(s) + 1;
    char *cstr = arena_alloc(arena, len);
    if (cstr == NULL)
    {
        return nil_string;
    }

    memcpy(cstr, s, len);

    return (String) { len, cstr };
}

String string_copy(Arena *arena, const String *s)
{
    char *str = arena_alloc(arena, s->len);
    if (str == NULL)
    {
        return nil_string;
    }

    memcpy(str, s->string, s->len);

    return (String) { s->len, str };
}

// Slice
StringSlice _string_slice(const String *s, struct String_Slice_Params params)
{
    if ((size_t) labs(params.from) > s->len
        || (size_t) labs(params.to) > s->len)
    {
        return nil_string;
    }

    size_t _from = (size_t)(
        params.from >= 0 and params.from or params.from + (i64) s->len
    );
    size_t _to = (size_t)(
        params.to >= 0 and params.to or params.to + (i64) s->len
    );
    if (_from > _to)
    {
        return nil_string;
    }

    size_t len = _to - _from;

    return (String) {
        .string = &s->string[_from],
        .len = len
    };
}

// String Builder
void string_builder_init(Arena *arena, StringBuilder *sb, size_t capacity)
{
    sb->capacity = capacity;
    sb->buf.string = arena_alloc(arena, sb->capacity);
    // Error handling here
}

String string_builder_build(Arena *arena, StringBuilder *sb, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    i32 len = vsnprintf(NULL, 0, fmt, ap);
    if (len < 0)
    {
        return nil_string;
    }
    va_end(ap);

    va_start(ap, fmt);
    if (vsnprintf((char *)sb->buf.string, sb->capacity, fmt, ap) < 0)
    {
        return nil_string;
    }
    va_end(ap);

    sb->buf.len = (size_t) ++len;
    arena_resize(arena, (void *)sb->buf.string, sb->buf.len);

    return sb->buf;
}
