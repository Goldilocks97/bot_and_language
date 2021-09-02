#ifndef H_RPN
#define H_RPN

#include <stdio.h>

#include "vartable.hpp"
#include "botstr.hpp"

class RPNError {
    const char *fun;
public:
    RPNError(const char *f) : fun(f) {}
    ~RPNError() {}
    void Report() {printf("Malfunction at %s.\n", fun);}
};

class RPNElem;

struct RPNItem {
    RPNElem *elem;
    RPNItem *next;
};

class RPNElem {
public:
    virtual void Evaluate(RPNItem **stack,
                          RPNItem **cur,
                          Vartable *tbl,
                          Bot *bot) const = 0;
    virtual ~RPNElem() {}
protected:
    static void Push(RPNItem **stack, RPNElem *elem);
    static RPNElem* Pop(RPNItem **stack);
};

class RPNNoop: public RPNElem {
public:
    ~RPNNoop() {}
    RPNNoop() {}
    void Evaluate(RPNItem **stack,
                  RPNItem **cur,
                  Vartable *tbl,
                  Bot *bot) const;
};

class RPNConstant: public RPNElem {
public:
    void Evaluate(RPNItem **stack,
                  RPNItem **cur,
                  Vartable *tbl,
                  Bot *bot) const;
protected:
    virtual RPNElem* Clone() const = 0;
};

class RPNFunction: public RPNElem {
public:
    void Evaluate(RPNItem **stack,
                  RPNItem **cur,
                  Vartable *tbl,
                  Bot *bot) const;
protected:
    virtual RPNElem* EvaluateFun(RPNItem **stack,
                                 Vartable *tbl,
                                 Bot *bot) const = 0;
};

class RPNInt: public RPNConstant {
    int value;
public:
    RPNInt(int v) : value(v) {}
    virtual ~RPNInt() {}
    RPNElem* Clone() const {return new RPNInt(value);}
    int Get() const {return value;}
};

class RPNString: public RPNConstant {
    char *value; //const?
public:
    RPNString(char *v) : value(v) {}
    virtual ~RPNString() {}
    RPNElem* Clone() const {return new RPNString(value);}
    char* Get() const {return value;}
};

class RPNLabel : public RPNConstant {
    RPNItem *value;
public:
    RPNLabel(RPNItem *v = 0) : value(v) {}
    virtual ~RPNLabel() {}
    void Set(RPNItem *l) {value = l;}
    RPNElem* Clone() const {return new RPNLabel(value);}
    RPNItem* Get() const {return value;}
};

class RPNVarAddr : public RPNConstant {
    char *value;
public:
    RPNVarAddr(char *v) : value(v) {}
    virtual ~RPNVarAddr() {}
    RPNElem* Clone() const {return new RPNVarAddr(value);}
    char* Get() const {return value;}
};

class RPNOpGo : public RPNElem {
public:
    virtual ~RPNOpGo() {}
    RPNOpGo() {}
    void Evaluate(RPNItem **stack, RPNItem **cur,
                  Vartable *tbl,
                  Bot *bot) const;
};

class RPNOpGoFalse : public RPNElem {
public:
    virtual ~RPNOpGoFalse() {}
    RPNOpGoFalse() {}
    void Evaluate(RPNItem **stack,
                  RPNItem **cur,
                  Vartable *tbl,
                  Bot *bot) const;    
};

