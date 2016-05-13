#!/bin/bash

rm -rf ./bin
mkdir ./bin
gcc -o ./bin/highlight src/highlighter.c src/args_parser.c
