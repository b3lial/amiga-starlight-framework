#ifndef __UTILS_H__
#define __UTILS_H__

BOOL mouseClickDetected(void);
BOOL writeLogFS(const char* formatString, ...);
BOOL initLog(void);
BOOL writeLog(char*);

#endif
