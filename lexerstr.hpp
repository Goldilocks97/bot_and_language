#ifndef H_LEXERSTR
#define H_LEXERSTR

enum states {
    wait,
    name, 
    constant,
    keyword,
    string,
    assigment,
    null,
    work,
    error
};

struct lex_list {
    char *lexem;
    int line_num;
    int pos;
    int type;
    lex_list *next;
};

class Lexer {
    lex_list *first;
    lex_list *last;
    char *buf;
    int possition;
    int line;
    states state_machine;
    int lex_type;
    int current;
    int bufsize;
    states state;

    void DelimiterMet(int c, states new_type);
    void StateWait(int c);
    void StateName(int c);
    void StateConstant(int c);
    void StateKeyword(int c);
    void StateString(int c);
    void StateAssigment(int c);
    void SetState(int c);
    void ThrowError();
    void AddChar(int c);
    void AddLexem(char *str);
    void SendLexem();
    char *GetLexem();
    void StateMachine(int c);
public:
    Lexer();
    ~Lexer() { delete[] buf; }
    states GetMachineState();
    lex_list *GetList() const;
    void Run(int c);
};

bool delimiter_general(int c);
bool delimiter_wait(int c);
bool delimiter(int c);
bool char_name(int c);
bool char_constant(int c);
bool char_keyword(int c);
void report_error(const lex_list *p);
void print_type(int type);
void print_lexems(const lex_list *p);
void add_null_lexem(lex_list **first);
void delete_lexlist(lex_list *first);
#endif
