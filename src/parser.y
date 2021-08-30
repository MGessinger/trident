%{
	#include <stdio.h>
	extern int yylex();
	static int yyerror();
%}

%union
{
	char * string;
	void * pointer;
}

%token keyword_job keyword_cmd keyword_out keyword_file
%token token_file token_ident token_any

%type <string> token_file token_ident token_any
%%
INPUT   : 
	| INPUT JOB				{ printf("Finished reading a JOB.\n"); }
	;

JOB : JOB_HEAD COMMAND 
    | JOB_HEAD COMMAND TARGETS
    ;

JOB_HEAD : keyword_job token_ident		{ printf("Began reading a JOB: %s\n", $2); free($2); }
	 ;

COMMAND : keyword_cmd token_file		{ printf("Read a command: %s\n", $2); free($2); }
	| keyword_cmd token_ident		{ printf("Read a command: %s\n", $2); free($2); }
	| COMMAND token_any			{ printf("Additional argument: %s\n", $2); free($2); }
	;

TARGETS : keyword_out '-' TARGET		{ printf("Read a target.\n"); }
	| TARGETS '-' TARGET
	;

TARGET : keyword_job token_ident		{ free($2); }
       | keyword_file token_file		{ free($2); }
       | keyword_file token_ident		{ free($2); }
       | keyword_job REGEX token_ident		{ free($3); }
       | keyword_file REGEX token_file		{ free($3); }
       | keyword_file REGEX token_ident		{ free($3); }
       ;

REGEX : token_any			{ printf("Read a \"regex\": /%s/\n", $1); free($1); }
      ;
%%
int yyerror()
{
	return 1;
}
