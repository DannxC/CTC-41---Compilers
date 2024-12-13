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

#include "hashtable.h"

static int tmpOffset = -2; // Próxima memória vazia em relação ao FP

static void cGen(TreeNode *tree);

char *currentScopeName = "global";
int compoundNum = 0;
char comment[100]; // auxiliar para comentários
bool isParamFromCallK = FALSE;

// Variáveis para controle de função main
bool isFirstFuncK = TRUE;
int main_loc = 3;

/* Gera o código para statements */
static void genStmt(TreeNode *tree)
{
    TreeNode *p1, *p2, *p3;
    int savedLoc1, savedLoc2, savedLoc3, currentLoc;
    BucketList sym;

    currentLoc = emitSkip(0);

    if (tree == NULL)
        return;

    switch (tree->kind.stmt)
    {
    case IfK:
        emitComment("-> if");

        p1 = tree->child[0]; // Condição
        cGen(p1);

        emitComment("if: jump to else belongs here");

        savedLoc1 = emitSkip(1);

        p2 = tree->child[1]; // Corpo do if
        cGen(p2);

        emitComment("if: jump to end belongs here");

        savedLoc2 = emitSkip(1);
        
        emitBackup(savedLoc1);
        emitRM_Abs("JEQ", ac, savedLoc2 + 1, "if: jmp to else");

        emitRestore();

        p3 = tree->child[2]; // Corpo do else
        if (p3 != NULL)
        {
            cGen(p3);
        }
        savedLoc3 = emitSkip(0);
        emitBackup(savedLoc2);
        emitRM_Abs("LDA", PC, savedLoc3, "jmp to end");
        
        emitRestore();
        
        emitComment("<- if");
        break;

    case WhileK:
        emitComment("-> while");
        emitComment("repeat: jump after body comes back here");

        savedLoc1 = emitSkip(0);

        p1 = tree->child[0]; // Condição
        cGen(p1);

        savedLoc2 = emitSkip(1);

        p2 = tree->child[1]; // Corpo
        cGen(p2);

        emitRM_Abs("LDA", PC, savedLoc1, "jump to savedLoc1");
        savedLoc1 = emitSkip(0);
        emitBackup(savedLoc2);
        emitRM_Abs("JEQ", ac, savedLoc1, "repeat: jmp to end");

        emitRestore();

        emitComment("<- repeat");
        break;

    case ReturnK:
        emitComment("-> return");
        p1 = tree->child[0]; // Expressão de retorno
        cGen(p1);

        emitRM("LDA", ac1, 0, fp, "save current fp into ac1");
        emitRM("LD", fp, 0, fp, "make fp = ofp");
        emitRM("LD", PC, -1, ac1, "return to caller");

        emitComment("<- return");
        break;

    case VarK:
        // emitRM("DEBUG", 0, tmpOffset, 0, "checking offset");                                             
        if (tree->isArray)
        {
            emitComment("-> declare vector");
            if (strcmp(tree->sc->funcName, "global") == 0)
            {
                int mem = st_lookup_memLoc(tree->attr.name, tree->sc);
                emitRM("LDC", ac, mem, 0, "load global position to ac");
                emitRM("LDC", gp, 0, 0, "load 0");
                emitRM("ST", ac, mem, gp, "store ac in global_position_aux");
            }
            else 
            {
                emitRM("LDA", ac, tmpOffset, fp, "guard addr of vector");
                emitRM("ST", ac, tmpOffset, fp, "store addr of vector");
                tmpOffset -= tree->child[0]->attr.val + 1;
            }
            emitComment("<- declare vector");
        }
        else
        {
            emitComment("-> declare var");
            tmpOffset--;
            emitComment("<- declare var");
        }
        break;

    case FuncK:
        sprintf(comment, "-> Init Function (%s)", tree->attr.name);
        emitComment(comment);

        int init_loc = emitSkip(0);
        if (isFirstFuncK)
        {
            main_loc = emitSkip(1);
            isFirstFuncK = FALSE;
            hashtable_set(tree->attr.name, main_loc + 1);
        }
        else
        {
            hashtable_set(tree->attr.name, init_loc);
        }

        currentScopeName = tree->attr.name;
        tmpOffset = -2; // Resetar offset temporário

        if (strcmp(tree->attr.name, "main") == 0)
        {
            savedLoc1 = emitSkip(0);
            emitBackup(main_loc);
            if (savedLoc1 == 3) emitRM_Abs("LDA", PC, savedLoc1+1, "jump to main");
            else emitRM_Abs("LDA", PC, savedLoc1, "jump to main");
            emitRestore();

            p1 = tree->child[0]; // Parâmetros
            cGen(p1);
            p2 = tree->child[1]; // Corpo da função
            cGen(p2);
        }
        else
        {
            emitRM("ST", ac, -1, fp, "store return address from ac");

            p1 = tree->child[0]; // Parâmetros
            cGen(p1);
            p2 = tree->child[1]; // Corpo da função
            cGen(p2);

            if (tree->type == Void) {
                emitRM("LDA", ac1, 0, fp, "save current fp into ac1");
                emitRM("LD", fp, 0, fp, "make fp = ofp");
                emitRM("LD", PC, -1, ac1, "return to caller");
            }
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
        if (tree->isArray) {
            emitComment("-> Param vet");
            tmpOffset--;
            emitComment("<- Param vet");
        }
        else {
            emitComment("-> Param");
            tmpOffset--;
            emitComment("<- Param");
        }


        break;

    default:
        break;
    }
}

/* Gera o código para expressões */
static void genExp(TreeNode *tree)
{
    BucketList sym;
    TreeNode *p1, *p2;

    if (tree == NULL)
        return;

    switch (tree->kind.exp)
    {
    case ConstK:
        emitComment("-> Const");
        emitRM("LDC", ac, tree->attr.val, 0, "load const");
        emitComment("<- Const");
        break;

    case IdK:
        emitComment("-> Id");
        if (tree->isArray)
        {
            p1 = tree->child[0];

            char * scope_name = st_lookup_scope_name(tree->attr.name, tree->sc);
            int mem = st_lookup_memLoc(tree->attr.name, tree->sc);

            emitComment("-> Vector");

            if (strcmp(scope_name, "global") == 0)
            {
                emitRM("LDC", gp, 0, 0, "load 0");
                emitRM("LD", ac, mem, gp, "get the address of the vector");
            }
            else 
            {
                emitRM("LD", ac, mem - MAX_MEM, fp, "get the address of the vector");
            }

            int index;
            if (p1->nodekind == ExpK && p1->kind.exp == ConstK)
            {
                index = p1->attr.val;
                emitRM("LDC", ip, index, 0, "get the value of the index");
            }
            else
            {
                int mem2 = st_lookup_memLoc(p1->attr.name, p1->sc);
                emitRM("LD", ip, mem2 - MAX_MEM, fp, "get the value of the index");
            }

            emitRM("LDC", sp, 1, 0, "load 1");
            emitRO("ADD", ip, ip, sp, "sub 3 by 1");
            emitRO("SUB", ac, ac, ip, "get the address");
            emitRM("LD", ac, 0, ac, "get the value of the vector");
        }
        else
        {
            char * scope_name = st_lookup_scope_name(tree->attr.name, tree->sc);
            int mem = st_lookup_memLoc(tree->attr.name, tree->sc);

            if (strcmp(scope_name, "global") == 0)
            {
                emitRM("LDC", gp, 0, 0, "load 0");
                emitRM("LD", ac, mem, gp, "load id value");
            }
            else
            {
                emitRM("LD", ac, mem - MAX_MEM, fp, "load id value");
            }
        }

        emitComment("<- Id");
        break;

    case OpK:
        emitComment("-> Op");
        p1 = tree->child[0];
        p2 = tree->child[1];
        /* gen code for ac = left arg */
        cGen(p1);
        /* gen code to push left operand */
        emitRM("ST", ac, tmpOffset--, fp, "op: push left");
        /* gen code for ac = right operand */
        cGen(p2);
        /* now load left operand */
        emitRM("LD", ac1, ++tmpOffset, fp, "op: load left");
        switch (tree->attr.op)
        {
            case PLUS:
                emitRO("ADD", ac, ac1, ac, "op +");
                break;
            case MINUS:
                emitRO("SUB", ac, ac1, ac, "op -");
                break;
            case TIMES:
                emitRO("MUL", ac, ac1, ac, "op *");
                break;
            case OVER:
                emitRO("DIV", ac, ac1, ac, "op /");
                break;
            case LT:
                emitRO("SUB", ac, ac1, ac, "op <");
                emitRM("JLT", ac, 2, PC, "br if true");
                emitRM("LDC", ac, 0, ac, "false case");
                emitRM("LDA", PC, 1, PC, "unconditional jmp");
                emitRM("LDC", ac, 1, ac, "true case");
                break;
            case LE:
                emitRO("SUB", ac, ac1, ac, "op <=");
                emitRM("JLE", ac, 2, PC, "br if true");
                emitRM("LDC", ac, 0, ac, "false case");
                emitRM("LDA", PC, 1, PC, "unconditional jmp");
                emitRM("LDC", ac, 1, ac, "true case");
                break;
            case GT:
                emitRO("SUB", ac, ac1, ac, "op >");
                emitRM("JGT", ac, 2, PC, "br if true");
                emitRM("LDC", ac, 0, ac, "false case");
                emitRM("LDA", PC, 1, PC, "unconditional jmp");
                emitRM("LDC", ac, 1, ac, "true case");
                break;
            case GE:
                emitRO("SUB", ac, ac1, ac, "op >=");
                emitRM("JGE", ac, 2, PC, "br if true");
                emitRM("LDC", ac, 0, ac, "false case");
                emitRM("LDA", PC, 1, PC, "unconditional jmp");
                emitRM("LDC", ac, 1, ac, "true case");
                break;
            case EQ:
                emitRO("SUB", ac, ac1, ac, "op ==");
                emitRM("JEQ", ac, 2, PC, "br if true");
                emitRM("LDC", ac, 0, ac, "false case");
                emitRM("LDA", PC, 1, PC, "unconditional jmp");
                emitRM("LDC", ac, 1, ac, "true case");
                break;
            case NE:
                emitRO("SUB", ac, ac1, ac, "op !=");
                emitRM("JNE", ac, 2, PC, "br if true");
                emitRM("LDC", ac, 0, ac, "false case");
                emitRM("LDA", PC, 1, PC, "unconditional jmp");
                emitRM("LDC", ac, 1, ac, "true case");
                break;
            default:
                emitComment("BUG: Unknown operator");
                break;
        } /* case op */

        emitComment("<- Op");
        break; /* OpK */

    case AssignK:
        // emitRM("DEBUG", 0, tmpOffset, 0, "checking offset");                                             
        p1 = tree->child[0];
        if (p1->isArray)
        {
            emitComment("-> assign vector");
            emitComment("-> Vector");

            p2 = tree->child[1];
            cGen(p2); // acc tem o resultado do cGen de p2

            char * scope_name = st_lookup_scope_name(p1->attr.name, p1->sc);
            // sprintf(comment, "DEBUG: SCOPE NAME =  %s", scope_name);
            // emitComment(comment);

            if (strcmp(scope_name, "global") == 0)
            {
                emitRM("LDC", gp, 0, 0, "load 0");
                int mem = st_lookup_memLoc(p1->attr.name, p1->sc);
                emitRM("LD", ac1, mem, gp, "get the address of the vector");
            }
            else
            {
                int mem = st_lookup_memLoc(p1->attr.name, p1->sc);
                emitRM("LD", ac1, mem - MAX_MEM, fp, "get the address of the vector");
            }


            TreeNode *p1_p1 = p1->child[0];
            int index;
            int mem2;
            if (p1_p1->nodekind == ExpK && p1_p1->kind.exp == ConstK)
            {
                index = p1_p1->attr.val;
                emitRM("LDC", ip, index, 0, "get the value of the index");
            }
            else
            {
                mem2 = st_lookup_memLoc(p1_p1->attr.name, p1_p1->sc);
                emitRM("LD", ip, mem2 - MAX_MEM, fp, "get the value of the index");
            }


            emitRM("LDC", sp, 1, 0, "load 1");
            emitRO("ADD", ip, ip, sp, "sub 3 by 1");
            emitRO("SUB", ac1, ac1, ip, "get the address");
            emitRM("ST", ac, 0, ac1, "get the value of the vector");
        }
        else
        {
            emitComment("-> assign");

            cGen(tree->child[1]);

            int mem = st_lookup_memLoc(p1->attr.name, p1->sc);
            emitRM("ST", ac, mem - MAX_MEM, fp, "assign: store value");

            emitComment("<- assign");
        }

        break;

    case CallK:
        sprintf(comment, "-> Function Call (%s)", tree->attr.name);
        emitComment(comment);

        if (strcmp(tree->attr.name, "input") == 0)
        {
            emitRO("IN", ac, 0, 0, "read input");
        }
        else if (strcmp(tree->attr.name, "output") == 0) 
        {
            cGen(tree->child[0]);
            emitRO("OUT", ac, 0, 0, "print value");
        }
        else
        {
            int aux = tmpOffset;
            emitRM("ST", fp, tmpOffset, fp, "Guard fp");
            tmpOffset -= 2;

            p1 = tree->child[0]; // Parâmetros

            isParamFromCallK = TRUE;
            while (p1 != NULL)
            {
                cGen(p1);

                emitRM("ST", ac, tmpOffset--, fp, "Store value of func argument");

                p1 = p1->sibling;
            }
            isParamFromCallK = FALSE;
            tmpOffset = aux;
            
            emitRM("LDA", fp, tmpOffset, fp, "change fp");
            int savedLoc1 = emitSkip(0);
            emitRM("LDC", ac, savedLoc1 + 2, 0, "Guard return adress");
            int firstInstruction = hashtable_get(tree->attr.name);
            // emitRM("DEBUG", 0,firstInstruction, 0,"first instruction");
            emitRM_Abs("LDA", PC, firstInstruction , "jump to function");

            emitComment("<- Function Call");
        }

        break;

    default:
        break;
    }
}

/**
 * Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen(TreeNode *tree)
{
    if (tree == NULL)
        return;
    switch (tree->nodekind)
    {
    case StmtK:
        genStmt(tree);
        break;
    case ExpK:
        genExp(tree);
        break;
    default:
        break;
    }
    if (!isParamFromCallK)
        cGen(tree->sibling); // modifiquei para prints corretos no caso do CallK
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/**
 * Procedure codeGen generates code to a code
 * file by traversal of the syntax tree.
 */
void codeGen(TreeNode *syntaxTree)
{
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
