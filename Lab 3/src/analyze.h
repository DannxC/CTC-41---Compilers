/****************************************************/
/* File: analyze.h                                  */
/* Semantic analyzer interface for the C- compiler  */
/****************************************************/

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

#include "globals.h"
#include "symtab.h"

/* Função buildSymtab constrói a tabela de símbolos
 * por meio de um percurso em pré-ordem da árvore sintática
 */
void buildSymtab(TreeNode *syntaxTree);

/* Função typeCheck realiza verificação de tipos
 * por meio de um percurso em pós-ordem da árvore sintática
 */
void typeCheck(TreeNode *syntaxTree);

#endif /* _ANALYZE_H_ */
