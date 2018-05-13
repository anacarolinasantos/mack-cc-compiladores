/*
 ============================================================================
 Name        : MiniVisuAlgSyntaxAnalyzerCompiler.c
 Author      : Ana Carolina dos Santos Silva, TIA: 31525962
 	 	 	   Michelle Faifer dos Santos, TIA: 31581463
 	 	 	   Piera Carolina Marchesini Dutra, TIA: 31581773
 	 	 	   Ricardo Lima, TIA: 31417493
 	 	 	   Victor Zeronian Mattoso, TIA: 31461026
 ============================================================================
 */

//Inclusão das bibliotecas padrão, para limites de inteiros e para lidar com strings
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "readFile.h"

//Constantes de valores máximos
#define MAXTOKENAMOUNT 1000
#define MAXVARIABLES 1000
#define MAXVARIABLELENGTH 30

//Constantes de nomes dos arquivos de entrada e saida
#define ARQENTRADA "entrada.txt"
#define ARQSAIDA "saida.txt"

//Estrutura do token que será impresso no arquivo de saida
typedef struct {
	char* classification;
	char* attribute;
}Token;

//Arquivos de entrada e saida estaticos para serem acessados em qualquer função
static FILE *source_file, *destination_file;

int main() {
	int i = 0;

	openFile(&source_file, ARQENTRADA, "r");
	openFile(&destination_file, ARQSAIDA, "w");

	return 0;
}
