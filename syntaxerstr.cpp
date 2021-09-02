#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psl.hpp"
#include "syntaxerstr.hpp"
#include "lexerstr.hpp"
#include "rpn.hpp"

void Syntaxer::AddRPNElem(RPNElem *el)
{
    RPNItem *new_item = new RPNItem;

    new_item->elem = el;
    new_item->next = 0;
    if (last) {
        last->next = new_item;
        last = new_item;
    } else {
        first = new_item;
        last = first;
    }
}

bool Syntaxer::GetNextLexem()
{
    current = current->next;
    if (current->lexem)
        return true;
    else
        return false;
}

void Syntaxer::Run()
{
    Start();
}

void Syntaxer::Start()
{
    Command();
    GetNextLexem();
    if (current->type != null)
        Start();
}

void Syntaxer::Command()
{
    if (current->type == name) {
        try {
            Name();
        }
        catch (SyntaxError err) {
            if (err.GetType() == nothing_after_exp)
                throw SyntaxError(no_end, current);
            else
                throw err;
        }
        if (this_char(current, 0) != ';')
            throw SyntaxError(wrong_end, current);
        return;
    }
    if (current->type == keyword) {
        Keyword();
        return;
    }
    throw SyntaxError(no_state_op, current);
}

void Syntaxer::Name()
{
    if (this_char(current, 0) == '?')
        Function();
    else
        Variable();
}

void Syntaxer::OneArg()
{
    if (this_char(current, 0) == ')')
        throw SyntaxError(req_one_arg, current);
    try {
        Exp1();
    }
    catch (SyntaxError err) {
        if (err.GetType() == nothing_after_exp)
            throw SyntaxError(req_one_arg, current);
        else
            throw err;
    }
    if (this_char(current, 0) == ',')
        throw SyntaxError(req_one_arg, current);
    if (this_char(current, 0) != ')')
        throw SyntaxError(right_bracket, current);
    if (!GetNextLexem())
        throw SyntaxError(nothing_after_exp, current);
}

void Syntaxer::TwoArg()
{
    if (this_char(current, 0) == ')')
        throw SyntaxError(req_two_arg, current);
    try {
        Exp1();
    }
    catch(SyntaxError err) {
        if (err.GetType() == nothing_after_exp)
            throw SyntaxError(req_two_arg, current);
        else
            throw err;
    }
    if (this_char(current, 0) != ',')
        throw SyntaxError(wrong_comma, current);
    if ((!GetNextLexem()) || (this_char(current, 0) == ')'))
        throw SyntaxError(req_two_arg, current);
    OneArg();
}

void Syntaxer::ZeroArg()
{
    if (!GetNextLexem())
        throw SyntaxError(no_right_bracket, current);
    if (this_char(current, 0) != ')')
        throw SyntaxError(req_zero_arg, current);
    if (!GetNextLexem())
        throw SyntaxError(nothing_after_exp, current);
}

void Syntaxer::ManyArg()
{
    try {
        Exp1();
    }
    catch (SyntaxError err) {
        if (err.GetType() == nothing_after_exp)
            throw SyntaxError(no_comma_or_bracket, current);
        else
            throw err;
    }
    if (this_char(current, 0) == ',') {
        if (!GetNextLexem())
            throw SyntaxError(no_exp, current);
        ManyArg();
    } else
        if (this_char(current, 0) == ')')
            return;
    else
        throw SyntaxError(wrong_comma_bracket, current);
}

ftype is_two_arg(lex_list *p)
{
    if (!strcmp("?buy", p->lexem))
        return fbuy;
    if (!strcmp("?sell", p->lexem))
        return fsell;
    if (!strcmp("?max", p->lexem))
        return fmax;
    if (!strcmp("?min", p->lexem))
        return fmin;
    if (!strcmp("?num_by_percent", p->lexem))
        return fnbp;
    if (!strcmp("?perc_by_number", p->lexem))
        return fpbn;
    if (!strcmp("?random", p->lexem))
        return frand;
    return fno;
}

ftype is_one_arg(lex_list *p)
{
    if (!strcmp("?triple", p->lexem))
        return ftrip;
    if (!strcmp("?prod", p->lexem))
        return fprod;
    if (!strcmp("?build", p->lexem))
        return fbuild;
    if (!strcmp("?money", p->lexem))
        return fmoney;
    if (!strcmp("?raw", p->lexem))
        return fraw;
    if (!strcmp("?production", p->lexem))
        return fproduction;
    if (!strcmp("?factories", p->lexem))
        return ffact;
    if (!strcmp("?auto_factories", p->lexem))
        return fautofact;
    if (!strcmp("?manufactured", p->lexem))
        return fmanuc;
    if (!strcmp("?result_raw_amount", p->lexem))
        return fresrawamount;
    if (!strcmp("?result_raw_price", p->lexem))
        return fresrawprice;
    if (!strcmp("?result_prod_amount", p->lexem))
        return fresprodamount;
    if (!strcmp("?result_prod_price", p->lexem))
        return fresprodprice;
    return fno;
}

