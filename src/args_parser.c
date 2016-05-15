#include <string.h>
#include <stdlib.h>
#include "args_parser.h"
#include "errors.h"
#include "bool.h"


typedef struct OptionInfo{
  option opt;
  const char* short_str;
  const char* long_str;
}OptionInfo;

#define ADD_OPTION(opt, short_str, long_str) {opt, short_str, long_str}

static const struct OptionInfo all_options[4] = {
  ADD_OPTION(PRINT_HELP, "-h", "--help"),
  ADD_OPTION(SELECTION_ONLY, "-s", "--selection-only"),
  ADD_OPTION(BACKGROUND, "-b", "--background"),
  ADD_OPTION(IGNORE_CASE, "-i", "--ignore-case")
};

int parse_color(char* color_str, color* parsed_color);
int parse_options(int argc, char** argv, int* last_location, Arguments* parsed_arguments);
int parse_patterns(int argc, char** argv, int* last_location, Arguments* parsed_arguments);
int parse_input_file(int argc, char** argv, int* last_location, Arguments* parsed_arguments);

int parse_arguments(int argc, char** argv, Arguments** parsed_arguments){
  Arguments* result = (Arguments*)malloc(sizeof(Arguments));

  if (result == NULL){
    return ERR_MEM_ALOC;
  }

  int last_location = 1; // skip argv[0]
  int err;

  err = parse_options(argc, argv, &last_location, result);
  if (err){
    return err;
  }

  err = parse_patterns(argc, argv, &last_location, result);
  if (err){
    return err;
  }

  err = parse_input_file(argc, argv, &last_location, result);
  if (err){
    return err;
  }

  *parsed_arguments = result;
  return SUCCESS;
}

int parse_options(int argc, char** argv, int* last_location, Arguments* parsed_arguments){

  OptionList* prev_option;
  while (*last_location < argc && argv[*last_location][0] == '-'){

    OptionList* one_option;
    int unknown_option = TRUE;
    int i =0;
    for (; i< sizeof(all_options)/sizeof(OptionInfo); i++){

      if (!strcmp(all_options[i].short_str, argv[*last_location]) ||
        !strcmp(all_options[i].long_str, argv[*last_location])){

        one_option = (OptionList*)malloc(sizeof(OptionList));
        one_option->opt = all_options[i].opt;

        unknown_option=FALSE;
      }
    }
    if (unknown_option) {
      // unknown option
      return ERR_UNKNOWN_OPTION;
    }

    if (parsed_arguments->options_head == NULL) {
      parsed_arguments->options_head = one_option;
    } else {
      prev_option->next = one_option;
    }
    prev_option = one_option;
    (*last_location)++;
  }

  return SUCCESS;
}

int parse_patterns(int argc, char** argv, int* last_location, Arguments* parsed_arguments){
  if (*last_location >= argc) {
    return SUCCESS;
  }

  if (strchr(argv[*last_location], ':') == NULL){
    // must provide at least one valid pattern
    return ERR_INVALID_PATTERN;
  }

  PatternList* prev_pattern;
  while (*last_location < argc && strchr(argv[*last_location], ':') != NULL){

    char* arg = (char*)malloc(strlen(argv[*last_location])+1);
    strcpy(arg, argv[*last_location]);

    char* regexStr=strtok(arg, ":");
    color col;
    int err = parse_color(strtok(NULL, ":"), &col);
    if (err){
      return err;
    }

    PatternList* one_pattern = (PatternList*)malloc(sizeof(PatternList));
    one_pattern->pattern.regex = regexStr;
    one_pattern->pattern.col = col;

    if (parsed_arguments->patterns_head == NULL){
      parsed_arguments->patterns_head = one_pattern;
    } else {
      prev_pattern->next = one_pattern;
    }
    prev_pattern = one_pattern;
    (*last_location)++;
  }

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
  if (!strcmp("black", color_str)){
    *parsed_color = black;
  } else if (!strcmp("red", color_str)){
    *parsed_color = red;
  } else if (!strcmp("green", color_str)){
    *parsed_color = green;
  } else if (!strcmp("yellow", color_str)){
    *parsed_color = yellow;
  } else if (!strcmp("blue", color_str)) {
    *parsed_color = blue;
  } else if (!strcmp("magneta", color_str)){
    *parsed_color = magneta;
  } else if (!strcmp("cyan", color_str)) {
    *parsed_color = cyan;
  } else if (!strcmp("white", color_str)) {
    *parsed_color = white;
  } else {
    return ERR_UNKNOWN_COLOR;
  }
  return SUCCESS;
}
