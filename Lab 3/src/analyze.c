/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the C- compiler                              */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "util.h"

/* Contador global para Escopos Anônimos */
static int compoundScopeNum = 0;

/* Contador para localização de memória (opcional) */
static int location = 0;

/* Nome da função atual */
static char *currentFunction = NULL;

/* See if main was declared*/
bool hasMain = FALSE;

/* Função para criar um novo escopo */
static void enterScope(char *scopeName)
{
    Scope newScope = (Scope)malloc(sizeof(struct ScopeRec));
    newScope->funcName = copyString(scopeName);
    newScope->parent = currentScope;
    newScope->next = NULL;
    // Inicializar a tabela hash do novo escopo
    int i;
    for (i = 0; i < SIZE; ++i)
        newScope->hashTable[i] = NULL;
    if (scopeList == NULL)
        scopeList = newScope;
    else
    {
        Scope s = scopeList;
        while (s->next != NULL)
            s = s->next;
        s->next = newScope;
    }
    currentScope = newScope;
}

/* Função para sair do escopo atual */
static void leaveScope(TreeNode *t)
{
    if (t->kind.stmt == CompoundK)
    currentScope = currentScope->parent;
}

/* Função traverse é um percurso genérico recursivo
 * da árvore sintática:
 * aplica preProc em pré-ordem e postProc em pós-ordem
 * na árvore apontada por t
 */
