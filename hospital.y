%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hospital.h"

int yylex(void);
void yyerror(const char *s);
%}

/* Define yylval types for Lex and Yacc */
%union {
    int ival;
    float fval;
    char sval[256];
    char cval;
}

/* Terminal Tokens (Keywords) */
%token REGISTER PATIENT DOCTOR
%token SCHEDULE APPOINTMENT FOR WITH
%token ADD RECORD DIAGNOSIS PRESCRIPTION ADMITTED
%token GENERATE BILL LAB_CHARGES MEDICINE_CHARGES ROOM_CHARGES DISCOUNT
%token SHOW PATIENTS DOCTORS APPOINTMENTS RECORDS SUMMARY
%token NAME AGE GENDER CONTACT DEPT FEE DATE TIME
%token YES NO
%token PERCENT SEMI

/* Terminal Tokens with Values */
%token <ival> INT_VAL
%token <fval> FLOAT_VAL
%token <sval> IDENTIFIER STRING_VAL
%token <cval> GENDER_VAL

/* Non-Terminal Type Definitions */
%type <fval> num_val
%type <cval> gender_token
%type <ival> bool_val

%%

/* -------------------------------------------------------------
 * GRAMMAR PRODUCTIONS (CFG - Context Free Grammar)
 * ------------------------------------------------------------- */

program:
      /* Empty program is valid */
    | program statement
    ;

statement:
      register_doctor_stmt
    | register_patient_stmt
    | schedule_appointment_stmt
    | add_record_stmt
    | generate_bill_stmt
    | query_stmt
    | error_stmt
    ;

/* 1. Doctor Registration */
register_doctor_stmt:
      REGISTER DOCTOR IDENTIFIER NAME STRING_VAL DEPT STRING_VAL FEE num_val SEMI
      {
          stats.statements_parsed++;
          register_doctor($3, $5, $7, $9, yylineno);
      }
    ;

/* 2. Patient Registration */
register_patient_stmt:
      REGISTER PATIENT IDENTIFIER NAME STRING_VAL AGE INT_VAL GENDER gender_token CONTACT STRING_VAL SEMI
      {
          stats.statements_parsed++;
          register_patient($3, $5, $7, $9, $11, yylineno);
      }
    ;

gender_token:
      GENDER_VAL
      {
          $$ = $1;
      }
    | IDENTIFIER
      {
          /* Handles bare M, F, O tokens */
          $$ = $1[0];
      }
    | STRING_VAL
      {
          /* Handles "M", "F", "O" strings */
          char clean[10];
          strip_quotes(clean, $1);
          $$ = clean[0];
      }
    ;

/* 3. Appointment Scheduling */
schedule_appointment_stmt:
      SCHEDULE APPOINTMENT FOR PATIENT IDENTIFIER WITH DOCTOR IDENTIFIER DATE STRING_VAL TIME STRING_VAL SEMI
      {
          stats.statements_parsed++;
          schedule_appointment($5, $8, $10, $12, yylineno);
      }
    ;

/* 4. Medical Record Addition */
add_record_stmt:
      ADD RECORD FOR PATIENT IDENTIFIER DIAGNOSIS STRING_VAL PRESCRIPTION STRING_VAL ADMITTED bool_val SEMI
      {
          stats.statements_parsed++;
          add_medical_record($5, $7, $9, $11, yylineno);
      }
    ;

bool_val:
      YES { $$ = 1; }
    | NO  { $$ = 0; }
    ;

/* 5. Billing and Intermediate Code Generation */
generate_bill_stmt:
      GENERATE BILL FOR PATIENT IDENTIFIER LAB_CHARGES num_val MEDICINE_CHARGES num_val ROOM_CHARGES num_val DISCOUNT num_val PERCENT SEMI
      {
          stats.statements_parsed++;
          generate_patient_bill($5, $7, $9, $11, $13, yylineno);
      }
    | GENERATE BILL FOR PATIENT IDENTIFIER LAB_CHARGES num_val MEDICINE_CHARGES num_val ROOM_CHARGES num_val DISCOUNT num_val SEMI
      {
          stats.statements_parsed++;
          generate_patient_bill($5, $7, $9, $11, $13, yylineno);
      }
    ;

/* 6. Query and Symbol Table Inspection Commands */
query_stmt:
      SHOW PATIENTS SEMI
      {
          stats.statements_parsed++;
          show_patients();
      }
    | SHOW DOCTORS SEMI
      {
          stats.statements_parsed++;
          show_doctors();
      }
    | SHOW APPOINTMENTS SEMI
      {
          stats.statements_parsed++;
          show_appointments();
      }
    | SHOW RECORDS FOR PATIENT IDENTIFIER SEMI
      {
          stats.statements_parsed++;
          show_records_for_patient($5);
      }
    | SHOW BILL FOR PATIENT IDENTIFIER SEMI
      {
          stats.statements_parsed++;
          show_bill_for_patient($5);
      }
    | SHOW SUMMARY SEMI
      {
          stats.statements_parsed++;
          show_hospital_summary();
      }
    ;

/* Generalized Number: accepts integer or floating point */
num_val:
      INT_VAL   { $$ = (float)$1; }
    | FLOAT_VAL { $$ = $1; }
    ;

/* Syntax Error Recovery Rule: Discard tokens until the next ';' */
error_stmt:
      error SEMI
      {
          /* Recover from syntax error and continue parsing subsequent statements */
          yyerrok;
      }
    ;

%%

/* -------------------------------------------------------------
 * USER SUBROUTINES & DRIVER
 * ------------------------------------------------------------- */

void yyerror(const char *s) {
    char buf[256];
    sprintf(buf, "Near '%s': %s", yytext, s);
    report_syntax_error(buf, yylineno);
}

int main(int argc, char **argv) {
    extern FILE *yyin;

    printf(COLOR_BOLD COLOR_BLUE);
    printf("================================================================================\n");
    printf("        COMPILER DESIGN PROJECT: HOSPITAL MANAGEMENT DOMAIN-SPECIFIC LANGUAGE\n");
    printf("             Lexical & Syntax Analyzer with Semantic Engine & TAC\n");
    printf("================================================================================\n");
    printf(COLOR_RESET);

    init_compiler();

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            fprintf(stderr, COLOR_RED "Error: Could not open source file '%s'\n" COLOR_RESET, argv[1]);
            return 1;
        }
        yyin = f;
        printf(">> Reading Hospital Language source code from: %s\n\n", argv[1]);
    } else {
        printf(">> Running in Interactive Console Mode.\n");
        printf(">> Type HML statements ending with ';' (Press Ctrl+Z then Enter to evaluate on Windows):\n\n");
    }

    /* Start bottom-up LALR syntax parsing */
    yyparse();

    /* Final statistics */
    stats.lines_processed = yylineno;
    print_compiler_summary();

    return (stats.lexical_errors + stats.syntax_errors + stats.semantic_errors == 0) ? 0 : 1;
}
