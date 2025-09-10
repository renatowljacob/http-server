#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

#define and ?
#define or :
#define or_else ?:

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

#define GENERIC_MAP(TYPE)  \
struct TYPE##Map           \
{                          \
    size_t capacity;       \
    size_t len;            \
    TYPE##Array *key;      \
    TYPE##Array *value;    \
}

#define GENERIC_ARRAY(TYPE)  \
struct TYPE##Array           \
{                            \
    size_t capacity;         \
    size_t len;              \
    TYPE *items;             \
}

GENERIC_ARRAY(i8);
GENERIC_ARRAY(i16);
GENERIC_ARRAY(i32);
GENERIC_ARRAY(i64);

typedef struct i8Array i8Array;
typedef struct i16Array i16Array;
typedef struct i32Array i32Array;
typedef struct i64Array i64Array;

GENERIC_MAP(i8);

typedef struct
{
    size_t len;
    const char *string;
} String;
typedef String StringSlice;

typedef struct
{
    String header;
    String value;
} Header;

#define KB(n) ((n) << 10)
#define MB(n) ((n) << 20)

#define ALIGN_POW2(x,b) (((x) + (b) - 1)&(~((b) - 1)))
#define IS_POW2(x) (((x) & ((x) - 1)) == 0)

#define MAX(x, y) (x) > (y) and (x) or (y)
#define MIN(x, y) (x) < (y) and (x) or (y)

// It kills the purpose of having custom strings if they are meant to be
// compatible with cstrings, so I'll probably write my own set of
// procedures later
#define S(cstr) { sizeof(cstr), (cstr) }

#define SIZEOF_ARRAY(a) sizeof(a) / sizeof(a[0])

#define array_append(a, item)                             \
    do                                                    \
    {                                                     \
        assert((a)->len < (a)->capacity                   \
            && "array_append: array capacity overflow");  \
                                                          \
        (a)->items[(a)->len++] = (item);                  \
    }                                                     \
    while (0)

#define array_concat(dst, src)                                      \
    do                                                              \
    {                                                               \
        assert(sizeof((dst)->items[0]) == sizeof((src)->items[0])   \
            && "array_concat: items of distinct sizes");            \
                                                                    \
        assert((dst)->len + (src)->len <= (dst)->capacity           \
            && "array_concat: dst capacity overflow");              \
                                                                    \
        memmove(                                                    \
                (dst)->items + sizeof(dst->items[0]) * (dst)->len,  \
                (src)->items,                                       \
                sizeof((src)->items[0]) * (src)->len                \
        );                                                          \
        (dst)->len += (src)->len;                                   \
    }                                                               \
    while (0)

#endif
