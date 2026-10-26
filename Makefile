# Makefile for Hospital Management Language (Lex & Yacc / Flex & Bison)

CC = gcc
CFLAGS = -Wall -Wextra -O2
LEX = flex
YACC = bison -d -y

TARGET = hospital
ifeq ($(OS),Windows_NT)
    TARGET := hospital.exe
    RM = del /f /q
else
    RM = rm -f
endif

all: $(TARGET)

$(TARGET): y.tab.c lex.yy.c hospital_core.c hospital.h
	$(CC) y.tab.c lex.yy.c hospital_core.c -o $(TARGET)

y.tab.c y.tab.h: hospital.y hospital.h
	$(YACC) hospital.y

lex.yy.c: hospital.l y.tab.h hospital.h
	$(LEX) hospital.l

clean:
	$(RM) y.tab.c y.tab.h lex.yy.c $(TARGET)

run-valid: $(TARGET)
	./$(TARGET) test_valid.hml

run-errors: $(TARGET)
	./$(TARGET) test_errors.hml

.PHONY: all clean run-valid run-errors
