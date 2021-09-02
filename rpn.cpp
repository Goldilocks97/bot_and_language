#include <stdlib.h>
#include <stdio.h>

#include "rpn.hpp"
#include "vartable.hpp"
#include "botstr.hpp"

void delete_RPNlist(RPNItem *p)
{
    RPNItem *tmp;

    while (p) {
        tmp = p;
        p = p->next;
        delete tmp->elem;
        delete tmp;
    }
}

void RPNConstant::Evaluate(RPNItem **stack,
                           RPNItem **cur,
                           Vartable *tbl,
                           Bot *bot) const
{
    Push(stack, Clone());
    *cur = (*cur)->next;
}

void RPNFunction::Evaluate(RPNItem **stack,
                           RPNItem **cur,
                           Vartable *tbl,
                           Bot *bot) const
{
    RPNElem *res = EvaluateFun(stack, tbl, bot);
    if (res)
        Push(stack, res);
    *cur = (*cur)->next;
}

void RPNNoop::Evaluate(RPNItem **stack,
                       RPNItem **cur,
                       Vartable *tbl,
                       Bot *bot) const
{
    *cur = (*cur)->next;
}

void RPNOpGo::Evaluate(RPNItem **stack,
                       RPNItem **cur,
                       Vartable *tbl,
                       Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNLabel *lab = dynamic_cast<RPNLabel*>(operand1);
    if (!lab)
        throw RPNError("OpGo");
    RPNItem *addr = lab->Get();
    *cur = addr;
    delete operand1;
}

void RPNElem::Push(RPNItem **stack, RPNElem *elem)
{
    RPNItem *tmp = new RPNItem;
    tmp->elem = elem;
    tmp->next = *stack;
    *stack = tmp;
}

RPNElem* RPNElem::Pop(RPNItem **stack)
{
    RPNItem *tmp = *stack;
    RPNElem *p = tmp->elem;

    *stack = (*stack)->next;
    delete tmp;
    return p;   
}

void RPNOpGoFalse::Evaluate(RPNItem **stack,
                            RPNItem **cur,
                            Vartable *tbl,
                            Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNLabel *lab = dynamic_cast<RPNLabel*>(operand1);
    if (!lab)
        throw RPNError("OpGoFalse");
    RPNElem *operand2 = Pop(stack);
    RPNInt *val = dynamic_cast<RPNInt*>(operand2);
    if (!val)
        throw RPNError("OpGoFalse");
    if (val->Get() <= 0)
        *cur = lab->Get();
    else
        *cur = (*cur)->next;
    delete operand1;
    delete operand2;
}

