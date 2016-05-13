#!/bin/bash
rm -rf ./execution
mkdir ./execution
gcc -o ./execution/test_arg_parser test_arg_parser.c ../src/args_parser.c
./execution/test_arg_parser

(cd ..; exec ./build.bash);

../bin/highlight --help > execution/help_short_result.txt
