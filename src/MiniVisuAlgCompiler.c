/*
 ============================================================================
 Name        : MiniVisuAlgCompiler.c
 Author      : Ana Carolina dos Santos Silva
 ============================================================================

 https://rosettacode.org/wiki/Compiler/lexical_analyzer
 VER ESTE ANALISADOR LÉXICO!!!!!!!!

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUFFERLENGTH 1000
#define MAXVARIABLES 1000

typedef struct {
	char* classification;
	char* attribute;
}Token;

static FILE *source_file, *destination_file;

const size_t bufferSize = MAXBUFFERLENGTH;

static int begin = 0, forward = 0, line = 0, indexVariables = 0;

static char *buffer;

static int isStringLiteral = 0;

static char *variables[MAXVARIABLES];

void openFile(FILE **file, char fileName[], char type[]) {
	*file = fopen(fileName, type);

	if (!*file) {
		printf("Unable to open file, please enter a valid file name\n");
		exit(1);
	}
}

char getNextChar(){
	forward++;
	if(forward < MAXBUFFERLENGTH) {
		return buffer[forward];
	}

	return '\n';
}

Token* getToken() {
	Token *token = (Token *) calloc(255,sizeof(Token));
	char c, nextC, result[12];
	int reachSpace = 0, i, foundId = 0;
	char* word;

	c = buffer[forward];

	for(i = 0; i < MAXVARIABLES; i++) {
		variables[i] = "";
	}


	if (c == ' ') {
		begin++;
		forward = begin;

		c = buffer[forward];
	} else if (c == '\n') {
		return NULL;
	}

	printf("c %c\n", c);

	if (isStringLiteral) {
		if (c == '"') {
			isStringLiteral = 0;
			token->classification = "text";
			token->attribute = "";
		}
	} else {
		switch (c) {
		case '(':
			token->classification = "ponctuation";
			token->attribute = "OP";
			break;
		case ')':
			token->classification = "ponctuation";
			token->attribute = "CP";
			break;
		case ',':
			token->classification = "ponctuation";
			token->attribute = "COMMA";
			break;
		case '"':
			isStringLiteral = 1;
			break;
		case ':':
			token->classification = "ponctuation";
			token->attribute = "COL";
			break;
		case '/':
			if(getNextChar() == '/'){
				forward = MAXBUFFERLENGTH;
			} else {
				//TODO: Erooooooou
			}
			break;
		case '+':
			token->classification = "arith-op";
			token->attribute = "PLUS";
			break;
		case '*':
			token->classification = "arith-op";
			token->attribute = "MULT";
			break;
		case '\\':
			token->classification = "arith-op";
			token->attribute = "INTD";
			break;
		case '-':
			token->classification = "arith-op";
			token->attribute = "MIN";
			break;
		case '<':
			nextC = getNextChar();

			if (nextC == '=') {
				token->classification = "rel-op";
				token->attribute = "LE";
			} else if (nextC == '-') {
				token->classification = "attribution";
				token->attribute = "";
			} else if (nextC == '>') {
				token->classification = "rel-op";
				token->attribute = "NE";
			} else {
				token->classification = "rel-op";
				token->attribute = "LT";
				forward--;
			}
			break;
		case '>':
			nextC = getNextChar();

			if (nextC == '=') {
				token->classification = "rel-op";
				token->attribute = "GE";
			} else {
				token->classification = "rel-op";
				token->attribute = "GT";
				forward--;
			}
			break;
		case '=':
			token->classification = "rel-op";
			token->attribute = "EQ";
			break;
		default:
//			if (c == ' ' || c == '\n') {
//				return NULL;
//			}
			reachSpace = 0;
			word = &buffer[begin];
			while(!reachSpace) {
				word[forward - begin] = buffer[forward];

				if (getNextChar() == ' ' || getNextChar() == '\n') {
					word[forward - begin] = '\0';
					reachSpace = 1;
				}
			}

			if (strcmp(word, "algoritmo") == 0) {
				token->classification = "algoritmo";
				token->attribute = "";
			} else if (strcmp(word, "var") == 0) {
				token->classification = "var";
				token->attribute = "";
			} else if (strcmp(word, "inteiro") == 0) {
				token->classification = "inteiro";
				token->attribute = "";
			} else if (strcmp(word, "inicio") == 0) {
				token->classification = "inicio";
				token->attribute = "";
			} else if (strcmp(word, "fimalgoritmo") == 0) {
				token->classification = "fimalgoritmo";
				token->attribute = "";
			} else if (strcmp(word, "logico") == 0) {
				token->classification = "logico";
				token->attribute = "";
			} else if (strcmp(word, "verdadeiro") == 0) {
				token->classification = "verdadeiro";
				token->attribute = "";
			} else if (strcmp(word, "falso") == 0) {
				token->classification = "falso";
				token->attribute = "";
			} else if (strcmp(word, "mod") == 0) {
				token->classification = "arith-op";
				token->attribute = "MOD";
			} else if (strcmp(word, "exp") == 0) {
				token->classification = "arith-op";
				token->attribute = "EXP";
			} else if (strcmp(word, "leia") == 0) {
				token->classification = "leia";
				token->attribute = "";
			} else if (strcmp(word, "escreva") == 0) {
				token->classification = "escreva";
				token->attribute = "";
			} else if (strcmp(word, "se") == 0) {
				token->classification = "se";
				token->attribute = "";
			} else if (strcmp(word, "entao") == 0) {
				token->classification = "entao";
				token->attribute = "";
			} else if (strcmp(word, "senao") == 0) {
				token->classification = "senao";
				token->attribute = "";
			} else if (strcmp(word, "fimse") == 0) {
				token->classification = "fimse";
				token->attribute = "";
			} else if (strcmp(word, "para") == 0) {
				token->classification = "para";
				token->attribute = "";
			} else if (strcmp(word, "de") == 0) {
				token->classification = "de";
				token->attribute = "";
			} else if (strcmp(word, "ate") == 0) {
				token->classification = "ate";
				token->attribute = "";
			} else if (strcmp(word, "fimpara") == 0) {
				token->classification = "fimpara";
				token->attribute = "";
			} else if (strcmp(word, "enquanto") == 0) {
				token->classification = "enquanto";
				token->attribute = "";
			} else if (strcmp(word, "faca") == 0) {
				token->classification = "faca";
				token->attribute = "";
			} else if (strcmp(word, "fimenquanto") == 0) {
				token->classification = "fimenquanto";
				token->attribute = "";
			} else {
				printf("VARIAAAAAAAAAAAVEL");
				foundId = 0;
				for (i = 0; i < MAXVARIABLES; i++) {
					if (strcmp(word, variables[i]) == 0) {
						token->classification = "id";
					    sprintf(result, "%d", i);
						token->attribute = result;
						foundId = 1;
						break;
					}
				}
				if (!foundId) {
					variables[indexVariables] = word;
					token->classification = "id";
					sprintf(result, "%d", indexVariables);
					token->attribute = result;
					indexVariables++;
				}
			}

			printf("PALAVR: %s \n", word);


			//TODO: tratar variaveis (armazenar numa matriz/vetor), palavra reservada e numeros
			printf("palavra ou numero ");
			break;
		}
	}

	forward++;

	return token;
}

void readFile() {
	buffer = malloc(bufferSize);
	Token *t = NULL;

	while (fgets(buffer, bufferSize, source_file) != NULL) {
		begin = 0;
		forward = 0;

		printf("buffer = %s", buffer);

		while (buffer[forward] != '\n' && forward < MAXBUFFERLENGTH) {

			t = getToken();

			if (t != NULL && t->classification != NULL) {
				if (t->attribute == NULL) {
					fprintf(destination_file, "<%s>\n", t->classification);
				} else {
					fprintf(destination_file, "<%s, %s>\n", t->classification, t->attribute);
				}
			}
			printf("%s %s\n", t->classification, t->attribute);

			begin = forward;
		}
		line++;
	}
}



int main() {

	openFile(&source_file, "entrada.txt", "r");
	openFile(&destination_file, "saida.txt", "w");

	readFile();

	return 0;
}
