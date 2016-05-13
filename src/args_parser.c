#include <string.h>
#include <stdlib.h>
#include "args_parser.h"
#include "util.h"


color parse_color(char* color_str);

int parse_arguments(int argc, char** argv, Arguments** parsed_arguments){
  Arguments* result = (Arguments*)malloc(sizeof(Arguments));
  int last_location = 1; // skip argv[0]
  int err;

  err = parse_options(argc, argv, &last_location, result);
  if (err){
    return FAILURE;
  }

  err = parse_patterns(argc, argv, &last_location, result);
  if (err){
    return FAILURE;
  }

  err = parse_input_file(argc, argv, &last_location, result);
  if (err){
    return FAILURE;
  }

  *parsed_arguments = result;
  return SUCCESS;
}

int parse_options(int argc, char** argv, int* last_location, Arguments* parsed_arguments){
  if (*last_location >= argc){
    return FAILURE;
  }


  OptionList* prev_option;
  while (*last_location < argc && argv[*last_location][0] == '-'){

    OptionList* one_option;
    if (!strcmp("-s", argv[*last_location]) ||
        !strcmp("--selection-only", argv[*last_location])){

      one_option = (OptionList*)malloc(sizeof(OptionList));
      one_option->opt = SELECTION_ONLY;

    } else if (!strcmp("-h", argv[*last_location]) ||
        !strcmp("--help", argv[*last_location])) {

      one_option = (OptionList*)malloc(sizeof(OptionList));
      one_option->opt = PRINT_HELP;

    } else {

      // unknown option
      return FAILURE;
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
    return FAILURE;
  }

  PatternList* prev_pattern;
  while (*last_location < argc && strchr(argv[*last_location], ':') != NULL){
    char* arg = (char*)malloc(strlen(argv[*last_location])+1);
    strcpy(arg, argv[*last_location]);
    char* regexStr=strtok(arg, ":");
    color c = parse_color(strtok(NULL, ":"));

    if (c == FAILURE){
      // unknown color
      return FAILURE;
    }

    PatternList* one_pattern = (PatternList*)malloc(sizeof(PatternList));
    one_pattern->pattern.regex = regexStr;
    one_pattern->pattern.col = c;

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

color parse_color(char* color_str){
  if (!strcmp("black", color_str)){
    return black;
  } else if (!strcmp("red", color_str)){
    return red;
  } else if (!strcmp("green", color_str)){
    return green;
  } else if (!strcmp("yellow", color_str)){
    return yellow;
  } else if (!!strcmp("blue", color_str)) {
    return blue;
  } else if (!strcmp("magneta", color_str)){
    return magneta;
  } else if (!strcmp("cyan", color_str)) {
    return cyan;
  } else if (!strcmp("white", color_str)) {
    return white;
  } else {
    // unknown color
    return FAILURE;
  }
}
