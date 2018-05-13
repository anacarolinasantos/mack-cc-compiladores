/*
 * lexicalAnalyzer.h
 *
 *  Created on: May 13, 2018
 *      Author: anacarolinasantos
 */

#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

/*
 ============================================================================
 Name        : MiniVisuAlgLexicalAnalyzerCompiler.c
 Author      : Ana Carolina dos Santos Silva, TIA: 31525962
 	 	 	   Michelle Faifer dos Santos, TIA: 31581463
 	 	 	   Piera Carolina Marchesini Dutra, TIA: 31581773
 	 	 	   Ricardo Lima, TIA: 31417493
 	 	 	   Victor Zeronian Mattoso, TIA: 31461026
 ============================================================================

Referência Bibliográfica:
	Compiler/lexical analyzer. Rosettacode.
	Disponível em: <https://rosettacode.org/wiki/Compiler/lexical_analyzer>.
	Acesso em: 5 Abril 2018.
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

//Tamanho maximo do buffer de entrada
const size_t bufferSize = MAXTOKENAMOUNT;

//Inicialização de variaveis estáticas que serão utilizadas em diversas funções do código
static int forward = 0, line = 1, indexVariables = 0;

//Declaração do buffer de entrada
static char *buffer;

//Verifica se está lendo uma string, pois deve-se ignorar tudo até as próximas aspas
static int isStringLiteral = 0;

//Vetor de variaveis que já foram instanciadas no código
//Cada variável pode ter no máximo 30 caracteres de acordo com as regras do VisuAlg
static char variables[MAXVARIABLES][MAXVARIABLELENGTH];

//Função que retorna qual o próximo caracter no buffer de entrada
Token* getNumberVariableOrReservedWord(char* splitString){
	Token *token = (Token *) calloc(1,sizeof(Token));
	int num, i = 0, isANumber = 0, isAVariable = 0;

	char * idNumber = (char *) calloc (31, sizeof(char));
	char * checkNumber = (char *) calloc (31, sizeof(char));

	token->classification = (char *) calloc (30, sizeof(char));
	token->attribute = (char *) calloc (30, sizeof(char));

	//Primeiramente verifica se é um número, caso seja, já retorna o token
	num = atoi(splitString);
	if (num != 0 || splitString[0] == '0') {
		/* Função atoi retorna se é um inteiro, retornando um numero diferente
		  de 0.
		  Porém, caso exista um numero e caracteres depois, por exemplo
		  "123junk", a função ignora os caracteres e retorna apenas o
		  inteiro da frente, porém não queremos reconhecer este tipo de dado
		  (com inteiros e strings misturados). Por este motivo, neste for
		  abaixo verificamos se todos os caracteres são números de acordo
		  com a tabela ASCII*/
		sprintf(checkNumber, "%d", num);

		if (strcmp(splitString, checkNumber) != 0) {
			fprintf(destination_file, "Error %s at line %d integer out of bounds.", splitString, line);
			printf("Error %s at line %d integer out of bounds.", splitString, line);
			exit(1);
		}

		free(checkNumber);

		for (i = 0; i < strlen(splitString); i++) {
			/* Caso o caracter verificado seja 13 (que é o enter na tabela
			  ASCII) ou 10 (que é o \n, ou seja, quebra de linha), então nem
			  verifica, pois não interfere na validação do número*/
			if (splitString[i] != 13 && splitString[i] != 10) {
				if (i == 0) {
					/* Na tabela ASCII 45 significa - e os números vão de 48
					  até 57*/
					if (splitString[i] != 45 && (splitString[i] < 48 || splitString[i] > 57)) {
						isANumber = 1;
						break;
					}
				} else {
					if (splitString[i] < 48 || splitString[i] > 57)  {
						isANumber = 1;
						break;
					}
				}
			}
		}

		if (isANumber == 0) {
			strcpy(token->classification, "number");
			strcpy(token->attribute, splitString);
		} else {
			fprintf(destination_file, "Error symbol %s at line %d could not be resolved", splitString, line);
			printf("Error symbol %s at line %d could not be resolved", splitString, line);
			exit(1);
		}
		return token;
	}

	//Verifica se é alguma das palavras reservadas
	if (strcmp(splitString, "algoritmo") == 0) {
		strcpy(token->classification, "algoritmo");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "ate") == 0) {
		strcpy(token->classification, "ate");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "de") == 0) {
		strcpy(token->classification, "de");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "e") == 0) {
		strcpy(token->classification, "rel-op");
		strcpy(token->attribute, "AND");
	} else if (strcmp(splitString, "enquanto") == 0) {
		strcpy(token->classification, "enquanto");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "entao") == 0) {
		strcpy(token->classification, "entao");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "escreva") == 0) {
		strcpy(token->classification, "escreva");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "exp") == 0) {
		strcpy(token->classification, "expression-op");
		strcpy(token->attribute, "EXP");
	} else if (strcmp(splitString, "faca") == 0) {
		strcpy(token->classification, "faca");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "falso") == 0) {
		strcpy(token->classification, "falso");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "fimalgoritmo") == 0) {
		strcpy(token->classification, "fimalgoritmo");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "fimenquanto") == 0) {
		strcpy(token->classification, "fimenquanto");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "fimpara") == 0) {
		strcpy(token->classification, "fimpara");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "fimse") == 0) {
		strcpy(token->classification, "fimse");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "inicio") == 0) {
		strcpy(token->classification, "inicio");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "inteiro") == 0) {
		strcpy(token->classification, "inteiro");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "leia") == 0) {
		strcpy(token->classification, "leia");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "logico") == 0) {
		strcpy(token->classification, "logico");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "mod") == 0) {
		strcpy(token->classification, "arith-op");
		strcpy(token->attribute, "MOD");
	} else if (strcmp(splitString, "ou") == 0) {
		strcpy(token->classification, "rel-op");
		strcpy(token->attribute, "OR");
	} else if (strcmp(splitString, "para") == 0) {
		strcpy(token->classification, "para");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "passo") == 0) {
		strcpy(token->classification, "passo");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "se") == 0) {
		strcpy(token->classification, "se");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "senao") == 0) {
		strcpy(token->classification, "senao");
		strcpy(token->attribute, "\0");
	} else if (strcmp(splitString, "verdadeiro") == 0) {
		strcpy(token->classification, "verdadeiro");
		strcpy(token->attribute, "\0");

		//Caso não seja nenhuma das palavras reservadas, então é uma variável
	} else {
		/* Primeiro verifica se todos os caracteres são aceitos e se é
		  realmente uma variável*/
		if (strlen(splitString) > 30) {
			fprintf(destination_file, "Error %s at line %d variables names are too long (maximum length is 30 characters).", splitString, line);
			printf("Error %s at line %d variables names are too long (maximum length is 30 characters).", splitString, line);
			exit(1);
		}

		for (i = 0; i < strlen(splitString); i++) {
			/* Caracteres em letra minúscula vão de 97 até 122, numeros
			  vão de 48 até 57 e underline que é o 95 na tabela ASCII,
			  portanto, caso seja algo diferente disso, não é uma variavel
			  válida, então não deve ser reconhecida!*/
			if ((splitString[i] < 97 || splitString[i] > 122) && (splitString[i] < 48 || splitString[i] > 57) && splitString[i] != 95) {
				isAVariable = 1;
				break;
			}
		}

		if (isAVariable == 0) {
			/* Verifica na matriz variables se a variavel já foi declarada
			  anteriormente, caso tenha sido, utiliza o mesmo numero do id para
			  o atributo do token, caso não tenha sido, insere na matriz e
			  coloca o próximo id*/
			for (i = 0; i < MAXVARIABLES; i++) {
				if (i <= indexVariables) {
					if (strcmp(variables[i], splitString) == 0) {
						strcpy(token->classification, "id");

						/* Função sprintf salva variaveis em uma string! Portanto foi feito o cast
						  do index da variavel para uma string, pois o atributo do token é uma
						  string e não um inteiro!*/
						sprintf(idNumber, "%d", i);

						strcpy(token->attribute, idNumber);
						break;
					}
				} else {
					indexVariables++;
					strcpy(variables[i], splitString);
					strcpy(token->classification, "id");

					sprintf(idNumber, "%d", i);

					strcpy(token->attribute, idNumber);
					break;
				}
			}

			free(idNumber);
		} else {
			fprintf(destination_file, "Error symble %s at line %d could not be resolved", splitString, line);
			printf("Error symble %s at line %d could not be resolved", splitString, line);
			exit(1);
		}


	}

	return token;
}

