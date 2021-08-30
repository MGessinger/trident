%{
	#include <stdio.h>
	#include "jobs.h"
	extern int yylex();
	static int yyerror();
	regex_t * compileRegex (char * string);

	const char * filename = "stdin";
	#define ERROR(str, loc) { \
		fprintf(stderr, "%s:%i:0: Syntax Error: %s!\n", filename, loc.first_line, str);\
		YYERROR;\
	}
%}

%union
{
	char * string;
	void * pointer;
}

%token keyword_job keyword_cmd keyword_out keyword_file
%token token_file token_ident token_any

%type <string> token_file token_ident token_any
%type <string> JOB_HEAD
%type <pointer> JOB COMMAND TARGETS TARGET REGEX
%%
INPUT   : 
	| INPUT JOB				{ dumpJob($2); clearJob($2); }
	| INPUT error				{ ERROR("Malformed JOB", @2); }
	;

JOB : JOB_HEAD COMMAND				{ $$ = newJob($1, $2, NULL); }
    | JOB_HEAD COMMAND TARGETS			{ $$ = newJob($1, $2, $3); }
    | JOB_HEAD error				{ ERROR("Missing command", @2); $$ = NULL; }
    ;

JOB_HEAD : keyword_job token_ident		{ $$ = $2; }
	 | keyword_job error			{ ERROR("Missing job name", @2); $$ = NULL; }
	 ;

COMMAND : COMMAND token_any			{ $$ = appendArgument($1, $2); }
	| keyword_cmd token_file		{ $$ = newCommand($2); }
	| keyword_cmd token_ident		{ $$ = newCommand($2); }
	| keyword_cmd error			{ ERROR("Missing command name", @2); $$ = NULL; }
	;

TARGETS : keyword_out '-' TARGET		{ $$ = newTargets($3); }
	| TARGETS '-' TARGET			{ $$ = appendTarget($1, $3); }
	| TARGETS '-' error			{ ERROR("Invalid Target", @2); $$ = NULL; }
	;

TARGET : keyword_job token_ident		{ $$ = newSingleTarget($2, NULL, TARGET_JOB); }
       | keyword_file token_file		{ $$ = newSingleTarget($2, NULL, TARGET_FILE); }
       | keyword_file token_ident		{ $$ = newSingleTarget($2, NULL, TARGET_FILE); }
       | keyword_job REGEX token_ident		{ $$ = newSingleTarget($3, $2, TARGET_JOB); }
       | keyword_file REGEX token_file		{ $$ = newSingleTarget($3, $2, TARGET_FILE); }
       | keyword_file REGEX token_ident		{ $$ = newSingleTarget($3, $2, TARGET_FILE); }
       | keyword_job REGEX error		{ regfree($2); ERROR("Missing target job name", @3); $$ = NULL; }
       | keyword_file REGEX error		{ regfree($2); ERROR("Missing target file name", @3); $$ = NULL; }
       ;

REGEX : token_any				{ $$ = compileRegex($1); }
      ;
%%
regex_t * compileRegex (char * string)
{
	regex_t * r = malloc(sizeof(regex_t));
	if (r == NULL)
		return NULL;

	int ret = regcomp(r, string, REG_NOSUB | REG_NEWLINE);
	free(string);
	if (ret != 0)
	{
		free(r);
		return NULL;
	}
	return r;
}

int yyerror()
{
	return 1;
}
