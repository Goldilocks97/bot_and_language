#ifndef H_VARTABLE
#define H_VARTABLE

class Vartable {
    struct var_item {
        const char *name;
        int value;
        var_item *next;
    };
    var_item *table;

    void AddVariable(const char *name, int value);
public:
    Vartable() : table(0) {}
    ~Vartable();
    int GetValue(const char *name);
    void SetValue(const char *name, int value);
};
#endif
