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

//Inclusão das bibliotecas padrão e para limites de inteiros e para lidar com strings
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

//Constantes de valores maximos
#define MAXBUFFERLENGTH 1000
#define MAXVARIABLES 1000
#define MAXVARIABLELENGTH 30
//INT_MAX

//Constantes de nomes dos arquivos de entrada e saida
#define ARQENTRADA "teste.txt"
#define ARQSAIDA "saida.txt"

//Estrutura do token que será impresso no arquivo de saida
typedef struct {
	char* classification;
	char* attribute;
}Token;

//Arquivos de entrada e saida estaticos para serem acessados em qualquer função
static FILE *source_file, *destination_file;

//Tamanho maximo do buffer de entrada
const size_t bufferSize = MAXBUFFERLENGTH;

//Inicialização de variaveis estáticas que serão utilizadas em diversas funções do código
static int forward = 0, line = 0, indexVariables = 0;

//Declaração do buffer de entrada
static char *buffer;

//Verifica se está lendo uma string, pois deve-se ignorar tudo até as próximas aspas
static int isStringLiteral = 0;

//Vetor de variaveis que já foram instanciadas no código
//Cada variável pode ter no máximo 30 caracteres de acordo com as regras do VisuAlg
static char variables[MAXVARIABLES][MAXVARIABLELENGTH];

//Função que abre os arquivos de acordo com o modo de acesso passado na chamada da função
void openFile(FILE **file, char fileName[], char type[]) {
	*file = fopen(fileName, type);

	if (!*file) {
		printf("Unable to open file, please enter a valid file name\n");
		exit(1);
	}
}

