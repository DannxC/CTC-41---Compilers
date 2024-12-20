/****************************************************/
/* File: cminus.y                                   */
/* The C- Yacc/Bison specification file             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char *);

%}

%union {
    int val;
    char *name;
    TokenType token;
    TreeNode* node;
    ExpType type;
}

%token <name> ID                                  // Identificadores
%token <val> NUM                                  // Números
%token IF ELSE INT VOID WHILE RETURN              // Palavras reservadas
%token ASSIGN EQ NE LT LE GT GE                   // Operadores relacionais e de atribuição
%token PLUS MINUS TIMES OVER                      // Operadores aritméticos
%token LPAREN RPAREN                              // Parênteses
%token LBRACE RBRACE                              // Chaves
%token LBRACKET RBRACKET                          // Colchetes
%token SEMI COMMA                                 // Pontuação
%token ERROR                                      // Token de erro

/* Definição da precedência e associatividade dos operadores */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGN
%left EQ NE LT LE GT GE
%left PLUS MINUS
%left TIMES OVER

/* Definição dos tipos para símbolos não-terminais */
%type <node> program declaration_list declaration var_declaration fun_declaration params param_list param
%type <node> compound_stmt local_declarations statement_list statement expression_stmt selection_stmt iteration_stmt return_stmt
%type <node> expression var simple_expression additive_expression term factor call args arg_list
%type <token> relop addop mulop
%type <type> type_specifier

%%

/* Regras da Gramática para C- */

program
    : declaration_list
        { savedTree = $1; }
    ;

declaration_list
    : declaration_list declaration
        { 
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
        }
    | declaration
        { $$ = $1; }
    ;

declaration
    : var_declaration
        { $$ = $1; }
    | fun_declaration
        { $$ = $1; }
    ;

var_declaration
    : type_specifier ID SEMI
        { 
            $$ = newStmtNode(VarK);
            $$->attr.name = $2;
            $$->type = $1;
            $$->isArray = FALSE;
            $$->lineno = lineno;
        }
    | type_specifier ID LBRACKET NUM RBRACKET SEMI
        { 
            $$ = newStmtNode(VarK);
            $$->attr.name = $2;
            $$->type = $1;
            $$->isArray = TRUE;
            $$->child[0] = newExpNode(ConstK);
            $$->child[0]->attr.val = $4;
            $$->lineno = lineno;
            /* Definir o pai do nó filho */
            $$->child[0]->parent = $$;
        }
    ;

type_specifier
    : INT
        { $$ = Integer; }
    | VOID
        { $$ = Void; }
    ;

fun_declaration
    : type_specifier ID {savedLineNo = lineno;} LPAREN params RPAREN compound_stmt
        {
            $$ = newStmtNode(FuncK);
            $$->attr.name = $2;
            $$->type = $1;
            $$->child[0] = $5;    /* parâmetros */
            $$->child[1] = $7;    /* corpo da função */
            $$->lineno = savedLineNo;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
        }
    ;

params
    : param_list
        { $$ = $1; }
    | VOID
        { $$ = NULL; }
    ;

param_list
    : param_list COMMA param
        { 
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $3;
                $$ = $1;
            } else {
                $$ = $3;
            }
        }
    | param
        { $$ = $1; }
    ;

param
    : type_specifier ID
        { 
            $$ = newStmtNode(ParamK);
            $$->attr.name = $2;
            $$->type = $1;
            $$->isArray = FALSE;
            $$->lineno = lineno;
        }
    | type_specifier ID LBRACKET RBRACKET
        { 
            $$ = newStmtNode(ParamK);
            $$->attr.name = $2;
            $$->type = $1;
            $$->isArray = TRUE;
            $$->lineno = lineno;
        }
    ;

compound_stmt
    : LBRACE local_declarations statement_list RBRACE
        {
            $$ = newStmtNode(CompoundK);
            $$->child[0] = $2;   /* local_declarations */
            $$->child[1] = $3;   /* statement_list */
            $$->lineno = lineno;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
        }
    ;

local_declarations
    : local_declarations var_declaration
        { 
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
            /* Definir o pai */
            $2->parent = NULL; // Será atribuído no nó composto
        }
    | /* vazio */
        { $$ = NULL; }
    ;

statement_list
    : statement_list statement
        { 
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
            /* Definir o pai */
            $2->parent = NULL; // Será atribuído no nó composto
        }
    | /* vazio */
        { $$ = NULL; }
    ;

statement
    : expression_stmt
        { $$ = $1; }
    | compound_stmt
        { $$ = $1; }
    | selection_stmt
        { $$ = $1; }
    | iteration_stmt
        { $$ = $1; }
    | return_stmt
        { $$ = $1; }
    ;

