#include "../src/args_parser.h"
#include "../src/util.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_no_args(){
  Arguments* parsed;
  char* test[] = {"test"};
  int result = parse_arguments(1, test, &parsed);
  assert(result == FAILURE);
}

void test_help_short(){
  Arguments* parsed;
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
  Arguments* parsed;
  char* test[] = {"test", "--help"};
  int result = parse_arguments(2, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->patterns_head == NULL);
  assert(parsed->options_head != NULL);
  assert(parsed->options_head->next == NULL);
  assert(parsed->options_head->opt == PRINT_HELP);
}

void test_two_options(){
  Arguments* parsed;
  char* test[] = {"test", "-s", "--help"};
  int result = parse_arguments(3, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->patterns_head == NULL);
  assert(parsed->options_head != NULL);
  assert(parsed->options_head->opt == SELECTION_ONLY);
  assert(parsed->options_head->next != NULL);
  assert(parsed->options_head->next->opt == PRINT_HELP);
}

void test_one_option_one_pattern(){
  Arguments* parsed;
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
  Arguments* parsed;
  char* test[] = {"test", "regex1:red", "regex2:red"};
  int result = parse_arguments(3, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->options_head == NULL);
  assert(parsed->patterns_head != NULL);
  assert(!strcmp(parsed->patterns_head->pattern.regex, "regex1"));
  assert(parsed->patterns_head->pattern.col == red);
  assert(parsed->patterns_head->next != NULL);
  assert(!strcmp(parsed->patterns_head->next->pattern.regex, "regex2"));
  assert(parsed->patterns_head->next->pattern.col == red);
}

void test_two_patterns_filename(){
  Arguments* parsed;
  char* test[] = {"test", "regex1:red", "regex2:red", "input/file/path"};
  int result = parse_arguments(4, test, &parsed);
  assert(result == SUCCESS);

  assert(!strcmp(parsed->input_file, "input/file/path"));
  assert(parsed->options_head == NULL);
  assert(parsed->patterns_head != NULL);
  assert(!strcmp(parsed->patterns_head->pattern.regex, "regex1"));
  assert(parsed->patterns_head->pattern.col == red);
  assert(parsed->patterns_head->next != NULL);
  assert(!strcmp(parsed->patterns_head->next->pattern.regex, "regex2"));
  assert(parsed->patterns_head->next->pattern.col == red);
}

void main(int argc, char** argv){
  printf("Started testing arg_parser...\n");

  test_no_args();
  test_help_short();
  test_help_long();
  test_two_options();
  test_two_patterns();
  test_one_option_one_pattern();
  test_two_patterns_filename();

  printf("Finished testing arg_parser SUCCESSFULY!\n\n");
}
