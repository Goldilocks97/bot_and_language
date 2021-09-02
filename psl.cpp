#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "psl.hpp"

//make a number from a string. 
//Return a <number> or <-1> if the string contains something exept numbers.
int is_number(const char *str)
{
	int i, num = 0, flag = 0;
	
	if (str[0] == '-') {
		flag = 1;
		i = 1;
	} else
		i = 0;
	while (str[i] != '\0') {
		if ((str[i] < '0') || (str[i] > '9'))
			return -1;
		num = (num * 10) + (str[i] - '0');
		i++;
	}
	if (flag)
		return num * (-1);
	else
		return num;
}

//return index of "newline" symbol.
//if there is no such - return "-1".
int i_newline(const char *str)
{
	int i;

	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == '\n')
			return i;
	}
	return -1;
}

//copy N symbols from source to dest.
void str_copy(const char *source, char *dest, int n)
{
	int i;

	for (i = 0; i < n; i++)
		dest[i] = source[i];
}

//fill an arr with '\0'.
void defbuf(char *arr, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		arr[i] = '\0';
	}
}

//return count of new_lines in a string.
int cnl(const char *str)
{
	int i, n = 0;

	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == '\n')
			n++;
	}
	return n;
}

//return len of a string.
int str_len(const char *str)
{
	int len;
	
	for (len = 0; str[len] != 0; len++)
		;
	return len;
}

//function check wheather 'c' is symb or a numeral.
//if so - return 0, else - return -1. 
int check_symb(const char c)
{
    if (((c >= '0') && (c <= '9')) ||
        ((c >= 'A') && (c <= 'Z')) ||
        ((c >= 'a') && (c <= 'z')))
        return 0;
    return -1;
}

//open a file as the main in_string
//returns "-1" if wrong else returns fd
int use_file(const char *file)
{
    int fd;
    
    fd = open(file, O_RDONLY);
    dup2(fd, 0);
    close(fd);
    return fd;
}