//Função que retorna o token da proxima string que foi lida do buffer de entrada
Token* getToken(char* splitString) {
	Token *token = (Token *) calloc(1,sizeof(Token));

	token->classification = (char *) calloc (30, sizeof(char));
	token->attribute = (char *) calloc (30, sizeof(char));

	/* Caso esteja lendo uma string (esta variavel foi settada como verdadeira
	  quando leu as primeiras aspas), então só vai parar de ler uma string quando
	  encontrar outras aspas, caso não tenha encontrado, vai retornar o token vazio
	 */
	if (isStringLiteral == 1) {
		if (splitString[0] == '"') {
			strcpy(token->classification, "text");
			strcpy(token->attribute, "\0");
			isStringLiteral = 0;
		}
		return token;
	}

	//Verifica se é um parênteses aberto
	if (strcmp(splitString, "(") == 0) {
		strcpy(token->classification, "ponctuation");
		strcpy(token->attribute, "OP");

		//Caso não seja, verifica se é um parênteses fechado
	} else if (strcmp(splitString, ")") == 0) {
		strcpy(token->classification, "ponctuation");
		strcpy(token->attribute, "CP");

		//Verifica se é uma vírgula
	} else if (strcmp(splitString, ",") == 0) {
		strcpy(token->classification, "ponctuation");
		strcpy(token->attribute, "COMMA");

		//Verifica se é uma abertura de aspas, caso seja, setta a variavél verificada acima como verdadeira
	} else if (splitString[0] == '\"') {
		isStringLiteral = 1;

		//Verifica se são dois pontos (usado na declaração de variáveis)
	} else if (strcmp(splitString, ":") == 0) {
		strcpy(token->classification, "ponctuation");
		strcpy(token->attribute, "COL");

		//Verifica se é um comentário, caso seja, não retorna nenhum token e pula para a próxima linha
	} else if (strcmp(splitString, "//") == 0) {
		forward = MAXTOKENAMOUNT;

		//Verifica se é operador de soma
	} else if (strcmp(splitString, "+") == 0) {
		strcpy(token->classification, "arith-op");
		strcpy(token->attribute, "PLUS");

		//Verifica se é operador de menos
	} else if (strcmp(splitString, "-") == 0) {
		strcpy(token->classification, "arith-op");
		strcpy(token->attribute, "MIN");

		//Verifica se é uma multiplicação
	} else if (strcmp(splitString, "*") == 0) {
		token->classification = "arith-op";
		token->attribute = "MULT";

		//Verifica se é uma divisão inteira
	} else if (strcmp(splitString, "\\") == 0) {
		strcpy(token->classification, "arith-op");
		strcpy(token->attribute, "INTD");

		//Verifica se é menor
	} else if (strcmp(splitString, "<") == 0) {
		strcpy(token->classification, "rel-op");
		strcpy(token->attribute, "LT");

		//Verifica se é menor ou igual
	} else if (strcmp(splitString, "<=") == 0) {
		strcpy(token->classification, "rel-op");
		strcpy(token->attribute, "LE");

		//Verifica se é uma atribuição de variável
	} else if (strcmp(splitString, "<-") == 0) {
		strcpy(token->classification, "attribution");
		strcpy(token->attribute, "\0");

		//Verifica se é diferente
	} else if (strcmp(splitString, "<>") == 0) {
		strcpy(token->classification, "rel-op");
		strcpy(token->attribute, "NE");

		//Verifica se é maior
	} else if (strcmp(splitString, ">") == 0) {
		strcpy(token->classification, "rel-op");
		strcpy(token->attribute, "GT");

		//Verifica se é menor ou igual
	} else if (strcmp(splitString, ">=") == 0) {
		strcpy(token->classification, "rel-op");
		strcpy(token->attribute, "GE");

		//Verifica se é igual
	} else if (strcmp(splitString, "=") == 0) {
		strcpy(token->classification, "rel-op");
		strcpy(token->attribute, "EQ");

		/* Caso não seja nenhuma dessas, verifica se é numero, palavra reservada
	  ou variável*/
	} else {
		token = getNumberVariableOrReservedWord(splitString);
	}

	return token;
}

