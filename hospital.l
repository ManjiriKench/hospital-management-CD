%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hospital.h"
#include "y.tab.h"

/* Count tokens recognized */
#define COUNT_TOKEN() stats.tokens_recognized++
%}

%option noyywrap
%option yylineno
%option case-insensitive

/* Regular Expression Definitions */
DIGIT           [0-9]
INT             [-+]?{DIGIT}+
FLOAT           [-+]?{DIGIT}+"."{DIGIT}+
ID              [a-zA-Z][a-zA-Z0-9_]*
STRING          \"([^\"\\]|\\.)*\"
WS              [ \t\r]+
SINGLE_COMMENT  \/\/[^\n]*
MULTI_COMMENT   \/\*([^*]|\*+[^*/])*\*+\/

%%

{WS}                { /* Ignore whitespace */ }
{SINGLE_COMMENT}    { /* Ignore single-line comments */ }
{MULTI_COMMENT}     { /* Ignore multi-line comments */ }
\n                  { /* yylineno automatically tracks lines */ }

"REGISTER"          { COUNT_TOKEN(); return REGISTER; }
"PATIENT"           { COUNT_TOKEN(); return PATIENT; }
"DOCTOR"            { COUNT_TOKEN(); return DOCTOR; }
"SCHEDULE"          { COUNT_TOKEN(); return SCHEDULE; }
"APPOINTMENT"       { COUNT_TOKEN(); return APPOINTMENT; }
"FOR"               { COUNT_TOKEN(); return FOR; }
"WITH"              { COUNT_TOKEN(); return WITH; }
"ADD"               { COUNT_TOKEN(); return ADD; }
"RECORD"            { COUNT_TOKEN(); return RECORD; }
"DIAGNOSIS"         { COUNT_TOKEN(); return DIAGNOSIS; }
"PRESCRIPTION"      { COUNT_TOKEN(); return PRESCRIPTION; }
"ADMITTED"          { COUNT_TOKEN(); return ADMITTED; }
"GENERATE"          { COUNT_TOKEN(); return GENERATE; }
"BILL"              { COUNT_TOKEN(); return BILL; }
"LAB_CHARGES"       { COUNT_TOKEN(); return LAB_CHARGES; }
"MEDICINE_CHARGES"  { COUNT_TOKEN(); return MEDICINE_CHARGES; }
"ROOM_CHARGES"      { COUNT_TOKEN(); return ROOM_CHARGES; }
"DISCOUNT"          { COUNT_TOKEN(); return DISCOUNT; }
"SHOW"              { COUNT_TOKEN(); return SHOW; }
"PATIENTS"          { COUNT_TOKEN(); return PATIENTS; }
"DOCTORS"           { COUNT_TOKEN(); return DOCTORS; }
"APPOINTMENTS"      { COUNT_TOKEN(); return APPOINTMENTS; }
"RECORDS"           { COUNT_TOKEN(); return RECORDS; }
"SUMMARY"           { COUNT_TOKEN(); return SUMMARY; }
"NAME"              { COUNT_TOKEN(); return NAME; }
"AGE"               { COUNT_TOKEN(); return AGE; }
"GENDER"            { COUNT_TOKEN(); return GENDER; }
"CONTACT"           { COUNT_TOKEN(); return CONTACT; }
"DEPT"              { COUNT_TOKEN(); return DEPT; }
"FEE"               { COUNT_TOKEN(); return FEE; }
"DATE"              { COUNT_TOKEN(); return DATE; }
"TIME"              { COUNT_TOKEN(); return TIME; }
"YES"               { COUNT_TOKEN(); return YES; }
"NO"                { COUNT_TOKEN(); return NO; }

";"                 { COUNT_TOKEN(); return SEMI; }
"%"                 { COUNT_TOKEN(); return PERCENT; }

{FLOAT}             {
                        COUNT_TOKEN();
                        yylval.fval = (float)atof(yytext);
                        return FLOAT_VAL;
                    }

{INT}               {
                        COUNT_TOKEN();
                        yylval.ival = atoi(yytext);
                        return INT_VAL;
                    }

{STRING}            {
                        COUNT_TOKEN();
                        strncpy(yylval.sval, yytext, 255);
                        yylval.sval[255] = '\0';
                        return STRING_VAL;
                    }

{ID}                {
                        COUNT_TOKEN();
                        strncpy(yylval.sval, yytext, 255);
                        yylval.sval[255] = '\0';
                        return IDENTIFIER;
                    }

.                   {
                        /* Any unrecognized character triggers lexical error */
                        COUNT_TOKEN();
                        report_lexical_error(yytext, yylineno);
                    }

%%
