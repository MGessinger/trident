%{
	#include <stdio.h>
	#include <unistd.h>
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
%token token_file token_ident token_regex token_any

%type <string> token_file token_ident token_any token_regex
%type <string> JOB_HEAD
%type <pointer> JOB COMMAND TARGETS TARGET REGEX
%%
INPUT : %empty
      | INPUT JOB				{ executeJob($2, STDIN_FILENO); clearJob($2); }
      | INPUT error				{ ERROR("Malformed JOB", @2); }
      ;

JOB : JOB_HEAD COMMAND TARGETS			{ $$ = newJob($1, $2, $3); }
    | JOB_HEAD COMMAND error			{ free($1); clearCommand($2); $$ = NULL; YYERROR; }
    | JOB_HEAD error				{ free($1); $$ = NULL; ERROR("Missing command", @2); }
    ;

JOB_HEAD : keyword_job token_ident		{ $$ = $2; }
	 | keyword_job error			{ $$ = NULL; ERROR("Missing job name", @2); }
	 ;

COMMAND : COMMAND token_any			{ $$ = appendArgument($1, $2); }
	| keyword_cmd token_file		{ $$ = newCommand($2); }
	| keyword_cmd token_ident		{ $$ = newCommand($2); }
	| keyword_cmd error			{ $$ = NULL; ERROR("Missing command name", @2); }
	;

TARGETS : keyword_out '-' TARGET		{ $$ = newTargets($3); }
	| TARGETS '-' TARGET			{ $$ = appendTarget($1, $3); }
	| TARGETS '-' error			{ $$ = NULL; clearTargets($1); ERROR("Invalid Target", @2); }
	;

TARGET : keyword_job token_ident		{ $$ = newSingleTarget($2, TARGET_JOB); }
       | keyword_file token_file		{ $$ = newSingleTarget($2, TARGET_FILE); }
       | keyword_file token_ident		{ $$ = newSingleTarget($2, TARGET_FILE); }
       | keyword_job error			{ $$ = NULL; ERROR("Missing target job name", @2); }
       | keyword_file error			{ $$ = NULL; ERROR("Missing target file name", @2); }
       | REGEX TARGET				{ $$ = addFilter($2, $1); }
       ;

REGEX : token_regex				{ $$ = compileRegex($1); if($$ == NULL) YYERROR; }
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
		char buffer[128];
		regerror(ret, r, buffer, 128);
		fprintf(stderr, "RegEx Compilation Failed: %s\n", buffer);
		free(r);
		return NULL;
	}
	return r;
}

int yyerror()
{
	return 1;
}