//Função que retorna qual o próximo caracter no buffer de entrada
Token* getNumberVariableOrReservedWord(char* splitString){
	Token *token = (Token *) calloc(255,sizeof(Token));
	int num, i = 0, isANumber = 0, isAVariable = 0;

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
			token->classification = "number";
			token->attribute = splitString;

		} else {
			//TODO: retorna um erro (temos que ver como fazer isso!)
		}
		return token;
	}

	//Verifica se é alguma das palavras reservadas
	if (strcmp(splitString, "algoritmo") == 0) {
		token->classification = "algoritmo";
		token->attribute = NULL;
	} else if (strcmp(splitString, "ate") == 0) {
		token->classification = "ate";
		token->attribute = NULL;
	} else if (strcmp(splitString, "de") == 0) {
		token->classification = "de";
		token->attribute = NULL;
	} else if (strcmp(splitString, "e") == 0) {
		token->classification = "rel-op";
		token->attribute = "AND";
	} else if (strcmp(splitString, "enquanto") == 0) {
		token->classification = "enquanto";
		token->attribute = NULL;
	} else if (strcmp(splitString, "entao") == 0) {
		token->classification = "entao";
		token->attribute = NULL;
	} else if (strcmp(splitString, "escreva") == 0) {
		token->classification = "escreva";
		token->attribute = NULL;
	} else if (strcmp(splitString, "exp") == 0) {
		token->classification = "expression-op";
		token->attribute = "EXP";
	} else if (strcmp(splitString, "faca") == 0) {
		token->classification = "faca";
		token->attribute = NULL;
	} else if (strcmp(splitString, "falso") == 0) {
		token->classification = "falso";
		token->attribute = NULL;
	} else if (strcmp(splitString, "fimalgoritmo") == 0) {
		token->classification = "fimalgoritmo";
		token->attribute = NULL;
	} else if (strcmp(splitString, "fimenquanto") == 0) {
		token->classification = "fimenquanto";
		token->attribute = NULL;
	} else if (strcmp(splitString, "fimpara") == 0) {
		token->classification = "fimpara";
		token->attribute = NULL;
	} else if (strcmp(splitString, "fimse") == 0) {
		token->classification = "fimse";
		token->attribute = NULL;
	} else if (strcmp(splitString, "inicio") == 0) {
		token->classification = "inicio";
		token->attribute = NULL;
	} else if (strcmp(splitString, "inteiro") == 0) {
		token->classification = "inteiro";
		token->attribute = NULL;
	} else if (strcmp(splitString, "leia") == 0) {
		token->classification = "leia";
		token->attribute = NULL;
	} else if (strcmp(splitString, "logico") == 0) {
		token->classification = "logico";
		token->attribute = NULL;
	} else if (strcmp(splitString, "mod") == 0) {
		token->classification = "arith-op";
		token->attribute = "MOD";
	} else if (strcmp(splitString, "ou") == 0) {
		token->classification = "rel-op";
		token->attribute = "OR";
	} else if (strcmp(splitString, "para") == 0) {
		token->classification = "para";
		token->attribute = NULL;
	} else if (strcmp(splitString, "passo") == 0) {
		token->classification = "passo";
		token->attribute = NULL;
	} else if (strcmp(splitString, "se") == 0) {
		token->classification = "se";
		token->attribute = NULL;
	} else if (strcmp(splitString, "senao") == 0) {
		token->classification = "senao";
		token->attribute = NULL;
	} else if (strcmp(splitString, "var") == 0) {
		token->classification = "var";
		token->attribute = NULL;
	} else if (strcmp(splitString, "verdadeiro") == 0) {
		token->classification = "verdadeiro";
		token->attribute = NULL;

	//Caso não seja nenhuma das palavras reservadas, então é uma variável
	} else {
		/* Primeiro verifica se todos os caracteres são aceitos e se é
		  realmente uma variável*/
		for (i = 0; i < strlen(splitString); i++) {
			/* Caracteres vão de XXXX até XXXX, numeros vão de XXXX até XXXX
			  e underline que é o XXXX na tabela ASCII, portanto, caso seja
			  algo diferente disso, não é uma variavel válida, então não deve
			  ser reconhecida!*/
			//TODO: fazer esta comparção!!
		}

		if (isAVariable == 0) {
			/* Verifica na matriz variables se a variavel já foi declarada
			  anteriormente, caso tenha sido, utiliza o mesmo numero do id para
			  o atributo do token, caso não tenha sido, insere na matriz e
			  coloca o próximo id*/
			for (i = 0; i < MAXVARIABLES; i++) {
				if (i <= indexVariables) {
					if (strcmp(variables[i], splitString) == 0) {
						token->classification = "id";
						token->attribute = "x";
						break;
					}
				} else {
					indexVariables++;
					strcpy(variables[i], splitString);
					token->classification = "id";

					//TODO: fazer a transformação de int para string para salvar no attribute

					token->attribute = "x";
					break;
				}
			}
		} else {
			//TODO: retorna um erro de que não é uma variavel!
		}


	}

	return token;
}

