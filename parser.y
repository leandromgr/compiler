/****INSTITUTO DE INFORMATICA UFRGS****
Leandro Mateus Giacomini Rocha
Luciano Farias Puhl
*/

%{
    #include <stdio.h>
    #include "hash.h"
    #include "ast.h"
	//int yydebug = 1;
%}

%union{
    HASH_NODE * hashNode;
    AST_NODE * astNode;
}

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

%token <hashNode>TK_IDENTIFIER  280
%token <hashNode>LIT_INTEGER    281
%token <hashNode>LIT_FALSE      283
%token <hashNode>LIT_TRUE       284
%token <hashNode>LIT_CHAR       285
%token <hashNode>LIT_STRING     286

%token TOKEN_ERROR   290

%left  '+'  '-'
%left  '*'  '/'

%type <astNode>expression
%type <astNode>attribution_command
%type <astNode>parse_tk_identifier
//%type <astNode>flow_control


//FRIDAY
%type <astNode>global_variable_list
%type <astNode>global_variable
%type <astNode>normal_or_vector
%type <astNode>global_vector_initial_values
%type <astNode>global_vector_initialization
%type <hashNode>initial_value

//SATURDAY
%type <astNode>lang152

%type <astNode>function
%type <astNode>function_header
%type <astNode>function_parameters
%type <astNode>optional_parameter_list
%type <astNode>parameter

%type <astNode>function_variable
%type <astNode>function_variable_list



%%

// ------------ The definition of lang152 ---------------


// ------------ Function processing ---------------
lang152: function lang152 {$$ = astCreate(AST_FUNCTION_LIST, NULL, $1, $2, NULL, NULL); astPrint($$,0);}
		| global_variable_list {$$ = $1;}
		;

//TODO include COMMAND in the structure
function: function_header function_variable_list command ';'	{$$ = astCreate(AST_FUNCTION,  NULL, $1, $2, NULL, NULL);}
		;

function_header:  KW_INT  TK_IDENTIFIER '(' function_parameters ')'	{$$ = astCreate(AST_INT,  $2, $4, NULL, NULL, NULL);}
				| KW_BOOL TK_IDENTIFIER '(' function_parameters ')'	{$$ = astCreate(AST_BOOL, $2, $4, NULL, NULL, NULL);}
				| KW_CHAR TK_IDENTIFIER '(' function_parameters ')'	{$$ = astCreate(AST_REAL, $2, $4, NULL, NULL, NULL);}
				| KW_REAL TK_IDENTIFIER '(' function_parameters ')'	{$$ = astCreate(AST_CHAR, $2, $4, NULL, NULL, NULL);}
				;

//Modif
function_parameters: parameter optional_parameter_list			{$$ = astCreate(AST_PARAMETER_LIST, NULL, $1, $2, NULL, NULL);}
					| %empty									{$$ = NULL;}
					;
optional_parameter_list: ',' parameter optional_parameter_list	{$$ = astCreate(AST_PARAMETER_LIST, NULL, $2, $3, NULL, NULL);}
						| %empty								{$$ = NULL;}
						;

parameter: KW_INT  TK_IDENTIFIER 	{$$ = astCreate(AST_INT, $2, NULL, NULL, NULL, NULL);}
		 | KW_BOOL TK_IDENTIFIER 	{$$ = astCreate(AST_BOOL, $2, NULL, NULL, NULL, NULL);}
		 | KW_CHAR TK_IDENTIFIER 	{$$ = astCreate(AST_CHAR, $2, NULL, NULL, NULL, NULL);}
		 | KW_REAL TK_IDENTIFIER 	{$$ = astCreate(AST_REAL, $2, NULL, NULL, NULL, NULL);}
		 ;

function_variable_list: function_variable function_variable_list	{$$ = astCreate(AST_LOCAL_VAR_LIST, NULL, $1, $2, NULL, NULL);}
						| %empty									{$$ = NULL;}
						;

function_variable:    KW_INT TK_IDENTIFIER  ':' initial_value ';'	{AST_NODE* newSymbol = astCreate(AST_SYMBOL, $4, NULL, NULL, NULL, NULL);
																 	 $$ = astCreate(AST_INT, $2, newSymbol, NULL, NULL, NULL);}
		 			| KW_BOOL TK_IDENTIFIER ':' initial_value ';'	{AST_NODE* newSymbol = astCreate(AST_SYMBOL, $4, NULL, NULL, NULL, NULL);
																 	 $$ = astCreate(AST_INT, $2, newSymbol, NULL, NULL, NULL);}
					| KW_CHAR TK_IDENTIFIER ':' initial_value ';'	{AST_NODE* newSymbol = astCreate(AST_SYMBOL, $4, NULL, NULL, NULL, NULL);
																 	 $$ = astCreate(AST_INT, $2, newSymbol, NULL, NULL, NULL);}
		 			| KW_REAL TK_IDENTIFIER ':' initial_value ';'	{AST_NODE* newSymbol = astCreate(AST_SYMBOL, $4, NULL, NULL, NULL, NULL);
																 	 $$ = astCreate(AST_INT, $2, newSymbol, NULL, NULL, NULL);}



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

