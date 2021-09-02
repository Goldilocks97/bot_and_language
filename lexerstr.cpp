#include <stdio.h>
#include <string.h>

#include "psl.hpp"
#include "lexerstr.hpp"

bool delimiter_general(int c)
{
    char symb[] = "+-*/%<>=:|&!{}()[];,";
    int i = 0;

    while (symb[i]) {
        if (c == symb[i])
            return true;
        i++;
    }
    return false;
    
    /*return (c == '+' ||
            c == '-' ||
            c == '*' ||
            c == '/' ||
            c == '%' ||
            c == '<' ||
            c == '>' ||
            c == '=' ||
            c == ':' ||
            c == '|' ||
            c == '&' ||
            c == '!' ||
            c == '{' ||
            c == '}' ||
            c == '(' ||
            c == ')' ||
            c == '[' ||
            c == ']' ||
            c == ';' ||
            c == ',');*/
}

void print_type(int type)
{
    switch (type) {
        case wait:
            printf("delimiter");
            break;
        case name:
            printf("name");
            break;
        case constant:
            printf("constant");
            break;
        case keyword:
            printf("keyword");
            break;
        case string:
            printf("string");
            break;
        case assigment:
            printf("assigment");
            break;
        default:
            break;
    }
}

bool char_name(int c)
{
    return (((c >= 'a') && (c <= 'z')) ||
            ((c >= 'A') && (c <= 'Z')) ||
            ((c >= '0') && (c <= '9')) ||
            (c == '_'));
}

bool char_constant(int c)
{
    return ((c >= '0') && (c <= '9'));
}

bool char_keyword(int c)
{
    return (((c >= 'a') && (c <= 'z')) ||
             ((c >= 'A') && (c <= 'Z')));
}

bool delimiter_wait(int c)
{
    return ((c == ' ') || (c == '\n') || (c == '\t') || (c == -1));
}

bool delimiter(int c)
{
    return (delimiter_wait(c) || delimiter_general(c));
}

lex_list *Lexer::GetList() const
{
    return first;
}

Lexer::Lexer()
{
    first = 0;
    last = 0;
    current = 0;
    lex_type = wait;
    line = 1;
    state_machine = work;
    possition = 0;
    state = wait;
    bufsize = 1024;
    buf = new char[bufsize];
    buf[current] = 0;
}

states Lexer::GetMachineState()
{
    return state_machine;
}

void delete_lexlist(lex_list *first)
{
    while (first) {
        lex_list *p = first;
        first = first->next;
        delete[] p->lexem;
        delete p;
    }    	
}

void Lexer::SetState(int c)
{
    if ((c == '$') || (c == '?'))
        state = name;
    if ((c >= '0') && (c <= '9'))
        state = constant;
    if (((c >= 'a') && (c <= 'z')) ||
         ((c >= 'A') && (c <= 'Z')))
        state = keyword;
    if (c == '"')
        state = string;
    if (c == ':')
        state = assigment;
}

char *Lexer::GetLexem()
{
    int i;
    char *lx;

    i = str_len(buf);
    lx = new char[i+1];
    strncpy(lx, buf, i+1);
    current = 0;
    buf[current] = 0;
    return lx;
}

void add_null_lexem(lex_list **first)
{
    if (*first) {
        lex_list *p, *last = *first;
        while (last->next)
            last = last->next;
        p = new lex_list;
        p->lexem = 0;
        p->line_num = last->line_num;
        p->type = null;
        p->next = 0;
        last->next = p;
    } else {
        *first = new lex_list;
        (*first)->lexem = 0;
        (*first)->line_num = 1;
        (*first)->type = null;
        (*first)->next = 0;
    }
}

void Lexer::AddLexem(char *lx)
{
    if (first != 0) {
        lex_list *p = new lex_list;
        p->lexem = lx;
        p->line_num = line;
        p->pos = possition;
        p->type = lex_type;
        p->next = 0;
        last->next = p;
        last = p;
    } else {
        first = new lex_list;
        first->lexem = lx;
        first->type = lex_type;
        first->pos = possition;
        first->line_num = line;
        first->next = 0;
        last = first;
    }
}

void Lexer::SendLexem()
{
    char *lx;

    buf[current] = 0;
    lx = GetLexem();
    AddLexem(lx);
}

void Lexer::AddChar(int c)
{
    if (c == '"')
        return;
    if ((current - 1) != bufsize) {
        buf[current] = c;
        current++;
    } else {
        char *p;

        bufsize *= 2;
        p = new char[bufsize];
        strcpy(p, buf);
        delete[] buf;
        buf = p;
        buf[current] = c;
        current++;
    }
}

void Lexer::ThrowError() 
{
    state_machine = error;
    lex_type = error;
    SendLexem();
}

void Lexer::DelimiterMet(int c, states new_type)
{
    state = wait;
    lex_type = new_type;
    StateMachine(c);
}

void Lexer::StateWait(int c)
{
    if (buf[0] != 0) {
        SendLexem();
        lex_type = wait;
    }
    if (c == '\n') {
        possition = 0;
        line++;
    }
    if (delimiter_wait(c))
        return;
    SetState(c);
    AddChar(c);
    if ((state == wait) && (!delimiter_general(c)))
        ThrowError();
}

void Lexer::StateName(int c)
{
    if (delimiter(c)) {
        DelimiterMet(c, name);
        return;
    }
    AddChar(c);
    if (!char_name(c))
        ThrowError();
}

void Lexer::StateConstant(int c)
{
    if (delimiter(c)) {
        DelimiterMet(c, constant);
        return;
    }
    AddChar(c);
    if (!char_constant(c))
        ThrowError();
}

void Lexer::StateKeyword(int c)
{
    if (delimiter(c)) {
        DelimiterMet(c, keyword);
        return;
    }
    AddChar(c);
    if (!char_keyword(c))
        ThrowError();
}

void Lexer::StateString(int c)
{
    //if (c != '"')
    AddChar(c);
    if (c == '"') {
        state = wait;
        lex_type = string;
    }
}

void Lexer::StateAssigment(int c)
{
    if (delimiter_wait(c)) {
        DelimiterMet(c, wait);
    } else {
        AddChar(c);
        state = wait;
        lex_type = assigment;
    if (c != '=')
        ThrowError();
    }
}

void Lexer::StateMachine(int c)
{
    if (state_machine == error)
        return;
    switch (state) {
        case wait:
            StateWait(c);
            break;
        case name:
            StateName(c);
            break;
        case constant:
            StateConstant(c);
            break;
        case keyword:
            StateKeyword(c);
            break;
        case string:
            StateString(c);
            break;
        case assigment:
            StateAssigment(c);           
            break;
        default:
            break;
    }
}

void Lexer::Run(int c)
{
    if (state_machine != error)
        possition++;
    StateMachine(c);
}

void report_error(const lex_list *p)
{
    printf("Forbidden char.");
    printf("\n\tLine %d, possition %d.\n", p->line_num, p->pos);
}

void print_lexems(const lex_list *p)
{
    while (p) {
        if (p->type == error) {
            report_error(p);
            return;
        }
        /*printf("line: %d ", p->line_num);
        printf("type: ");
        print_type(p->type);
        if (p->lexem)
            printf(" lexem: %s\n", p->lexem);
        else
            printf("null\n");*/
        p = p->next;
    }
}
