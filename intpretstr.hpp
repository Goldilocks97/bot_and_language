#ifndef H_INTERPRETER
#define H_INTERPRETER

#include "rpn.hpp"
#include "botstr.hpp"

class Interpreter {
    Bot *bot;
    RPNItem *first;
    RPNItem *stack;
    Vartable tbl;
public:
    Interpreter(Bot *b, RPNItem *f)
        : bot(b), first(f) {}
    ~Interpreter() {}
    void Interpretation();
};
#endif