ftype is_zero_arg(lex_list *p)
{
    if (!strcmp("?endturn", p->lexem))
        return fendturn;
    if (!strcmp("?my_id", p->lexem))
        return fmyid;
    if (!strcmp("?turn", p->lexem))
        return fturn;
    if (!strcmp("?players", p->lexem))
        return fplayers;
    if (!strcmp("?active_players", p->lexem))
        return factplayers;
    if (!strcmp("?supply", p->lexem))
        return fsupply;
    if (!strcmp("?raw_price", p->lexem))
        return frawprice;
    if (!strcmp("?demand", p->lexem))
        return fdemand;
    if (!strcmp("?production_price", p->lexem))
        return fprodprice;
    return fno;
}

int this_char(lex_list *p, int index)
{
    return p->lexem[index];
}

RPNElem* Syntaxer::ConvertToRPNFunc(ftype ft)
{
    switch (ft) {
        case ftrip:
            return new RPNFunTriple;
        case fbuy:
            return new RPNFunBuy;
        case fprod:
            return new RPNFunProd;
        case fbuild:
            return new RPNFunBuild;
        case fmoney:
            return new RPNFunMoney;
        case fraw:
            return new RPNFunRaw;
        case fproduction:
            return new RPNFunProduction;
        case ffact:
            return new RPNFunFactories;
        case fautofact:
            return new RPNFunAutoFactories;
        case fmanuc:
            return new RPNFunManufactured;
        case fresrawamount:
            return new RPNFunResultRawAmount;
        case fresrawprice:
            return new RPNFunResultRawPrice;
        case fresprodamount:
            return new RPNFunResultProdAmount;
        case fresprodprice:
            return new RPNFunResultProdPrice;
        case fendturn:
            return new RPNFunEndturn;
        case fmyid:
            return new RPNFunMyId;
        case fplayers:
            return new RPNFunPlayers;
        case factplayers:
            return new RPNFunActivePlayers;
        case fsupply:
            return new RPNFunSupply;
        case frawprice:
            return new RPNFunRawPrice;
        case fdemand:
            return new RPNFunDemand;
        case fprodprice:
            return new RPNFunProductionPrice;
        case fsell:
            return new RPNFunSell;
        case fturn:
            return new RPNFunTurn;
        case fmax:
            return new RPNFunMax;
        case fmin:
            return new RPNFunMin;
        case fnbp:
            return new RPNFunNumPerc;
        case fpbn:
            return new RPNFunPerc;
        case frand:
            return new RPNFunRandom;
        default:
            return 0;
    }
}

void Syntaxer::Function()
{
    lex_list *func = current;
    ftype res;

    if (!GetNextLexem())
        throw SyntaxError(no_left_bracket, current);
    if (this_char(current, 0) != '(')
        throw SyntaxError(left_bracket, current);
    if ((res = is_zero_arg(func)) != fno) {
        ZeroArg();
        AddRPNElem(ConvertToRPNFunc(res));
        return;
    }
    if (!GetNextLexem())
        throw SyntaxError(no_exp, current);
    if ((res = is_one_arg(func)) != fno) {
            OneArg();
            AddRPNElem(ConvertToRPNFunc(res)); 
    } else
        if (( res = is_two_arg(func)) != fno) {
            TwoArg();
            AddRPNElem(ConvertToRPNFunc(res));
        } else
            throw SyntaxError(no_such_func, current);
}

void Syntaxer::Variable()
{
    RPNVarAddr addr(current->lexem);
    AddRPNElem(addr.Clone());
    if (!GetNextLexem())
        throw SyntaxError(no_assigment, current);
    if (current->type != assigment)
        throw SyntaxError(wrong_assigment, current);
    if(!GetNextLexem())
        throw SyntaxError(no_exp, current);
    Exp1();
    RPNFunAssig *assig = new RPNFunAssig;
    AddRPNElem(assig);
}

void Syntaxer::While()
{
    if (!GetNextLexem())
        throw SyntaxError(no_left_bracket, current);
    if (this_char(current, 0) != '(')
        throw SyntaxError(left_bracket, current);
    RPNItem *before_exp = last;
    try {
        Exp1();
    }
    catch (SyntaxError err) {
        if (err.GetType() == nothing_after_exp)
            throw SyntaxError(no_right_body_bracket, current);
        else
            throw err;
    }
    RPNLabel *label1 = new RPNLabel;
    AddRPNElem(label1);
    AddRPNElem(new RPNOpGoFalse);
    if (this_char(current, 0) != '{')
        throw SyntaxError(left_body_bracket, current);
    if (!GetNextLexem())
        throw SyntaxError(right_body_bracket, current);
    Body();
    AddRPNElem(new RPNLabel(before_exp->next));
    AddRPNElem(new RPNOpGo);
    AddRPNElem(new RPNNoop);
    label1->Set(last);
}


