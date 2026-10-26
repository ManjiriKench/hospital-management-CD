# Compiler Design Project: Hospital Management Language (HML)
### Lexical and Syntax Analyzer with Semantic Engine, Symbol Table, and Intermediate Code Generation (TAC)

---

## 1. Project Overview

This project implements a complete Domain-Specific Compiler/Interpreter for a **Hospital Management Language (HML)** using **Lex (Flex)** and **Yacc (Bison)** written in standard C. 

The language processes statements for:
1. **Patient Registration** (`REGISTER PATIENT ...`)
2. **Doctor Registration** (`REGISTER DOCTOR ...`)
3. **Doctor Appointments** (`SCHEDULE APPOINTMENT ...`)
4. **Medical Records** (`ADD RECORD ...`)
5. **Billing and Invoicing** (`GENERATE BILL ...`)
6. **Symbol Table Inspection & Analytics** (`SHOW PATIENTS`, `SHOW DOCTORS`, `SHOW BILL`, `SHOW SUMMARY`)

---

## 2. Syllabus & Lab Assignment Alignment

This project satisfies all key components of your Compiler Design syllabus:

| Syllabus Unit | Compiler Phase | Implementation in this Project |
| :--- | :--- | :--- |
| **Unit 1** | **Lexical Analysis (LEX)** | Specification of tokens using Regular Expressions in `hospital.l`. Recognition of keywords, identifiers, integers, floats, and strings. Tracking line numbers (`yylineno`). Detection and reporting of lexical phase errors. |
| **Unit 2** | **Syntax Analysis (YACC)** | Context-Free Grammar (CFG) in `hospital.y`. Bottom-up LALR(1) parsing. Dynamic symbol table creation for Doctors, Patients, Appointments, and Records. Error detection with panic-mode recovery (`error ';'` rule). |
| **Unit 3** | **Semantic Analysis & SDT** | Type checking, range checking (e.g., age between 1-120, discount between 0-100%), duplicate ID checking, referential integrity (checking if patient/doctor exists before booking or billing). Dynamic consultation fee retrieval. |
| **Unit 3 / Unit 7** | **Intermediate Code Gen (TAC)** | Generation of **Three-Address Code (TAC)** in Quadruple format `(Op, Arg1, Arg2, Result)` for billing calculations. |
| **Unit 4** | **Code Optimization** | TAC optimizer demonstrating **Constant Folding**, **Algebraic Simplification**, and **Dead Code Elimination** (e.g., zero room charge or 0% discount identity reduction). |

---

## 3. Project File Structure

```
CD/
├── hospital.l          # LEX (Flex) Specification: Token definitions and Regular Expressions
├── hospital.y          # YACC (Bison) Grammar: CFG rules, Syntax error recovery, Driver
├── hospital.h          # Header file: Symbol table structures, AST/TAC definitions, Prototypes
├── hospital_core.c     # Core C logic: Symbol tables, Semantic rules, TAC Generator, Display
├── test_valid.hml      # Complete test script showing all valid language features
├── test_errors.hml     # Test script demonstrating Lexical, Syntax, and Semantic errors + Recovery
├── build.bat           # 1-click Windows build script
├── Makefile            # Makefile for Linux / Unix / GCC environments
└── README.md           # Full project documentation & Viva prep
```

---

## 4. Language Syntax & Command Specification

### 1. Doctor Registration
```sql
REGISTER DOCTOR <doc_id> NAME "<name>" DEPT "<department>" FEE <amount> ;
```
*Example:*
```sql
REGISTER DOCTOR DOC101 NAME "Dr. Rajesh Sharma" DEPT "Cardiology" FEE 800.00 ;
```

### 2. Patient Registration
```sql
REGISTER PATIENT <patient_id> NAME "<name>" AGE <int> GENDER <M|F|O> CONTACT "<phone>" ;
```
*Example:*
```sql
REGISTER PATIENT PAT201 NAME "Aarav Mehta" AGE 34 GENDER M CONTACT "9876543210" ;
```

