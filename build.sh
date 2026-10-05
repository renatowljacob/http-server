#!/bin/sh

debug_args="-std=gnu11 -Wall -Wextra -Wformat-overflow -Wuse-after-free=1 \
        -Wstrict-prototypes -Wconversion -Wno-override-init -O0 -ggdb "
release_args="-std=gnu11 -Wall -Wextra -Wformat-overflow -Wuse-after-free=1 \
        -Wstrict-prototypes -Wconversion -Wno-override-init -O2 "
analyzer_args="-fanalyzer "
sanitizer_args="-fsanitize=address,undefined -Werror -fmax-errors=1 "
output_dir="build"
output_file="${output_dir}/main"

# TODO: debug and release builds

mkdir -p "${output_dir}"

case ${1} in
analyzer)
        gcc ${debug_args} ${analyzer_args} -o /dev/null main.c \
                && printf '%b\n' "No analyzer errors reported"
        ;;
sanitizer)
        gcc ${debug_args} ${sanitizer_args} -o ${output_file} main.c
        ;;
release)
        gcc ${release_args} -o ${output_file} main.c
        ;;
*)
        gcc ${debug_args} -o ${output_file} main.c
        ;;
esac
