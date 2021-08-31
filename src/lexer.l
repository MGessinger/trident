%{
	#define YY_NO_INPUT
	#define YY_NO_UNPUT
	#include <string.h>
	#include "parser.h"
	#define YY_USER_ACTION { yylloc.first_line = yylineno; }
	char * unescape (char * string, size_t len);
%}
%option yylineno

IDENT	[[:alpha:]_][[:alnum:]_]*
FILE	([^[:space:]!$'&*()+]|(\\[[:space:]!$'&*()+]))+

ANY	[^[:space:]]|\\[[:space:]]
%%
"#".*\n			|
[[:space:]]		

JOB			return keyword_job;
CMD			return keyword_cmd;
OUT			return keyword_out;
FILE			return keyword_file;

"-"			return yytext[0];
"/"([^\/]|\\\/)+"/"	{ yylval.string = strndup(yytext + 1, yyleng - 2); return token_regex; }
"\""([^\"]|\\\")+"\""	{ yylval.string = unescape(yytext + 1, yyleng - 2); return token_any; }

{IDENT}			{ yylval.string = strndup(yytext, yyleng); return token_ident; }
{FILE}			{ yylval.string = unescape(yytext, yyleng); return token_file; }

{ANY}+			{ yylval.string = unescape(yytext, yyleng); return token_any; }
%%
int yywrap ()
{
	return 1;
}

char * unescape (char * string, size_t len)
{
	size_t i, j;
	char * out;

	i = j = 0;
	out = malloc((len + 1) * sizeof(char));
	if (out == NULL)
		return NULL;

	while (i < len)
	{
		if (string[i] == '\\')
			i++;

		out[j] = string[i];
		i++;
		j++;
	}
	out[j] = '\0';
	return out;
}
