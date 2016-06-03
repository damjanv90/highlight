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
  assert(parsed->options.first != NULL);

  OptionListItem* first = (OptionListItem*)(parsed->options.first);

  assert(first->opt == PRINT_HELP);
}

void test_help_long(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "--help"};
  int result = parse_arguments(2, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  assert(parsed->options.first != NULL);

  OptionListItem* first = (OptionListItem*)(parsed->options.first);

  assert(first->opt == PRINT_HELP);
}

void test_all_options_short(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-s", "-b", "-h", "-i"};
  int result = parse_arguments(5, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);

  assert(parsed->options.first != NULL);
  OptionListItem* first = (OptionListItem*)(parsed->options.first);

  assert(first->item.next != NULL);
  OptionListItem* second = (OptionListItem*)(first->item.next);

  assert(second->item.next != NULL);
  OptionListItem* third = (OptionListItem*)(second->item.next);

  assert(third->item.next != NULL);
  OptionListItem* fourth = (OptionListItem*)(third->item.next);

  assert(first->opt == SELECTION_ONLY);
  assert(second->opt == BACKGROUND);
  assert(third->opt == PRINT_HELP);
  assert(fourth->opt == IGNORE_CASE);
}

void test_all_options_long(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "--selection-only", "--background", "--help", "--ignore-case"};
  int result = parse_arguments(5, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);

  assert(parsed->options.first != NULL);
  OptionListItem* first = (OptionListItem*)(parsed->options.first);

  assert(first->item.next != NULL);
  OptionListItem* second = (OptionListItem*)(first->item.next);

  assert(second->item.next != NULL);
  OptionListItem* third = (OptionListItem*)(second->item.next);

  assert(third->item.next != NULL);
  OptionListItem* fourth = (OptionListItem*)(third->item.next);

  assert(first->opt == SELECTION_ONLY);
  assert(second->opt == BACKGROUND);
  assert(third->opt == PRINT_HELP);
  assert(fourth->opt == IGNORE_CASE);
}

void test_one_option_one_pattern(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-s", "-p", "regex", "red"};
  int result = parse_arguments(5, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);
  OptionListItem* first_opt = (OptionListItem*)(parsed->options.first);

  assert(first_opt->opt == SELECTION_ONLY);
  assert(first_opt->item.next == NULL);

  PatternListItem* first_patt = (PatternListItem*)(parsed->patterns.first);

  assert(!strcmp(first_patt->pattern.regex, "regex"));
  assert(first_patt->pattern.col == red);
  assert(first_patt->item.next == NULL);
}

void test_two_patterns(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-p", "regex1", "yellow", "-p", "regex2", "red"};
  int result = parse_arguments(7, test, &parsed);
  assert(result == SUCCESS);

  assert(parsed->input_file == NULL);

  assert(parsed->patterns.first != NULL);
  PatternListItem* first = (PatternListItem*)(parsed->patterns.first);
  assert(first->item.next != NULL);
  PatternListItem* second = (PatternListItem*)(first->item.next);

  assert(!strcmp(first->pattern.regex, "regex1"));
  assert(first->pattern.col == yellow);
  assert(second != NULL);
  assert(!strcmp(second->pattern.regex, "regex2"));
  assert(second->pattern.col == red);
}

void test_two_patterns_filename(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-p", "regex1", "blue", "-p", "regex2", "green", "input/file/path"};
  int result = parse_arguments(8, test, &parsed);
  assert(result == SUCCESS);

  assert(!strcmp(parsed->input_file, "input/file/path"));

  assert(parsed->patterns.first != NULL);
  PatternListItem* first = (PatternListItem*)(parsed->patterns.first);
  assert(first->item.next != NULL);
  PatternListItem* second = (PatternListItem*)(first->item.next);

  assert(!strcmp(first->pattern.regex, "regex1"));
  assert(first->pattern.col == blue);
  assert(!strcmp(second->pattern.regex, "regex2"));
  assert(second->pattern.col == green);
}

void test_invalid_color(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-p", "regex1", "invalid_color"};
  int result = parse_arguments(4, test, &parsed);
  assert(result == ERR_UNKNOWN_COLOR);
  assert(parsed == NULL);
}

void test_invalid_pattern(){
  Arguments* parsed = NULL;
  char* test[] = {"test", "-p", "regex"};
  int result = parse_arguments(3, test, &parsed);
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

int main(int argc, char** argv){
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
