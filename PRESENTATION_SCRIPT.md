# Hospital Management Language (HML) Compiler
## 4-Member Viva & Project Presentation Script (With Line-by-Line Output Walkthrough)

---

### **Team Roles & Time Budget (Approx. 8–10 Minutes Total)**
- **Speaker 1 (2 mins):** The Hook, Problem Statement (PS), Product Pitch & Lexical Analysis (`hospital.l`)
- **Speaker 2 (2 mins):** Syntax Analysis, Context-Free Grammar (CFG) & Panic-Mode Error Recovery (`hospital.y`)
- **Speaker 3 (2 mins):** Semantic Engine, 5 In-Memory Symbol Tables & Business Integrity Rules (`hospital_core.c`)
- **Speaker 4 (3–4 mins):** Intermediate Code (TAC), Optimization & **Step-by-Step Output Walkthrough**

---

## 🎬 PRESENTATION SCRIPT

### **SPEAKER 1: Problem Statement, Product Vision & Lexical Analysis**
*(Tone: Confident, engaging, pitching an innovative compiler product)*

> **[SPEAKER 1]:**
> *"Good morning, respected professors and evaluators.*
> 
> *Imagine a busy multi-specialty hospital. Every day, hospital staff process hundreds of patient registrations, doctor appointments, diagnosis notes, and billing calculations.*
> 
> *In traditional hospital software, basic user mistakes cause massive problems: typos crash systems, patient records get assigned to doctors who don't exist, duplicate patient IDs corrupt databases, and negative charges generate wrong invoices.*
> 
> *Our **Problem Statement** was to solve this at the root: by creating a dedicated **Domain-Specific Language and Compiler: the Hospital Management Language (HML)**, built in C using **Lex and Yacc**.*
> 
> *Instead of relying on loose web forms, HML acts as a high-speed, formal compiler engine. Every command—from doctor registration to billing—is tokenized, checked against grammar rules, validated by semantic logic, and converted into intermediate code.*
> 
> *Let us look at Phase 1: **The Lexical Analyzer** in `hospital.l`.*
> 
> *Our Lexer converts raw text into clean tokens using **Regular Expressions** for hospital keywords like `REGISTER`, `PATIENT`, `DOCTOR`, `SCHEDULE`, `DIAGNOSIS`, and `BILL`.*
> * *It distinguishes between integers (like patient age), floats (like consultation fees), and quoted strings (like names and prescriptions).*
> * *It is **case-insensitive**, so staff can type in uppercase or lowercase.*
> * *It tracks line numbers using `yylineno`. If someone types an illegal symbol like `@` or `#`, it immediately catches it as a **Lexical Error** without crashing.*
> 
> *I will now hand over to **[Speaker 2]** to explain our Syntax Analysis and Grammar."*

---

### **SPEAKER 2: Syntax Analysis, Grammar & Error Recovery**
*(Tone: Structured and technical)*

> **[SPEAKER 2]:**
> *"Thank you, [Speaker 1]. Good morning, professors.*
> 
> *Once Lex turns text into tokens, the **Syntax Analyzer** in `hospital.y` takes over, generated using **GNU Bison**.*
> 
> *We designed a formal **Context-Free Grammar (CFG)** that parses statements using a **Bottom-Up LALR(1)** algorithm:*
> * *Yacc produces `y.tab.h` containing token constants.*
> * *`yyparse()` calls `yylex()` to fetch tokens one by one, while data like names and numbers are shared through the `%union` and `yylval`.*
> 
> *Our grammar supports 6 core command types:*
> 1. *`REGISTER DOCTOR` with department and consultation fee.*
> 2. *`REGISTER PATIENT` with age, gender, and contact.*
> 3. *`SCHEDULE APPOINTMENT` linking patients to doctors on a date and time.*
> 4. *`ADD RECORD` documenting diagnosis, medicine, and admission status.*
> 5. *`GENERATE BILL` calculating lab, pharmacy, and room charges with discounts.*
> 6. *`SHOW` commands to inspect live symbol tables.*
> 
> *Our biggest feature in this phase is **Panic-Mode Syntax Error Recovery**:*
> ```yacc
> error_stmt: error SEMI { yyerrok; } ;
> ```
> *In a hospital, a typo must **never crash the system**. If someone misses the keyword `DOCTOR` or forgets an argument, the parser catches the error, logs the exact line number, discards bad tokens until the semicolon `;`, and calls `yyerrok` to resume parsing the next statement smoothly.*
> 
> *Now, syntax only verifies grammar structure. To check if the data actually makes real-world medical sense, we built our Semantic Engine. I invite **[Speaker 3]** to explain it."*

