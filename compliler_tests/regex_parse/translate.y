%{
#include <stdlib.h>
#include <ctype.h>
#include "symbol_table.h"
int inn = 0;
char infix_expr[4096];
#define GETC(c) (infix_expr[inn++])
int outn = 0;
char suffix_expr[4096];
#define PUTC(c) (suffix_expr[outn++] = c);
int yylex();
int yyerror(char *e);
%}


%token ALPHA

%%
line : expr '\n' { PUTC('\0'); YYACCEPT; }
     ;
expr : expr '|' or_r { PUTC('|'); }
     | or_r
     ;
or_r : or_r dot_r { PUTC('-'); }
     | dot_r
     ;
dot_r : factor '*' { PUTC('*'); }
      | factor
      ;
factor : ALPHA { PUTC((char)$1); }
       | '(' expr ')'
       ;
%%
#include <stdio.h>
#include <string.h>

extern struct symbol_table symbol_tbl;

int yylex()
{
	int c;
	c = GETC();
	if (isalpha(c)) {
		yylval = c;
		symbol_add(&symbol_tbl, (char) c);
		return ALPHA;
	}
	return c;
}

int yyerror(char *e)
{
	fprintf(stderr, "%s\n", e);
	return 0;
}

/* Nonreentrant */
int to_suffix_expr(const char *inf, char *suf, int olen)
{
	int ret;
	strncpy(infix_expr, inf, 4096-1);
	strcat(infix_expr, "\n");
	ret = yyparse();
	if (!ret)
		strncpy(suf, suffix_expr, olen);
	return ret;
}
