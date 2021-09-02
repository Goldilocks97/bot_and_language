#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "botstr.hpp"
#include "psl.hpp"
#include "rpn.hpp"

bool correct_nick(const char *nick)
{
    int i = 0;
    
    while (nick[i] != 0) {
        if ((nick[i] >= 'a' && nick[i] <= 'z') ||
            (nick[i] >= 'A' && nick[i] <= 'Z' ))
        {
            i++;
            continue;
        }
        return false;
    }
    return true;
}

Bot::Bot(const char *nickname)
{ 
	defbuf(buf, INBUFSIZE);
	state = nothing;
	bufused = 0;
    nick = nickname;
	players = 0;
    bought = 0;
    sold = 0;
    table = 0;
    cnt_pl = 0;
    data = false;
    market.turn = 0;
    plan.buy_price = 0;
    plan.buy_amount = 0;
    plan.sell_price = 0;
    plan.sell_amount = 0;
    plan.prod = 0;
}

Bot::~Bot()
{
    DeleteBoughtList();
    DeleteSoldList();
    DeletePlayersList();
    DeleteTable();
    close(sock);
}

void Bot::GetRoom(const char *mode, int lobby)
{
    
    if (strcmp("join", mode) == 0) {
        char cmd[30];
        sprintf(cmd, ".%s %d\n", mode, lobby);
        num_pl = -1;
        WriteCommand(cmd);
    } else {
        WriteCommand(".create\n");
        num_pl = lobby;
    }
}