class RPNFunPlus : public RPNFunction {
public:
    virtual ~RPNFunPlus() {}
    RPNFunPlus() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunMinus : public RPNFunction {
public:
    virtual ~RPNFunMinus() {}
    RPNFunMinus() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunMult : public RPNFunction {
public:
    virtual ~RPNFunMult() {}
    RPNFunMult() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunNumPerc: public RPNFunction {
public:
    virtual ~RPNFunNumPerc() {}
    RPNFunNumPerc() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunDiv : public RPNFunction {
public:
    virtual ~RPNFunDiv() {}
    RPNFunDiv() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunMod : public RPNFunction {
public:
    virtual ~RPNFunMod() {}
    RPNFunMod() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunLess : public RPNFunction {
public:
    virtual ~RPNFunLess() {}
    RPNFunLess() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunTriple : public RPNFunction {
public:
    virtual ~RPNFunTriple() {}
    RPNFunTriple() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunGreater : public RPNFunction {
public:
    virtual ~RPNFunGreater() {}
    RPNFunGreater() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunMax : public RPNFunction {
public:
    virtual ~RPNFunMax() {}
    RPNFunMax() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunMin : public RPNFunction {
public:
    virtual ~RPNFunMin() {}
    RPNFunMin() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunEqual : public RPNFunction {
public:
    virtual ~RPNFunEqual() {}
    RPNFunEqual() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunAnd : public RPNFunction {
public:
    virtual ~RPNFunAnd() {}
    RPNFunAnd() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunOr : public RPNFunction {
public:
    virtual ~RPNFunOr() {}
    RPNFunOr() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunNot : public RPNFunction {
public:
    virtual ~RPNFunNot() {}
    RPNFunNot() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunUnary : public RPNFunction {
public:
    virtual ~RPNFunUnary() {}
    RPNFunUnary() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunRandom : public RPNFunction {
public:
    virtual ~RPNFunRandom() {}
    RPNFunRandom() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunVar : public RPNFunction {
public:
    virtual ~RPNFunVar() {}
    RPNFunVar() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunAssig : public RPNFunction {
public:
    virtual ~RPNFunAssig() {}
    RPNFunAssig() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunMyId : public RPNFunction {
public:
    virtual ~RPNFunMyId() {}
    RPNFunMyId() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunTurn : public RPNFunction {
public:
    virtual ~RPNFunTurn() {}
    RPNFunTurn() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunPlayers : public RPNFunction {
public:
    virtual ~RPNFunPlayers() {}
    RPNFunPlayers() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunActivePlayers : public RPNFunction {
public:
    virtual ~RPNFunActivePlayers() {}
    RPNFunActivePlayers() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunSupply : public RPNFunction {
public:
    virtual ~RPNFunSupply() {}
    RPNFunSupply() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunRawPrice : public RPNFunction {
public:
    virtual ~RPNFunRawPrice() {}
    RPNFunRawPrice() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunDemand : public RPNFunction {
public:
    virtual ~RPNFunDemand() {}
    RPNFunDemand() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunPerc : public RPNFunction {
public:
    virtual ~RPNFunPerc() {}
    RPNFunPerc() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunProductionPrice : public RPNFunction {
public:
    virtual ~RPNFunProductionPrice() {}
    RPNFunProductionPrice() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunMoney : public RPNFunction {
public:
    virtual ~RPNFunMoney() {}
    RPNFunMoney() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunRaw : public RPNFunction {
public:
    virtual ~RPNFunRaw() {}
    RPNFunRaw() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunProduction : public RPNFunction {
public:
    virtual ~RPNFunProduction() {}
    RPNFunProduction() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunFactories : public RPNFunction {
public:
    virtual ~RPNFunFactories() {}
    RPNFunFactories() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunAutoFactories : public RPNFunction {
public:
    virtual ~RPNFunAutoFactories() {}
    RPNFunAutoFactories() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunManufactured : public RPNFunction {
public:
    virtual ~RPNFunManufactured() {}
    RPNFunManufactured() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunResultRawAmount : public RPNFunction {
public:
    virtual ~RPNFunResultRawAmount() {}
    RPNFunResultRawAmount() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunResultRawPrice : public RPNFunction {
public:
    virtual ~RPNFunResultRawPrice() {}
    RPNFunResultRawPrice() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunResultProdAmount : public RPNFunction {
public:
    virtual ~RPNFunResultProdAmount() {}
    RPNFunResultProdAmount() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunResultProdPrice : public RPNFunction {
public:
    virtual ~RPNFunResultProdPrice() {}
    RPNFunResultProdPrice() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunBuy : public RPNFunction {
public:
    virtual ~RPNFunBuy() {}
    RPNFunBuy() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunSell : public RPNFunction {
public:
    virtual ~RPNFunSell() {}
    RPNFunSell() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunEndturn : public RPNFunction {
public:
    virtual ~RPNFunEndturn() {}
    RPNFunEndturn() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunBuild : public RPNFunction {
public:
    virtual ~RPNFunBuild() {}
    RPNFunBuild() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNFunProd : public RPNFunction {
public:
    virtual ~RPNFunProd() {}
    RPNFunProd() {}
    RPNElem* EvaluateFun(RPNItem **stack,
                         Vartable *tbl,
                         Bot *bot) const;
};

class RPNPrint : public RPNElem {
public:
    virtual ~RPNPrint() {}
    RPNPrint() {}
    void Evaluate(RPNItem **stack,
                      RPNItem **cur,
                      Vartable *tbl,
                      Bot *bot) const;
};

void delete_RPNlist(RPNItem *p);

#endif
