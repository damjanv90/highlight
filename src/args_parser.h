/*
The MIT License (MIT)

Copyright (c) 2016 Damjan Vukovic (email:damjanvu@gmail.com, github:damjanv90)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ARGS_PARSER_H

  #define ARGS_PARSER_H

  #include "utils/list.h"
  #include "color.h"


  // ########## PATTERNS #################

  typedef struct {
    char* regex;
    color col;
  } Pattern;

  typedef struct {
    BasicItem item;
    Pattern pattern;
  } PatternListItem;

  // ########## PATTERNS #################


  // ########## OPTIONS #################

  typedef enum {PRINT_HELP, SELECTION_ONLY, BACKGROUND, IGNORE_CASE, PATTERN} option;

  typedef struct {
    BasicItem item;
    option opt;
  } OptionListItem;

  // ########## OPTIONS #################

  typedef struct{
    List options;
    List patterns;
    char* input_file;
  } Arguments;

  int parse_arguments(int argc, char** argv, Arguments** parsed_arguments);

#endif