void Bot::ConnectGame(
                      const char *serv_ip,
                      int prt,
                      const char *mode,
                      int lobby)
{   
	int skt;
    char cmd[80];
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(prt);

	if ((skt = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(3);
	}
	if (!inet_aton(serv_ip, &(addr.sin_addr))) {
        perror("inet_aton");
        exit(4);
    }
	if ((connect(skt, (struct sockaddr *)&addr, sizeof(addr))) !=     0) {
		perror("connect");
		exit(5);
	}
	sock = skt;
    sprintf(cmd, "%s\n", nick);
    WriteCommand(cmd);
    GetRoom(mode, lobby);
}

int Bot::Read()
{
	int res;
	
	res = read(sock, &buf, INBUFSIZE-1-bufused);
	if (res) {
		bufused += res;
	}
	return res;
}

char *Bot::GetString()
{
	char *str;
	int index;
	
	index = i_newline(buf);
	str = new char[index+2];
	str_copy(buf, str, index+1);
    str[index+1] = 0;
	str_copy(buf+index+1, buf, INBUFSIZE-index-1);
	bufused -= (index + 1);
	defbuf(buf+(INBUFSIZE-index-1), index+1);
	return str;
}

int Bot::GetOneNumber(const char *str, int &field)
{
	int i, flag = 0, num = 0;
	
	for (i = 0; str[i] != '\0'; i++) {
		if ((str[i] >= '0') && (str[i] <= '9')) {
			if (flag == 0)
				flag = 1;
		} else {
			if (flag) {
				field = num;
				break;
			}
		}
		if (flag) {
			num = num * 10 + (str[i] - '0');
		}
	}
    return i;
}

player_tag *Bot::GetElem(const char *name)
{
	player_tag *p = players;
    
	while (p) {
		if (strcmp(name, p->nick) == 0)
			return p;
		else
			p = p->next;
	}
	return 0;
}

void Bot::MakePlayerElem()
{
    if (players != 0) {
        player_tag *tmp;
        tmp = new player_tag;
        tmp->next = players;
        players = tmp;
    } else {
        players = new player_tag;
        players->next = 0;
    }
}

int Bot::GetNickname(const char *str, char **name)
{
    int i, flag = 0, j = 0;
    
    for (i = 0; str[i] != '\n'; i++) {
        if (str[i] != ' ') {
            if (!flag) {
                flag = !flag;
            }
        } else {
            if (flag)
                break;
            continue;
        }
        if (flag) {
            (*name)[j] = str[i];
            j++;
        }
    }
    (*name)[j] = 0;
    return i;
}

void Bot::AssigNickNum(const char *nick)
{
    char *cpynick = new char[20];

    cpynick = strcpy(cpynick, nick);
    if (table) {
        nick_num *tmp = new nick_num;
        tmp->nick = cpynick;
        tmp->num = table->num + 1;
        tmp->next = table;
        table = tmp;
    } else {
        table = new nick_num;
        table->nick = cpynick;
        table->num = 1;
        table->next = 0;
    }
}

void Bot::GetPlayer(const char *str)
{
    int res = 4;
    char *name;
    int &ref_raw = players->raw;
    int &ref_prod = players->products;
    int &ref_money = players->money;
    int &ref_plants = players->plants;
    int &ref_auto = players->autoplants;
    
    name = new char[20];
    res += GetNickname(str+res, &name);
    players->nick = name;
    res += GetOneNumber(str+res, ref_raw);
    res += GetOneNumber(str+res, ref_prod);
    res += GetOneNumber(str+res, ref_money);
    res += GetOneNumber(str+res, ref_plants);
    res += GetOneNumber(str+res, ref_auto);
    if (market.turn == 1)
        AssigNickNum(name);
}

void Bot::GetMarket(const char *str)
{
    int res = 6;
    int &ref_raw = market.raw;
    int &ref_min_price = market.min_price;
    int &ref_prod = market.prod;
    int &ref_max_price = market.max_price;

    res += GetOneNumber(str+res, ref_raw);
    res += GetOneNumber(str+res, ref_min_price);
    res += GetOneNumber(str+res, ref_prod);
    res += GetOneNumber(str+res, ref_max_price);
}

void Bot::GetBoughtValues(const char *str, bought_tag *p)
{
    int res = 0;
    int &ref_ba = p->bought_amount;
    int &ref_bp = p->bought_price;

    res += GetOneNumber(str+res, ref_ba);
    res += GetOneNumber(str+res, ref_bp);
}

void Bot::GetResultBought(const char *str)
{
    char *name;
    int res = 6;
    
    name = new char[20];
    res += GetNickname(str+res, &name);
    bought->nick = name;    
    GetBoughtValues(str+res, bought);
}

void Bot::GetSoldValues(const char *str, sold_tag *p)
{
    int res = 0;
    int &ref_sa = p->sold_amount;
    int &ref_sp = p->sold_price;

    res += GetOneNumber(str, ref_sa);
    res += GetOneNumber(str+res, ref_sp);
}

void Bot::GetResultSold(const char *str)
{
    char *name;
    int res = 6;
    
    name = new char[20];
    res += GetNickname(str+res, &name);
    sold->nick = name;
    GetSoldValues(str+res, sold);
}

void Bot::MakeBoughtElem()
{
    if (bought) {
        bought_tag *tmp = new bought_tag;
        tmp->next = bought;
        bought = tmp;
    } else {
        bought = new bought_tag;
        bought->next = 0;
    }
}

void Bot::MakeSoldElem()
{
    if (sold) {
        sold_tag *tmp = new sold_tag;
        tmp->next = sold;
        sold = tmp;
    } else {
        sold = new sold_tag;
        sold->next = 0;
    }
}

void Bot::DeleteSoldList()
{
    while (sold) {
        sold_tag *tmp = sold;
        sold = sold->next;
        delete[] tmp->nick;
        delete tmp;
    }
    sold = 0;
}

void Bot::DeleteBoughtList()
{
    while (bought) {
        bought_tag *tmp = bought;
        bought = bought->next;
        delete[] tmp->nick;
        delete tmp;
    }
    bought = 0;
}

void Bot::DeletePlayersList()
{
    while (players) {
        player_tag *tmp = players;
        players = players->next;
        delete[] tmp->nick;
        delete tmp;
    }
    players = 0;
}

void Bot::PrintBoughtResult()
{
    bought_tag *b = bought;
    
    while (b) {
        printf("\n-nick: %s\n-bought: %d price %d\n",
                b->nick, b->bought_amount, b->bought_price);
        b = b->next;
    }
}

void Bot::PrintSoldResult()
{
    sold_tag *s = sold;
    
    while (s) {
        printf("\n-nick: %s\n-sold: %d price %d\n",
                s->nick, s->sold_amount, s->sold_price);
        s = s->next;
    } 
}

void Bot::CheckBankrupt(const char *str)
{
    char *name = new char[20];
    int res = 0;
    res += GetNickname(str, &name);
    if ((strcmp(name, nick)) == 0) {
        state = quit;
        Write();
    }
    delete[] name;
}

void Bot::AnalyseMsg(const char *str){
	
	switch (str[0]) {
    	case 'S':
            if (str[1] == 'O') {
                MakeSoldElem();
                GetResultSold(str);
            }
            else {
             //   state = info;
                market.turn = 1;
               // Write();
            }
            break;
        case 'I':
            MakePlayerElem();
            GetPlayer(str);
            break;
        case 'P':
            //state = market_s;
           // Write();
            break;
        case 'M':
            GetMarket(str);
            //state = act;
            //Write();
            //DeleteBoughtList();
            //DeleteSoldList();
            data = true;
            break;
        case 'B':
            if (str[1] == 'A') {
                CheckBankrupt(str+9);
                break;
            }
            MakeBoughtElem();
            GetResultBought(str);
            break;
        case 'E':
            //PrintAllPlayersInfo();
            DeletePlayersList();
            //PrintBoughtResult();
            //PrintSoldResult();
            market.turn++;
            //state = info;
            //Write();
            break;
        case 'W':
        case 'N':
        case 'Y':
            state = quit;
            Write();
            break;
        default:
			break;
	}
}

void Bot::AnalyseString(const char *str)
{
	switch (str[0]) {
		case '&':
			AnalyseMsg(str + 2);
			break;
        case '@':
            if ((state == nothing) && (num_pl != -1)){
                if (str[1] == '+')
                    cnt_pl++;
                if (str[1] == '-')
                    cnt_pl--;
                if (cnt_pl == num_pl)
                    WriteCommand("start\n");
            }
        case '#':
            if (str[2] == 'T' && str[17] == ':') {
                DeleteSoldList();
                DeleteBoughtList();
            }
            break;
		default:
			break;
	}
}

void Bot::Analyser()
{
	char *str;
	int i;
	
	for (i = cnl(buf); i > 0; i--) {
		if (state != quit) {
            str = GetString();
		    printf("%s", str);
		    AnalyseString(str);
            delete[] str;
        }
	}
}

void Bot::WriteCommand(const char *cmd)
{
	write(sock, cmd, str_len(cmd));
}

void Bot::PrintOnePlayerInfo(player_tag *p)
{ 
    printf("\n-nick: %s\n-money: %d\n-raw: %d\n-products:"
           "%d\n-plants: %d\n-autoplants: %d\n",
            p->nick, p->money, p->raw, p->products,
            p->plants, p->autoplants);
}

void Bot::PrintAllPlayersInfo()
{
	player_tag *p = players;

	while (p) {
        PrintOnePlayerInfo(p);
        p = p->next;
	}
}

void Bot::Write()
{
    switch (state) {
        case info:
            WriteCommand("info\n");
            break;
        case market_s:
            WriteCommand("market\n");
            break;
        case act: {
            char cmd[80];
            player_tag *me = GetElem(nick);
            sprintf(cmd, "buy 2 %d\n", market.min_price);
            WriteCommand(cmd);
            sprintf(cmd, "sell %d %d\n", me->products, market.max_price);
            WriteCommand(cmd);
            WriteCommand("prod 2\n");
            WriteCommand("turn\n");
            break;
        }
        case quit:
            WriteCommand("quit\n");
            WriteCommand(".quit\n");
            break;
        default:
            break;
	}
}

const char* Bot::GetNickByNum(int n)
{
    nick_num *tmp = table;

    while (tmp) {
        if (tmp->num == n)
            return tmp->nick;
        tmp = tmp->next;
    }
    throw RPNError("finding_user");
}

int Bot::GetPlayerMoney(int n)
{
    player_tag *tmp = players;
    const char *name = GetNickByNum(n);

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->money;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetPlayerRaw(int n)
{
    player_tag *tmp = players;
    const char *name = GetNickByNum(n);

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->raw;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetPlayerProduction(int n)
{
    player_tag *tmp = players;
    const char *name = GetNickByNum(n);

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->products;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetPlayerFactories(int n)
{
    player_tag *tmp = players;
    const char *name = GetNickByNum(n);

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->plants;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetPlayerAutoplants(int n)
{
    player_tag *tmp = players;
    const char *name = GetNickByNum(n);

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->autoplants;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetMyId()
{
    nick_num *tmp = table;

    while (tmp) {
        if (!strcmp(nick, tmp->nick))
            return tmp->num;
        tmp = tmp->next;
    }
    return 0;
}

void Bot::DeleteTable()
{
    nick_num *p;

    while (table) {
        p = table;
        table = table->next;
        delete[] p->nick;
        delete p;
    }
}

int Bot::GetResultProdAmount(int n)
{
    const char *name = GetNickByNum(n);
    sold_tag *tmp = sold;///?????

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->sold_amount;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetResultProdPrice(int n)
{
    const char *name = GetNickByNum(n);
    sold_tag *tmp = sold;

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->sold_price;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetResultRawAmount(int n)
{
    const char *name = GetNickByNum(n);
    printf("NAME: %s\n", name);
    bought_tag *tmp = bought;

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->bought_amount;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetResultRawPrice(int n)
{
    const char *name = GetNickByNum(n);
    bought_tag *tmp = bought;

    while (tmp) {
        if (!strcmp(name, tmp->nick))
            return tmp->bought_price;
        tmp = tmp->next;
    }
    return 0;
}

int Bot::GetActivePlayers()
{
    player_tag *tmp = players;
    int i = 0;

    while (tmp) {
        i++;
        tmp = tmp->next;
    }
    return i;
}
