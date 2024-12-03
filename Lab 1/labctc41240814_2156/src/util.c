/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printLine prints a full line
 * of the source code, with its number
 */
void printLine()
{
  char line[1024];
  char *ret = fgets(line, 1024, redundant_source);
  // If an error occurs, or if end-of-file is reached and no characters were read, fgets returns NULL.
  if (ret)
  {
    pc("%d: %-1s", lineno, line);
    // if EOF, the string does not contain \n. add it to separate from EOF token
    if (feof(redundant_source))
      pc("\n");
  }
  // lineno++;
}

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { 
    /* Palavras reservadas */
    case IF:
    case ELSE:
    case INT:
    case VOID:
    case WHILE:
    case RETURN:
        pc("reserved word: %s\n", tokenString);
        break;

    /* Operadores e símbolos especiais */
    case ASSIGN:    pc("=\n");        break;
    case EQ:        pc("==\n");       break;
    case NE:        pc("!=\n");       break;
    case LT:        pc("<\n");        break;
    case LE:        pc("<=\n");       break;
    case GT:        pc(">\n");        break;
    case GE:        pc(">=\n");       break;
    case PLUS:      pc("+\n");        break;
    case MINUS:     pc("-\n");        break;
    case TIMES:     pc("*\n");        break;
    case OVER:      pc("/\n");        break;
    case LPAREN:    pc("(\n");        break;
    case RPAREN:    pc(")\n");        break;
    case LBRACE:    pc("{\n");        break;
    case RBRACE:    pc("}\n");        break;
    case LBRACKET:  pc("[\n");        break;
    case RBRACKET:  pc("]\n");        break;
    case SEMI:      pc(";\n");        break;
    case COMMA:     pc(",\n");        break;
    
    case ENDFILE:   pc("EOF\n");      break;

    /* Tokens multicaracteres */
    case ID:
        pc("ID, name= %s\n", tokenString);
        break;
    case NUM:
        pc("NUM, val= %s\n", tokenString);
        break;

    /* Erro */
    case ERROR:
        pce("ERROR: %s\n", tokenString);
        break;

    /* Desconhecido */
    default:
        pce("Unknown token: %d\n", token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    pce("Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    pce("Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    pce("Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=4
#define UNINDENT indentno-=4

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    pc(" ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
    while (tree != NULL)
    {
        printSpaces();
        if (tree->nodekind == StmtK)
        {
            switch (tree->kind.stmt)
            {
            case IfK:
                pc("Conditional selection\n");
                INDENT;
                printTree(tree->child[0]);  // Condição
                printTree(tree->child[1]);  // Statement "then"
                if (tree->child[2] != NULL)
                    printTree(tree->child[2]);  // Statement "else"
                UNINDENT;
                break;
            case WhileK:
                pc("Iteration (loop)\n");
                INDENT;
                printTree(tree->child[0]);  // Condição
                printTree(tree->child[1]);  // Corpo do loop
                UNINDENT;
                break;
            case ReturnK:
                pc("Return\n");
                INDENT;
                if (tree->child[0] != NULL)
                    printTree(tree->child[0]);  // Expressão de retorno
                UNINDENT;
                break;
            case VarK:
                if (tree->isArray)
                {
                    pc("Declare %s array: %s\n", (tree->type == Integer ? "int" : "void"), tree->attr.name);
                    INDENT;
                    if (tree->child[0] != NULL) {
                        printTree(tree->child[0]);  // Tamanho do array
                    }
                    UNINDENT;
                }
                else
                {
                    pc("Declare %s var: %s\n", (tree->type == Integer ? "int" : "void"), tree->attr.name);
                }
                break;
            case FuncK:
                pc("Declare function (return type \"%s\"): %s\n", (tree->type == Integer ? "int" : "void"), tree->attr.name);
                INDENT;
                printTree(tree->child[0]);  // Parâmetros
                printTree(tree->child[1]);  // Corpo da função
                UNINDENT;
                break;
            case CompoundK:
                printTree(tree->child[0]);  // Declarações locais
                printTree(tree->child[1]);  // Lista de statements
                break;
            case ParamK:
                // Ajuste para diferenciar "int array" de "int var" para parâmetros
                pc("Function param (%s %s): %s\n", 
                    (tree->type == Integer ? "int" : "void"), 
                    (tree->isArray ? "array" : "var"), 
                    tree->attr.name);
                break;
            default:
                pc("Unknown StmtNode kind\n");
                break;
            }
        }
        else if (tree->nodekind == ExpK)
        {
            switch (tree->kind.exp)
            {
            case OpK:
                pc("Op: ");
                printToken(tree->attr.op, "\0");
                INDENT;
                printTree(tree->child[0]);
                printTree(tree->child[1]);
                UNINDENT;
                break;
            case ConstK:
                pc("Const: %d\n", tree->attr.val);
                break;
            case IdK:
                pc("Id: %s\n", tree->attr.name);
                if (tree->isArray && tree->child[0] != NULL)
                {
                    INDENT;
                    printTree(tree->child[0]);  // Índice do array
                    UNINDENT;
                }
                break;
            case AssignK:
                if (tree->child[0] != NULL && tree->child[0]->kind.exp == IdK)
                {
                    pc("Assign to %s: %s\n", (tree->child[0]->isArray ? "array" : "var"), tree->child[0]->attr.name);
                    if (tree->child[0]->isArray && tree->child[0]->child[0] != NULL)
                    {
                        INDENT;
                        printTree(tree->child[0]->child[0]);  // Índice do array
                        UNINDENT;
                    }
                }
                else
                {
                    pc("Assign\n");
                    INDENT;
                    printTree(tree->child[0]);  // Variável
                    UNINDENT;
                }
                INDENT;
                printTree(tree->child[1]);  // Expressão atribuída
                UNINDENT;
                break;
            case CallK:
                pc("Function call: %s\n", tree->attr.name);
                INDENT;
                printTree(tree->child[0]);  // Argumentos da função
                UNINDENT;
                break;
            default:
                pc("Unknown ExpNode kind\n");
                break;
            }
        }
        else
        {
            pc("Unknown node kind\n");
        }
        tree = tree->sibling;
    }
}
