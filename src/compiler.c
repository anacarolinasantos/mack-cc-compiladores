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
static int forward = 0, line = 1, indexVariables = 0, allTokenSize = 0;

//Declaração do buffer de entrada
static char *buffer;

//Verifica se está lendo uma string, pois deve-se ignorar tudo até as próximas aspas
static int isStringLiteral = 0;

//Vetor de variaveis que já foram instanciadas no código
//Cada variável pode ter no máximo 30 caracteres de acordo com as regras do VisuAlg
static char variables[MAXVARIABLES][MAXVARIABLELENGTH];

//Lista de todos os tokens que foram lidos no arquivo de entrada
Token *allTokens;

//Token que está sendo analisado
Token currentToken;

//Funções da leitura de arquivos
void openFile(FILE **file, char fileName[], char type[]) {
	*file = fopen(fileName, type);

	if (!*file) {
		printf("Unable to open file, please enter a valid file name\n");
		exit(1);
	}
}

//Declaração de funçoes da análise léxica
Token* getNumberVariableOrReservedWord(char* splitString);
Token* getToken(char* splitString);
void readProgramFile();

//Declaração de funçoes da análise sintática
void syntaxError(char* expected, char* returned);
Token getNextToken();
int tokenClassificationMatch(char* classificationExpected);
int tokenMatch(char* classificationExpected, char* attributeExpected);
void dataType();
void variableListSyntax();
void variableDeclarationSyntax();
void sequenceVariableListSyntax();
void selectionSyntax();
void outputSyntax();
void inputSyntax();
void commandSyntax();
void commandSequenceSyntax();
void programSyntax();
void readTokenFile();

//-------------- Funções da analise léxica --------------
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
			strcpy(token->classification, "texto");
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

/* Função que lê o arquivo de entrada da linguagem MiniVisuAlg, chama a função
  getToken(word), sendo word a palavra que acabou de ser lida para identificar qual é o token */
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

//-------------- Funções da analise sintática --------------
//Função que escreve o erro sintatico
void syntaxError(char* expected, char* returned) {
	printf("Erro: esperava %s e veio %s", expected, returned);
	exit(1);
}

/* Fução que pega o token seguinte e soma mais um a variável forward, que
 controla qual o atual token que está sendo lido */
Token getNextToken() {
	Token t;

	if (forward >= allTokenSize) {
		return t;
	}

	forward++;

	return allTokens[forward-1];
}

//Função que verifica se a classificação do Token é a mesma que a esperada
int tokenClassificationMatch(char* classificationExpected) {
	Token t = getNextToken();

	if (strcmp(classificationExpected, t.classification) == 0) {
		currentToken = t;
		return 1;
	}

	forward--;
	return 0;
}

//Função que verifica se a classificação e o atributo do Token são os mesmos que o esperado
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

void factor() {
    if ((strcmp(currentToken.classification, "id") != 0) || (strcmp(currentToken.classification, "number") != 0)) {
		syntaxError("variavel ou numero", currentToken.classification);
    }
}

void term() {
    factor();
}

void simpleExpression() {
    if ((strcmp(currentToken.attribute, "PLUS") == 0) || (strcmp(currentToken.attribute, "MIN") == 0)) {
        term();
    }
    term();
}

void expression() {
    simpleExpression();
}

/* Regra de sintaxe de expressao de saida:
 texto | id | <expressao> */
void expressionOutput() {
    if ((strcmp(currentToken.classification, "texto") != 0) && (strcmp(currentToken.classification, "id") != 0)) {
        expression();
    }
}

/* Regra de sintaxe de lista de expressao de saida:
 <expSaida> | <expSaida> + <listExpr> | <expSaida> , <listExpr> */
void expressionListSyntax() {
    expressionOutput();
}

/* Regra de sintaxe da lista de variáveis:
  <id> | <id>,<listaVar> */
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

/* Regra de sintaxe da declaração de variáveis:
  <listaVar> : <tipo> */
void variableDeclarationSyntax() {
	//Chama a função de lista de variáveis
	variableListSyntax();

	currentToken = getNextToken();
	if(strcmp(currentToken.attribute, "COL") == 0) {
		currentToken = getNextToken();
		dataType();
	} else {
		syntaxError("COL", currentToken.attribute);
	}
}

