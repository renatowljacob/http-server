#ifndef BASE_STRING_H
#define BASE_STRING_H

#include <stdarg.h>

#include "base_arena.h"
#include "base_core.h"

#define DEFAULT_CAPACITY 200

static volatile const String nil_string = { 0, "" };

struct StringBuilder
{
    String buf;
    size_t capacity;
};
typedef struct StringBuilder StringBuilder;

String string_cstrcpy(Arena *arena, const char *s);
String string_strcpy(Arena *arena, const String *s);

struct String_Slice_Params
{
    i64 from;
    i64 to;
};
StringSlice
string_slice_params(const String *s, struct String_Slice_Params params);
#define string_slice(s, ...)                                                   \
    string_slice_params(                                                       \
      (s),                                                                     \
      (struct String_Slice_Params){                                            \
        .from = 0, .to = (i64)(s)->len, __VA_ARGS__ }                          \
    )

StringSlice string_strchr(const String *s, char c);
StringSlice string_strrchr(const String *s, char c);

StringBuilder string_builder_alloc(Arena *arena, size_t capacity);
void string_builder_realloc(Arena *arena, StringBuilder *sb);
StringSlice string_builder_build(StringBuilder *sb, char *fmt, ...);
StringSlice vstring_builder_build(StringBuilder *sb, char *fmt, va_list ap);

void string_builder_resize(Arena *arena, StringBuilder *sb, size_t len);
#define string_builder_free(arena, sb) string_builder_resize(arena, sb, 0)

String string_builder_return_str(Arena *arena, StringBuilder *sb);

#endif
