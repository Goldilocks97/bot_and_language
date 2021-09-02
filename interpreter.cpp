#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "psl.hpp"
#include "lexerstr.hpp"
#include "syntaxerstr.hpp"
#include "rpn.hpp"
#include "intpretstr.hpp"

int lexer_work(
               int fd,
               const char *serv_ip,
               int port,
               const char *mode,
               int lob_num,
               const char *nick)
{
    Lexer lxr;
    lex_list *lexfirst;
    RPNItem *RPNfirst = 0;
    Bot mark(nick);
    int c;

    do {
        c = getchar();
        lxr.Run(c);
    } while (c != -1);
    lexfirst = lxr.GetList();
    add_null_lexem(&lexfirst);
    print_lexems(lexfirst);
    if (lxr.GetMachineState() == error)
        return 6;
    try {
        Syntaxer sntx(lexfirst);
        sntx.Run();
        RPNfirst = sntx.GetRPN();
        mark.ConnectGame(serv_ip, port, mode, lob_num);
        Interpreter intpr(&mark, RPNfirst);
        intpr.Interpretation();
        delete_RPNlist(RPNfirst);
        delete_lexlist(lexfirst);
    }
    catch (SyntaxError syn_err) {
        syn_err.Report();
    }
    catch (RPNError rpn_elem) {
        rpn_elem.Report();
    }
    close (fd);
    return 0;
}

int main(int argc, char **argv)
{
    int fd, port, num;

    if (argc != 7) {
        fprintf(stderr, "Not enough arguments.\n"
               "Note: %s <script> <server_ip> <port> "
               "<mode> <lobby/num_players> "
               "<nick>.\n", argv[0]);
        return 1;
    }
    if ((port = is_number(argv[3])) == -1) {
        fprintf(stderr, "Incorrect port.\n");
        return 2;
    }
    if ((fd = use_file(argv[1])) == -1) {
        fprintf(stderr, "Cannot use this file\n");
        return 3;
    }
    if ((num = is_number(argv[5])) == -1) {
        fprintf(stderr, "Invalid format of lobby/num_players.\n");
        return 4;
    }
    if (strcmp("create", argv[4]) && strcmp("join", argv[4])) {
        fprintf(stderr, "Invalid format of mode.\n");
        return 5;
    }
    if (!correct_nick(argv[6])) {
        fprintf(stderr, "Invalid format of nick.\n");
        return 6;
    }
    return lexer_work(fd, argv[2], port, argv[4], num, argv[6]);
}
