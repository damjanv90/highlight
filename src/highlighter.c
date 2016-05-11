#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#include "settings.h"

void usage(){
  fprintf(stderr,
                  "\nhighlight v%s\n\n"

                  "Usage: highlight <pattern1>... <infile>\n"
                  "Example: highlight ERROR:red WARN:yellow debug.log\n\n"

                  "<pattern> - <regex>:<color>\n"
                  " <regex> - a regular expression\n"
                  " <color> - red, yellow, green, blue, white\n"
                  "<infile>  - a path to an input text file\n\n",
                  VERSION
          );
}

typedef enum {red=31} color;

typedef struct pattern {
  regex_t regex;
  int color;
  struct pattern* next;
}Pattern;


Pattern* Pattern_new(char* regexStr, int color){

  Pattern* newPattern = (Pattern*)malloc(sizeof(Pattern*));

  /* Compile regular expression */
  int regexCompilationError;
  regexCompilationError = regcomp(&(newPattern->regex), regexStr, 0);
  if (regexCompilationError) {
      fprintf(stderr, "Could not compile regex\n%s\n", regexStr);
      exit(1);
  }
  newPattern->color=color;

  return newPattern;
}


void parsePatterns(char** patternStrings, int patternCnt, Pattern** patterns){
  Pattern* prev = NULL;

  int i;
  for (i=0; i< patternCnt; i++){

    char* regexStr=strtok(patternStrings[i], ":");
    int color = atoi(strtok(NULL, ":"));

    Pattern* newPattern = Pattern_new(regexStr, color);

    if (prev != NULL){
      prev->next = newPattern;
      prev = newPattern;
    } else {
      // first
      prev = newPattern;
      *patterns = newPattern;
    }
  }
}

int main(int argc, char** argv){

  if (argc < 2){
    usage();
    exit(1);
  }

  FILE* out = stdout;
  FILE* in;
  if (argc == 2){
    in = stdin;
  } else {
    //todo
    in = fopen(argv[argc - 1], "r");
    if (in == NULL) {
      fprintf(stderr, "Could not open file\n%s\n", argv[argc - 1]);
      exit(1);
    }
  }

  Pattern* patterns;
  parsePatterns(&(argv[1]), argc==2 ? 1 : argc-2, &patterns);

  char * line = NULL;
  size_t len = 0;
  int hasInput;

  hasInput = getline(&line, &len, in) != -1;

  while (hasInput){

    int matches=0;
    Pattern* onePattern;
    for (onePattern = patterns; onePattern; onePattern = onePattern->next){
      /* Execute regular expression */
      int regexResult;
      regexResult = regexec(&(onePattern->regex), line, 0, NULL, 0);
      if (!regexResult) {
          fprintf(out, "\e[1;%dm%s\e[0m", onePattern->color, line );
          matches = 1;
          break;
      }else if(regexResult != REG_NOMATCH) {
          regerror(regexResult, &(onePattern->regex), line, len);
          fprintf(stderr, "Regex match failed: %s\n", line);
          exit(1);
      }
    }

    if (!matches){
      fprintf(out, "%s", line );
    }

    hasInput = getline(&line, &len, in) != -1;
  }

  /* Free memory allocated to the pattern buffer by regcomp() */
  Pattern* p;
  while (p){
    regfree(&(p->regex));
    Pattern* tmp = p;
    p = p->next;
    free(tmp);
  }

  if (line){
    free (line);
  }

  return 0;
}
