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

#define MAXBUFFERLENGTH 1000

typedef struct {
	char* classification;
	char* attribute;
}Token;

static FILE *source_file, *destination_file;

const size_t bufferSize = MAXBUFFERLENGTH;

static int begin = 0, forward = 0;

static char *buffer;

static int isStringLiteral = 0;

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
	char c;
	char nextC;

	c = buffer[forward];

	if (c == ' ') {
		begin++;
		forward = begin;

		c = buffer[forward];
	}

	printf("caracter %c\n", c);

	if (isStringLiteral) {
		if (c == '"') {
			token->classification = "text";
			token->attribute = "";
			isStringLiteral = 0;
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
			if (getNextChar() == '=') {
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

			printf("%s %s\n", t->classification, t->attribute);

			begin = forward;
		}
	}
}



int main() {

	openFile(&source_file, "entrada.txt", "r");
	openFile(&destination_file, "saida.txt", "w");

	readFile();

	return 0;
}
