#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define and ?
#define or :
#define or_else ?:
#define not     !

#define global     static
#define internal   static
#define persistent static

#define KB(n) ((n) << 10)
#define MB(n) ((n) << 20)
#define GB(n) ((n) << 30)

#define ALIGN_POW2(x, b) (((x) + (b) - 1) & (~((b) - 1)))
#define IS_POW2(x)       (((x) & ((x) - 1)) == 0)

// clang-format off
#define MAX(x, y) (x) > (y) and (x) or (y)
#define MIN(x, y) (x) < (y) and (x) or (y)
// clang-format on

/*
 *  Type aliases
 */
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

/*
 *  STRING
 */

/*
 *  String type definition
 */
struct String
{
    size_t len;
    char *value;
};
typedef struct String String;
typedef String StringSlice;

// It kills the purpose of having custom strings if they are meant to be
// compatible with cstrings, so I'll probably write my own set of
// procedures later
// clang-format off
#define SEXPR(cstr) (String){ sizeof(cstr), (cstr) }
#define SSTAT(s, cstr)                                                         \
    do                                                                         \
    {                                                                          \
        (s)->value = (cstr);                                                   \
        (s)->len = sizeof(cstr);                                               \
    }                                                                          \
    while (0)
// clang-format on

/*
 *  DYNAMIC ARRAYS
 */

/*
 *  Generic Array definition
 */
#define GENERIC_ARRAY(TYPE)                                                    \
    struct TYPE##Array                                                         \
    {                                                                          \
        size_t capacity;                                                       \
        size_t len;                                                            \
        TYPE *items;                                                           \
    }

GENERIC_ARRAY(i8);
GENERIC_ARRAY(i16);
GENERIC_ARRAY(i32);
GENERIC_ARRAY(i64);

typedef struct i8Array i8Array;
typedef struct i16Array i16Array;
typedef struct i32Array i32Array;
typedef struct i64Array i64Array;

/*
 *  Array helpers
 */
#define array_len(a) (sizeof(a) / sizeof(a[0]))

// clang-format off
#define array_append(a, item)                                                  \
    do                                                                         \
    {                                                                          \
        assert(                                                                \
          (a)->len < (a)->capacity && "array_append: array capacity overflow"  \
        );                                                                     \
                                                                               \
        (a)->items[(a)->len++] = (item);                                       \
    }                                                                          \
    while (0)

#define array_concat(dst, src)                                                 \
    do                                                                         \
    {                                                                          \
        assert(                                                                \
          sizeof((dst)->items[0]) == sizeof((src)->items[0]) &&                \
          "array_concat: items of distinct sizes"                              \
        );                                                                     \
                                                                               \
        assert(                                                                \
          (dst)->len + (src)->len <= (dst)->capacity &&                        \
          "array_concat: dst capacity overflow"                                \
        );                                                                     \
                                                                               \
        memmove(                                                               \
          (dst)->items + sizeof(dst->items[0]) * (dst)->len,                   \
          (src)->items,                                                        \
          sizeof((src)->items[0]) * (src)->len                                 \
        );                                                                     \
        (dst)->len += (src)->len;                                              \
    }                                                                          \
    while (0)
// clang-format on

/*
 *  LINKED LISTS
 */

/*
 *  Singly Linked List definition
 */
struct SListNode
{
    struct SListNode *next;
};
typedef struct SListNode SListNode;

/*
 *  Doubly Linked List definition
 */
struct DListNode
{
    struct DListNode *prev;
    struct DListNode *next;
};
typedef struct DListNode DListNode;

#define list_get_node_or_nil(node, type, nil)                                  \
    (node) and(void *)((uintptr_t)(node) - offsetof(type, list)) or(nil)

#define list_get_node(node, type) list_get_node_or_nil((node), type, NULL)

struct SList_Append_Params
{
    void *next;
};
SListNode *_slist_append_params(
  SListNode *node,
  SListNode *tail,
  struct SList_Append_Params params
);
#define slist_append(node, tail, ...)                                          \
    _slist_append_params(                                                      \
      (node),                                                                  \
      (tail),                                                                  \
      (struct SList_Append_Params){ .next = NULL, __VA_ARGS__ }                \
    )

SListNode *slist_preppend(SListNode *node, SListNode *head);
SListNode *
slist_insert(SListNode *prev_node, SListNode *next_node, SListNode *node);

struct DList_Append_Params
{
    void *next;
};
DListNode *_dlist_append_params(
  DListNode *node,
  DListNode *tail,
  struct DList_Append_Params params
);
#define dlist_append(node, tail, ...)                                          \
    _dlist_append_params(                                                      \
      (node),                                                                  \
      (tail),                                                                  \
      (struct DList_Append_Params){ .next = NULL, __VA_ARGS__ }                \
    )

struct DList_Preppend_Params
{
    void *prev;
};
DListNode *_dlist_preppend_params(
  DListNode *node,
  DListNode *head,
  struct DList_Preppend_Params params
);
#define dlist_preppend(node, tail, ...)                                        \
    _dlist_preppend_params(                                                    \
      (node),                                                                  \
      (tail),                                                                  \
      (struct DList_Preppend_Params){ .prev = NULL, __VA_ARGS__ }              \
    )

DListNode *
dlist_insert(DListNode *node, DListNode *prev_node, DListNode *next_node);
DListNode *dlist_insert2(DListNode *node, DListNode *next_node);

bool is_nil(const void *ptr, const void *nil);

#endif
