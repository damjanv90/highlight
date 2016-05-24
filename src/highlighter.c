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
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#include "settings.h"
#include "args_parser.h"
#include "errors.h"
#include "utils/bool.h"
#include "utils/list.h"
#include "color.h"
#include "range.h"

void usage(){
  fprintf(stdout,
                  "\nhighlight v%s\n\n"

                  "Usage: highlight [OPTIONS...] PATTERN... [FILE]\n"
                  "Example: highlight ERROR:red WARN:yellow debug.log\n\n"

                  "PATTERN\t\tREGEX:COLOR\n"
                  "  REGEX\t\ta regular expression\n"
                  "  COLOR\t\tred, yellow, green, blue, white\n"
                  "FILE\t\ta path to an input text file\n"
                  "OPTIONS:\n"
                  "  -h, --help\t\tdisplay this help and exit\n"
                  "  -b, --background\t\thighlight background\n"
                  //TODO: uncomment when implemented "  -s, --selection-only\t\thighlight only matched parts of the line, not the whole line\n"
                  "  -i, --ignore-case\t\tignore case in regex match\n\n",
                  VERSION
          );
}

int highlight_background = FALSE;
int regex_flags = 0;
int selection_only = FALSE;

typedef struct {
  BasicItem item;
  regex_t regex;
  color col;
}PreparedPattern;


PreparedPattern* PreparedPattern_new(char* regexStr, color col, int regex_flags){
  PreparedPattern* newPattern = (PreparedPattern*)calloc(1, sizeof(PreparedPattern));

  // Compile regular expression
  int regexCompilationError = regcomp(&(newPattern->regex), regexStr, regex_flags);
  if (regexCompilationError) {
      fprintf(stderr, "Could not compile regex\n%s\n", regexStr);
      exit(EXIT_FAILURE);
  }
  newPattern->col=col;

  return newPattern;
}


List* prepare_patterns(Arguments* parsed_args, int regex_flags){
  List* result = (List*)calloc(1, sizeof(List));

  PatternListItem* raw_pattern = (PatternListItem*)parsed_args->patterns.first;
  while (raw_pattern != NULL){
    append(result, (BasicItem*)PreparedPattern_new(raw_pattern->pattern.regex, raw_pattern->pattern.col, regex_flags));

    raw_pattern = (PatternListItem*)raw_pattern->item.next;
  }

  return result;
}

void begin_style(color _color, int highlight_background){
  int color_style = highlight_background ? _color + 40 : _color + 30;

  fprintf(stdout, "\e[1;%dm", color_style);
}

void end_style(){
  fprintf(stdout, "\e[0m");
}

int remove_eol(char* string){
  char* new_line_position = strchr(string, '\n');
  if (new_line_position != NULL){
    *new_line_position = '\0';
    return TRUE;
  }
  return FALSE;
}

void handle_options(List* options){
  if (is_empty(options)){
    return;
  }

  OptionListItem* opt = (OptionListItem*)options->first;
  while (opt != NULL){

    switch (opt->opt){

      case PRINT_HELP:
        usage();
        exit(EXIT_SUCCESS);

      case SELECTION_ONLY:
        selection_only = TRUE;
        break;

      case BACKGROUND:
        highlight_background = TRUE;
        break;

      case IGNORE_CASE:
        regex_flags |= REG_ICASE;
        break;

      default:
        fprintf(stderr, "Unknown option\n\n");
        usage();
        exit(EXIT_FAILURE);
    }

    opt = (OptionListItem*)opt->item.next;
  }
}

int main(int argc, char** argv){

  Arguments* parsed_args;
  int args_err = parse_arguments(argc, argv, &parsed_args);

  if (args_err){
    fprintf(stderr, "\n\nError parsing input arguments: %d\n\n", args_err);
    usage();
    exit(EXIT_FAILURE);
  }

  handle_options(&parsed_args->options);

  if (parsed_args->patterns.first == NULL){
    fprintf(stderr, "You must provide at least one valid pattern\n\n");
    usage();
    exit(EXIT_SUCCESS);
  }

  List* patterns = prepare_patterns(parsed_args, regex_flags);

  FILE* in;
  if (parsed_args->input_file == NULL){
    in = stdin;
  } else {
    //todo
    in = fopen(parsed_args->input_file, "r");
    if (in == NULL) {
      fprintf(stderr, "Could not open file\n%s\n", parsed_args->input_file);
      exit(EXIT_FAILURE);
    }
  }

  List match_ranges = EMPTY_LST;
  char * line = NULL;
  size_t len = 0;
  int has_input;

  //TODO: remove feof
  has_input = !feof(in) && getline(&line, &len, in) != -1;
  while (has_input){
    int priority = 0;
    PreparedPattern* onePattern;
    for (onePattern = (PreparedPattern*)patterns->first; onePattern; onePattern = (PreparedPattern*)onePattern->item.next, priority--){
      // Execute regular expression
      regmatch_t pmatch[onePattern->regex.re_nsub + 1];
      int regexResult = regexec(&(onePattern->regex), line, onePattern->regex.re_nsub + 1, pmatch, 0);
      if (!regexResult) {
          // ######### MATCH! ###########
          if (selection_only){
            int i = 0;
            if (onePattern->regex.re_nsub > 0){
              // there are subexpressions so skip the whole expression
              i = 1;
            }
            for(; i < onePattern->regex.re_nsub + 1; i++){
              process_range(&match_ranges, pmatch[i].rm_so, pmatch[i].rm_eo - 1, onePattern->col, priority);
            }
          }else{
            process_range(&match_ranges, 0, strlen(line), onePattern->col, priority);
            break;
          }

      } else if(regexResult != REG_NOMATCH) {
          regerror(regexResult, &(onePattern->regex), line, len);
          fprintf(stderr, "Regex match failed: %s\n", line);
          exit(EXIT_FAILURE);
      }
    }

    remove_eol(line);
    int line_length = strlen(line);
    int indx = 0;
    Range* range;
    for (range = (Range*)match_ranges.first; range != NULL; range = (Range*)range->item.next){
      while (indx < line_length && indx < range->start ){
        fprintf(stdout, "%c", line[indx]);
        indx++;
      }
      while (indx < line_length && indx <= range->end ){
        begin_style(range->_color, highlight_background);
        fprintf(stdout, "%c", line[indx]);
        end_style();
        indx++;
      }
    }
    while (indx < line_length){
      fprintf(stdout, "%c", line[indx]);
      indx++;
    }

    clear(&match_ranges);

    fprintf(stdout, "\n");

    has_input = !feof(in) && getline(&line, &len, in) != -1;
  }

  return SUCCESS;
}
