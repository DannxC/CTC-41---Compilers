/****************************************************/
/* File: cminus.l                                   */
/* Lex specification for C-                         */
/* Project for CES41: Compiladores                  */
/****************************************************/

%option noyywrap 
/* opção noyywrap pode ser necessária para novas versões do flex
  limitação: não compila mais de um arquivo fonte de uma só vez (não precisamos disso)
  https://stackoverflow.com/questions/1480138/undefined-reference-to-yylex 
*/ 

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
void printLine(void);
%}

digit       [0-9]
letter      [a-zA-Z]
identifier  {letter}{letter}*
number      {digit}+
newline     \n|\r\n
whitespace  [ \t]+

%%

"/*"([^*]|\*+[^*/])*\*+"/" { 
    strcpy(tokenString, yytext);
    return COMMENT;
}

"void"          { return VOID; }
"int"           { return INT; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"return"        { return RETURN; }

"=="            { return EQ; }
"!="            { return NE; }
"<="            { return LE; }
">="            { return GE; }
"<"             { return LT; }
">"             { return GT; }
"="             { return ASSIGN; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return OVER; }
";"             { return SEMI; }
","             { return COMMA; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }

{identifier}    { strcpy(tokenString, yytext); return ID; }
{number}        { strcpy(tokenString, yytext); return NUM; }

{whitespace}    { /* Ignora espaços em branco */ }
{newline}       { printLine(); /* Não incrementa lineno aqui */ }

.               { strcpy(tokenString, yytext); return ERROR;}

%%
TokenType getToken(void)
{ 
  static int firstTime = TRUE;
  TokenType currentToken;

  if (firstTime)
  {
      firstTime = FALSE;
      lineno = 1;
      yyin = source;
      yyout = listing;
  }

  currentToken = yylex();
  strncpy(tokenString, yytext, MAXTOKENLEN);

  if (TraceScan) 
  {
    pc("\t%d: ", lineno);
    printToken(currentToken, tokenString);
  }

  return currentToken;
}
