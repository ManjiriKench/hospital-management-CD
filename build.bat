@echo off
echo ===================================================================
echo   COMPILING HOSPITAL MANAGEMENT LANGUAGE (LEX AND YACC / FLEX AND BISON)
echo ===================================================================

echo [1/3] Running Bison (Parser Generator)...
bison -d -y hospital.y
if %ERRORLEVEL% NEQ 0 (
    echo Error: Bison execution failed!
    exit /b %ERRORLEVEL%
)

echo [2/3] Running Flex (Lexical Analyzer Generator)...
flex hospital.l
if %ERRORLEVEL% NEQ 0 (
    echo Error: Flex execution failed!
    exit /b %ERRORLEVEL%
)

echo [3/3] Compiling C source files with GCC...
gcc y.tab.c lex.yy.c hospital_core.c -o hospital.exe
if %ERRORLEVEL% NEQ 0 (
    echo Error: GCC compilation failed!
    exit /b %ERRORLEVEL%
)

echo.
echo ===================================================================
echo   BUILD SUCCESSFUL! Created 'hospital.exe'
echo ===================================================================
echo.
echo To run test suite with valid statements:
echo     hospital.exe test_valid.hml
echo.
echo To run error handling test suite:
echo     hospital.exe test_errors.hml
echo.
echo To run interactive console mode:
echo     hospital.exe
echo.
