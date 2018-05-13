/*
 ============================================================================
 Name        : MiniVisuAlgSyntaxAnalyzerCompiler.c
 Author      : Ana Carolina dos Santos Silva, TIA: 31525962
 	 	 	   Michelle Faifer dos Santos, TIA: 31581463
 	 	 	   Piera Carolina Marchesini Dutra, TIA: 31581773
 	 	 	   Ricardo Lima, TIA: 31417493
 	 	 	   Victor Zeronian Mattoso, TIA: 31461026
 ============================================================================

Referência Bibliográfica:
	Compiler/syntax analyzer. Rosettacode.
	Disponível em: <https://rosettacode.org/wiki/Compiler/syntax_analyzer>.
	Acesso em: 2 Maio 2018.

	AHO, Alfred V. et al. Compilers Principles, Techniques, & Tools. 2. ed.: Pearson Education,
	2007. p.64--66.
 */

//Inclusão das bibliotecas padrão, para limites de inteiros e para lidar com strings
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "readFile.h"

//Constantes de nomes dos arquivos de entrada e saida
#define ARQTOKENENTRADA "entrada.txt"

//Constantes de valores máximos
#define MAXTOKENAMOUNT 1000

//Estrutura do token que será lido no arquivo de entrada
typedef struct {
	char* classification;
	char* attribute;
}Token;

//Tamanho maximo do buffer de entrada
const size_t bufferSize = MAXTOKENAMOUNT;

//Arquivo de entrada estaticos para ser acessado em qualquer função
static FILE *source_file;

//Inicialização de variaveis estáticas que serão utilizadas em diversas funções do código
static int allTokenSize = 0, forward = 0;

//Lista de todos os tokens que foram lidos no arquivo de entrada
Token *allTokens;

//Token que está sendo analisado
Token currentToken;

void syntaxError(char* expected, char* returned) {
	printf("Erro: esperava %s e veio %s", expected, returned);
	exit(1);
}

Token getNextToken() {
	Token t;

	if (forward >= allTokenSize) {
		return t;
	}

	forward++;
	printf("token: %s %s\n", allTokens[forward-1].classification, allTokens[forward-1].attribute);
	return allTokens[forward-1];
}

int classificationMatch(char* classificationExpected) {
	Token t = getNextToken();

	if (strcmp(classificationExpected, t.classification) == 0) {
		currentToken = t;
		return 1;
	}

	forward--;
	return 0;
}

int tokenMatch(char* classificationExpected, char* attributeExpected) {
	Token t = getNextToken();

	if (strcmp(classificationExpected, t.classification) == 0) {
		if (strcmp(attributeExpected, t.attribute) == 0) {
			currentToken = t;
			return 1;
		}
	}

	forward--;
	return 0;
}

/* Regra de sintaxe de tipos de dados:
  inteiro | logico */
void dataType() {
	if ((strcmp(currentToken.classification, "inteiro") != 0)
			&& (strcmp(currentToken.classification, "logico") != 0)) {
		syntaxError("inteiro ou logico", currentToken.classification);
	}
}

// <id> | <id>,<listaVar>
void variableListSyntax() {

	if(strcmp(currentToken.classification, "id") == 0) {
		if (tokenMatch("ponctuation", "COMMA")) {
			currentToken = getNextToken();
			variableListSyntax();
		}
	} else {
		syntaxError("id", currentToken.classification);
	}
}

// <listaVar> : <tipo>
void variableDeclarationSyntax() {
	variableListSyntax();
	printf("token esta aqui: %s %s\n", currentToken.classification, currentToken.attribute);


	currentToken = getNextToken();

	if(strcmp(currentToken.attribute, "COL") == 0) {
		currentToken = getNextToken();
		dataType();
	} else {
		syntaxError("COL", currentToken.attribute);
	}
}

// <decVar> | <decVar><seqDecVar>
void sequenceVariableList() {
	variableDeclarationSyntax();

	printf("declaracao ok!\n");

	printf("token este: %s", currentToken.classification);

	if (classificationMatch("id") == 1) {

		printf("mais uma declaracao!\n");
		sequenceVariableList();
	}
}

/* Regra de sintaxe do começo do programa:
  algoritmo <declaracao-variaveis> inicio <bloco-comandos> fimalgoritmo */
void programSyntax() {
	currentToken.classification = (char *) calloc (30, sizeof(char));
	currentToken.attribute = (char *) calloc (30, sizeof(char));

	currentToken = getNextToken();
	if(strcmp(currentToken.classification, "algoritmo") == 0) {

		printf("algoritmo ok!\n");

		currentToken = getNextToken();
		if(strcmp(currentToken.classification, "texto") == 0) {

			printf("nome do algoritmo ok!\n");

			currentToken = getNextToken();
			if (strcmp(currentToken.classification, "id") == 0) {
				sequenceVariableList();
				printf("sequencia ok!\n");
				printf("%s \n\n", currentToken.classification);

				currentToken = getNextToken();
			}

			if(strcmp(currentToken.classification, "inicio") == 0) {

				printf("inicio ok!\n");

				//TODO: chamar funcao de regra de bloco
				if(strcmp(currentToken.classification, "fimalgoritmo") == 0) {
					//TODO: chamar funcao de sucesso de analise sintatica
				} else {
					//TODO: colocar erro sintatico
				}
			} else {
				syntaxError("inicio", currentToken.classification);
			}
		} else {
			syntaxError("texto", currentToken.classification);
		}
	} else {
		syntaxError("algoritmo", currentToken.classification);
	}
}

void readTokenFile() {
	char* buffer = malloc(bufferSize);
	int i = 0;
	allTokens = malloc(MAXTOKENAMOUNT * sizeof(Token));
	Token *token = (Token *) calloc(1,sizeof(Token));
	token->classification = (char *) calloc (30, sizeof(char));
	token->attribute = (char *) calloc (30, sizeof(char));

	while (fgets(buffer, bufferSize, source_file) != NULL) {
		allTokens[i].classification = (char *) calloc (30, sizeof(char));
		allTokens[i].attribute = (char *) calloc (30, sizeof(char));

		char* word;
		/* Quebra o buffer em palavras separadas por espaços, além de ignorar
			  as scape sequences de quebra de linha (\n) e do enter (\r) */
		word = strtok(buffer," ,<>\n\r");
		strcpy(token->classification, word);

		while (word != NULL) {
			strcpy(token->attribute, word);
			word = strtok(NULL, " ,<>\n\r");
		}

		strcpy(allTokens[i].classification, token->classification);
		strcpy(allTokens[i].attribute, token->attribute);

		i++;
	}

	allTokenSize = i;
	free(token);
}

