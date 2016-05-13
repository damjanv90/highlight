#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#include "settings.h"
#include "args_parser.h"
#include "util.h"

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
                  "  -s, --selection-only\t\thighlight only matched parts of the line, not the whole line\n\n",
                  VERSION
          );
}



typedef struct pattern {
  regex_t regex;
  color col;
  struct pattern* next;
}PreparedPattern;


PreparedPattern* PreparedPattern_new(char* regexStr, color col){
  PreparedPattern* newPattern = (PreparedPattern*)malloc(sizeof(PreparedPattern));

  /* Compile regular expression */
  int regexCompilationError;
  regexCompilationError = regcomp(&(newPattern->regex), regexStr, 0);
  if (regexCompilationError) {
      fprintf(stderr, "Could not compile regex\n%s\n", regexStr);
      exit(FAILURE);
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

int main(int argc, char** argv){

  Arguments* parsed_args;
  int args_err = parse_arguments(argc, argv, &parsed_args);

  if (args_err){
    usage();
    exit(FAILURE);
  }

  OptionList* opt = parsed_args->options_head;
  while (opt != NULL){

    switch (opt->opt){

      case PRINT_HELP:
        usage();
        exit(SUCCESS);

      case SELECTION_ONLY:
        // TODO: implement
        break;

      default:
        fprintf(stderr, "Unknown option\n\n");
        usage();
        exit(FAILURE);
    }

    opt = opt->next;
  }

  if (parsed_args->patterns_head == NULL){
    fprintf(stderr, "You must provide at least one valid pattern\n\n");
    usage();
    exit(FAILURE);
  }

  PreparedPattern* patterns;
  prepare_patterns(parsed_args, &patterns);

  FILE* out = stdout;
  FILE* in;
  if (parsed_args->input_file == NULL){
    in = stdin;
  } else {
    //todo
    in = fopen(parsed_args->input_file, "r");
    if (in == NULL) {
      fprintf(stderr, "Could not open file\n%s\n", parsed_args->input_file);
      exit(FAILURE);
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
          fprintf(out, "\e[1;%dm%s\e[0m", onePattern->col, line );
          matches = TRUE;
          break;
      } else if(regexResult != REG_NOMATCH) {
          //TODO: vidjeti sta radi ova regerror
          regerror(regexResult, &(onePattern->regex), line, len);
          fprintf(stderr, "Regex match failed: %s\n", line);
          exit(FAILURE);
      }
    }

    if (!matches){
      fprintf(out, "%s", line );
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
