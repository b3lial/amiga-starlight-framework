#ifndef __UTILS_H__
#define __UTILS_H__

void waitForMouseClick(void);

#define FIR1 (1<<7)//0b10000000
#define FIR0 (1<<6)//0b01000000

int writeLogInt(const char* formatString, int n);
int initLog(void);
int writeLog(char*);

#endif
