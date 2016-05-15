#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../src/args_parser.h"
#include "../src/errors.h"

void test_no_args(){
  Arguments* parsed = NULL;
  char* test[] = {"test"};
  int result = parse_arguments(1, test, &parsed);
  assert(result == SUCCESS);
}

void test_help_short(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-h"};
  int result = parse_arguments(2, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->patterns_head == NULL);
  assert(parsed->options_head != NULL);
  assert(parsed->options_head->next == NULL);
  assert(parsed->options_head->opt == PRINT_HELP);
}

void test_help_long(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "--help"};
  int result = parse_arguments(2, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->patterns_head == NULL);
  assert(parsed->options_head != NULL);
  assert(parsed->options_head->next == NULL);
  assert(parsed->options_head->opt == PRINT_HELP);
}

void test_all_options_short(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-s", "-b", "-h", "-i"};
  int result = parse_arguments(5, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->patterns_head == NULL);
  assert(parsed->options_head != NULL);
  assert(parsed->options_head->opt == SELECTION_ONLY);
  assert(parsed->options_head->next != NULL);
  assert(parsed->options_head->next->opt == BACKGROUND);
  assert(parsed->options_head->next->next != NULL);
  assert(parsed->options_head->next->next->opt == PRINT_HELP);
  assert(parsed->options_head->next->next->next != NULL);
  assert(parsed->options_head->next->next->next->opt == IGNORE_CASE);
}

void test_all_options_long(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "--selection-only", "--background", "--help", "--ignore-case"};
  int result = parse_arguments(5, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->patterns_head == NULL);
  assert(parsed->options_head != NULL);
  assert(parsed->options_head->opt == SELECTION_ONLY);
  assert(parsed->options_head->next != NULL);
  assert(parsed->options_head->next->opt == BACKGROUND);
  assert(parsed->options_head->next->next != NULL);
  assert(parsed->options_head->next->next->opt == PRINT_HELP);
  assert(parsed->options_head->next->next->next != NULL);
  assert(parsed->options_head->next->next->next->opt == IGNORE_CASE);
}

void test_one_option_one_pattern(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-s", "regex:red"};
  int result = parse_arguments(3, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->options_head != NULL);
  assert(parsed->options_head->opt == SELECTION_ONLY);
  assert(parsed->options_head->next == NULL);
  assert(parsed->patterns_head != NULL);
  assert(!strcmp(parsed->patterns_head->pattern.regex, "regex"));
  assert(parsed->patterns_head->pattern.col == red);
  assert(parsed->patterns_head->next == NULL);
}

void test_two_patterns(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "regex1:yellow", "regex2:red"};
  int result = parse_arguments(3, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->options_head == NULL);
  assert(parsed->patterns_head != NULL);
  assert(!strcmp(parsed->patterns_head->pattern.regex, "regex1"));
  assert(parsed->patterns_head->pattern.col == yellow);
  assert(parsed->patterns_head->next != NULL);
  assert(!strcmp(parsed->patterns_head->next->pattern.regex, "regex2"));
  assert(parsed->patterns_head->next->pattern.col == red);
}

void test_two_patterns_filename(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "regex1:blue", "regex2:green", "input/file/path"};
  int result = parse_arguments(4, test, &parsed);
  assert(result == SUCCESS);

  assert(!strcmp(parsed->input_file, "input/file/path"));
  assert(parsed->options_head == NULL);
  assert(parsed->patterns_head != NULL);
  assert(!strcmp(parsed->patterns_head->pattern.regex, "regex1"));
  assert(parsed->patterns_head->pattern.col == blue);
  assert(parsed->patterns_head->next != NULL);
  assert(!strcmp(parsed->patterns_head->next->pattern.regex, "regex2"));
  assert(parsed->patterns_head->next->pattern.col == green);
}

void test_invalid_color(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "regex1:invalid_color"};
  int result = parse_arguments(2, test, &parsed);
  assert(result == ERR_UNKNOWN_COLOR);
  assert(parsed == NULL);
}

void test_invalid_pattern(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "regex-red"};
  int result = parse_arguments(2, test, &parsed);
  assert(result == ERR_INVALID_PATTERN);
  assert(parsed == NULL);
}

void test_invalid_option(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "--invalid_option"};
  int result = parse_arguments(2, test, &parsed);
  assert(result == ERR_UNKNOWN_OPTION);
  assert(parsed == NULL);
}

void main(int argc, char** argv){
  printf("Started testing arg_parser...\n");

  test_no_args();
  test_help_short();
  test_help_long();
  test_all_options_short();
  test_all_options_long();
  test_two_patterns();
  test_one_option_one_pattern();
  test_two_patterns_filename();
  test_invalid_color();
  test_invalid_pattern();
  test_invalid_option();

  printf("Finished testing arg_parser SUCCESSFULY!\n\n");
}