void Syntaxer::If()
{
    if (!GetNextLexem())
        throw SyntaxError(no_left_bracket, current);
    if (this_char(current, 0) != '(')
        throw SyntaxError(left_bracket, current);
    try {
        Exp1();
    }
    catch (SyntaxError err) {
        if (err.GetType() == nothing_after_exp)
            throw SyntaxError(no_right_body_bracket, current);
        else
            throw err;
    }
    RPNLabel *label1 = new RPNLabel;
    AddRPNElem(label1);
    AddRPNElem(new RPNOpGoFalse);
    if (this_char(current, 0) != '{')
        throw SyntaxError(left_body_bracket, current);
    if (!GetNextLexem())
        throw SyntaxError(right_body_bracket, current);
    Body();
    AddRPNElem(new RPNNoop);
    label1->Set(last);
}

void Syntaxer::Body()
{
    if (this_char(current, 0) == '}')
        return;
    Command();
    if (!GetNextLexem())
        throw SyntaxError(right_body_bracket, current);
    if (this_char(current, 0) != '}')
        Body();
}

bool is_print(lex_list *p)
{
    return (!strcmp("print", p->lexem));
}

bool is_while_or_if(lex_list *p)
{
    return ((!strcmp("while", p->lexem)) ||
            (!strcmp("if", p->lexem)));
}

void Syntaxer::Keyword()
{
    if (is_while_or_if(current)) {
        if (this_char(current, 0) == 'w')
            While();
        else
            If();
    }
    else
        if (is_print(current))
            Print();
    else
        throw SyntaxError(wrong_keyword, current);
}

void Syntaxer::ArgPr()
{
    if (current->type == string) {
        AddRPNElem(new RPNString(current->lexem));
        if (!GetNextLexem())
            throw SyntaxError(no_end_print, current);
        return;
    } else
        Exp1();
}

void Syntaxer::Print()
{
    if (!GetNextLexem())
        throw SyntaxError(no_arg_print, current);
    try {
        ArgPr();
    }
    catch (SyntaxError err) {
        if (err.GetType() == nothing_after_exp)
            throw SyntaxError(no_arg_print, current);
        else
            throw err;
    }
    AddRPNElem(new RPNPrint);
    if (this_char(current, 0) == ',')
        Print();
    else
        if (this_char(current, 0) == ';')
            return;
    else
        throw SyntaxError(wrong_arg_print, current);
}

RPNElem* Syntaxer::ConvertToRPNBinary(int c)
{
    switch (c) {
        case '+':
            return new RPNFunPlus();
        case '-':
            return new RPNFunMinus();
        case '*':
            return new RPNFunMult();
        case '%':
            return new RPNFunMod();
        case '/':
            return new RPNFunDiv();
        case '<':
            return new RPNFunLess();
        case '>':
            return new RPNFunGreater();
        case '=':
            return new RPNFunEqual();
        case '&':
            return new RPNFunAnd();
        case '|':
            return new RPNFunOr();
        default:
            return 0;
    }
}

RPNElem* Syntaxer::ConvertToRPNUnary(int c)
{
    if (c == '!')
        return new RPNFunNot();
    else
        return new RPNFunUnary();
}

bool priority_1(int c)
{
    return (c == '|');
}

bool priority_2(int c)
{
    return (c == '&');
}

bool priority_3(int c)
{
    return (c == '<') || (c == '>') || (c == '=');
}

bool priority_4(int c)
{
    return (c == '+') || (c == '-');
}

bool priority_5(int c)
{
    return (c == '*') || (c == '/') || (c == '%');
}

bool priority_6(int c)
{
    return (c == '!') || (c == '-');
}

void Syntaxer::Exp1()
{
    Exp2();
    if (priority_1(current->lexem[0])) {
        RPNElem *symb = ConvertToRPNBinary(current->lexem[0]);
        if (!GetNextLexem())
            throw SyntaxError(no_exp, current);
        Exp1();
        AddRPNElem(symb);
    }
}

void Syntaxer::Exp2()
{
    Exp3();
    if (priority_2(current->lexem[0])) {
        RPNElem *symb = ConvertToRPNBinary(current->lexem[0]);
        if (!GetNextLexem())
            throw SyntaxError(no_exp, current);
        Exp2();
        AddRPNElem(symb);
    }
}

