/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation                */
/* for the C- compiler                              */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "code.h"
#include "cgen.h"
#include <string.h> /* para strcmp */

static int tmpOffset = -2;  // Próxima memória vazia em relação ao FP

static void cGen(TreeNode *tree);

char * currentScopeName = "global";
int compoundNum = 0;
char comment[100];  // auxiliar para comentários

/* Gera o código para statements */
static void genStmt(TreeNode *tree) {
    TreeNode *p1, *p2, *p3;
    int savedLoc1, savedLoc2, currentLoc;
    BucketList sym;
    
    currentLoc = emitSkip(0);

    if (tree == NULL) return;

    switch (tree->kind.stmt) {
    case IfK:
        emitComment("-> if");
        // code
        emitComment("<- if");
        break;

    case WhileK:
        emitComment("-> while");
        // code
        emitComment("<- while");
        break;

    case ReturnK:
        emitComment("-> return");
        // code
        emitComment("<- return");
        break;

    case VarK:
        if (tree->isArray) {
            emitComment("-> declare vector");
            emitRM("LDA", ac, tmpOffset, fp, "guard addr of vector");
            emitRM("ST", ac, tmpOffset, fp, "store addr of vector");
            tmpOffset -= tree->child[0]->attr.val + 1;
            emitComment("<- declare vector");
        } else {
            emitComment("-> declare var");
            tmpOffset--;
            emitComment("<- declare var");
        }
        break;

    case FuncK:
        sprintf(comment, "-> Init Function (%s)", tree->attr.name);
        emitComment(comment);

        currentScopeName = tree->attr.name;

        tmpOffset = -2;  // Resetar offset temporário
        if (strcmp(tree->attr.name, "main") == 0) {
            emitBackup(3);
            if (currentLoc == 3) {
                emitRM_Abs("LDA", PC, currentLoc + 1, "");
            } else {
                emitRM_Abs("LDA", PC, currentLoc, "");
            }
            emitRestore();
        }

        p1 = tree->child[0]; // Parâmetros
        while (p1 != NULL) {
            emitComment("-> Param");

            tmpOffset--;

            emitComment("<- Param");

            p1 = p1->sibling;
        }
        
        p2 = tree->child[1]; // Corpo da função
        cGen(p2);

        if (strcmp(tree->attr.name, "main") == 0) {
            // code
        } else {
            // code
        }

        emitComment("<- End Function");
        break;

    case CompoundK:
        
        sprintf(currentScopeName, "compound%d", ++compoundNum);

        p1 = tree->child[0]; // Declarações locais
        cGen(p1);
        p2 = tree->child[1]; // Lista de statements
        cGen(p2);
        break;

    case ParamK:
        break;

    default:
        break;
    }
}

/* Gera o código para expressões */
static void genExp(TreeNode *tree) {
    BucketList sym;
    TreeNode *p1, *p2;

    if (tree == NULL) return;

    switch (tree->kind.exp) {
    case ConstK:
        emitComment("-> Const");
        emitRM("LDC", ac, tree->attr.val, 0, "load const");
        emitComment("<- Const");
        break;

    case IdK:
        emitComment("-> Id");
        // code
        emitComment("<- Id");
        break;

    case OpK:
        emitComment("-> Op");
        // code
        switch (tree->attr.op) {
        case PLUS:
            // code
            break;
        case MINUS:
            // code
            break;
        case TIMES:
            // code
            break;
        case OVER:
            // code
            break;
        case LT:
            // code
            break;
        case EQ:
            // code
            break;
        default:
            emitComment("BUG: Unknown operator");
            break;
        }
        emitComment("<- Op");
        break;

    case AssignK:
        emitComment("-> assign");
        p2 = tree->child[1];
        cGen(p2);   // acc tem o resultado do cGen de p2

        p1 = tree->child[0];

        sprintf(comment, "TESTANDO NOME ESCOPO: %s", p1->sc->parent->funcName);
        emitComment(comment);

        int mem = st_lookup_memLoc(p1->attr.name, p1->sc);
        emitRM("ST", ac, mem - MAX_MEM, fp, "assign: store value");

        
        emitComment("<- assign");
        break;

    case CallK:
        emitComment("-> Call");
        // code
        emitComment("<- Call");
        break;

    default:
        break;
    }
}

/**
 * Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen(TreeNode *tree) {
    if (tree == NULL) return;
    switch (tree->nodekind) {
    case StmtK:
        genStmt(tree);
        break;
    case ExpK:
        genExp(tree);
        break;
    default:
        break;
    }
    if (!(tree->nodekind == StmtK && tree->kind.stmt == FuncK)) cGen(tree->sibling); // modifiquei para prints corretos no caso do FucnK
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/**
 * Procedure codeGen generates code to a code
 * file by traversal of the syntax tree.
 */
void codeGen(TreeNode *syntaxTree) {
    emitComment("TINY Compilation to TM Code");

    /* generate standard prelude */
    emitComment("Standard prelude:");
    emitRM("LD", mp, 0, 0, "load maxaddress from location 0");
    emitRM("LD", fp, 0, 0, "load maxaddress from location 0");
    emitRM("ST", ac, 0, 0, "clear location 0");
    emitComment("End of standard prelude.");

    /* Generate code for CMINuS program */
    currentScopeName = "global";
    cGen(syntaxTree);

    emitComment("End of execution.");
    emitRO("HALT", 0, 0, 0, "");
}
