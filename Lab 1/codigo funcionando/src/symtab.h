/****************************************************/
/* File: symtab.h                                    */
/* Symbol table interface for the C- compiler        */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

/* Global variable for current scope */
extern Scope currentScope;

/* Global variable for scope list */
extern Scope scopeList; // Lista de escopos

/* Function prototypes */

/* Insert a symbol into the symbol table */
void st_insert(char *name, int lineno, int loc, ExpType type, StmtKind kind, bool isArray, char *scope);

/* Lookup a symbol in all scopes given the name of the scope... and returns memloc */
int st_lookup_memLoc(char *name, Scope scope);

/* Lookup a symbol in all scopes */
BucketList st_lookup(char *name);

/* Lookup a symbol in the current scope */
BucketList st_lookup_top(char *name);

/* Add a line number to a symbol */
void st_add_lineno(char *name, int lineno);

/* Print the symbol table */
void printSymTab(FILE *listing);

#endif /* _SYMTAB_H_ */
