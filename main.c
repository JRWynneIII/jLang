#include<stdio.h>
#include<stdlib.h>
#include "jlang.tab.hpp"
#include "tree.h"

extern FILE* yyin;

int main(int argc, char*argv[])
{
  if (argc >1)
    yyin = fopen(argv[1],"r");
  yyparse();
  return EXIT_SUCCESS;
}
