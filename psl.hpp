#ifndef H_PSL
#define H_PSL
	int is_number(const char *str);
	int i_newline(const char *str);
	void str_copy(const char *source, char *dest, int n);
	void defbuf(char *arr, int len);
	int cnl(const char *str);
	int str_len(const char *str);
    int check_symb(const char c);
    int use_file(const char *file);
#endif
