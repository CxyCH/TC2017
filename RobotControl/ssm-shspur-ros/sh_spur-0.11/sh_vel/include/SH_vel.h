#ifndef __SH_VEL__
#define __SH_VEL__



#define LOCK 1
#define UNLOCK 0

void init(void);
void initCMT(void);
int command_analyze(char *data, int len);

#endif
