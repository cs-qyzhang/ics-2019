%{
#include "def.h"
#include "parser.tab.h"

int yycolumn = 1;

#define YY_USER_ACTION {                              \
  yylloc.first_line   = yylloc.last_line = yylineno;  \
  yylloc.first_column = yycolumn;                     \
  yylloc.last_column  = yycolumn + yyleng - 1;        \
  yycolumn            += yyleng;                      \
}

typedef union {
  int32_t    type_int;
  float      type_float;
  char      *type_register;
  enum relop type_relop;
} YYLVAL;

#define YYSTYPE YYLVAL
%}

%option yylineno

int             [0-9]+
hex             0(x|X)[0-9A-Fa-f]+
float           ([0-9]*\.[0-9]+)|([0-9]+\.)
register        (eip|(e?(ax|bx|cx|dx|si|di|sp|bp))|((a|b|c|d)(h|l)))

%%
 /*常量*/
{int}           {yylval.type_int = atoi(yytext);                return INT;}
{hex}           {yylval.type_int = strtol(yytext, NULL, 16);    return INT;}
{float}         {yylval.type_float = atof(yytext);              return FLOAT;}
{register}      {yylval.type_register = malloc(sizeof(4)); strcpy(yylval.type_register, yytext); return REGISTER;}
 
 /*操作符*/
"?"             {return QUES;}
":"             {return COLON;}
";"             {return SEMI;}
","             {return COMMA;}
">"             {yylval.type_relop = GT; return RELOP;}
"<"             {yylval.type_relop = LT; return RELOP;}
">="            {yylval.type_relop = GE; return RELOP;}
"<="            {yylval.type_relop = LE; return RELOP;}
"=="            {yylval.type_relop = EQ; return RELOP;}
"!="            {yylval.type_relop = NE; return RELOP;}
"="             {return ASSIGNOP;}
"++"            {return PLUSPLUS;}
"--"            {return MINUSMINUS;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return STAR;}
"/"             {return DIV;}
"%"             {return MOD;}
">>"            {return RSHIFT;}
"<<"            {return LSHIFT;}
"&&"            {return ANDAND;}
"&"             {return AND;}
"||"            {return OROR;}
"|"             {return OR;}
"!"             {return NOT;}
"^"             {return NOR;}
"~"             {return TILDE;}
"("             {return LP;}
")"             {return RP;}
"["             {return LB;}
"]"             {return RB;}
[\n]            {yycolumn = 1;}
[ \r\t]         {}

 /*匹配出错*/
.               {printf("Flex Error: Mysterious character \"%s\"\n\t at Line %d, Column %d\n", yytext, yylineno, yycolumn);parse_success=false;}

%%

int yywrap() {
  return 1;
}
