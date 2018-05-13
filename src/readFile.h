/*
 * readFile.h
 *
 *  Created on: May 13, 2018
 *      Author: anacarolinasantos
 */

#ifndef READFILE_H_
#define READFILE_H_

//Função que abre os arquivos de acordo com o modo de acesso passado na chamada da função
void openFile(FILE **file, char fileName[], char type[]);

#endif /* READFILE_H_ */
