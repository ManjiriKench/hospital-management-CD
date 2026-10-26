#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ANSI Colors for beautiful terminal output */
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_WHITE   "\033[1;37m"

/* Maximum string length */
#define MAX_ID_LEN     32
#define MAX_NAME_LEN   100
#define MAX_STR_LEN    256
#define MAX_TAC_LINES  50

/* -------------------------------------------------------------
 * 1. SYMBOL TABLE DATA STRUCTURES
 * ------------------------------------------------------------- */

/* Doctor Symbol Entry */
typedef struct Doctor {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    char dept[MAX_STR_LEN];
    float fee;
    int appointment_count;
    struct Doctor *next;
} Doctor;

/* Patient Symbol Entry */
typedef struct Patient {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    int age;
    char gender;           /* 'M', 'F', 'O' */
    char contact[MAX_STR_LEN];
    int has_appointment;
    char doctor_id[MAX_ID_LEN];
    int has_record;
    int has_bill;
    struct Patient *next;
} Patient;

/* Appointment Record */
typedef struct Appointment {
    char patient_id[MAX_ID_LEN];
    char doctor_id[MAX_ID_LEN];
    char date[MAX_STR_LEN];
    char time[MAX_STR_LEN];
    struct Appointment *next;
} Appointment;

/* Medical Diagnosis Record */
typedef struct MedicalRecord {
    char patient_id[MAX_ID_LEN];
    char diagnosis[MAX_STR_LEN];
    char prescription[MAX_STR_LEN];
    int admitted;          /* 1 = YES, 0 = NO */
    struct MedicalRecord *next;
} MedicalRecord;

/* Billing Statement & Calculation */
typedef struct Bill {
    char patient_id[MAX_ID_LEN];
    float doc_fee;
    float lab_charges;
    float medicine_charges;
    float room_charges;
    float discount_percent;
    float discount_amount;
    float subtotal;
    float tax;
    float total;
    struct Bill *next;
} Bill;

/* Three Address Code (TAC) Quadruple */
typedef struct TAC {
    char op[16];
    char arg1[32];
    char arg2[32];
    char result[32];
} TAC;

/* -------------------------------------------------------------
 * 2. COMPILER STATISTICS
 * ------------------------------------------------------------- */
typedef struct CompilerStats {
    int lines_processed;
    int tokens_recognized;
    int statements_parsed;
    int lexical_errors;
    int syntax_errors;
    int semantic_errors;
} CompilerStats;

extern CompilerStats stats;
extern int yylineno;
extern char *yytext;

/* -------------------------------------------------------------
 * 3. FUNCTION PROTOTYPES
 * ------------------------------------------------------------- */

/* Initialization */
void init_compiler(void);

/* Doctor Table Functions */
Doctor* find_doctor(const char *id);
int register_doctor(const char *id, const char *name, const char *dept, float fee, int lineno);

/* Patient Table Functions */
Patient* find_patient(const char *id);
int register_patient(const char *id, const char *name, int age, char gender, const char *contact, int lineno);

/* Appointment Functions */
int schedule_appointment(const char *pat_id, const char *doc_id, const char *date, const char *time, int lineno);

/* Medical Record Functions */
int add_medical_record(const char *pat_id, const char *diag, const char *presc, int admitted, int lineno);

/* Billing Functions */
int generate_patient_bill(const char *pat_id, float lab, float med, float room, float discount, int lineno);

/* Query & Display Functions */
void show_patients(void);
void show_doctors(void);
void show_appointments(void);
void show_records_for_patient(const char *pat_id);
void show_bill_for_patient(const char *pat_id);
void show_hospital_summary(void);

/* Intermediate Code Generation (TAC) */
void generate_billing_tac(const char *pat_id, float doc_fee, float lab, float med, float room, float discount);

/* Error Reporting */
void report_lexical_error(const char *token, int lineno);
void report_syntax_error(const char *msg, int lineno);
void report_semantic_error(const char *msg, int lineno);
void print_compiler_summary(void);

/* Helper */
void strip_quotes(char *dest, const char *src);

#endif /* HOSPITAL_H */