void Syntaxer::Exp3()
{
    Exp4();
    if (priority_3(current->lexem[0])) {
        RPNElem *symb = ConvertToRPNBinary(current->lexem[0]);
        if (!GetNextLexem())
            throw SyntaxError(no_exp, current);
        Exp3();
        AddRPNElem(symb);
    }
}

void Syntaxer::Exp4()
{
    Exp5();
    if (priority_4(current->lexem[0])) {
        RPNElem *symb = ConvertToRPNBinary(current->lexem[0]);
        if (!GetNextLexem())
            throw SyntaxError(no_exp, current);
        Exp4();
        AddRPNElem(symb);
    }
}

void Syntaxer::Exp5()
{
    Exp6();
    if (priority_5(current->lexem[0])) {
        RPNElem *symb = ConvertToRPNBinary(current->lexem[0]);
        if (!GetNextLexem())
            throw SyntaxError(no_exp, current);
        Exp5();
        AddRPNElem(symb);
    }
}

void Syntaxer::Exp6()
{
    if (priority_6(current->lexem[0])) {
        RPNElem *symb = ConvertToRPNUnary(current->lexem[0]);
        if (!GetNextLexem())
            throw SyntaxError(no_exp, current);
        Exp7();
        AddRPNElem(symb);
    } else
        Exp7();
}

void Syntaxer::Exp7()
{
    if (current->type == constant) {
        RPNInt i(is_number(current->lexem));
        AddRPNElem(i.Clone());
        if (!GetNextLexem())
            throw SyntaxError(nothing_after_exp, current);
        return;
    }
    if (this_char(current, 0) == '$') {
        AddRPNElem(new RPNString(current->lexem));
        AddRPNElem(new RPNFunVar());
        if (!GetNextLexem())
            throw SyntaxError(nothing_after_exp, current);
        return;
    }
    if (this_char(current, 0) == '?') {
        Function();
        //if (!GetNextLexem())
          //  throw SyntaxError(nothing_after_exp, current);
        return;
    }
    if (current->lexem[0] == '(') {
        if (!GetNextLexem())
            throw SyntaxError(no_exp, current);
        Exp1();
        if (this_char(current, 0) != ')')
            throw SyntaxError(right_bracket, current);
        if (!GetNextLexem())
            throw SyntaxError(nothing_after_exp, current);
        return;
    }
    throw SyntaxError(wrong_exp, current);
}

errors SyntaxError::GetType()
{
    return type;
}

void SyntaxError::Report()
{
    printf("Error!\nLine: %d. ", line);
    switch (type) {
        case no_such_func:
            printf("There is no such function.\n");
            break;
        case req_zero_arg:
            printf("This function requires zero parametrs.\n");
            break;
        case req_one_arg:
            printf("This function requires one parametr.\n");
            break;
        case req_two_arg:
            printf("This function requires two parametres.\n");
            break;
        case no_left_bracket:
            printf("Missed '(' before condition.\n");
            break;
        case no_right_bracket:
            printf("Missed ')' after condition.\n");
            break;
        case no_left_body_bracket:
            printf("Missed '{' before body of statement.\n");
            break;
        case no_right_body_bracket:
            printf("Missed '}' after body of statement.\n");
            break;
        case left_bracket:
            printf("Expected '(' before condition.\n");
            break;
        case right_bracket:
            printf("Expected ')' after condition.\n");
            break;
        case left_body_bracket:
            printf("Expected '{' before body of statement.\n");
            break;
        case right_body_bracket:
            printf("Expected '}' after body of statement.\n");
            break;
        case wrong_exp:
            printf("Wrong expression.\nNote: you can use "
                   "functions, constants, variables.\n");
            break;
        case no_exp:
            printf("Expected expression.\n");
            break;
        case no_comma:
            printf("Missed ','.\n");
            break;
        case no_comma_or_bracket:
            printf("Missed ')' or "
                   "',' (if you wanted one more paramete.\n)");
            break;
        case wrong_comma_bracket:
            printf("Expected ')' or"
                   "',' (if you wanted one more parametr.\n)" );
            break;
        case wrong_comma:
            printf("Expected ','.\n");
            break;
        case no_assigment:
            printf("Missed assigment.\n");
            break;
        case wrong_assigment:
            printf("Expected assigmnet.\n");
            break;
        case wrong_keyword:
            printf("There is no such keyword.\n");
            break;
        case no_arg_print:
            printf("Missed an argument in print.\n");
            break;
        case wrong_arg_print:
            printf("Wrong argument in print.\n"
                   "Note: you can use strings and expressions.\n");
            break;
        case no_end_print:
            printf("Missed ';' at the end of print.\n");
            break;
        case no_state_op:
            printf("Expected an operator or a statement.\n");
            break;
        case no_end:
            printf("Missed ';'.\n");
            break;
        case wrong_end:
            printf("Expected ';'.\n");
            break;
        default:
            break;
    }
}