static void traverse(TreeNode *t,
                     void (*preProc)(TreeNode *),
                     void (*postProc)(TreeNode *))
{
    if (t != NULL)
    {
        preProc(t);
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
            {
                if (t->child[i] != NULL)
                    t->child[i]->parent = t; // Definir o pai
                traverse(t->child[i], preProc, postProc);
            }
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

/* nullProc é um procedimento que não faz nada
 * para gerar percursos apenas em pré-ordem ou pós-ordem
 * a partir de traverse
 */
static void nullProc(TreeNode *t)
{
    if (t == NULL)
        return;
    else
        return;
}

/* Função insertNode insere
 * identificadores armazenados em t na
 * tabela de símbolos
 */
static void insertNode(TreeNode *t)
{
    switch (t->nodekind)
    {
    case StmtK:
        switch (t->kind.stmt)
        {
        case VarK:
            BucketList symbol = st_lookup_top(t->attr.name);
            BucketList symbol2 = st_lookup(t->attr.name);

            if (t->type == Void)
            {
                pce("Semantic error at line %d: variable declared void\n",
                    t->lineno);
                Error = TRUE;
                break;
            }
            
            // Inserir variável na tabela de símbolos
            if (symbol2 != NULL && symbol2->kind == FuncK)
            {
                pce("Semantic error at line %d: '%s' was already declared as a function\n",
                    t->lineno, t->attr.name);
                Error = TRUE;
            }
            else if (symbol == NULL)
            {
                st_insert(t->attr.name, t->lineno, location++, t->type, VarK, t->isArray, currentScope->funcName);
            }
            else if (strcmp(symbol->scope, copyString(currentScope->funcName)) == 0)
            {
                pce("Semantic error at line %d: '%s' was already declared as a variable\n",
                    t->lineno, t->attr.name);
                Error = TRUE;
            }

            break;
        case FuncK:
            // Verificar se a função main foi declarada
            if (strcmp(t->attr.name, "main") == 0)
            {
                hasMain = TRUE;
            }

            // Inserir função na tabela de símbolos
            if (st_lookup(t->attr.name) == NULL)
            {
                st_insert(t->attr.name, t->lineno, location++, t->type, FuncK, t->isArray, "");
                // Entrar no escopo da função
                enterScope(t->attr.name);
                currentFunction = t->attr.name; // Definir a função atual
            }
            else
            {
                pce("Semantic error at line %d: function '%s' already declared\n",
                    t->lineno, t->attr.name);
                Error = TRUE;
            }
            break;
        case ParamK:
            // Inserir parâmetro na tabela de símbolos
            if (st_lookup_top(t->attr.name) == NULL)
            {
                st_insert(t->attr.name, t->lineno, location++, t->type, ParamK, t->isArray, currentScope->funcName);
            }
            else
            {
                pce("Semantic error at line %d: '%s' was already declared as a variable\n",
                    t->lineno, t->attr.name);
                Error = TRUE;
            }
            break;
        case CompoundK:
        {
            if (t->parent != NULL && t->parent->kind.stmt == FuncK)
            {
                // Não criar um novo escopo, já estamos no escopo da função
            }
            else
            {
                compoundScopeNum++;
                char scopeName[20];
                sprintf(scopeName, "compound%d", compoundScopeNum);
                enterScope(scopeName);
                t->attr.name = copyString(scopeName);
            }
            break;
        }
        default:
            break;
        }
        break;
    case ExpK:
        switch (t->kind.exp)
        {
        case IdK:
            // Uso de identificador
            if (st_lookup(t->attr.name) == NULL)
            {
                pce("Semantic error at line %d: '%s' was not declared in this scope\n",
                    t->lineno, t->attr.name);
                Error = TRUE;
            }
            else
            {
                st_add_lineno(t->attr.name, t->lineno);
            }
            break;
        case CallK:
            // Verificar se a função foi declarada
            if (st_lookup(t->attr.name) == NULL)
            {
                pce("Semantic error at line %d: '%s' was not declared in this scope\n",
                    t->lineno, t->attr.name);
                Error = TRUE;
            }
            else
            {
                st_add_lineno(t->attr.name, t->lineno);
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/* Função typeError imprime uma mensagem de erro de tipo e seta Error para TRUE */
static void typeError(TreeNode *t, char *message)
{
    fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
    Error = TRUE;
}

/* Função buildSymtab constrói a tabela de símbolos
 * por meio de um percurso em pré-ordem da árvore sintática
 */
void buildSymtab(TreeNode *syntaxTree)
{
    // Inicializar o escopo global
    enterScope("global");

    // Inserir funções built-in no escopo global
    // Função input: int input(void);
    st_insert("input", -1, location++, Integer, FuncK, false, "");
    // Função output: void output(int);
    st_insert("output", -1, location++, Void, FuncK, false, "");

    // Percorrer a árvore sintática
    traverse(syntaxTree, insertNode, leaveScope);

    // Sair do escopo global
    leaveScope(syntaxTree);

    // Imprimir a tabela de símbolos
    if (TraceAnalyze)
    {
        printSymTab(listing);
    }
}

/* Função checkNode realiza
 * verificação de tipos em um único nó da árvore
 */
static void checkNode(TreeNode *t)
{
    switch (t->nodekind)
    {
    case ExpK:
        switch (t->kind.exp)
        {
        case OpK:
            // Verificar tipos dos operandos
            if ((t->child[0]->type != Integer) || (t->child[1]->type != Integer))
                typeError(t, "Operator applied to non-integer operands.");
            // Definir tipo resultante
            if ((t->attr.op == EQ) || (t->attr.op == NE) || (t->attr.op == LT) ||
                (t->attr.op == LE) || (t->attr.op == GT) || (t->attr.op == GE))
                t->type = Boolean;
            else
                t->type = Integer;
            break;
        case ConstK:
            t->type = Integer;
            break;
        case IdK:
        {
            // Obter tipo do identificador da tabela de símbolos
            BucketList symbol = st_lookup(t->attr.name);
            if (symbol != NULL)
                t->type = symbol->type;
            else
                t->type = Integer; // Assume integer para evitar propagação de erros
            break;
        }
        case AssignK:
            // Verificar se o identificador foi declarado
            if (t->child[1] == NULL) break;
            if (t->child[1]->nodekind != ExpK) break;
            if (t->child[1]->kind.exp != CallK) break;
            if (t->child[1]->type != Void) break;
            if (st_lookup(t->child[1]->attr.name) == NULL) break;
            pce("Semantic error at line %d: invalid use of void expression\n",
                t->lineno);
            Error = TRUE;
            break;
        case CallK:
        {
            // Obter tipo da função da tabela de símbolos
            BucketList symbol = st_lookup(t->attr.name);
            if (symbol != NULL)
                t->type = symbol->type;
            else
                t->type = Void; // Assume void para evitar propagação de erros
            break;
        }
        default:
            break;
        }
        break;
    case StmtK:
        switch (t->kind.stmt)
        {
        case IfK:
            if (t->child[0]->type != Boolean)
                typeError(t->child[0], "if test expression is not Boolean");
            break;
        case WhileK:
            if (t->child[0]->type != Boolean)
                typeError(t->child[0], "while test expression is not Boolean");
            break;
        case ReturnK:
        {
            // Verificar se o tipo do retorno é compatível com o tipo da função
            BucketList symbol = st_lookup(currentFunction);
            if (symbol != NULL)
            {
                ExpType funcType = symbol->type;
                if (t->child[0] != NULL)
                {
                    // Função com retorno de valor
                    if (funcType == Void)
                        typeError(t, "Return with a value in void function");
                    else if (t->child[0]->type != funcType)
                        typeError(t, "Type mismatch in return statement");
                }
                else
                {
                    // Função sem retorno de valor
                    if (funcType != Void)
                        typeError(t, "Missing return value in function returning non-void");
                }
            }
            break;
        }
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/* Função typeCheck realiza verificação de tipos
 * por meio de um percurso em pós-ordem da árvore sintática
 */
void typeCheck(TreeNode *syntaxTree)
{
    if (!hasMain)
    {
        pc("Semantic error: undefined reference to 'main'\n");
        Error = TRUE;
    }
    traverse(syntaxTree, nullProc, checkNode);
}
