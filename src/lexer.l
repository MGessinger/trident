%{
	#define YY_NO_INPUT
	#define YY_NO_UNPUT
	#include <string.h>
	#include "parser.h"
%}
%option yylineno

NAME	[[:alnum:]_.-]+
FILE	(\/?({NAME}\/)*)?{NAME}

IDENT	[[:alpha:]_][[:alnum:]_]*

ANY	[^[:space:]]|\\[[:space:]]
%%
"#".*\n		|
[[:space:]]	

JOB		return keyword_job;
CMD		return keyword_cmd;
OUT		return keyword_out;
FILE		return keyword_file;

"-"		return yytext[0];

{IDENT}		{ yylval.string = strndup(yytext, yyleng); return token_ident; }
{FILE}		{ yylval.string = strndup(yytext, yyleng); return token_file; }
{ANY}+		{ yylval.string = strndup(yytext, yyleng); return token_any; }
%%
int yywrap ()
{
	return 1;
}