---

### **SPEAKER 3: Semantic Engine & Multi-Entity Symbol Tables**
*(Tone: Analytical, business-logic focused)*

> **[SPEAKER 3]:**
> *"Thank you, [Speaker 2]. Good morning, professors.*
> 
> *A statement can be syntactically correct but logically absurd—for example, booking an appointment with a doctor who isn't registered, or entering an age of 150.*
> 
> *To protect data integrity, we built our **Semantic Engine and Symbol Table** in `hospital_core.c` and `hospital.h`.*
> 
> *Our compiler maintains **5 in-memory Symbol Tables** using dynamic linked lists: Doctors, Patients, Appointments, Medical Records, and Bills.*
> 
> *During compilation, our semantic engine enforces 5 crucial rules:*
> 1. * **Duplicate ID Check:** Disallows registering two doctors or two patients with the same ID.*
> 2. * **Referential Integrity:** Verifies that patient and doctor IDs exist before allowing an appointment or medical record.*
> 3. * **Range Validation:** Ensures age is strictly between 1 and 120, discount is between 0% and 100%, and fees are non-negative.*
> 4. * **Dynamic Fee Retrieval:** When generating a bill, the compiler automatically looks up the patient's doctor in the Doctor Symbol Table and **injects the doctor's consultation fee** into the bill.*
> 5. * **Duplicate Bill Protection:** Ensures only one finalized bill is created per patient.*
> 
> *Now, how are calculations processed into intermediate code, and what does the final output look like? I hand over to **[Speaker 4]** for the live demonstration and output walkthrough."*

---

### **SPEAKER 4: Intermediate Code (TAC) & Step-by-Step Output Walkthrough**
*(Tone: Practical, energetic, pointing to screen results)*

