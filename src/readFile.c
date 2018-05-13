/*
 ============================================================================
 Name        : MiniVisuAlgLexicalAnalyzerCompiler.c
 Author      : Ana Carolina dos Santos Silva, TIA: 31525962
 	 	 	   Michelle Faifer dos Santos, TIA: 31581463
 	 	 	   Piera Carolina Marchesini Dutra, TIA: 31581773
 	 	 	   Ricardo Lima, TIA: 31417493
 	 	 	   Victor Zeronian Mattoso, TIA: 31461026
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "readFile.h"

void openFile(FILE **file, char fileName[], char type[]) {
	*file = fopen(fileName, type);

	if (!*file) {
		printf("Unable to open file, please enter a valid file name\n");
		exit(1);
	}
}
