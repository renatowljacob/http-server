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

// TEST: Read request without message body

// TEST: Read request with message body

// TEST: Read LF and ignore preceding CR

// TEST: Invalidate request with bare CR

// TEST: Ignore CRs before request-line

// TEST: Reject whitespace-preceded line without further processing

// TEST: Reject request preceded by invalid octets

// NOTE: Maybe implement version downgrading

// TEST: Reject request-line components separated by more than one SP

// TEST: Reject request-line with invalid Host header (lacking, one or more, invalid field value)

// TODO: Accept origin, absolute and asterisk form of request targets

// TODO: Reject authority-form request target

// TEST: Derive host from absolute-form request target
