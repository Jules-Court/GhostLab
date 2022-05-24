#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "jeu.h"

    
int tab[15][15] = {{-1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                   {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1},
                   {-1, -1, 0, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1},
                   {-1, -1, 0, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1},
                   {-1, -1, 0, -1, -1, 0, -1, 0, 0, -1, -1, -1, 0, -1, -1},
                   {-1, -1, 0, -1, -1, 0, -1, -1, -1, -1, 0, 0, 0, -1, -1},
                   {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1},
                   {-1, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1},
                   {-1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1},
                   {-1, 0, -1, -1, -1, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1},
                   {-1, 0, -1, -1, -1, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1},
                   {-1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1},
                   {-1, 0, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1},
                   {-1, 0, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1},
                   {-1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

void initialisation_5(int *t){
    t[0]=-1;
    t[1]=0;
    t[2]=-1;
    t[3]=0;
    t[4]=-1;
    t[5]=0;
    t[6]=0;
    t[7]=0;
    t[8]=0;
    t[9]=0;
    t[10]=-1;
    t[11]=-1;
    t[12]=0;
    t[13]=-1;
    t[14]=0;
    t[15]=0;
    t[16]=0;
    t[17]=0;
    t[18]=0;
    t[19]=0;
    t[20]=-1;
    t[21]=0;
    t[22]=-1;
    t[23]=-1;
    t[24]=-1;
}

int tabTest[15][15] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

};

int printTab(int *t, int h, int w)
{
    int i, j;

    for (i = 0; i < h; i++)
    {
        printf("%d ", i);
        for (j = 0; j < w; j++)
        {
            if (t[i*h+j] == -1)
            {
                printf(".");
            }
            else if (t[i*h+j] == 0)
            {
                printf(" ");
            }
            else if (t[i*h+j] == -2)
            {
                printf("F");
            }
            else
            {
                printf("%d", t[i*h+j]);
            }
        }
        printf("\n");
    }
    return 0;
}

/* Mets un le joueur d'id 'id' a une position aléatoire */

int setRandomPosition(int *t, int *x, int *y, int h, int w)
{
    int random_i, random_j;
    srand(time(NULL));

    random_i = rand() % h;
    random_j = rand() % w;
    printf("Joueur position i :%d, j :%d\n", random_i, random_j);

    if (t[random_i*h+random_j] == 0)
    {
        *x=random_i;
        *y=random_j;
    }
    else
    {
        printf("Placement impossible");
        sleep(1);
        return setRandomPosition(t, x,y,h,w);
    }

    return 0;
}

/* Mets un fantome a une position aléatoire */
int setRandomPositionToGhost(int *t, int h, int w)
{
    int random_i, random_j;
    srand(time(NULL));

    random_i = rand() % h;
    random_j = rand() % w;
    printf("Fantome position : i :%d, j :%d\n", random_i, random_j);

    if (t[random_i*h+random_j] == 0)
    {
        t[random_i*h+random_j] = -2;
    }
    else
    {
        printf("Placement pas possible");
        return 1;
    }

    return 0;
}

/* Bouge le joueur pos de une case vers le haut */
int goTop(int *t, int x, int y, int h, int w)
{

    if(x-1<0){
        return 0;
    }
    else if (t[(x-1)*h+y] == 0)
    {
        return 1;
    }
    else if (t[(x-1)*h+y] == -2)
    {
        t[(x-1)*h+y]=0;
        return 2;
    }
    else
    {
        return 0;
    }

    return 0;
}

/* Bouge le joueur pos de une case vers le bas */
int goBot(int *t, int x, int y, int h, int w)
{
    if(x+1>=h){
        return 0;
    }
    else if (t[(x+1)*h+y] == 0)
    {
        return 1;
    }
    else if (t[(x+1)*h+y] == -2)
    {
        t[(x+1)*h+y]=0;
        return 2;
    }
    else
    {
        return 0;
    }

    return 0;
}

/* Bouge le joueur pos de une case vers la droite */
int goRight(int *t, int x, int y, int h, int w)
{
    if(y+1>=w){
        return 0;
    }
    else if (t[x*h+y+1] == 0)
    {
        return 1;
    }
    else if (t[x*h+y+1] == -2)
    {
        t[x*h+y+1]=0;
        return 2;
    }
    else
    {
        return 0;
    }

    return 0;
}

/* Bouge le joueur pos de une case vers la gauche */
int goLeft(int *t, int x, int y, int h, int w)
{
    if(y-1<0){
        return 0;
    }
    else if (t[x*h+y-1] == 0)
    {
        return 1;
    }
    else if (t[x*h+y-1] == -2)
    {
        t[x*h+y-1]=0;
        return 2;
    }
    else
    {
        return 0;
    }

    return 0;
}

// Avoir la position du joueur d'id Pos
int getPos(int tab[15][15], int pos)
{
    int cpt = 0;
    int cpt2 = 0;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (tab[i][j] == pos)
            {
                cpt = i;
                cpt2 = j;
            }
        }
    }
    printf("Position du joueur en i : %d, j : %d\n", cpt, cpt2);
    return 1;
}