attribution_command: command_variable ':' '=' expression    { $$ = NULL; }
                    | expression '=' ':' command_variable   { $$ = NULL; }
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

flow_control: KW_IF '(' expression ')' command  
			| KW_IF '(' expression ')' command KW_ELSE command 
			| KW_IF '(' expression ')' command KW_LOOP 
			;

/*
{ $$ = astCreate(AST_IF, NULL, $3, $5, NULL, NULL); astPrint($$, 0);}
{ $$ = astCreate(AST_IFELSE, NULL, $3, $5, $7, NULL); astPrint($$, 0);}
{ $$ = astCreate(AST_LOOP, NULL, $3, $5, NULL, NULL); astPrint($$, 0);}
*/

command_block: '{' command_block_list '}'
			 ;
//Modif
command_block_list: command optional_command
				  ;
optional_command: ';'  command optional_command
				| %empty
				;

// ------------ Expression parsing -----------------

expression: expression '+' expression       { $$ = astCreate(AST_SUM, NULL, $1, $3, NULL, NULL); astPrint($$, 0);}
         | expression '-' expression        { $$ = astCreate(AST_SUB, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression '*' expression        { $$ = astCreate(AST_MULT, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression '/' expression        { $$ = astCreate(AST_DIV, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression '>' expression        { $$ = astCreate(AST_GT, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression '<' expression        { $$ = astCreate(AST_LT, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression OPERATOR_LE expression{ $$ = astCreate(AST_LET, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression OPERATOR_GE expression{ $$ = astCreate(AST_GET, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression OPERATOR_EQ expression{ $$ = astCreate(AST_EQ, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression OPERATOR_NE expression{ $$ = astCreate(AST_NE, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression OPERATOR_AND expression{ $$ = astCreate(AST_AND, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | expression OPERATOR_OR expression{ $$ = astCreate(AST_OR, NULL, $1, $3, NULL, NULL); astPrint($$, 0); }
         | '(' expression ')'               { $$ = $2; }
         | LIT_INTEGER                      { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL);}
         | LIT_TRUE                         { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
         | LIT_FALSE                        { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
         | LIT_CHAR                         { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
         | parse_tk_identifier              { $$ = $1; }
		 ;

parse_tk_identifier: TK_IDENTIFIER '(' function_arguments ')' { $$ = astCreate(AST_FUNCALL, $1, NULL, NULL, NULL, NULL); } 
				   | TK_IDENTIFIER '[' expression ']' { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
				   | TK_IDENTIFIER 			{ $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
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

global_variable_list: global_variable global_variable_list 	{ $$ = astCreate(AST_GLOBAL_VAR_LIST, NULL, $1, $2, NULL, NULL); astPrint($$,0);}
					| %empty								{ $$ = NULL;}
					;



global_variable:  KW_INT  TK_IDENTIFIER normal_or_vector 	{$$ = astCreate(AST_INT, $2, $3, NULL, NULL, NULL);}
		 		| KW_BOOL TK_IDENTIFIER normal_or_vector	{$$ = astCreate(AST_BOOL, $2, $3, NULL, NULL, NULL);}
				| KW_CHAR TK_IDENTIFIER normal_or_vector	{$$ = astCreate(AST_CHAR, $2, $3, NULL, NULL, NULL);}
		 		| KW_REAL TK_IDENTIFIER normal_or_vector	{$$ = astCreate(AST_REAL, $2, $3, NULL, NULL, NULL);}
		 		;


normal_or_vector:	':' initial_value ';' 								{$$ = astCreate(AST_SYMBOL, $2, NULL, NULL, NULL, NULL);}
			  	|  	'[' LIT_INTEGER ']' global_vector_initialization 	{$$ = astCreate(AST_GLOBAL_VECTOR, $2, $4, NULL, NULL, NULL);}
			  	;

global_vector_initialization: ':' global_vector_initial_values ';' 	{$$ = $2;}
							| ';'									{$$ = NULL;}
							;

global_vector_initial_values: initial_value global_vector_initial_values {$$ = astCreate(AST_SYMBOL, $1, $2, NULL, NULL, NULL);}
							| %empty									 {$$ = NULL;}
							;

// ------------ General purpose -----------------
initial_value: LIT_INTEGER	{$$ = $1;}
			 | LIT_TRUE		{$$ = $1;}
			 | LIT_FALSE	{$$ = $1;}
			 | LIT_CHAR		{$$ = $1;}
			 ;



%%

int yyerror(char *s)
{
	return 1;
}
