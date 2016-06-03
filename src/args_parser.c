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

#include <string.h>
#include <stdlib.h>
#include "args_parser.h"
#include "errors.h"
#include "utils/bool.h"


typedef struct OptionInfo{
  option opt;
  const char* short_str;
  const char* long_str;
}OptionInfo;

static const struct OptionInfo all_options[4] = {
  {PRINT_HELP, "-h", "--help"},
  {SELECTION_ONLY, "-s", "--selection-only"},
  {BACKGROUND, "-b", "--background"},
  {IGNORE_CASE, "-i", "--ignore-case"}
};

static const struct OptionInfo OPT_PATTERN = {
  PATTERN, "-p", "--pattern"
};

int parse_color(char* color_str, color* parsed_color);
int parse_pattern(int argc, char** argv, int* last_location, Arguments* parsed_arguments);
int parse_input_file(int argc, char** argv, int* last_location, Arguments* parsed_arguments);

int parse_arguments(int argc, char** argv, Arguments** parsed_arguments){
  Arguments* result = (Arguments*)calloc(1, sizeof(Arguments));

  int last_location = 1; // skip argv[0]
  int err;
  while (last_location < argc && argv[last_location][0] == '-'){

    OptionListItem* new_option;
    int unknown_option = TRUE;
    int i =0;
    for (; i< sizeof(all_options)/sizeof(OptionInfo); i++){

      if (!strcmp(all_options[i].short_str, argv[last_location]) ||
        !strcmp(all_options[i].long_str, argv[last_location])){

        new_option = (OptionListItem*)calloc(1, sizeof(OptionListItem));
        new_option->opt = all_options[i].opt;

        unknown_option=FALSE;
        break;
      }
    }
    if (!unknown_option){
      append(&(result->options), (BasicItem*)new_option);
    } else if (!strcmp(OPT_PATTERN.short_str, argv[last_location]) ||
      !strcmp(OPT_PATTERN.long_str, argv[last_location])) {

      err = parse_pattern(argc, argv, &last_location, result);
      if (err){
        return err;
      }
    } else {
      // unknown option
      return ERR_UNKNOWN_OPTION;
    }

    (last_location)++;
  }

  err = parse_input_file(argc, argv, &last_location, result);
  if (err){
    return err;
  }

  *parsed_arguments = result;
  return SUCCESS;
}

int parse_pattern(int argc, char** argv, int* last_location, Arguments* parsed_arguments){
  if (*last_location + 2 >= argc){
      return ERR_INVALID_PATTERN;
  }

  (*last_location)++;
  char* regexStr = argv[*last_location];
  (*last_location)++;
  char* colorStr = argv[*last_location];

  color col;
  int err = parse_color(colorStr, &col);
  if (err){
    return err;
  }

  PatternListItem* new_pattern = (PatternListItem*)calloc(1, sizeof(PatternListItem));
  new_pattern->pattern.regex = regexStr;
  new_pattern->pattern.col = col;

  append(&(parsed_arguments->patterns), (BasicItem*)new_pattern);

  return SUCCESS;
}

int parse_input_file(int argc, char** argv, int* last_location, Arguments* parsed_arguments){
  if (*last_location < argc) {
    parsed_arguments->input_file = argv[*last_location];
  }

  (*last_location)++;
  return SUCCESS;
}

int parse_color(char* color_str, color* parsed_color){
  if (!strcmp("dark", color_str) ||
    !strcmp("d", color_str)){
    *parsed_color = black;
  } else if (!strcmp("red", color_str) ||
    !strcmp("r", color_str)){
    *parsed_color = red;
  } else if (!strcmp("green", color_str) ||
    !strcmp("g", color_str)){
    *parsed_color = green;
  } else if (!strcmp("yellow", color_str) ||
    !strcmp("y", color_str)){
    *parsed_color = yellow;
  } else if (!strcmp("blue", color_str) ||
    !strcmp("b", color_str)) {
    *parsed_color = blue;
  } else if (!strcmp("magneta", color_str) ||
    !strcmp("m", color_str)){
    *parsed_color = magneta;
  } else if (!strcmp("cyan", color_str) ||
    !strcmp("c", color_str)) {
    *parsed_color = cyan;
  } else if (!strcmp("white", color_str) ||
    !strcmp("w", color_str)) {
    *parsed_color = white;
  } else {
    return ERR_UNKNOWN_COLOR;
  }
  return SUCCESS;
}
