/****************************************************/
/* File: cgen.h                                     */
/* Interface do gerador de código para C-            */
/* Gera código para a TM Machine                     */
/****************************************************/

#ifndef _CGEN_H_
#define _CGEN_H_

#include "globals.h"

/* codeGen gera código a partir da syntax tree.
 * Presume-se que a tabela de símbolos já está construída
 * e o typeCheck já foi executado.
 * O código gerado é escrito nos arquivos de saída 
 * correspondentes (TAB/GEN).
 */
void codeGen(TreeNode * syntaxTree);

#endif
