#ifndef H_BOTSTR
#define H_BOTSTR

#define INBUFSIZE 1024

bool correct_nick(const char *str);

struct nick_num {
    const char *nick;
    int num;
    nick_num *next;
};

enum state_tag {
    nothing,
    info,
    market_s,
    act,
    quit
};

struct bot_plan {
    int buy_amount;
    int buy_price;
    int sell_amount;
    int sell_price;
    int prod;
};

struct player_tag {
	char *nick;
	int money;
	int raw;
	int products;
	int plants;
    int autoplants;
    player_tag *next;
};

struct bought_tag {
    char *nick;
    int bought_amount;
    int bought_price;
    bought_tag *next;
};

struct sold_tag {
    char *nick;
    int sold_amount;
    int sold_price;
	sold_tag *next;
};

struct market_tag {
    int turn;
    int raw;
    int min_price;
    int prod;
    int max_price;
};

    class Bot {
    char buf[INBUFSIZE];
    int bufused;
    enum state_tag state;
    struct market_tag market;
    const char *nick;
    int sock;
    nick_num *table;
    bool data;
    int cnt_pl;
    int num_pl;
    bot_plan plan;
    bought_tag *bought;
    sold_tag *sold;
    player_tag *players;
    
    void CheckBankrupt(const char *str);
    void GetRoom(const char *mode, int lobby);
    void AnalyseMsg(const char *str);
    void GetPlayer(const char *str);
    char *GetString();
    void AnalyseString(const char *str);
    void ProcessWait(const char *str);
    void Write();
    int GetOneNumber(const char *str, int &ref_month);
    player_tag *GetElem(const char *str);
    int GetNickname(const char *str, char **name);
    player_tag *SetId();
    void PrintOnePlayerInfo(player_tag *p);
    void CreateListPlayers();
    void MakePlayerElem();
    void GetMarket(const char *str);
    void GetBoughtValues(const char *str, bought_tag *p);
    void GetResultBought(const char *str);
    void SetValuesAuct();
    void GetSoldValues(const char *str, sold_tag *p);
    void GetResultSold(const char *str);
    void MakeBoughtElem();
    void MakeSoldElem();
    void DeleteSoldList();
    void DeleteBoughtList();
    void DeleteTable();
    const char *GetNickByNum(int n);
    void AssigNickNum(const char *nick);
    void DeletePlayersList();
public:
    Bot(const char *nickname);
    ~Bot();
    void SetBuyAmount(int a) {plan.buy_amount = a;}
    void SetBuyPrice(int p) {plan.buy_price = p;}
    void SetSellAmount(int a) {plan.sell_amount = a;}
    void SetSellPrice(int p) {plan.sell_price = p;}
    void SetProd(int n) {plan.prod = n;}
    int GetResultRawAmount(int n);
    int GetResultRawPrice(int n);
    int GetResultProdAmount(int n);
    int GetResultProdPrice(int n);
    int Read();
    int GetMyId();
    bool DataIsReady() {return data;}
    int GetTurn() {return market.turn;}
    int GetPlayers() {return num_pl;}
    int GetSupply() {return market.raw;}
    int GetRawPrice() {return market.min_price;}
    int GetDemand() {return market.prod;}
    int GetProductionPrice() {return market.max_price;}
    void SetData(bool d) {data = d;}
    int GetPlayerMoney(int n); 
    int GetPlayerRaw(int n);
    int GetActivePlayers();
    int GetPlayerProduction(int n);
    int GetPlayerFactories(int n);
    int GetPlayerAutoplants(int n);
    void Analyser();
    void ConnectGame(
                     const char *serv_ip,
                     int prt,
                     const char *mode,
                     int lobby);
    void WriteCommand(const char *cmd);
    void PrintAllPlayersInfo();
    void PrintBoughtResult();
    void PrintSoldResult();
};
#endif