/* Regra de sintaxe da sequência de declaração de variáveis:
  <decVar> | <decVar><seqDecVar> */
void sequenceVariableListSyntax() {
	//Chama a função de declaração de variaveis
	variableDeclarationSyntax();

	//Quando termina a declaração de variável, verifica se a próxima linha continua sendo declaração de variáveis ou se já começa o início do programa
	if (tokenClassificationMatch("id")) {
		//Caso seja mais uma declaração de variavel, chama a mesma função e faz de forma recursiva
		sequenceVariableListSyntax();
	}
}

/* Regra de sintaxe de repetição enquanto:
   <id> <- <number>
 | <id> <- <id>
 | <id> <- <expressao> */
void allocationSyntax() {
	if (strcmp(currentToken.classification, "attribution") == 0) {
		if ((strcmp(currentToken.classification, "number") != 0) && (strcmp(currentToken.classification, "id") != 0)) {
			expression();
		}
	} else {
		syntaxError("attribution", currentToken.classification);
	}
}

/* Regra de sintaxe de repetição enquanto:
   enquanto <expLogica> faca <sequenciaComando> fimenquanto */
void repetitionWhileSyntax() {
	//TODO: chamar expressao logica
	if (strcmp(currentToken.classification, "faca") == 0) {
		commandSequenceSyntax();
		if (strcmp(currentToken.classification, "fimenquanto") != 0) {
			syntaxError("fimenquanto", currentToken.classification);
		}
	} else {
		syntaxError("faca", currentToken.classification);
	}
}

/* Regra de sintaxe de repetição para:
   para <id> de <expressao> ate <expressao> faca <sequenciaComando> fimpara
 | para <id> de <expressao> ate <expressao> passo <expressao> faca <sequenciaComando> fimpara */
void repetitionForSyntax() {
	currentToken = getNextToken();
	if(strcmp(currentToken.classification, "id") == 0) {
		currentToken = getNextToken();
		if(strcmp(currentToken.classification, "de") == 0) {
			expression();
			currentToken = getNextToken();
			if(strcmp(currentToken.classification, "ate") == 0) {
				expression();
				currentToken = getNextToken();
				if(strcmp(currentToken.classification, "passo") == 0) {
					expression();
					currentToken = getNextToken();
				}
				if(strcmp(currentToken.classification, "faca") == 0) {
					commandSequenceSyntax();
					if(strcmp(currentToken.classification, "fimpara") != 0) {
						syntaxError("fimpara", currentToken.classification);
					}
				} else {
					syntaxError("faca", currentToken.classification);
				}
			} else {
				syntaxError("ate", currentToken.classification);
			}
		} else {
			syntaxError("de", currentToken.classification);
		}
	} else {
		syntaxError("id", currentToken.classification);
	}
}

/* Regra de sintaxe de seleção:
   se <expLogica> entao <sequenciaComando> fimse
 | se <expLogica> entao <sequenciaComando> senao <sequenciaComando> fimse */
void selectionSyntax() {
	//TODO: chamar expressao logica
	currentToken = getNextToken();
	if(strcmp(currentToken.classification, "entao") == 0) {
		commandSequenceSyntax();
		currentToken = getNextToken();
		if (strcmp(currentToken.classification, "senao") == 0) {
			commandSequenceSyntax();
			currentToken = getNextToken();
		}
		if (strcmp(currentToken.classification, "fimse") != 0) {
			syntaxError("fimse", currentToken.classification);
		}
	} else {
		syntaxError("entao", currentToken.classification);
	}
}

/* Regra de sintaxe de saida de dados:
 escreva ( <listaExpr> ) */
void outputSyntax() {
	currentToken = getNextToken();
	if(strcmp(currentToken.attribute, "OP") == 0) {
		currentToken = getNextToken();
		expressionListSyntax();
		currentToken = getNextToken();
		if(strcmp(currentToken.attribute, "CP") != 0) {
			syntaxError("CP", currentToken.attribute);
		}
	} else {
		syntaxError("OP", currentToken.attribute);
	}
}

/* Regra de sintaxe de entrada de dados:
 leia ( <listaVar> ) */