### 3. Appointment Scheduling
```sql
SCHEDULE APPOINTMENT FOR PATIENT <patient_id> WITH DOCTOR <doc_id> DATE "<YYYY-MM-DD>" TIME "<HH:MM>" ;
```
*Example:*
```sql
SCHEDULE APPOINTMENT FOR PATIENT PAT201 WITH DOCTOR DOC101 DATE "2026-11-02" TIME "10:30 AM" ;
```

### 4. Medical Record Addition
```sql
ADD RECORD FOR PATIENT <patient_id> DIAGNOSIS "<condition>" PRESCRIPTION "<drugs>" ADMITTED <YES|NO> ;
```
*Example:*
```sql
ADD RECORD FOR PATIENT PAT201 DIAGNOSIS "Mild Hypertension" PRESCRIPTION "Amlodipine 5mg" ADMITTED NO ;
```

### 5. Billing and Intermediate Code Generation
```sql
GENERATE BILL FOR PATIENT <patient_id> LAB_CHARGES <amt> MEDICINE_CHARGES <amt> ROOM_CHARGES <amt> DISCOUNT <percent>% ;
```
*Example:*
```sql
GENERATE BILL FOR PATIENT PAT201 LAB_CHARGES 1200.00 MEDICINE_CHARGES 450.00 ROOM_CHARGES 0.00 DISCOUNT 10.00% ;
```
*Automatically queries the Doctor Symbol Table to fetch the Doctor's consultation fee if the patient had an appointment!*

### 6. Symbol Table Dumps and Querying
```sql
SHOW DOCTORS ;
SHOW PATIENTS ;
SHOW APPOINTMENTS ;
SHOW RECORDS FOR PATIENT <patient_id> ;
SHOW BILL FOR PATIENT <patient_id> ;
SHOW SUMMARY ;
```

---

## 5. Star Features (College Evaluator High-Scorers)

1. **Three-Address Code (TAC) Quadruple Generation**:
   Directly translates billing statements into standard compiler intermediate representation:
   ```
   (ASSIGN,  800.00,       NULL,       t1)    ; t1 = Doctor Consultation Fee
   (ADD,     t1,         1200.00,       t2)    ; t2 = t1 + Lab Charges
   (ADD,     t2,          450.00,       t3)    ; t3 = t2 + Medicine Charges
   (ADD,     t3,            0.00,       t4)    ; t4 = t3 + Room Charges
   (DIV,      10.00,       100.0,      t5)    ; t5 = Discount Rate
   (MULT,    t4,              t5,       t6)    ; t6 = Discount Amount
   (SUB,     t4,              t6,       t7)    ; t7 = Subtotal - Discount
   (MULT,    t7,            0.05,       t8)    ; t8 = Hospital Tax (5%)
   (ADD,     t7,              t8,       t9)    ; t9 = Final Payable Amount
   (STORE,   t9,            NULL,       TOTAL_PAT201)
   ```

2. **Code Optimization Analysis (Unit 4)**:
   - Identifies algebraic identities (e.g. `t4 = t3 + 0` is optimized to eliminate addition with identity zero).
   - Constant folding for GST factor (`t9 = t7 * 1.05`).
   - Dead-code detection when discount is 0%.

3. **Multi-Entity Dynamic Symbol Table**:
   - Doctor, Patient, Appointment, Medical Record, and Invoice tables maintained as linked lists with cross-references.

4. **Rigorous Semantic Analysis**:
   - Duplicate Doctor / Patient ID check.
   - Non-existent ID reference check (Referential Integrity).
   - Value validation (e.g., Age 1–120, Discount 0–100%, Non-negative fees).
   - Duplicate billing prevention.

5. **Graceful Syntax Error Recovery**:
   - Uses `error ';'` production in YACC grammar. If a syntax error occurs, the parser reports the error with line number, discards bad tokens up to `;`, and resumes parsing subsequent statements without crashing.

