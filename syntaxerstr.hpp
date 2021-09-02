#ifndef H_SYNTAXERSTR
#define H_SYNTAXERSTR

#include "lexerstr.hpp"
#include "rpn.hpp"

enum ftype {
    fno,
    fbuy,
    fsell,
    fprod,
    fbuild,
    fmoney,
    fraw,
    fproduction,
    ffact,
    fautofact,
    fmanuc,
    fresrawamount,
    fresrawprice,
    fresprodamount,
    fresprodprice,
    fendturn,
    fmyid,
    fturn,
    fplayers,
    factplayers,
    fsupply,
    frawprice,
    fdemand,
    fprodprice,
    fmax,
    fmin,
    fnbp,
    fpbn,
    frand,
    ftrip
};

enum errors {
    no_left_bracket,
    no_right_bracket,
    no_left_body_bracket,
    no_right_body_bracket,
    left_bracket,
    right_bracket,
    left_body_bracket,
    right_body_bracket,
    no_end,
    wrong_end,
    wrong_exp,
    no_exp,
    no_comma,
    no_comma_or_bracket,
    wrong_comma_bracket,
    wrong_comma,
    no_state_op,
    no_assigment,
    wrong_assigment,
    wrong_keyword,
    no_arg_print,
    nothing_after_exp,
    wrong_arg_print,
    req_one_arg,
    req_two_arg,
    req_zero_arg,
    no_end_print,
    no_such_func
};

class Syntaxer {
    lex_list *current;
    RPNItem *first;
    RPNItem *last;

    void Start();
    void Command();
    void Name();
    void Function();
    void Keyword();
    void Variable();
    void Print();
    void ZeroArg();
    void OneArg();
    void TwoArg();
    void ManyArg();
    void Body();
    void ArgPr();
    void While();
    void If();
    void Exp1();
    void Exp2();
    void Exp3();
    void Exp4();
    void Exp5();
    void Exp6();
    void Exp7();
    void AddRPNElem(RPNElem *elem);
    char *Exchange(char *s, int lvl);
    static RPNElem *ConvertToRPNFunc(ftype ft);
    static RPNElem *ConvertToRPNBinary(int c);
    static RPNElem *ConvertToRPNUnary(int c);
public:
    void Run();
    Syntaxer(lex_list *p) : current(p), first(0), last(0) {}
    bool GetNextLexem();
    inline RPNItem* GetRPN() {return first;}
};

class SyntaxError {
    errors type;
    int line;
public:
    SyntaxError(errors t, lex_list *p)
        : type(t), line(p->line_num) {}
    void Report();
    errors GetType();
};

ftype is_two_arg(lex_list *p);
ftype is_one_arg(lex_list *p);
ftype is_zero_arg(lex_list *p);
bool is_print(lex_list *p);
bool is_while_or_if(lex_list *p);
int this_char(lex_list *p, int index);
bool priority_1(int c);
bool priority_2(int c);
bool priority_3(int c);
bool priority_4(int c);
bool priority_5(int c);
bool priority_6(int c);
#endif
