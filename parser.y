/****INSTITUTO DE INFORMATICA UFRGS****
Leandro Mateus Giacomini Rocha
Luciano Farias Puhl
*/

%{
    #include <stdio.h>
    #include "hash.h"
    #include "ast.h"
    #include "semantic.h"
    #include "tac.h"
    #include "assembler.h"
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

%left OPERATOR_AND OPERATOR_OR
%nonassoc '>' '<' OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_NE
%left  '+'  '-'
%left  '*'  '/'

%type <astNode>expression
%type <astNode>parse_tk_identifier


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

%type <astNode>command
%type <astNode>simple_command
%type <astNode>command_block
%type <astNode>command_block_list
%type <astNode>optional_command

%type <astNode>attribution_command
%type <astNode>flow_control
%type <astNode>input_command
%type <astNode>output_command
%type <astNode>output_list
%type <astNode>optional_output_list
%type <astNode>output_element
%type <astNode>return_command

%type <astNode>function_arguments
%type <astNode>optional_argument_list
%type <hashNode>argument

%type <astNode>attribution_variable
%type <astNode>optional_variable_index



%%

// ------------ The definition of lang152 ---------------
initial_symbol: lang152 {astTree = $1;
                         setDeclarations(astTree);
                         checkUndeclared();
                         checkTypes(astTree);
                         TAC* tacList = tacReverseCode(generateTacs(astTree));
                         tacPrintNext(tacList);
                         avengersAssemble(astTree, tacList);
                         }

// ------------ Function processing ---------------
lang152: function lang152 {$$ = astCreate(AST_FUNCTION_LIST, NULL, $1, $2, NULL, NULL); }
		| global_variable_list {$$ = $1;}
		;

function: function_header function_variable_list command ';'	{$$ = astCreate(AST_FUNCTION,  NULL, $1, $2, $3, NULL);}
		;

function_header:  KW_INT  TK_IDENTIFIER '(' function_parameters ')'	{$$ = astCreate(AST_INT,  $2, $4, NULL, NULL, NULL);}
				| KW_BOOL TK_IDENTIFIER '(' function_parameters ')'	{$$ = astCreate(AST_BOOL, $2, $4, NULL, NULL, NULL);}
				| KW_CHAR TK_IDENTIFIER '(' function_parameters ')'	{$$ = astCreate(AST_REAL, $2, $4, NULL, NULL, NULL);}
				| KW_REAL TK_IDENTIFIER '(' function_parameters ')'	{$$ = astCreate(AST_CHAR, $2, $4, NULL, NULL, NULL);}
				;

function_parameters: parameter optional_parameter_list			{$$ = astCreate(AST_PARAMETER_LIST, NULL, $1, $2, NULL, NULL);}
					| 									{$$ = astCreate(AST_PARAMETER_LIST, NULL, NULL, NULL, NULL, NULL);}
					;
optional_parameter_list: ',' parameter optional_parameter_list	{$$ = astCreate(AST_PARAMETER_LIST, NULL, $2, $3, NULL, NULL);}
						| 										{$$ = NULL;}
						;

parameter: KW_INT  TK_IDENTIFIER 	{$$ = astCreate(AST_INT, $2, NULL, NULL, NULL, NULL);}
		 | KW_BOOL TK_IDENTIFIER 	{$$ = astCreate(AST_BOOL, $2, NULL, NULL, NULL, NULL);}
		 | KW_CHAR TK_IDENTIFIER 	{$$ = astCreate(AST_CHAR, $2, NULL, NULL, NULL, NULL);}
		 | KW_REAL TK_IDENTIFIER 	{$$ = astCreate(AST_REAL, $2, NULL, NULL, NULL, NULL);}
		 ;

function_variable_list: function_variable function_variable_list	{$$ = astCreate(AST_LOCAL_VAR_LIST, NULL, $1, $2, NULL, NULL);}
						| 									{$$ = NULL;}
						;

function_variable:    KW_INT TK_IDENTIFIER  ':' initial_value ';'	{AST_NODE* newSymbol = astCreate(AST_SYMBOL, $4, NULL, NULL, NULL, NULL);
																 	 $$ = astCreate(AST_INT, $2, newSymbol, NULL, NULL, NULL);}
		 			| KW_BOOL TK_IDENTIFIER ':' initial_value ';'	{AST_NODE* newSymbol = astCreate(AST_SYMBOL, $4, NULL, NULL, NULL, NULL);
																 	 $$ = astCreate(AST_BOOL, $2, newSymbol, NULL, NULL, NULL);}
					| KW_CHAR TK_IDENTIFIER ':' initial_value ';'	{AST_NODE* newSymbol = astCreate(AST_SYMBOL, $4, NULL, NULL, NULL, NULL);
																 	 $$ = astCreate(AST_CHAR, $2, newSymbol, NULL, NULL, NULL);}
		 			| KW_REAL TK_IDENTIFIER ':' initial_value ';'	{AST_NODE* newSymbol = astCreate(AST_SYMBOL, $4, NULL, NULL, NULL, NULL);
																 	 $$ = astCreate(AST_REAL, $2, newSymbol, NULL, NULL, NULL);}



// ------------ Command parsing ---------------
command: simple_command		{$$ = $1;}
	   	| command_block     {$$ = $1;}
		;

simple_command:   attribution_command	{$$ = $1;}
			  	| flow_control			{$$ = $1;}
				| input_command			{$$ = $1;}
				| output_command		{$$ = $1;}
				| return_command		{$$ = $1;}
				| 				{$$ = NULL;}
				;

attribution_command: attribution_variable ':' '=' expression 	{$$ = astCreate(AST_ATTRIBUTION, NULL, $1, $4, NULL, NULL);}
                    | expression '=' ':' attribution_variable 	{$$ = astCreate(AST_ATTRIBUTION, NULL, $4, $1, NULL, NULL);}
		   			;

attribution_variable: TK_IDENTIFIER optional_variable_index		{$$ = astCreate(AST_SYMBOL, $1, $2, NULL, NULL, NULL);}
				;
optional_variable_index: '[' expression ']'	{$$ = $2;}
					   	| 			{$$ = NULL;}
						;


input_command: KW_INPUT TK_IDENTIFIER 	{$$ = astCreate(AST_INPUT_CMD, $2, NULL, NULL, NULL, NULL);}
			 ;

output_command: KW_OUTPUT output_list 	{$$ = astCreate(AST_OUTPUT_CMD, NULL, $2, NULL, NULL, NULL);}
			  ;

output_list: output_element optional_output_list {$$ = astCreate(AST_OUTPUT_LIST, NULL, $1, $2, NULL, NULL);}
		   ;
optional_output_list : ',' output_element optional_output_list	{$$ = astCreate(AST_OUTPUT_LIST, NULL, $2, $3, NULL, NULL);}
					 | 									{$$ = NULL;}
					 ;

output_element: LIT_STRING 	{$$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL);}
			  | expression 	{$$ = $1;}
			  ;

