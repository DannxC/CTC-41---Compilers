/****************************************************/
/* File: symtab.c                                    */
/* Symbol table implementation for the C- compiler   */
/****************************************************/

#include "symtab.h"
#include "util.h"

/* Global variable for current scope */
Scope currentScope = NULL;

/* Global variable for scope list */
Scope scopeList = NULL; // Lista de escopos

/* Hash function */
static int hash(char *key)
{
    int temp = 0;
    int i = 0;
    while (key[i] != '\0')
    {
        temp = ((temp << 4) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/* Insert a symbol into the symbol table */
void st_insert(char *name, int lineno, int loc, ExpType type, StmtKind kind, bool isArray, char *scope)
{
    int h = hash(name);
    BucketList l = currentScope->hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) /* Variable not yet in table */
    {
        l = (BucketList)malloc(sizeof(struct BucketListRec));
        l->name = copyString(name);
        l->lines = (LineList)malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->lines->next = NULL;
        l->memloc = loc;
        l->type = type;
        l->kind = kind;
        l->isArray = isArray;
        l->scope = copyString(scope);
        l->next = currentScope->hashTable[h];
        currentScope->hashTable[h] = l;
    }
    else /* Found in table, just add line number */
    {
        st_add_lineno(name, lineno);
    }
}

int st_lookup_memLoc(char *name, Scope scope)
{
    while (scope != NULL)
    {
        int h = hash(name);
        BucketList l = scope->hashTable[h];
        while ((l != NULL) && (strcmp(name, l->name) != 0))
            l = l->next;
        if (l != NULL)
            return l->memloc; /* Found */
        scope = scope->parent;
    }

    return -1; /* Not found */
}   

/* Lookup a symbol in all scopes */
BucketList st_lookup(char *name)
{
    Scope sc = currentScope;
    while (sc != NULL)
    {
        int h = hash(name);
        BucketList l = sc->hashTable[h];
        while ((l != NULL) && (strcmp(name, l->name) != 0))
            l = l->next;
        if (l != NULL)
            return l; /* Found */
        sc = sc->parent;
    }
    return NULL; /* Not found */
}

/* Lookup a symbol in the current scope */
BucketList st_lookup_top(char *name)
{
    int h = hash(name);
    BucketList l = currentScope->hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    return l; /* NULL if not found */
}

/* Add a line number to a symbol */
void st_add_lineno(char *name, int lineno)
{
    BucketList l = st_lookup(name);

    if (l != NULL)
    {
        LineList t = l->lines;
        LineList prev = NULL;
        // Verificar se o número de linha já existe
        while (t != NULL)
        {
            if (t->lineno == lineno)
                return; // Número de linha já existe
            prev = t;
            t = t->next;
        }
        // Adicionar novo número de linha
        LineList newLine = (LineList)malloc(sizeof(struct LineListRec));
        newLine->lineno = lineno;
        newLine->next = NULL;
        prev->next = newLine;
    }
}

/* Print the symbol table */
void printSymTab(FILE *listing)
{
    int i;
    pc("\nSymbol table:\n\n"); // Adiciona linha em branco no início
    pc("Variable Name  Scope     ID Type  Data Type  Line Numbers\n");
    pc("-------------  --------  -------  ---------  -------------------------\n");
    Scope sc = scopeList;
    while (sc != NULL)
    {
        for (i = 0; i < SIZE; ++i)
        {
            if (sc->hashTable[i] != NULL)
            {
                BucketList l = sc->hashTable[i];
                while (l != NULL)
                {
                    pc("%-14s", l->name);
                    if (strcmp(l->scope, "global") == 0)
                        pc(" %-9s", ""); // Não imprime escopo para global
                    else
                        pc(" %-9s", l->scope);
                    // Ajustar o tipo
                    if (l->kind == FuncK)
                        pc(" %-8s", "fun");
                    else if (l->isArray)
                        pc(" %-8s", "array"); // Inclui VarK e ParamK como "var"
                    else {
                        pc(" %-8s", "var"); // Inclui VarK e ParamK como "var"
                    }
                    // Tipo de dado
                    switch (l->type)
                    {
                    case Integer:
                        pc(" %-9s", "int");
                        break;
                    case Void:
                        pc(" %-9s", "void");
                        break;
                    default:
                        pc(" %-9s", "unknown");
                        break;
                    }
                    // Imprimir números de linha
                    LineList t = l->lines;
                    if (t != NULL) pc("  ");
                    while (t != NULL)
                    {
                        if (t->lineno > 0) // Ignorar linhas negativas ou zero
                            pc("%2d ", t->lineno);
                        t = t->next;
                    }
                    pc("\n");
                    l = l->next;
                }
            }
        }
        sc = sc->next; // Próximo escopo na lista
    }
}