// Avoir la position du fantome
int getFantomePos(int tab[15][15])
{
    int cpt = 0;
    int cpt2 = 0;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (tab[i][j] == -2)
            {
                cpt = i;
                cpt2 = j;
                printf("Position du fantome en i : %d, j : %d\n", cpt, cpt2);
            }
        }
    }
    return 1;
}
int moveUnGhost(int *tab, int x, int y, int h, int w){
    if(tab[x*h+y]!=0){
        return 0;
    }

    for(int i=0; i<h; i++){
        for(int j=0; j<w; j++){
            if(tab[i*h+j]==-2){
                tab[i*h+j]=0;
                tab[x*h+y]=-2;
            }
        }
    }
    return 1;
}
// Mouvement des fantomes
int moveGhost(int *tab, int h, int w)
{
    srand(time(NULL));
    int r = rand()%((4+1)-1) + 1;
    
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (tab[i*h+j] == -2)
            {
                if (r == 1 && i+1<h && tab[(i + 1)*h+j] == 0) // descend les fantomes 
                {
                    tab[(i + 1)*h+j] = tab[i*h+j];
                    tab[i*h+j] = 0;
                    i=i+2;
                }
                if (r == 2 && i-1>=0 && tab[(i - 1)*h+j] == 0) // monte les fantomes
                {
                    tab[(i - 1)*h+j] = tab[i*h+j];
                    tab[i*h+j] = 0;
                }
                if (r == 3 && j+1<w && tab[i*h+j + 1] == 0) // décale a droite
                {
                    tab[i*h+j + 1] = tab[i*h+j];
                    tab[i*h+j] = 0;
                    i=i+2;
                }
                if (r == 4 && j-1>=0 && tab[i*h+j - 1] == 0) // décale a gauche
                {
                    tab[i*h+j - 1] = tab[i*h+j];
                    tab[i*h+j] = 0;
                    i=i+2;
                }
            }
        }
    }
    return 1;
}
/*
int main(void)
{
    int tab[5][5];
    initialisation_5((int*)tab);
    int x=0;
    int y=0;
    setRandomPosition((int*)tab,&x,&y,5,5);
    printf("%d %d\n",x,y);
    //printTab((int*)tab,5); // print tab de base sans rien
    /*setRandomPositionToGhost(tab); // place un fantome 
    setRandomPositionToGhost(tab); // place un fantome
    setRandomPosition(tab,1); // place le joueur 1 
    setRandomPosition(tab,2); // place le joueur 2
    printf("\n");
    getPos(tab,1); // on demande la position du joueur 1
    getPos(tab,2); // on demande la position du joueur 2
    getFantomePos(tab); // on demande la position des fantomes
    printf("----------------------------------------------------------------------------------\n");
    printTab(tab); // on print le tableau avec les fantomes et les joueurs
    // getPos(5);
    for (int i = 0; i < 2; i++) // On déplce de 2 cases le joueur qu'on veut [MOVUP 2***]
    {
         goTop(tab, 2); // le joueur numéro 2 se déplace vers le haut
         goLeft(tab, 1); // le joueur numéro 1 se déplace vers la gauche
    }

    printf("----------------------------------------------------------------------------------\n");
    moveGhost(tab); // on fait bouger les fantomes d'une case aléatoire
    printTab(tab); // on print le nouveau tableau avec les positions mise a jour
    
}*/
