%{
	#include <stdio.h>
	#include "hash.h"
	//int yydebug = 1;
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

%left  '+'  '-'
%left  '*'  '/'
%%

// ------------ The definition of lang152 ---------------


// ------------ Function processing ---------------
lang152: function lang152
		| global_variable_list
		;

function: function_header function_variable_list command ';'
		;

function_header:  KW_INT TK_IDENTIFIER '(' function_parameters ')'
				| KW_BOOL TK_IDENTIFIER '(' function_parameters ')'
				| KW_CHAR TK_IDENTIFIER '(' function_parameters ')'
				| KW_REAL TK_IDENTIFIER '(' function_parameters ')'
				;

function_parameters: list_parameters
					| %empty
					;

list_parameters: parameter optional_parameter_list
						;
optional_parameter_list: ',' list_parameters
						| %empty
						;

parameter: KW_INT TK_IDENTIFIER
		 | KW_BOOL TK_IDENTIFIER
		 | KW_CHAR TK_IDENTIFIER
		 | KW_REAL TK_IDENTIFIER
		 ;

function_variable_list: function_variable function_variable_list
						| %empty
						;

function_variable:    KW_INT TK_IDENTIFIER  ':' initial_value ';'
		 			| KW_BOOL TK_IDENTIFIER ':' initial_value ';'
					| KW_CHAR TK_IDENTIFIER ':' initial_value ';'
		 			| KW_REAL TK_IDENTIFIER ':' initial_value ';'



// ------------ Command parsing ---------------
command: simple_command
	   	| command_block
		;

simple_command: attribution_command
			  	| flow_control
				| input_command
				| output_command
				| return_command
				| %empty
				;

attribution_command: command_variable ':' '=' expression
		   			| expression '=' ':' command_variable
		   			;

command_variable: TK_IDENTIFIER optional_variable_index
				;
optional_variable_index: '[' expression ']'
					   	| %empty
						;


input_command: KW_INPUT TK_IDENTIFIER
			 ;

output_command: KW_OUTPUT output_list
			  ;

output_list: output_element optional_output_list
		   ;

optional_output_list : ',' output_list
					 | %empty
					 ;

output_element: LIT_STRING
			  | expression
			  ;

return_command: KW_RETURN expression
			  ;

flow_control: KW_IF '(' expression ')' command optional_flow_control
			;
optional_flow_control: KW_ELSE command
					 | KW_LOOP
					 | %empty
					 ;

command_block: '{' optional_command_block
			 ;

optional_command_block: command_block_list 
					  | '}'
					  ;

command_block_list: command optional_command
				  ;

optional_command: ';' command_block_list
				| '}'
				;

// ------------ Expression parsing -----------------

expression: expression '+' expression
		 | expression '-' expression
		 | expression '*' expression
		 | expression '/' expression
		 | expression '>' expression
		 | expression '<' expression
		 | expression OPERATOR_LE expression
		 | expression OPERATOR_GE expression
		 | expression OPERATOR_EQ expression
		 | expression OPERATOR_NE expression
		 | expression OPERATOR_AND expression
		 | expression OPERATOR_OR expression
		 | '(' expression ')'
		 | LIT_INTEGER
		 | LIT_TRUE
		 | LIT_FALSE
		 | LIT_CHAR
		 | parse_tk_identifier
		 ;

parse_tk_identifier: TK_IDENTIFIER possible_function_call
			;

possible_function_call: '(' function_arguments ')'
					  | %empty
					  ;

function_arguments: list_arguments
					| %empty
					;

list_arguments: argument optional_argument_list
						;
optional_argument_list: ',' list_arguments
						| %empty
						;
argument: TK_IDENTIFIER
		| LIT_INTEGER
		| LIT_TRUE
		| LIT_FALSE
		| LIT_CHAR
		;

// ------------ Global variables parsing -----------------

global_variable_list: global_variable global_variable_list
					| %empty
					;



global_variable:  KW_INT  TK_IDENTIFIER  normalOrVector
		 		| KW_BOOL TK_IDENTIFIER normalOrVector
				| KW_CHAR TK_IDENTIFIER normalOrVector
		 		| KW_REAL TK_IDENTIFIER normalOrVector
		 		;

normalOrVector:	':' initial_value ';'
			  |  '[' LIT_INTEGER ']' global_vector_initialization
			  ;

global_vector_initialization: ':' global_vector_initial_values ';'
							| ';'
							;

global_vector_initial_values: initial_value global_vector_initial_values
							| %empty
							;

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
	exit(3);
}