return_command: KW_RETURN expression 	{$$ = astCreate(AST_RETURN_CMD, NULL, $2, NULL, NULL, NULL);}
			  ;

flow_control: KW_IF '(' expression ')' command  				{$$ = astCreate(AST_IF, NULL, $3, $5, NULL, NULL);}
			| KW_IF '(' expression ')' command KW_ELSE command 	{$$ = astCreate(AST_IFELSE, NULL, $3, $5, $7, NULL);}
			| KW_IF '(' expression ')' command KW_LOOP 			{$$ = astCreate(AST_LOOP, NULL, $3, $5, NULL, NULL);}
			;

command_block: '{' command_block_list '}' {$$ = $2;}
			 ;
command_block_list: command optional_command {$$ = astCreate(AST_CMD_LIST, NULL, $1, $2, NULL, NULL);}
				  ;
optional_command: ';'  command optional_command {$$ = astCreate(AST_CMD_LIST, NULL, $2, $3, NULL, NULL);}
				| 						{$$ = NULL;}
				;

// ------------ Expression parsing -----------------

expression: expression '+' expression        { $$ = astCreate(AST_SUM, NULL, $1, $3, NULL, NULL);}
          | expression '-' expression        { $$ = astCreate(AST_SUB, NULL, $1, $3, NULL, NULL); }
          | expression '*' expression        { $$ = astCreate(AST_MULT, NULL, $1, $3, NULL, NULL); }
          | expression '/' expression        { $$ = astCreate(AST_DIV, NULL, $1, $3, NULL, NULL); }
          | expression '>' expression        { $$ = astCreate(AST_GT, NULL, $1, $3, NULL, NULL); }
          | expression '<' expression        { $$ = astCreate(AST_LT, NULL, $1, $3, NULL, NULL); }
          | expression OPERATOR_LE expression{ $$ = astCreate(AST_LET, NULL, $1, $3, NULL, NULL); }
          | expression OPERATOR_GE expression{ $$ = astCreate(AST_GET, NULL, $1, $3, NULL, NULL); }
          | expression OPERATOR_EQ expression{ $$ = astCreate(AST_EQ, NULL, $1, $3, NULL, NULL); }
          | expression OPERATOR_NE expression{ $$ = astCreate(AST_NE, NULL, $1, $3, NULL, NULL); }
          | expression OPERATOR_AND expression{ $$ = astCreate(AST_AND, NULL, $1, $3, NULL, NULL); }
          | expression OPERATOR_OR expression{ $$ = astCreate(AST_OR, NULL, $1, $3, NULL, NULL); }
          | '(' expression ')'               { $$ = $2; }
          | LIT_INTEGER                      { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL);}
          | LIT_TRUE                         { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
          | LIT_FALSE                        { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
          | LIT_CHAR                         { $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
          | parse_tk_identifier              { $$ = $1; }
		  ;

parse_tk_identifier: TK_IDENTIFIER '(' function_arguments ')' { $$ = astCreate(AST_FUNCALL, $1, $3, NULL, NULL, NULL); } 
				   | TK_IDENTIFIER '[' expression ']' { $$ = astCreate(AST_SYMBOL, $1, $3, NULL, NULL, NULL); }
				   | TK_IDENTIFIER 			{ $$ = astCreate(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
			;

function_arguments: argument optional_argument_list	{$$ = astCreate(AST_SYMBOL, $1, $2, NULL, NULL, NULL);}
					| 						{$$ = NULL;}
					;

optional_argument_list: ',' argument optional_argument_list {$$ = astCreate(AST_SYMBOL, $2, $3, NULL, NULL, NULL);}
						| 							{$$ = NULL;}
						;

argument: TK_IDENTIFIER {$$ = $1;}
		| LIT_INTEGER	{$$ = $1;}
		| LIT_TRUE		{$$ = $1;}
		| LIT_FALSE		{$$ = $1;}
		| LIT_CHAR		{$$ = $1;}
		;

// ------------ Global variables parsing -----------------

global_variable_list: global_variable global_variable_list 	{ $$ = astCreate(AST_GLOBAL_VAR_LIST, NULL, $1, $2, NULL, NULL);}
					| 								{ $$ = NULL;}
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
							| 									 {$$ = NULL;}
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