void inputSyntax() {
	currentToken = getNextToken();
	if(strcmp(currentToken.attribute, "OP") == 0) {
		currentToken = getNextToken();
		variableListSyntax();
		currentToken = getNextToken();
		if(strcmp(currentToken.attribute, "CP") != 0) {
			syntaxError("CP", currentToken.attribute);
		}
	} else {
		syntaxError("OP", currentToken.attribute);
	}
}

/* Regra de sintaxe de um comando:
 <entrada> | <saida> | <selecao> | <rep_para> | <rep_enquanto> | <atribuicao> */
void commandSyntax() {
	currentToken = getNextToken();
	if (strcmp(currentToken.classification, "leia") == 0) {
		inputSyntax();
	} else if (strcmp(currentToken.classification, "escreva") == 0) {
		outputSyntax();
	} else if (strcmp(currentToken.classification, "se") == 0) {
		selectionSyntax();
	} else if (strcmp(currentToken.classification, "para") == 0) {
		repetitionForSyntax();
	} else if (strcmp(currentToken.classification, "enquanto") == 0) {
		repetitionWhileSyntax();
	} else if (strcmp(currentToken.classification, "id") == 0) {
		allocationSyntax();
	} else {
		syntaxError("entrada de dados, saida de dados, selecao, repeticao ou atribuicao", currentToken.classification);
	}
}


/* Regra de sintaxe da sequência de comandos possíveis:
 <comando> | <comando> <sequenciaComando> */
void commandSequenceSyntax() {
	commandSyntax();

	//Quando termina o comando, verifica se a próxima linha é outro comando ou se é o final do programa ou final de um bloco de comando
	if ((tokenClassificationMatch("leia"))
		|| (tokenClassificationMatch("escreva"))
		|| (tokenClassificationMatch("se"))
		|| (tokenClassificationMatch("para"))
		|| (tokenClassificationMatch("enquanto"))
		|| (tokenClassificationMatch("id"))) {
			commandSequenceSyntax();
		}
}

/* Regra de sintaxe do começo do programa:
  algoritmo <sequencia-declaracao-variaveis> inicio <bloco-comandos> fimalgoritmo */
void programSyntax() {
	currentToken.classification = (char *) calloc (30, sizeof(char));
	currentToken.attribute = (char *) calloc (30, sizeof(char));

	currentToken = getNextToken();
	if(strcmp(currentToken.classification, "algoritmo") == 0) {
		currentToken = getNextToken();
		if(strcmp(currentToken.classification, "texto") == 0) {
			currentToken = getNextToken();
			if (strcmp(currentToken.classification, "id") == 0) {
				//Chama a função de sequencia de declaração de variáveis
				sequenceVariableListSyntax();
				currentToken = getNextToken();
			}
			if(strcmp(currentToken.classification, "inicio") == 0) {
				commandSequenceSyntax();
				if(strcmp(currentToken.classification, "fimalgoritmo") != 0) {
					syntaxError("fimalgoritmo", currentToken.classification);
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

/* Função que lê o arquivo de saida da análise léxica, salva todos os tokens na variável allTokens
  e chama a função programSyntax() para começar a fazer a análise sintática*/
void readTokenFile() {
	char* buffer = malloc(bufferSize);
	int i = 0;
	allTokens = malloc(MAXTOKENAMOUNT * sizeof(Token));
	Token *token = (Token *) calloc(1,sizeof(Token));
	token->classification = (char *) calloc (30, sizeof(char));
	token->attribute = (char *) calloc (30, sizeof(char));

	while (fgets(buffer, bufferSize, destination_file) != NULL) {
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

	programSyntax();
	free(token);
}

int main() {
	//Analisador léxico
	openFile(&source_file, ARQENTRADA, "r");
	openFile(&destination_file, ARQSAIDA, "w");

	readProgramFile();

	printf("Lexical analyzer finished on file %s\n\n", ARQSAIDA);

	//Fecha os arquivos, para abrir o arquivo de saída do analisador léxico com a opção de leitura de arquivo
	fclose(source_file);
	fclose(destination_file);

	//Começa o analisador sintátic
	openFile(&destination_file, ARQSAIDA, "r");

	readTokenFile();

	printf("Syntax analyzer finished with success");

	return 0;
}
