#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#include "settings.h"
#include "args_parser.h"
#include "errors.h"
#include "bool.h"

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
                  "  -s, --selection-only\t\thighlight only matched parts of the line, not the whole line\n"
                  "  -i, --ignore-case\t\tignore case in regex match\n\n",
                  VERSION
          );
}

int highlight_background = FALSE;
int ignore_case = FALSE;

typedef struct pattern {
  regex_t regex;
  color col;
  struct pattern* next;
}PreparedPattern;


PreparedPattern* PreparedPattern_new(char* regexStr, color col){
  PreparedPattern* newPattern = (PreparedPattern*)malloc(sizeof(PreparedPattern));

  /* Compile regular expression */
  int regexCompilationError;
  int compilation_flags = 0;
  if (ignore_case){
    compilation_flags |= REG_ICASE;
  }
  regexCompilationError = regcomp(&(newPattern->regex), regexStr, compilation_flags);
  if (regexCompilationError) {
      fprintf(stderr, "Could not compile regex\n%s\n", regexStr);
      exit(EXIT_FAILURE);
  }
  newPattern->col=col;
  newPattern->next = NULL;

  return newPattern;
}


void prepare_patterns(Arguments* parsed_args, PreparedPattern** patterns){
  PreparedPattern* prev = NULL;

  PatternList* raw_pattern = parsed_args->patterns_head;
  while (raw_pattern != NULL){
    PreparedPattern* newPattern = PreparedPattern_new(raw_pattern->pattern.regex, raw_pattern->pattern.col);

    if (prev != NULL){
      prev->next = newPattern;
    } else {
      // first
      *patterns = newPattern;
    }
    prev = newPattern;
    raw_pattern = raw_pattern->next;
  }
}

void highlight(color c, char* text){
  int format = highlight_background ? c + 40 : c + 30;
  fprintf(stdout, "\e[1;%dm%s\e[0m", format, text );
}

int main(int argc, char** argv){

  Arguments* parsed_args;
  int args_err = parse_arguments(argc, argv, &parsed_args);

  if (args_err){
    fprintf(stderr, "\n\nError parsing input arguments: %d\n\n", args_err);
    usage();
    exit(EXIT_FAILURE);
  }

  OptionList* opt = parsed_args->options_head;
  while (opt != NULL){

    switch (opt->opt){

      case PRINT_HELP:
        usage();
        exit(EXIT_SUCCESS);

      case SELECTION_ONLY:
        // TODO: implement
        break;

      case BACKGROUND:
        highlight_background = TRUE;
        break;

      case IGNORE_CASE:
        ignore_case = TRUE;
        break;

      default:
        fprintf(stderr, "Unknown option\n\n");
        usage();
        exit(EXIT_FAILURE);
    }

    opt = opt->next;
  }

  if (parsed_args->patterns_head == NULL){
    fprintf(stderr, "You must provide at least one valid pattern\n\n");
    usage();
    exit(EXIT_SUCCESS);
  }

  PreparedPattern* patterns;
  prepare_patterns(parsed_args, &patterns);

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

  char * line = NULL;
  size_t len = 0;
  int has_input;

  has_input = !feof(in) && getline(&line, &len, in) != -1;
  while (has_input){
    int matches = FALSE;
    PreparedPattern* onePattern;
    for (onePattern = patterns; onePattern; onePattern = onePattern->next){
      /* Execute regular expression */
      int regexResult = regexec(&(onePattern->regex), line, 0, NULL, 0);
      if (!regexResult) {
          int should_insert_new_line = FALSE;
          char* new_line_position = strchr(line, '\n');
          if (new_line_position != NULL){
            *new_line_position = '\0';
            should_insert_new_line = TRUE;
          }
          highlight(onePattern->col, line );
          if (should_insert_new_line){
            fprintf(stdout, "\n");
          }
          matches = TRUE;
          break;
      } else if(regexResult != REG_NOMATCH) {
          //TODO: vidjeti sta radi ova regerror
          regerror(regexResult, &(onePattern->regex), line, len);
          fprintf(stderr, "Regex match failed: %s\n", line);
          exit(EXIT_FAILURE);
      }
    }

    if (!matches){
      fprintf(stdout, "%s", line );
    }

    has_input = !feof(in) && getline(&line, &len, in) != -1;
  }

  // Free memory allocated to the pattern buffer by regcomp()
  PreparedPattern* p=patterns;
  while (p != NULL){
    regfree(&(p->regex));
    PreparedPattern* tmp = p;
    p = p->next;
    free(tmp);
  }

  if (line){
    free (line);
  }

  return SUCCESS;
}
