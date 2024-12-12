#ifndef _CODE_H_
#define _CODE_H_

/* Definição dos registradores */
#define ac 0   /* Accumulator */
#define ac1 1  /* Second accumulator */

#define fp 2   /* Frame pointer */
#define sp 4   /* Stack pointer, se quiser usar */

#define gp 5   /* Global pointer */
#define mp 6   /* Memory pointer */

#define PC 7   /* Program Counter */

/* Funções de emissão de código */
void emitComment(char *c);

void emitRO(char *op, int r, int s, int t, char *c);
void emitRM(char *op, int r, int d, int s, char *c);
void emitRM_Abs(char *op, int r, int a, char *c);

int emitSkip(int howMany);
void emitBackup(int loc);
void emitRestore(void);


#endif