RPNElem* RPNFunPlus::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("plus");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("plus");
    int res = i1->Get() + i2->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunMax::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("max");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("max");
    int res;
    if (i1->Get() > i2->Get())
        res = i1->Get();
    else
        res = i2->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunMin::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("min");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("min");
    int res;
    if (i1->Get() < i2->Get())
        res = i1->Get();
    else
        res = i2->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunMinus::EvaluateFun(RPNItem **stack,
                                  Vartable *tbl,
                                  Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("minus");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("minus");
    int res = i2->Get() - i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunMult::EvaluateFun(RPNItem **stack,
                                 Vartable *tbl,
                                 Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("mult");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("mult");
    int res = i1->Get() * i2->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunDiv::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("div");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("div");
    int res = i2->Get() / i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunMod::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("mod");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("mod");
    int res = i2->Get() % i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunLess::EvaluateFun(RPNItem **stack,
                                 Vartable *tbl,
                                 Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("less");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("less");
    int res = i2->Get() < i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunGreater::EvaluateFun(RPNItem **stack,
                                    Vartable *tbl,
                                    Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("greater");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("greater");
    int res = i2->Get() > i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunEqual::EvaluateFun(RPNItem **stack,
                                  Vartable *tbl,
                                  Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("equal");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("equal");
    int res = i2->Get() == i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunAnd::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("and");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("and");
    int res = i2->Get() && i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunNumPerc::EvaluateFun(RPNItem **stack,
                                    Vartable *tbl,
                                    Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("num_perc");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("num_perc");
    int res = (i2->Get() / 100) * i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunTriple::EvaluateFun(RPNItem **stack,
                                   Vartable *tbl,
                                   Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("triple");
    int res = i1->Get() * 3;
    return new RPNInt(res);
}

RPNElem* RPNFunPerc::EvaluateFun(RPNItem **stack,
                                 Vartable *tbl,
                                 Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("perc");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("perc");
    double num1 = i1->Get() * 1.0;
    double num2 = i2->Get() * 1.0;
    int res = (num1/num2) * 100.0;
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunOr::EvaluateFun(RPNItem **stack,
                               Vartable *tbl,
                               Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("or");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if(!i2)
        throw RPNError("or");
    int res = i2->Get() || i1->Get();
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunNot::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("not");
    int res = !(i->Get());
    delete operand;
    return new RPNInt(res);
}
RPNElem* RPNFunUnary::EvaluateFun(RPNItem **stack,
                                  Vartable *tbl,
                                  Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i= dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("unary");
    int res = (-1) * (i->Get());
    delete operand;
    return new RPNInt(res);
}

RPNElem* RPNFunVar::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNString *i = dynamic_cast<RPNString*>(operand);
    if (!i)
        throw RPNError("variable");
    int res = tbl->GetValue(i->Get());
    delete operand;
    return new RPNInt(res);
}

RPNElem* RPNFunAssig::EvaluateFun(RPNItem **stack,
                                  Vartable *tbl,
                                  Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("assigment");
    RPNElem *operand2 = Pop(stack);
    RPNVarAddr *i2 = dynamic_cast<RPNVarAddr*>(operand2);
    if (!i2)
        throw RPNError("assigment");
    tbl->SetValue(i2->Get(), i1->Get());
    delete operand1;
    delete operand2;
    return 0;
}

RPNElem* RPNFunRandom::EvaluateFun(RPNItem **stack,
                                   Vartable *tbl,
                                   Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("random");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if (!i2)
        throw RPNError("random");
    int num1 = i1->Get(), num2 = i2->Get();
    if (num2 >= num1)
        throw RPNError("random_parameters");
    int res = rand() % (num1 - num2) + num1; 
    delete operand1;
    delete operand2;
    return new RPNInt(res);
}

RPNElem* RPNFunMyId::EvaluateFun(RPNItem **stack,
                                 Vartable *tbl,
                                 Bot *bot) const
{
    return new RPNInt(bot->GetMyId());
}

RPNElem* RPNFunTurn::EvaluateFun(RPNItem **stack,
                                 Vartable *tbl,
                                 Bot *bot) const
{
    return new RPNInt(bot->GetTurn());
}

RPNElem* RPNFunPlayers::EvaluateFun(RPNItem **stack,
                                    Vartable *tbl,
                                    Bot *bot) const
{
    return new RPNInt(bot->GetPlayers());
}

RPNElem* RPNFunActivePlayers::EvaluateFun(RPNItem **stack,
                                          Vartable *tbl,
                                          Bot *bot) const
{
    return new RPNInt(bot->GetActivePlayers());
}

RPNElem* RPNFunSupply::EvaluateFun(RPNItem **stack,
                                   Vartable *tbl,
                                   Bot *bot) const
{
    return new RPNInt(bot->GetSupply());
}

RPNElem* RPNFunRawPrice::EvaluateFun(RPNItem **stack,
                                     Vartable *tbl,
                                     Bot *bot) const
{
    return new RPNInt(bot->GetRawPrice());
}

RPNElem* RPNFunDemand::EvaluateFun(RPNItem **stack,
                                   Vartable *tbl,
                                   Bot *bot) const
{
    return new RPNInt(bot->GetDemand());
}

RPNElem* RPNFunProductionPrice::EvaluateFun(RPNItem **stack,
                                            Vartable *tbl,
                                            Bot *bot) const
{
    return new RPNInt(bot->GetProductionPrice());
}

RPNElem* RPNFunMoney::EvaluateFun(RPNItem **stack,
                                  Vartable *tbl,
                                  Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("money");
    return new RPNInt(bot->GetPlayerMoney(i->Get()));
}

RPNElem* RPNFunRaw::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("raw");
    return new RPNInt(bot->GetPlayerRaw(i->Get()));
}

RPNElem* RPNFunProduction::EvaluateFun(RPNItem **stack,
                                       Vartable *tbl,
                                       Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("production");
    return new RPNInt(bot->GetPlayerProduction(i->Get()));
}

RPNElem* RPNFunFactories::EvaluateFun(RPNItem **stack,
                                      Vartable *tbl,
                                      Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("factories");
    return new RPNInt(bot->GetPlayerFactories(i->Get()));
}

RPNElem* RPNFunAutoFactories::EvaluateFun(RPNItem **stack,
                                          Vartable *tbl,
                                          Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("autofactories");
    return new RPNInt(bot->GetPlayerAutoplants(i->Get()));
}

RPNElem* RPNFunManufactured::EvaluateFun(RPNItem **stack,
                                         Vartable *tbl,
                                         Bot *bot) const
{
    printf("manufactured\n");
    return 0;
}

RPNElem* RPNFunResultRawAmount::EvaluateFun(RPNItem **stack,
                                          Vartable *tbl,
                                          Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("raw_amount");
    return new RPNInt(bot->GetResultRawAmount(i->Get()));
}

RPNElem* RPNFunResultRawPrice::EvaluateFun(RPNItem **stack,
                                           Vartable *tbl,
                                           Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("raw_price");
    return new RPNInt(bot->GetResultRawPrice(i->Get()));
}

RPNElem* RPNFunResultProdAmount::EvaluateFun(RPNItem **stack,
                                             Vartable *tbl,
                                             Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("prod_amount");
    return new RPNInt(bot->GetResultProdAmount(i->Get()));
}

RPNElem* RPNFunResultProdPrice::EvaluateFun(RPNItem **stack,
                                            Vartable *tbl,
                                            Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("prod_price");
    return new RPNInt(bot->GetResultProdPrice(i->Get()));
}

RPNElem* RPNFunBuy::EvaluateFun(RPNItem **stack,
                                Vartable *tbl,
                                Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("buy");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if (!i2)
        throw RPNError("buy");
    char msg[50];
    sprintf(msg, "buy %d %d\n", i2->Get(), i1->Get());
    bot->WriteCommand(msg);
    return 0;
}

RPNElem* RPNFunSell::EvaluateFun(RPNItem **stack,
                                 Vartable *tbl,
                                 Bot *bot) const
{
    RPNElem *operand1 = Pop(stack);
    RPNInt *i1 = dynamic_cast<RPNInt*>(operand1);
    if (!i1)
        throw RPNError("sell");
    RPNElem *operand2 = Pop(stack);
    RPNInt *i2 = dynamic_cast<RPNInt*>(operand2);
    if (!i2)
        throw RPNError("sell");
    char msg[50];
    sprintf(msg, "sell %d %d\n", i2->Get(), i1->Get());
    bot->WriteCommand(msg);
    return 0;
}

RPNElem* RPNFunEndturn::EvaluateFun(RPNItem **stack,
                                    Vartable *tbl,
                                    Bot *bot) const
{
    bot->WriteCommand("turn\n");
    return 0;
}

RPNElem* RPNFunProd::EvaluateFun(RPNItem **stack,
                                 Vartable *tbl,
                                 Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("prod");
    char msg[40];
    sprintf(msg, "prod %d\n", i->Get());
    bot->WriteCommand(msg);
    return 0;
}

RPNElem* RPNFunBuild::EvaluateFun(RPNItem **stack,
                                  Vartable *tbl,
                                  Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    if (!i)
        throw RPNError("build");
    char msg[40];
    sprintf(msg, "build %d\n", i->Get());
    bot->WriteCommand(msg);
    return 0;
}

void RPNPrint::Evaluate(RPNItem **stack,
                            RPNItem **cur,
                            Vartable *tbl,
                            Bot *bot) const
{
    RPNElem *operand = Pop(stack);
    RPNInt *i = dynamic_cast<RPNInt*>(operand);
    RPNString *s;

    if (!i) {
        s = dynamic_cast<RPNString*>(operand);
        if (!s)
            throw RPNError("print");
        printf("%s\n", s->Get());
        *cur = (*cur)->next;
        return;
    }
    printf("%d\n", i->Get());
    *cur = (*cur)->next;
}
