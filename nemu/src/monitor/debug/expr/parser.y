%error-verbose
%locations
%{
#include "def.h"
#include "common.h"
#include "monitor/expr.h"

typedef struct yy_buffer_state * YY_BUFFER_STATE;

void yyerror(const char* fmt, ...);
void print_node(struct node *node);
extern int yylex();
extern int yyparse();
extern int yylineno;
extern int yycolumn;
extern char *yytext;
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

struct node *result;
struct expr_info *info;
%}

%union {
  int32_t      type_int;
  float        type_float;
  char        *type_register;
  enum relop   type_relop;
  struct node *node;
};

%type <node> primary_expression postfix_expression
%type <node> unary_expression multiplicative_expression
%type <node> additive_expression  shift_expression relational_expression
%type <node> AND_expression NOR_expression OR_expression conditional_expression
%type <node> logical_AND_expression logical_OR_expression expression

// %token 定义终结符的语义值类型
%token <type_int> INT
%token <type_relop> RELOP
%token <type_float> FLOAT
%token <type_register> REGISTER

// 用bison对该文件编译时，带参数-d，生成的exp.tab.h中给这些单词进行编码，可在lex.l中包含parser.tab.h使用这些单词种类码
%token LP RP LB RB SEMI COMMA
%token PLUS MINUS STAR DIV MOD AND OR NOT PLUSPLUS MINUSMINUS
%token ANDAND OROR QUES COLON NOR LSHIFT RSHIFT

%left ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UMINUS NOT TILDE

%%
expression:
    conditional_expression  {result = $$;}
  ;

primary_expression:
    REGISTER            {$$=expr_access_register($1);}
  | INT                 {$$=mknode(); $$->type=TYPE_INT;   $$->type_int=$1;}
  | FLOAT               {$$=mknode(); $$->type=TYPE_FLOAT; $$->type_float=$1;}
  | LP expression RP    {$$=$2;}
  ;

postfix_expression:
    primary_expression                                      {$$=$1;}
  | postfix_expression PLUSPLUS                             {$$=expr_addi($1, 1);}
  | postfix_expression MINUSMINUS                           {$$=expr_addi($1, -1);}
  ;

unary_expression:
    postfix_expression                  {$$=$1;}
  | PLUSPLUS unary_expression           {$$=expr_addi($2, 1);}
  | MINUSMINUS unary_expression         {$$=expr_addi($2, -1);}
  | NOT unary_expression                {$$=$2; $$->type=TYPE_BOOL; $$->type_bool=!($$->type_bool);}
  | STAR unary_expression               {$$=expr_access_memory($2);}
  | PLUS unary_expression               {$$=expr_addi($2, 0);}
  | MINUS unary_expression              {$$=expr_reverse($2);}
  | TILDE unary_expression              {$$=$2; $$->type=TYPE_INT; $$->type_int=~($$->type_int);}
  ;

multiplicative_expression:
    unary_expression                                {$$=$1;}
  | multiplicative_expression STAR unary_expression {$$=expr_mul($1, $3);}
  | multiplicative_expression DIV unary_expression  {$$=expr_div($1, $3);}
  | multiplicative_expression MOD unary_expression  {$$=expr_mod($1, $3);}
  ;

additive_expression:
    multiplicative_expression                           {$$=$1;}
  | additive_expression PLUS multiplicative_expression  {$$=expr_add($1, $3);}
  | additive_expression MINUS multiplicative_expression {$$=expr_sub($1, $3);}
  ;

shift_expression:
    additive_expression                         {$$=$1;}
  | shift_expression LSHIFT additive_expression {$$=expr_lshift($1, $3);}
  | shift_expression RSHIFT additive_expression {$$=expr_rshift($1, $3);}
  ;

relational_expression:
    shift_expression                                {$$=$1;}
  | relational_expression RELOP shift_expression    {$$=expr_relation($1, $3, $2);}
  ;

AND_expression:
    relational_expression                           {$$=$1;}
  | AND_expression AND relational_expression        {$$=expr_bitand($1, $3);}
  ;

NOR_expression:
    AND_expression                                  {$$=$1;}
  | NOR_expression NOR AND_expression               {$$=expr_bitnor($1, $3);}
  ;

OR_expression:
    NOR_expression                                  {$$=$1;}
  | OR_expression OR NOR_expression                 {$$=expr_bitor($1, $3);}
  ;

logical_AND_expression:
    OR_expression                                   {$$=$1;}
  | logical_AND_expression ANDAND OR_expression     {$$=expr_and($1, $3);}
  ;

logical_OR_expression:
    logical_AND_expression                              {$$=$1;}
  | logical_OR_expression OROR logical_AND_expression   {$$=expr_or($1, $3);}
  ;

conditional_expression:
    logical_OR_expression                                                           {$$=$1;}
  | logical_OR_expression QUES conditional_expression COLON conditional_expression  {$$=expr_cond($1, $3, $5);}
  ;
%%

struct node *expr(char *e, bool *success, struct expr_info *e_info) {
  info = e_info;
  yylineno = 1;
  yycolumn = 1;
  YY_BUFFER_STATE buffer = yy_scan_string(e);
  result = NULL;
  yyparse();
  yy_delete_buffer(buffer);
  *success = true;
  return result;
}

#include<stdarg.h>
void yyerror(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "Grammar Error at Line %d Column %d: ", yylloc.first_line,yylloc.first_column);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, ".\n");
}
