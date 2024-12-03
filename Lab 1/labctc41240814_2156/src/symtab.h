/****************************************************/
/* File: symtab.h                                    */
/* Symbol table interface for the C- compiler        */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* Structure for line numbers list */
typedef struct LineListRec
{
    int lineno;
    struct LineListRec *next;
} * LineList;

/* Structure for symbol table bucket */
typedef struct BucketListRec
{
    char *name;
    LineList lines;
    int memloc;         /* Memory location */
    ExpType type;       /* Data type */
    StmtKind kind;      /* Variable, function, parameter */
    bool isArray;       /* Is this an array? */
    char *scope;        /* Scope name */
    struct BucketListRec *next;
} * BucketList;

/* Structure for scope */
typedef struct ScopeRec
{
    char *funcName;               /* Function or scope name */
    struct ScopeRec *parent;      /* Parent scope */
    BucketList hashTable[SIZE];   /* Hash table for symbols */
    struct ScopeRec *next;        /* Next scope in the list */
} * Scope;

/* Global variable for current scope */
extern Scope currentScope;

/* Global variable for scope list */
extern Scope scopeList; // Lista de escopos

/* Function prototypes */

/* Insert a symbol into the symbol table */
void st_insert(char *name, int lineno, int loc, ExpType type, StmtKind kind, bool isArray, char *scope);

/* Lookup a symbol in all scopes */
BucketList st_lookup(char *name);

/* Lookup a symbol in the current scope */
BucketList st_lookup_top(char *name);

/* Add a line number to a symbol */
void st_add_lineno(char *name, int lineno);

/* Print the symbol table */
void printSymTab(FILE *listing);

#endif /* _SYMTAB_H_ */
