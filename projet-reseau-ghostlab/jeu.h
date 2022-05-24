#ifndef JEU
#define JEU

void initialisation_5(int *t);
int setRandomPosition(int *t, int *x, int *y, int h, int w);
int setRandomPositionToGhost(int *t, int h, int w);
int goTop(int *t, int x, int y, int h, int w);
int goBot(int *t, int x, int y, int h, int w);
int goRight(int *t, int x, int y, int h, int w);
int goLeft(int *t, int x, int y, int h, int w);
int moveUnGhost(int *tab, int x, int y, int h, int w);
int moveGhost(int *tab, int h, int w);

#endif