# highlight
Linux shell program that highlights lines that match given regular expressions.

Usage: highlight [OPTIONS...] PATTERN... [FILE]
Example: highlight ERROR:red WARN:yellow debug.log

PATTERN         REGEX:COLOR
  REGEX         a regular expression
  COLOR         red, yellow, green, blue, white
FILE            a path to an input text file
OPTIONS:
  -h, --help            display this help and exit
  -b, --background              highlight background
  -i, --ignore-case             ignore case in regex match



This project is licensed under the terms of the MIT license.
