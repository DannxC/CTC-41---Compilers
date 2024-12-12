/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "log.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAX_MEM 1023

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8

// typedef enum 
// { 
//   /* book-keeping tokens */
//   ENDFILE, ERROR,

//   /* reserved words */
//   IF, ELSE, INT, VOID, WHILE, RETURN,

//   /* multicharacter tokens */
//   ID, NUM,

//   /* special symbols */
//   ASSIGN, EQ, NE, LT, LE, GT, GE,
//   PLUS, MINUS, TIMES, OVER,
//   LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
//   SEMI, COMMA

// } TokenType;

typedef int TokenType;  // TokenType é redefinido como int para compatibilidade com Bison - lab2

extern FILE* source; /* source code text file */
extern FILE* redundant_source; /* listing code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

// typedef enum {StmtK,ExpK} NodeKind;
// typedef enum {IfK,RepeatK,AssignK,ReadK,WriteK} StmtKind;  // modificar pro lab2
// typedef enum {OpK,ConstK,IdK} ExpKind;

typedef enum {StmtK, ExpK} NodeKind;

typedef enum { 
    IfK,          // Declaração if-else
    WhileK,       // Declaração while
    ReturnK,      // Declaração return
    VarK,         // Declaração de variável
    FuncK,        // Declaração de função
    CompoundK,    // Bloco composto de declarações
    ParamK        // Parâmetros de função
} StmtKind;

typedef enum { 
    OpK,          // Operador (+, -, *, /, etc.)
    ConstK,       // Constante numérica
    IdK,          // Identificador
    AssignK,      // Expressão de atribuição (=)
    CallK         // Chamada de função (ativação)
} ExpKind;

/* ExpType is used for type checking */
typedef enum {Void, Integer, Boolean} ExpType;

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

#define MAXCHILDREN 3

typedef struct treeNode
  { 
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;
    struct treeNode *parent; // Added for lab3
    int lineno;
    NodeKind nodekind;
    union { 
      StmtKind stmt; 
      ExpKind exp;
    } kind;
    union { 
      TokenType op;
      int val;
      char * name; 
    } attr;
    bool isArray; /* is this an array? 0 false, 1 true */
    ExpType type; /* for type checking of exps */
    Scope sc; // Added for lab3
  } TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error; 

/* Bison parser inclusion for tokens - lab2 */
#ifndef YYPARSER
#include "parser.h"  // Inclui o arquivo gerado pelo Bison
#define ENDFILE 0    // Define ENDFILE como 0, equivalente ao token YYEOF do Bison
#endif

#endif
