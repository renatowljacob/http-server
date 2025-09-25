#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "base_string.h"

String
string_cstrcpy(Arena *arena, const char *s)
{
    size_t len = strlen(s) + 1;
    char *cstr = arena_alloc(arena, len);
    if (cstr == NULL)
    {
        return nil_string;
    }

    memcpy(cstr, s, len);

    return (String){ len, cstr };
}

String
string_strcpy(Arena *arena, const String *s)
{
    char *str = arena_alloc(arena, s->len);
    if (str == NULL)
    {
        return nil_string;
    }

    memcpy(str, s->value, s->len);

    return (String){ s->len, str };
}

// Slice
StringSlice
string_slice_params(const String *s, struct String_Slice_Params params)
{
    if ((size_t)labs(params.from) > s->len || (size_t)labs(params.to) > s->len)
    {
        return nil_string;
    }

    size_t _from =
      (size_t)(params.from >= 0 and params.from or params.from + (i64)s->len);
    size_t _to =
      (size_t)(params.to >= 0 and params.to or params.to + (i64)s->len);
    if (_from > _to)
    {
        return nil_string;
    }

    size_t len = _to - _from;

    return (StringSlice){ .len = len, .value = &s->value[_from] };
}

StringSlice
string_strchr(const String *s, char c)
{
    size_t occurrence = s->len;
    for (size_t i = 0; i < s->len; i++)
    {
        if (s->value[i] == c)
        {
            occurrence = i;
            break;
        }
    }

    return (StringSlice){ s->len - occurrence, &s->value[occurrence] };
}

StringSlice
string_strrchr(const String *s, char c)
{
    size_t occurrence = s->len;
    for (size_t i = 0; i < s->len; i++)
    {
        if (s->value[i] == c)
        {
            occurrence = i;
        }
    }

    return (StringSlice){ s->len - occurrence, &s->value[occurrence] };
}

// String Builder
StringBuilder
string_builder_alloc(Arena *arena, size_t capacity)
{
    StringBuilder sb = {
        .buf = { .len = 0, .value = arena_alloc(arena, capacity) },
        .capacity = sb.buf.value != NULL and capacity or 0,
    };

    return sb;
}

void
string_builder_realloc(Arena *arena, StringBuilder *sb)
{
    sb->buf.len = 0;
    sb->buf.value = arena_alloc_align(arena, sb->capacity, sizeof(char));
    if (sb->buf.value == NULL)
    {
        sb->capacity = 0;
    }
}

StringSlice
vstring_builder_build(StringBuilder *sb, char *fmt, va_list ap)
{
    va_list copy;

    va_copy(copy, ap);
    i32 len = vsnprintf(NULL, 0, fmt, copy);
    if (len < 0)
    {
        return nil_string;
    }
    va_end(copy);

    if (sb->buf.len + (size_t)len > sb->capacity)
    {
        return nil_string;
    }

    // Account for the null char
    size_t actual_len = (size_t)len + 1;
    if (vsnprintf(&sb->buf.value[sb->buf.len], actual_len, fmt, ap) < 0)
    {
        return nil_string;
    }
    size_t start = sb->buf.len;
    sb->buf.len =
      sb->buf.len == 0 and actual_len or sb->buf.len + actual_len - 1;

    return (StringSlice){ actual_len, &sb->buf.value[start] };
}

StringSlice
string_builder_build(StringBuilder *sb, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    StringSlice s = vstring_builder_build(sb, fmt, ap);
    va_end(ap);

    return s;
}

void
string_builder_resize(Arena *arena, StringBuilder *sb, size_t len)
{
    sb->capacity = len;

    if (arena->buf + arena->prev_offset == (u8 *)sb->buf.value)
    {
        if (arena_resize_align(
              arena,
              (void *)sb->buf.value,
              arena->curr_offset - arena->prev_offset,
              sb->buf.len,
              sizeof(char)
            ) == NULL)
        {
            sb->buf = nil_string;
        }
    }
}

String
string_builder_return_str(Arena *arena, StringBuilder *sb)
{
    string_builder_resize(arena, sb, sb->buf.len);

    return sb->buf;
}