6. **Comprehensive Compilation Metrics**:
   - Displays lines scanned, tokens recognized, statements parsed, and categorized errors (Lexical, Syntax, Semantic).

---

## 6. How to Compile and Run

### On Windows (using MinGW & GnuWin32):
You can use the automated script:
```powershell
.\build.bat
```
Or run manually step-by-step:
```powershell
# Step 1: Run Bison (Parser Generator)
bison -d -y hospital.y

# Step 2: Run Flex (Lexical Analyzer)
flex hospital.l

# Step 3: Compile with GCC
gcc y.tab.c lex.yy.c hospital_core.c -o hospital.exe
```

### Running Test Files:
1. **Valid language features test**:
   ```powershell
   .\hospital.exe test_valid.hml
   ```
2. **Error detection & recovery test**:
   ```powershell
   .\hospital.exe test_errors.hml
   ```
3. **Interactive Console Mode**:
   ```powershell
   .\hospital.exe
   ```
   *Type commands ending with `;`. Press `Ctrl + Z` and then `Enter` on Windows to evaluate.*

### On Linux / macOS:
```bash
make
./hospital test_valid.hml
./hospital test_errors.hml
```

---

## 7. Viva Voce Questions & Answers (Compiler Design Lab)

### Q1: What is the role of Lex and Yacc?
- **Lex (Flex)**: Performs **Lexical Analysis**. It scans the raw character stream of source code, recognizes patterns using **Regular Expressions**, and outputs a stream of **Tokens**.
- **Yacc (Bison)**: Performs **Syntax Analysis**. It takes the token stream from Lex, applies **Context-Free Grammar (CFG)** rules using a **Bottom-Up LALR(1)** parsing algorithm, and builds parse trees while executing semantic actions.

### Q2: How do Lex and Yacc communicate?
- Yacc generates `y.tab.h`, which contains `#define` constants for each terminal token (e.g., `REGISTER`, `IDENTIFIER`, `INT_VAL`).
- When `yyparse()` in Yacc needs the next token, it calls `yylex()`.
- `yylex()` returns the integer token type.
- Attributes or values of tokens (e.g. number values, string text) are passed via the global union variable `yylval`.

### Q3: What is the purpose of `yytext`, `yyleng`, and `yylineno`?
- `yytext`: A C string holding the exact characters that matched the regular expression.
- `yyleng`: The length of the string in `yytext`.
- `yylineno`: Maintains the current line number of the source file, which is crucial for error reporting.

### Q4: What type of parser is generated by YACC?
- Yacc produces a **Bottom-Up LALR(1) Parser** (Look-Ahead Left-to-right, Rightmost derivation in reverse with 1 lookahead token). It uses a pushdown automaton with state transition tables (ACTION and GOTO tables).

### Q5: How does Error Recovery work in your YACC grammar?
- We implemented **Panic-Mode Error Recovery** using the special Yacc terminal `error`:
  ```yacc
  error_stmt: error SEMI { yyerrok; } ;
  ```
  When an invalid token sequence occurs, Yacc enters error recovery mode, pops tokens from its parse stack until it finds a rule with `error`, and discards incoming tokens until it encounters the synchronization token (`;`). Calling `yyerrok` resets the error state, enabling the parser to process subsequent statements smoothly.

### Q6: What is a Symbol Table and why is it needed?
- A **Symbol Table** is a data structure used by compilers to store information about source-program identifiers (variable names, types, scopes, attributes). In our Hospital Language, the symbol table stores registered Doctors, Patients, Appointments, and Bills, allowing semantic checks such as verifying patient existence and preventing duplicate registrations.

### Q7: What is Three-Address Code (TAC)?
- **Three-Address Code** is an Intermediate Representation (IR) where each instruction has at most one operator on the right-hand side and at most three operands/addresses (two sources and one destination):
  $$x = y \text{ op } z$$
  In our project, we use the **Quadruple** representation: `(Operator, Argument 1, Argument 2, Result)` to evaluate billing expressions.
