#!/usr/bin/env bash

lcc="$1"
bin="$2/out"
input="$3"
output="$2/test_output"
expected="$4"

"$lcc" "$input"
"$bin" > "$output"
diff "$expected" "$output"
