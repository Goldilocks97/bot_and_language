#include "botstr.hpp"
#include "intpretstr.hpp"
#include "rpn.hpp"

void Interpreter::Interpretation()
{
    int turn = 0, tmpturn;
    RPNItem *cur = first;

    for (;;) {
        if (bot->Read() < 1)
            return;
        bot->Analyser();
        tmpturn = bot->GetTurn();
        if (turn < tmpturn) {
            turn = tmpturn;
            bot->WriteCommand("info\nmarket\n");
        }
        if (bot->DataIsReady()) {
            while (cur)
                cur->elem->Evaluate(&stack, &cur, &tbl, bot);
            cur = first;
            bot->SetData(false);
        }
    }
}