> **[SPEAKER 4]:**
> *"Thank you, [Speaker 3]. Respected professors, I will now demonstrate our compiler live and explain **every single result on the screen in simple, step-by-step words**.*
> 
> *(**Action:** Run `build.bat`)*
> 
> *'First, we run `build.bat`. Bison creates `y.tab.c`, Flex creates `lex.yy.c`, and GCC compiles everything into `hospital.exe` in under 2 seconds.'*
> 
> *(**Action:** Run `hospital.exe test_valid.hml`)*
> 
> *'Now, let us examine the output of our valid test suite line by line:*
> 
> #### 1. Registration & Scheduling Output
> * *Lines with **`[OK] Registered Doctor`** and **`[OK] Registered Patient`**: The Lexer and Parser validated each field, and our C core created active records in the Symbol Table.*
> * *Lines with **`[OK] Scheduled Appointment`** and **`[OK] Added Medical Record`**: The compiler confirmed that both patient and doctor exist, and linked their records together.*
> 
> #### 2. Three-Address Code (TAC) Output (Our Big Star Feature)
> * *Look at this box labeled **`[INTERMEDIATE CODE GENERATION - THREE ADDRESS CODE]`**.*
> * *In compiler design, high-level math must be broken down into elementary instructions with at most three addresses. We implemented this in **Quadruple format: `(Operator, Arg1, Arg2, Result)`**.*
> * *Think of `t1`, `t2`, `t3` like temporary registers on a calculator:*
>   * *`t1` fetches Doctor Fee ($800).*
>   * *`t2 = t1 + 1200` adds Lab Charges ($2000).*
>   * *`t3 = t2 + 450` adds Medicine ($2450).*
>   * *`t4 = t3 + 0` adds Room Charges (Subtotal = $2450).*
>   * *`t5` and `t6` compute the 10% discount (-$245).*
>   * *`t7` gives the discounted amount ($2205).*
>   * *`t8` computes 5% hospital tax (+$110.25).*
>   * *`t9` computes final payable amount ($2315.25), saved into `TOTAL_PAT201`.*
> 
> #### 3. Code Optimization Output
> * *Notice the box below it: **`[CODE OPTIMIZATION]`**.*
> * *Our compiler detected that Patient Aarav Mehta had zero room charges. Instead of wasting CPU cycles calculating `+ 0`, it applied **Algebraic Identity Optimization** (`t4 = t3 + 0` simplifies directly to `t4 = t3`). That satisfies Unit 4 of our syllabus!*
> 
> #### 4. Formatted Tables & Hospital Summary
> * *Next, our `SHOW` commands display structured ASCII tables:*
>   * *Registered Doctors Table shows each doctor's fee and total appointments handled.*
>   * *Registered Patients Table shows age, gender, contact, and billing status.*
>   * *Appointments Table displays dates and doctor-patient pairings.*
>   * *Medical History Table prints diagnosis and prescriptions.*
>   * *Invoice Receipt prints an itemized bill showing Subtotal, Discount, Tax, and Final Due.*
>   * *Hospital Overall Summary aggregates total patients, appointments, and total revenue accrued ($13,188).*
> 
> #### 5. Compilation Metrics Dashboard
> * *At the very bottom: **42 lines scanned, 207 tokens recognized, 22 statements parsed, and 0 errors**. The compiler outputs **SUCCESS**.*
> 
> ---
> 
> *(**Action:** Run `hospital.exe test_errors.hml`)*
> 
> *'Now, to prove our compiler is completely robust against human error, we run `test_errors.hml`. Let us see how it handles mistakes step-by-step:*
> 
> #### 1. Lexical Errors
> * *Line 12 and 14 print **`[LEXICAL ERROR] Unrecognized token '@'` and `'#'`**. The Lexer rejected these characters because they do not belong to our language alphabet.*
> 
> #### 2. Syntax Errors & Panic Recovery
> * *Line 18 and 21 print **`[SYNTAX ERROR]`** because we intentionally omitted the keyword `DOCTOR` and missed arguments.*
> * *Notice: **The compiler did NOT crash!** Our panic-mode recovery discarded the invalid tokens up to the semicolon, recovered cleanly, and parsed the very next doctor on Line 22.*
> 
> #### 3. Semantic Logic Errors
> * *Line 29: Catches **Duplicate Doctor ID**.*
> * *Line 32: Catches **Duplicate Patient ID**.*
> * *Line 35 & 36: Catches **Invalid Age (155 and -5)**.*
> * *Line 39 & 42: Catches **Unregistered Patient and Unregistered Doctor**.*
> * *Line 51: Rejects **Negative Lab Charges**.*
> * *Line 54: Rejects a **150% Discount**.*
> * *Line 60: Blocks **Duplicate Final Billing** for the same patient.*
> 
> #### 4. Final Diagnostics
> * *The dashboard at the end precisely summarizes: **2 Lexical Errors, 2 Syntax Errors, 11 Semantic Errors = 15 Total Errors** with status FAILED WITH 15 ERRORS.*
> 
> *This concludes our live demonstration. Our system covers lexical analysis, LALR grammar parsing, panic recovery, semantic verification, intermediate code generation, and optimization. We are now ready for your questions. Thank you!'*

---

## 🎯 Viva Q&A Quick Reference (Who Speaks When Asked)

| If the Examiner asks about... | Who should immediately answer | Key phrases to say |
| :--- | :--- | :--- |
| **Tokens, regex, `yytext`, `yylineno`, or lexical errors** | **Speaker 1** | *"Regular expressions, Flex scanner, token stream, line counter."* |
| **Grammar rules, CFG, LALR parsing, shift-reduce, or syntax recovery** | **Speaker 2** | *"CFG, Bison parser, LALR(1), panic-mode recovery with `yyerrok`."* |
| **Symbol table design, linked lists, duplicate IDs, or age checks** | **Speaker 3** | *"Linked list symbol tables, referential integrity, semantic actions."* |
| **Three-Address Code (TAC), Quadruples, Optimization, or how to run** | **Speaker 4** | *"Quadruples `(Op, Arg1, Arg2, Result)`, constant folding, algebraic optimization."* |
