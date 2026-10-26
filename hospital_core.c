#include "hospital.h"

/* -------------------------------------------------------------
 * GLOBAL SYMBOL TABLE ROOTS
 * ------------------------------------------------------------- */
static Doctor *doctor_list = NULL;
static Patient *patient_list = NULL;
static Appointment *appointment_list = NULL;
static MedicalRecord *record_list = NULL;
static Bill *bill_list = NULL;

CompilerStats stats = {0, 0, 0, 0, 0, 0};

/* Initialize Compiler & Statistics */
void init_compiler(void) {
    doctor_list = NULL;
    patient_list = NULL;
    appointment_list = NULL;
    record_list = NULL;
    bill_list = NULL;
    memset(&stats, 0, sizeof(CompilerStats));
}

/* Helper to strip surrounding quotes from strings */
void strip_quotes(char *dest, const char *src) {
    int len = strlen(src);
    if (len >= 2 && src[0] == '"' && src[len - 1] == '"') {
        strncpy(dest, src + 1, len - 2);
        dest[len - 2] = '\0';
    } else {
        strcpy(dest, src);
    }
}

/* -------------------------------------------------------------
 * DOCTOR SYMBOL TABLE IMPLEMENTATION
 * ------------------------------------------------------------- */
Doctor* find_doctor(const char *id) {
    Doctor *curr = doctor_list;
    while (curr != NULL) {
        if (strcmp(curr->id, id) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

int register_doctor(const char *id, const char *name, const char *dept, float fee, int lineno) {
    char clean_name[MAX_NAME_LEN];
    char clean_dept[MAX_STR_LEN];
    strip_quotes(clean_name, name);
    strip_quotes(clean_dept, dept);

    /* Semantic Check 1: Duplicate Doctor ID */
    if (find_doctor(id) != NULL) {
        char err[256];
        sprintf(err, "Duplicate Doctor ID '%s'. Doctor already registered in symbol table.", id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Semantic Check 2: Consultation Fee non-negative */
    if (fee < 0.0f) {
        char err[256];
        sprintf(err, "Consultation fee for Doctor '%s' cannot be negative (provided: %.2f).", id, fee);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Allocate and insert into symbol table */
    Doctor *new_doc = (Doctor*)malloc(sizeof(Doctor));
    if (!new_doc) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    strncpy(new_doc->id, id, MAX_ID_LEN - 1);
    strncpy(new_doc->name, clean_name, MAX_NAME_LEN - 1);
    strncpy(new_doc->dept, clean_dept, MAX_STR_LEN - 1);
    new_doc->fee = fee;
    new_doc->appointment_count = 0;
    new_doc->next = doctor_list;
    doctor_list = new_doc;

    printf("  " COLOR_GREEN "[OK]" COLOR_RESET " Registered Doctor: %s | %s | Dept: %s | Fee: $%.2f\n",
           id, clean_name, clean_dept, fee);
    return 1;
}

/* -------------------------------------------------------------
 * PATIENT SYMBOL TABLE IMPLEMENTATION
 * ------------------------------------------------------------- */
Patient* find_patient(const char *id) {
    Patient *curr = patient_list;
    while (curr != NULL) {
        if (strcmp(curr->id, id) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

int register_patient(const char *id, const char *name, int age, char gender, const char *contact, int lineno) {
    char clean_name[MAX_NAME_LEN];
    char clean_contact[MAX_STR_LEN];
    strip_quotes(clean_name, name);
    strip_quotes(clean_contact, contact);

    /* Semantic Check 1: Duplicate Patient ID */
    if (find_patient(id) != NULL) {
        char err[256];
        sprintf(err, "Duplicate Patient ID '%s'. Patient is already registered.", id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Semantic Check 2: Age range validation (1 to 120) */
    if (age <= 0 || age > 120) {
        char err[256];
        sprintf(err, "Invalid age '%d' for Patient '%s'. Must be between 1 and 120.", age, id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Semantic Check 3: Gender validation ('M', 'F', 'O') */
    if (gender != 'M' && gender != 'F' && gender != 'O') {
        char err[256];
        sprintf(err, "Invalid gender '%c' for Patient '%s'. Must be 'M', 'F', or 'O'.", gender, id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Allocate and insert into patient table */
    Patient *new_pat = (Patient*)malloc(sizeof(Patient));
    if (!new_pat) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    strncpy(new_pat->id, id, MAX_ID_LEN - 1);
    strncpy(new_pat->name, clean_name, MAX_NAME_LEN - 1);
    new_pat->age = age;
    new_pat->gender = gender;
    strncpy(new_pat->contact, clean_contact, MAX_STR_LEN - 1);
    new_pat->has_appointment = 0;
    new_pat->doctor_id[0] = '\0';
    new_pat->has_record = 0;
    new_pat->has_bill = 0;
    new_pat->next = patient_list;
    patient_list = new_pat;

    printf("  " COLOR_GREEN "[OK]" COLOR_RESET " Registered Patient: %s | %s | Age: %d | Gender: %c | Contact: %s\n",
           id, clean_name, age, gender, clean_contact);
    return 1;
}

/* -------------------------------------------------------------
 * APPOINTMENT SCHEDULING WITH SEMANTIC CHECKS
 * ------------------------------------------------------------- */
int schedule_appointment(const char *pat_id, const char *doc_id, const char *date, const char *time, int lineno) {
    char clean_date[MAX_STR_LEN];
    char clean_time[MAX_STR_LEN];
    strip_quotes(clean_date, date);
    strip_quotes(clean_time, time);

    /* Semantic Check 1: Patient existence */
    Patient *p = find_patient(pat_id);
    if (p == NULL) {
        char err[256];
        sprintf(err, "Patient '%s' not registered. Cannot schedule appointment.", pat_id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Semantic Check 2: Doctor existence */
    Doctor *d = find_doctor(doc_id);
    if (d == NULL) {
        char err[256];
        sprintf(err, "Doctor '%s' not found in registry. Cannot schedule appointment.", doc_id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Allocate and link appointment */
    Appointment *app = (Appointment*)malloc(sizeof(Appointment));
    if (!app) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    strncpy(app->patient_id, pat_id, MAX_ID_LEN - 1);
    strncpy(app->doctor_id, doc_id, MAX_ID_LEN - 1);
    strncpy(app->date, clean_date, MAX_STR_LEN - 1);
    strncpy(app->time, clean_time, MAX_STR_LEN - 1);
    app->next = appointment_list;
    appointment_list = app;

    /* Update symbol cross-references */
    p->has_appointment = 1;
    strncpy(p->doctor_id, doc_id, MAX_ID_LEN - 1);
    d->appointment_count++;

    printf("  " COLOR_GREEN "[OK]" COLOR_RESET " Scheduled Appointment: Patient [%s : %s] with Dr. [%s : %s] on %s at %s\n",
           p->id, p->name, d->id, d->name, clean_date, clean_time);
    return 1;
}

/* -------------------------------------------------------------
 * MEDICAL RECORD ENTRY WITH SEMANTIC CHECKS
 * ------------------------------------------------------------- */
int add_medical_record(const char *pat_id, const char *diag, const char *presc, int admitted, int lineno) {
    char clean_diag[MAX_STR_LEN];
    char clean_presc[MAX_STR_LEN];
    strip_quotes(clean_diag, diag);
    strip_quotes(clean_presc, presc);

    /* Semantic Check 1: Patient existence */
    Patient *p = find_patient(pat_id);
    if (p == NULL) {
        char err[256];
        sprintf(err, "Patient '%s' does not exist. Cannot add medical record.", pat_id);
        report_semantic_error(err, lineno);
        return 0;
    }

    MedicalRecord *rec = (MedicalRecord*)malloc(sizeof(MedicalRecord));
    if (!rec) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    strncpy(rec->patient_id, pat_id, MAX_ID_LEN - 1);
    strncpy(rec->diagnosis, clean_diag, MAX_STR_LEN - 1);
    strncpy(rec->prescription, clean_presc, MAX_STR_LEN - 1);
    rec->admitted = admitted;
    rec->next = record_list;
    record_list = rec;

    p->has_record = 1;

    printf("  " COLOR_GREEN "[OK]" COLOR_RESET " Added Medical Record: Patient [%s : %s] | Diagnosis: \"%s\" | Rx: \"%s\" | Admitted: %s\n",
           p->id, p->name, clean_diag, clean_presc, (admitted ? "YES" : "NO"));
    return 1;
}

/* -------------------------------------------------------------
 * THREE ADDRESS CODE (TAC) INTERMEDIATE CODE GENERATOR (STAR FEATURE)
 * Directly implements Unit 3 & Unit 4 of Compiler Design Syllabus
 * ------------------------------------------------------------- */
void generate_billing_tac(const char *pat_id, float doc_fee, float lab, float med, float room, float discount) {
    printf("\n  " COLOR_MAGENTA "================================================================" COLOR_RESET "\n");
    printf("  " COLOR_BOLD COLOR_MAGENTA "[INTERMEDIATE CODE GENERATION - THREE ADDRESS CODE (TAC)]" COLOR_RESET "\n");
    printf("  " COLOR_WHITE "Target Patient: %s" COLOR_RESET "\n", pat_id);
    printf("  ----------------------------------------------------------------\n");
    printf("  " COLOR_CYAN "Quadruple Format: (Op, Arg1, Arg2, Result)" COLOR_RESET "\n\n");

    /* Step-by-step 3-Address Code generation */
    printf("    (ASSIGN,  %.2f,       NULL,       t1)    ; t1 = Doctor Consultation Fee\n", doc_fee);
    printf("    (ADD,     t1,         %.2f,       t2)    ; t2 = t1 + Lab Charges\n", lab);
    printf("    (ADD,     t2,         %.2f,       t3)    ; t3 = t2 + Medicine Charges\n", med);
    printf("    (ADD,     t3,         %.2f,       t4)    ; t4 = t3 + Room Charges (Subtotal)\n", room);
    printf("    (DIV,     %.2f,       100.0,      t5)    ; t5 = Discount Rate\n", discount);
    printf("    (MULT,    t4,         t5,         t6)    ; t6 = Discount Amount\n");
    printf("    (SUB,     t4,         t6,         t7)    ; t7 = Subtotal - Discount\n");
    printf("    (MULT,    t7,         0.05,       t8)    ; t8 = Hospital Tax / GST (5%%)\n");
    printf("    (ADD,     t7,         t8,         t9)    ; t9 = Final Payable Amount\n");
    printf("    (STORE,   t9,         NULL,       TOTAL_%s)\n", pat_id);

    /* Star Feature: Optimization demonstration (Unit 4) */
    printf("\n  " COLOR_BOLD COLOR_YELLOW "[CODE OPTIMIZATION: Constant Folding & Dead-Code Elimination]" COLOR_RESET "\n");
    if (discount == 0.0f) {
        printf("    * Note: Discount is 0%% -> Algebraic simplification: t6 = 0, t7 = t4 (eliminated t5, t6).\n");
    }
    if (room == 0.0f) {
        printf("    * Note: Room Charges are $0 -> Identity addition optimized out: t4 = t3.\n");
    }
    if (discount != 0.0f && room != 0.0f) {
        printf("    * Constant sub-expressions pre-folded for GST factor: t9 = t7 * 1.05\n");
    }
    printf("  " COLOR_MAGENTA "================================================================" COLOR_RESET "\n\n");
}

/* -------------------------------------------------------------
 * BILLING & FINANCIAL CALCULATIONS
 * ------------------------------------------------------------- */
int generate_patient_bill(const char *pat_id, float lab, float med, float room, float discount, int lineno) {
    /* Semantic Check 1: Patient existence */
    Patient *p = find_patient(pat_id);
    if (p == NULL) {
        char err[256];
        sprintf(err, "Patient '%s' does not exist. Cannot generate bill.", pat_id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Semantic Check 2: Already billed */
    if (p->has_bill) {
        char err[256];
        sprintf(err, "Bill already generated for Patient '%s'. Multiple final bills not permitted.", pat_id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Semantic Check 3: Non-negative charges */
    if (lab < 0.0f || med < 0.0f || room < 0.0f) {
        char err[256];
        sprintf(err, "Charges cannot be negative for Patient '%s'.", pat_id);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Semantic Check 4: Discount range */
    if (discount < 0.0f || discount > 100.0f) {
        char err[256];
        sprintf(err, "Discount must be between 0%% and 100%% (provided: %.2f%%).", discount);
        report_semantic_error(err, lineno);
        return 0;
    }

    /* Dynamic semantic retrieval: get doctor's consultation fee if appointment exists */
    float doc_fee = 0.0f;
    if (p->has_appointment) {
        Doctor *d = find_doctor(p->doctor_id);
        if (d != NULL) {
            doc_fee = d->fee;
        }
    } else {
        printf("  " COLOR_YELLOW "[SEMANTIC NOTE]" COLOR_RESET " Patient '%s' had no doctor appointment; doctor fee set to $0.00.\n", pat_id);
    }

    /* Mathematical computations */
    float subtotal = doc_fee + lab + med + room;
    float discount_amount = subtotal * (discount / 100.0f);
    float taxable = subtotal - discount_amount;
    float tax = taxable * 0.05f; /* 5% GST/Service tax */
    float total = taxable + tax;

    /* Generate Intermediate Code (TAC) - CD Unit 3 / 7 */
    generate_billing_tac(pat_id, doc_fee, lab, med, room, discount);

    /* Allocate and store bill */
    Bill *b = (Bill*)malloc(sizeof(Bill));
    if (!b) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    strncpy(b->patient_id, pat_id, MAX_ID_LEN - 1);
    b->doc_fee = doc_fee;
    b->lab_charges = lab;
    b->medicine_charges = med;
    b->room_charges = room;
    b->discount_percent = discount;
    b->discount_amount = discount_amount;
    b->subtotal = subtotal;
    b->tax = tax;
    b->total = total;
    b->next = bill_list;
    bill_list = b;

    p->has_bill = 1;

    printf("  " COLOR_GREEN "[OK]" COLOR_RESET " Bill Generated for Patient [%s : %s] -> Subtotal: $%.2f | Discount: -$%.2f | Tax: +$%.2f | " COLOR_BOLD COLOR_GREEN "TOTAL: $%.2f" COLOR_RESET "\n",
           p->id, p->name, subtotal, discount_amount, tax, total);
    return 1;
}

/* -------------------------------------------------------------
 * QUERY / DISPLAY PROCEDURES (SYMBOL TABLE DUMPS)
 * ------------------------------------------------------------- */
void show_patients(void) {
    printf("\n" COLOR_BOLD COLOR_CYAN "============================== REGISTERED PATIENTS ==============================" COLOR_RESET "\n");
    printf("%-10s | %-20s | %-5s | %-6s | %-15s | %-8s\n",
           "PATIENT ID", "NAME", "AGE", "GENDER", "CONTACT", "BILLED?");
    printf("---------------------------------------------------------------------------------\n");
    Patient *curr = patient_list;
    int count = 0;
    while (curr != NULL) {
        printf("%-10s | %-20s | %-5d | %-6c | %-15s | %-8s\n",
               curr->id, curr->name, curr->age, curr->gender, curr->contact,
               (curr->has_bill ? "YES" : "NO"));
        count++;
        curr = curr->next;
    }
    if (count == 0) {
        printf("  (No patients registered yet)\n");
    }
    printf("Total Patients: %d\n\n", count);
}

void show_doctors(void) {
    printf("\n" COLOR_BOLD COLOR_CYAN "============================== REGISTERED DOCTORS ===============================" COLOR_RESET "\n");
    printf("%-10s | %-22s | %-18s | %-10s | %-12s\n",
           "DOCTOR ID", "NAME", "DEPARTMENT", "FEE ($)", "APPOINTMENTS");
    printf("---------------------------------------------------------------------------------\n");
    Doctor *curr = doctor_list;
    int count = 0;
    while (curr != NULL) {
        printf("%-10s | %-22s | %-18s | $%-9.2f | %-12d\n",
               curr->id, curr->name, curr->dept, curr->fee, curr->appointment_count);
        count++;
        curr = curr->next;
    }
    if (count == 0) {
        printf("  (No doctors registered yet)\n");
    }
    printf("Total Doctors: %d\n\n", count);
}

void show_appointments(void) {
    printf("\n" COLOR_BOLD COLOR_CYAN "============================= SCHEDULED APPOINTMENTS ============================" COLOR_RESET "\n");
    printf("%-12s | %-18s | %-10s | %-20s | %-12s | %-8s\n",
           "PATIENT ID", "PATIENT NAME", "DOC ID", "DOCTOR NAME", "DATE", "TIME");
    printf("---------------------------------------------------------------------------------\n");
    Appointment *curr = appointment_list;
    int count = 0;
    while (curr != NULL) {
        Patient *p = find_patient(curr->patient_id);
        Doctor *d = find_doctor(curr->doctor_id);
        printf("%-12s | %-18s | %-10s | %-20s | %-12s | %-8s\n",
               curr->patient_id, (p ? p->name : "Unknown"),
               curr->doctor_id, (d ? d->name : "Unknown"),
               curr->date, curr->time);
        count++;
        curr = curr->next;
    }
    if (count == 0) {
        printf("  (No appointments scheduled)\n");
    }
    printf("Total Appointments: %d\n\n", count);
}

void show_records_for_patient(const char *pat_id) {
    Patient *p = find_patient(pat_id);
    if (!p) {
        printf(COLOR_RED "  [Error] Patient '%s' not found.\n" COLOR_RESET, pat_id);
        return;
    }
    printf("\n" COLOR_BOLD COLOR_CYAN "======================== MEDICAL HISTORY: %s (%s) ========================" COLOR_RESET "\n",
           p->name, p->id);
    MedicalRecord *curr = record_list;
    int found = 0;
    while (curr != NULL) {
        if (strcmp(curr->patient_id, pat_id) == 0) {
            printf("  Diagnosis    : %s\n", curr->diagnosis);
            printf("  Prescription : %s\n", curr->prescription);
            printf("  Hospitalized : %s\n", curr->admitted ? "YES" : "NO");
            printf("  ---------------------------------------------------------\n");
            found++;
        }
        curr = curr->next;
    }
    if (!found) {
        printf("  (No medical records filed for this patient)\n");
    }
    printf("\n");
}

void show_bill_for_patient(const char *pat_id) {
    Patient *p = find_patient(pat_id);
    if (!p) {
        printf(COLOR_RED "  [Error] Patient '%s' not found.\n" COLOR_RESET, pat_id);
        return;
    }
    Bill *curr = bill_list;
    while (curr != NULL) {
        if (strcmp(curr->patient_id, pat_id) == 0) {
            printf("\n" COLOR_BOLD COLOR_GREEN "============================ INVOICE / FINAL BILL =============================" COLOR_RESET "\n");
            printf("  Patient ID          : %s\n", p->id);
            printf("  Patient Name        : %s\n", p->name);
            printf("  Age / Gender        : %d / %c\n", p->age, p->gender);
            printf("  Doctor Consultation : $%.2f\n", curr->doc_fee);
            printf("  Laboratory Charges  : $%.2f\n", curr->lab_charges);
            printf("  Pharmacy/Medicines  : $%.2f\n", curr->medicine_charges);
            printf("  Room / Bed Charges  : $%.2f\n", curr->room_charges);
            printf("  ---------------------------------------------------------\n");
            printf("  Subtotal            : $%.2f\n", curr->subtotal);
            printf("  Discount Applied    : -%.2f%% (-$%.2f)\n", curr->discount_percent, curr->discount_amount);
            printf("  Hospital Tax (5%%)   : +$%.2f\n", curr->tax);
            printf("  ---------------------------------------------------------\n");
            printf("  " COLOR_BOLD COLOR_GREEN "FINAL AMOUNT DUE    : $%.2f" COLOR_RESET "\n", curr->total);
            printf(COLOR_BOLD COLOR_GREEN "================================================================================" COLOR_RESET "\n\n");
            return;
        }
        curr = curr->next;
    }
    printf(COLOR_YELLOW "  [Notice] No bill has been generated yet for patient '%s'.\n" COLOR_RESET, pat_id);
}

void show_hospital_summary(void) {
    int pat_count = 0, doc_count = 0, app_count = 0, rec_count = 0, bill_count = 0;
    float total_revenue = 0.0f;

    Patient *p = patient_list;
    while (p) { pat_count++; p = p->next; }

    Doctor *d = doctor_list;
    while (d) { doc_count++; d = d->next; }

    Appointment *a = appointment_list;
    while (a) { app_count++; a = a->next; }

    MedicalRecord *m = record_list;
    while (m) { rec_count++; m = m->next; }

    Bill *b = bill_list;
    while (b) { bill_count++; total_revenue += b->total; b = b->next; }

    printf("\n" COLOR_BOLD COLOR_MAGENTA "======================= HOSPITAL OVERALL STATUS SUMMARY =======================" COLOR_RESET "\n");
    printf("  Registered Doctors      : %d\n", doc_count);
    printf("  Registered Patients     : %d\n", pat_count);
    printf("  Appointments Booked     : %d\n", app_count);
    printf("  Medical Records Logged  : %d\n", rec_count);
    printf("  Invoices Generated      : %d\n", bill_count);
    printf("  Total Revenue Accrued   : " COLOR_BOLD COLOR_GREEN "$%.2f" COLOR_RESET "\n", total_revenue);
    printf(COLOR_BOLD COLOR_MAGENTA "================================================================================" COLOR_RESET "\n\n");
}

/* -------------------------------------------------------------
 * ERROR REPORTING PROCEDURES
 * ------------------------------------------------------------- */
void report_lexical_error(const char *token, int lineno) {
    stats.lexical_errors++;
    printf("  " COLOR_RED "[LEXICAL ERROR]" COLOR_RESET " Line %d: Unrecognized token '%s'\n", lineno, token);
}

void report_syntax_error(const char *msg, int lineno) {
    stats.syntax_errors++;
    printf("  " COLOR_RED "[SYNTAX ERROR]" COLOR_RESET " Line %d: %s\n", lineno, msg);
}

void report_semantic_error(const char *msg, int lineno) {
    stats.semantic_errors++;
    printf("  " COLOR_RED "[SEMANTIC ERROR]" COLOR_RESET " Line %d: %s\n", lineno, msg);
}

void print_compiler_summary(void) {
    printf("\n" COLOR_BOLD COLOR_BLUE "=========================== COMPILATION METRICS ===========================" COLOR_RESET "\n");
    printf("  Lines Scanned           : %d\n", stats.lines_processed);
    printf("  Tokens Recognized       : %d\n", stats.tokens_recognized);
    printf("  Statements Parsed       : %d\n", stats.statements_parsed);
    printf("  Lexical Phase Errors    : %d\n", stats.lexical_errors);
    printf("  Syntax Phase Errors     : %d\n", stats.syntax_errors);
    printf("  Semantic Phase Errors   : %d\n", stats.semantic_errors);
    int total_errors = stats.lexical_errors + stats.syntax_errors + stats.semantic_errors;
    printf("  Total Errors            : %d\n", total_errors);
    printf("  Compilation Status      : ");
    if (total_errors == 0) {
        printf(COLOR_BOLD COLOR_GREEN "SUCCESS (0 ERRORS)\n" COLOR_RESET);
    } else {
        printf(COLOR_BOLD COLOR_RED "FAILED WITH %d ERROR(S)\n" COLOR_RESET, total_errors);
    }
    printf(COLOR_BOLD COLOR_BLUE "===========================================================================" COLOR_RESET "\n\n");
}
