#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

// Test functions files
#include "parse_request.c"

// Test runner function
int main(void)
{
    // Test functions
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_request_root),
        cmocka_unit_test(test_parse_request_dir),
        cmocka_unit_test(test_parse_request_trailing_slash),
        cmocka_unit_test(test_parse_request_path_file),
    };

    // Run tests
    return cmocka_run_group_tests(tests, NULL, NULL);
}
