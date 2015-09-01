%{
	#include <stdio.h>
	int yydebug = 1;
%}

%token KW_INT        256
%token KW_REAL       257
%token KW_BOOL       258
%token KW_CHAR       259
%token KW_IF         261
%token KW_ELSE       263
%token KW_LOOP       264
%token KW_INPUT      266
%token KW_RETURN     267
%token KW_OUTPUT     268

%token OPERATOR_LE   270
%token OPERATOR_GE   271
%token OPERATOR_EQ   272
%token OPERATOR_NE   273
%token OPERATOR_AND  274
%token OPERATOR_OR   275

%token TK_IDENTIFIER 280
%token LIT_INTEGER   281
%token LIT_FALSE     283
%token LIT_TRUE	  	 284
%token LIT_CHAR      285
%token LIT_STRING    286

%token TOKEN_ERROR   290

%%

// ------------ The definition of lang152 ---------------
lang152: function_group 
		|
		;


// ------------ Function processing ---------------
function_group: function optional_function_group
				;
optional_function_group: function_group
						|
						;

function: function_header function_variable_list command ';'

function_header: KW_INT TK_IDENTIFIER '(' function_parameters ')'
				| KW_BOOL TK_IDENTIFIER '(' function_parameters ')'
				| KW_CHAR TK_IDENTIFIER '(' function_parameters ')'
				| KW_REAL TK_IDENTIFIER '(' function_parameters ')'
				;

function_parameters: list_parameters
					|
					;

list_parameters: parameter optional_parameter_list
						;
optional_parameter_list: ',' list_parameters
						|
						;

parameter: KW_INT TK_IDENTIFIER
		 | KW_BOOL TK_IDENTIFIER
		 | KW_CHAR TK_IDENTIFIER
		 | KW_REAL TK_IDENTIFIER
		 ;

function_variable_list: function_variable function_variable_list
						|
						;

function_variable:    KW_INT TK_IDENTIFIER  ':' initial_value ';'
		 			| KW_BOOL TK_IDENTIFIER ':' initial_value ';'
					| KW_CHAR TK_IDENTIFIER ':' initial_value ';'
		 			| KW_REAL TK_IDENTIFIER ':' initial_value ';'

command: ;



// ------------ Function processing ---------------

// ------------ General purpose -----------------
initial_value: LIT_INTEGER
			 | LIT_TRUE
			 | LIT_FALSE
			 | LIT_CHAR
			 ;


%%

int yyerror(char *s)
{
	fprintf(stderr, "%s\n", s);
}