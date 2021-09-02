#include <string.h>

#include "vartable.hpp"

Vartable::~Vartable()
{
    var_item *tmp;

    while (table) {
        tmp = table;
        table = table->next;
        delete tmp;
    }
}

void Vartable::AddVariable(const char *name, int value)
{
    var_item *tmp = new var_item;
    
    tmp->name = name;
    tmp->value = value;
    tmp->next = table;
    table = tmp;
}

void Vartable::SetValue(const char *name, int value)
{
    var_item *tmp = table;
    
    while (tmp) {
        if (!(strcmp(tmp->name, name))) {
            tmp->value = value;
            return;
        }
        tmp = tmp->next;
    }
    AddVariable(name, value);
}

int Vartable::GetValue(const char *name)
{
    var_item *tmp = table;

    while (tmp) {
        if (!strcmp(tmp->name, name))
            return tmp->value;
        tmp = tmp->next;
    }
    return 0;
}