void readProgramFile() {
	int i = 0;

	for (i = 0; i < MAXVARIABLES; i++) {
		variables[i][0] = '\0';
	}

	buffer = malloc(bufferSize);
	Token *t = NULL;

	while (fgets(buffer, bufferSize, source_file) != NULL) {
		forward = 0;

		char* word;
		/* Quebra o buffer em palavras separadas por espaços, além de ignorar
		  as scape sequences de quebra de linha (\n) e do enter (\r) */
		word = strtok(buffer," \n\r");

		while (word != NULL && forward < MAXTOKENAMOUNT) {
			t = getToken(word);
			if (strcmp(t->classification, "") != 0) {
				if (strcmp(t->attribute, "\0") == 0) {
					fprintf(destination_file, "<%s>\n", t->classification);
				} else {
					fprintf(destination_file, "<%s, %s>\n", t->classification, t->attribute);
				}
			}

			word = strtok(NULL, " \n\r");
		}
		line++;
	}

	free(t);
}



int main() {
	int i = 0;

	openFile(&source_file, ARQENTRADA, "r");
	openFile(&destination_file, ARQSAIDA, "w");

	for (i = 0; i < MAXVARIABLES; i++) {
		variables[i][0] = '\0';
	}

	readFile();

	printf("Lexical analyzer finished on file %s", ARQSAIDA);

	return 0;
}


#endif /* LEXICALANALYZER_H_ */