//Função que retorna o token da proxima string que foi lida do buffer de entrada
Token* getToken(char* splitString) {
	Token *token = (Token *) calloc(255,sizeof(Token));

	/* Caso esteja lendo uma string (esta variavel foi settada como verdadeira
	  quando leu as primeiras aspas), então só vai parar de ler uma string quando
	  encontrar outras aspas, caso não tenha encontrado, vai retornar o token vazio
	*/
	if (isStringLiteral == 1) {
		printf("É uma string\n");

		if (splitString[0] == '"') {
			token->classification = "text";
			token->attribute = NULL;
			isStringLiteral = 0;
		}
		return token;
	}

	//Verifica se é um parênteses aberto
	if (strcmp(splitString, "(") == 0) {
		token->classification = "ponctuation";
		token->attribute = "OP";

	//Caso não seja, verifica se é um parênteses fechado
	} else if (strcmp(splitString, ")") == 0) {
		token->classification = "ponctuation";
		token->attribute = "CP";

	//Verifica se é uma vírgula
	} else if (strcmp(splitString, ",") == 0) {
		token->classification = "ponctuation";
		token->attribute = "COMMA";

	//Verifica se é uma abertura de aspas, caso seja, setta a variavél verificada acima como verdadeira
	} else if (splitString[0] == '\"') {
		isStringLiteral = 1;

	//Verifica se são dois pontos (usado na declaração de variáveis)
	} else if (strcmp(splitString, ":") == 0) {
		token->classification = "ponctuation";
		token->attribute = "COL";

	//Verifica se é um comentário, caso seja, não retorna nenhum token e pula para a próxima linha
	} else if (strcmp(splitString, "//") == 0) {
		forward = MAXBUFFERLENGTH;

	//Verifica se é operador de soma
	} else if (strcmp(splitString, "+") == 0) {
		token->classification = "arith-op";
		token->attribute = "PLUS";

	//Verifica se é operador de menos
	} else if (strcmp(splitString, "-") == 0) {
		token->classification = "arith-op";
		token->attribute = "MIN";

	//Verifica se é uma multiplicação
	} else if (strcmp(splitString, "*") == 0) {
		token->classification = "arith-op";
		token->attribute = "MULT";

	//Verifica se é uma divisão inteira
	} else if (strcmp(splitString, "\\") == 0) {
		token->classification = "arith-op";
		token->attribute = "INTD";

	//Verifica se é menor
	} else if (strcmp(splitString, "<") == 0) {
		token->classification = "rel-op";
		token->attribute = "LT";

	//Verifica se é menor ou igual
	} else if (strcmp(splitString, "<=") == 0) {
		token->classification = "rel-op";
		token->attribute = "LE";

	//Verifica se é uma atribuição de variável
	} else if (strcmp(splitString, "<-") == 0) {
		token->classification = "attribution";
		token->attribute = NULL;

	//Verifica se é diferente
	} else if (strcmp(splitString, "<>") == 0) {
		token->classification = "rel-op";
		token->attribute = "NE";

	//Verifica se é maior
	} else if (strcmp(splitString, ">") == 0) {
		token->classification = "rel-op";
		token->attribute = "GT";

	//Verifica se é menor ou igual
	} else if (strcmp(splitString, ">=") == 0) {
		token->classification = "rel-op";
		token->attribute = "GE";

	//Verifica se é igual
	} else if (strcmp(splitString, "=") == 0) {
		token->classification = "rel-op";
		token->attribute = "EQ";

	/* Caso não seja nenhuma dessas, verifica se é numero, palavra reservada
	  ou variável*/
	} else {
		token = getNumberVariableOrReservedWord(splitString);
	}

	return token;
}

void readFile() {
	buffer = malloc(bufferSize);
	Token *t = NULL;

	while (fgets(buffer, bufferSize, source_file) != NULL) {
		forward = 0;

		printf("buffer = %s", buffer);
		char* word;
		/* Quebra o buffer em palavras separadas por espaços, além de ignorar
		  as scape sequences de quebra de linha (\n) e do enter (\r) */
		word = strtok(buffer," \n\r");

		while (word != NULL && forward < MAXBUFFERLENGTH) {
			printf("Palavra separada por espaços: %s\n", word);

			t = getToken(word);
			if (t != NULL && t->classification != NULL) {
				if (t->attribute == NULL) {
					fprintf(destination_file, "<%s>\n", t->classification);
				} else {
					fprintf(destination_file, "<%s, %s>\n", t->classification, t->attribute);
				}

				printf("%s %s\n", t->classification, t->attribute);
			}

			word = strtok(NULL, " \n\r");
		}

//		while (buffer[forward] != '\n' && forward < MAXBUFFERLENGTH) {
//
//			t = getToken();
//
//			if (t != NULL && t->classification != NULL) {
//				if (t->attribute == NULL) {
//					fprintf(destination_file, "<%s>\n", t->classification);
//				} else {
//					fprintf(destination_file, "<%s, %s>\n", t->classification, t->attribute);
//				}
//			}
//			printf("%s %s\n", t->classification, t->attribute);
//
//			begin = forward;
//		}
//		line++;
	}
}



int main() {
	int i = 0;

	//TODO: colocar constante de arquivo de entrada e de saida
	openFile(&source_file, "entrada.txt", "r");
	openFile(&destination_file, "saida.txt", "w");

	for (i = 0; i < MAXVARIABLES; i++) {
		variables[i][0] = '\0';
	}

	readFile();

	for (i = 0; i < MAXVARIABLES; i++) {
		if (variables[i][0] != '\0') {
			printf("%s\n", variables[i]);
		}
	}

	return 0;
}
