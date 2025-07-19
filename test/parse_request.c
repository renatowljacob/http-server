#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/parser.c"
#include "../src/http.c"

State state;

static void test_parse_request_root(void **state)
{
    (void) state;

    char string[] = "GET / HTTP/1.1\nHost: example.com";
    assert_string_equal(parse_request_target(string), "/index.html");
}

static void test_parse_request_dir(void **state)
{
    (void) state;

    char string[] = "GET /blog HTTP/1.1\nHost: example.com";
    assert_string_equal(parse_request_target(string), "/blog/index.html");
}

static void test_parse_request_trailing_slash(void **state)
{
    (void) state;

    char string[] = "GET /blog/ HTTP/1.1\nHost: example.com";
    assert_string_equal(parse_request_target(string), "/blog/index.html");
}

static void test_parse_request_path_file(void **state)
{
    (void) state;

    char string[] = "GET /something.html HTTP/1.1\nHost: example.com";
    assert_string_equal(parse_request_target(string), "/something.html");
}
