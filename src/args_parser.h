#ifndef ARGS_PARSER_H

  #define ARGS_PARSER_H

  typedef enum {black=30, red, green, yellow, blue, magneta, cyan, white} color;

  typedef struct {
    char* regex;
    color col;
  } Pattern;

  typedef struct pattern_list{
    Pattern pattern;
    struct pattern_list* next;
  } PatternList;

  typedef enum {PRINT_HELP, SELECTION_ONLY} option;

  typedef struct options_list{
    option opt;
    struct options_list* next;
  } OptionList;

  typedef struct{
    OptionList* options_head;
    PatternList* patterns_head;
    char* input_file;
  } Arguments;

  int parse_arguments(int argc, char** argv, Arguments** parsed_arguments);

#endif