expression_stmt
    : expression SEMI
        { $$ = $1; }
    | SEMI
        { $$ = NULL; }
    ;

selection_stmt
    : IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
        {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;   /* condição */
            $$->child[1] = $5;   /* statement */
            $$->child[2] = NULL; /* sem else */
            $$->lineno = lineno;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
        }
    | IF LPAREN expression RPAREN statement ELSE statement
        {
            $$ = newStmtNode(IfK);
            $$->child[0] = $3;   /* condição */
            $$->child[1] = $5;   /* statement if */
            $$->child[2] = $7;   /* statement else */
            $$->lineno = lineno;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
            if ($$->child[2] != NULL) $$->child[2]->parent = $$;
        }
    ;

iteration_stmt
    : WHILE LPAREN expression RPAREN statement
        {
            $$ = newStmtNode(WhileK);
            $$->child[0] = $3;   /* condição */
            $$->child[1] = $5;   /* corpo do loop */
            $$->lineno = lineno;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
        }
    ;

return_stmt
    : RETURN SEMI
        {
            $$ = newStmtNode(ReturnK);
            $$->child[0] = NULL; /* retorno vazio */
            $$->lineno = lineno;
        }
    | RETURN expression SEMI
        {
            $$ = newStmtNode(ReturnK);
            $$->child[0] = $2;   /* expressão de retorno */
            $$->lineno = lineno;
            /* Atribuir o pai ao filho */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
        }
    ;

expression
    : var ASSIGN expression
        {
            $$ = newExpNode(AssignK);
            $$->child[0] = $1;   /* variável */
            $$->child[1] = $3;   /* expressão */
            $$->lineno = lineno;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
        }
    | simple_expression
        { $$ = $1; }
    ;

var
    : ID
        {
            $$ = newExpNode(IdK);
            $$->attr.name = $1;
            $$->isArray = FALSE;
            $$->lineno = lineno;
        }
    | ID LBRACKET expression RBRACKET
        {
            $$ = newExpNode(IdK);
            $$->attr.name = $1;
            $$->isArray = TRUE;
            $$->child[0] = $3;   /* índice do array */
            $$->lineno = lineno;
            /* Atribuir o pai ao filho */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
        }
    ;

simple_expression
    : additive_expression relop additive_expression
        {
            $$ = newExpNode(OpK);
            $$->attr.op = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
            $$->lineno = lineno;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
        }
    | additive_expression
        { $$ = $1; }
    ;

relop
    : LE
        { $$ = LE; }
    | LT
        { $$ = LT; }
    | GT
        { $$ = GT; }
    | GE
        { $$ = GE; }
    | EQ
        { $$ = EQ; }
    | NE
        { $$ = NE; }
    ;

additive_expression
    : additive_expression addop term
        {
            $$ = newExpNode(OpK);
            $$->attr.op = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
            $$->lineno = lineno;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
        }
    | term
        { $$ = $1; }
    ;

addop
    : PLUS
        { $$ = PLUS; }
    | MINUS
        { $$ = MINUS; }
    ;

term
    : term mulop factor
        {
            $$ = newExpNode(OpK);
            $$->attr.op = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
            $$->lineno = lineno;
            /* Atribuir o pai aos filhos */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
            if ($$->child[1] != NULL) $$->child[1]->parent = $$;
        }
    | factor
        { $$ = $1; }
    ;

mulop
    : TIMES
        { $$ = TIMES; }
    | OVER
        { $$ = OVER; }
    ;

factor
    : LPAREN expression RPAREN
        { $$ = $2; }
    | var
        { $$ = $1; }
    | call
        { $$ = $1; }
    | NUM
        {
            $$ = newExpNode(ConstK);
            $$->attr.val = $1;
            $$->lineno = lineno;
        }
    ;

call
    : ID LPAREN args RPAREN
        {
            $$ = newExpNode(CallK);
            $$->attr.name = $1;
            $$->child[0] = $3;   /* argumentos */
            $$->lineno = lineno;
            /* Atribuir o pai ao filho */
            if ($$->child[0] != NULL) $$->child[0]->parent = $$;
        }
    ;

args
    : arg_list
        { $$ = $1; }
    | /* vazio */
        { $$ = NULL; }
    ;

arg_list
    : arg_list COMMA expression
        {
            TreeNode* t = $1;
            if (t != NULL) {
                while (t->sibling != NULL)
                    t = t->sibling;
                t->sibling = $3;
                $$ = $1;
            } else {
                $$ = $3;
            }
            /* Definir o pai */
            $3->parent = NULL; // Será atribuído na chamada da função
        }
    | expression
        { $$ = $1; }
    ;

%%

int yyerror(char * message)
{ pce("Syntax error at line %d: %s\n",lineno,message);
  pce("Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with earlier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}
