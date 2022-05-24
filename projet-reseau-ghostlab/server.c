#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>
#include <netdb.h>
#include "jeu.h"
// gcc -pthread -Wall -o server server.c

#define MAX 50
#define PORT 1234

struct Labyrinthe
{
    uint16_t h;
    uint16_t w;
    int nbdefantome;
    int matrice[5][5];
};

struct Joueur
{
    char *id;
    char *port;
    int points;
    int x;
    int y;
};

struct Game
{
    int partidanssalon;
    int partilance;
    uint8_t numeroPartie;
    int nbdejoueurpossible;
    uint8_t nbdejoueur;
    int joueurpret;
    char ip[16];
    char port[5];
    struct Joueur joueurs[MAX];
    struct Labyrinthe labyrinthe;
};

struct Game games[MAX];
// nombre de partie
uint8_t nombrePartie = 1;

int multidiffusion(char adressMulti[], char portMulti[], char msg[])
{
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    int r = getaddrinfo(adressMulti, portMulti, NULL, &first_info);
    if (r == 0)
    {
        if (first_info != NULL)
        {
            struct sockaddr *saddr = first_info->ai_addr;
            sendto(sock, msg, strlen(msg), 0, saddr, (socklen_t)sizeof(struct sockaddr_in));
        }
    }
    close(sock);
    return 0;
}

int sendUDP(char port[], char msg[])
{
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    int r = getaddrinfo("localhost", port, &hints, &first_info);
    if (r == 0)
    {
        if (first_info != NULL)
        {
            struct sockaddr *saddr = first_info->ai_addr;
         

            sendto(sock, msg, strlen(msg), 0, saddr,
                   (socklen_t)sizeof(struct sockaddr_in));
        }
    }
    close(sock);
    return 0;
}

int ip_to_15(char res[], char ip[]){
    int taille=strlen(ip);
    char diez[15-taille+1];
    for (int i=0; i<15-taille; i++){
        diez[i]='#';
    }
    diez[taille]='\0';
    printf("%s %s\n",ip,diez);
    strcpy(res,ip);
    strcat(res,diez);
    return 0;
}

int int_to_str_2(char str[], int n)
{
    if (n<10){
        sprintf(str,"0%d",n);
    }else {
        sprintf(str,"%d",n);
    }
    return 0;
}

int int_to_str_3(char str[], int n)
{
    if (n<10){
        sprintf(str,"00%d",n);
    }else if (n<100){
        sprintf(str,"0%d",n);
    }else{
        sprintf(str,"%d",n);
    }
    return 0;
}
int int_to_str_4(char str[], int n)
{
    if (n<10){
        sprintf(str,"000%d",n);
    }else if (n<100){
        sprintf(str,"00%d",n);
    }else if (n<1000){
        sprintf(str,"0%d",n);
    }else{
        sprintf(str,"%d",n);
    }
    return 0;
}

int entre_dans_la_partie(int sockcomm, char *identifiant, int m)
{
    char answer[100];
    int r;
    for (int i = 0; i < games[m].nbdejoueur; i++)
    {
        if (strcmp(identifiant, games[m].joueurs[i].id) == 0)
        {
            games[m].joueurs[i].points = 0;
            setRandomPosition((int *)games[m].labyrinthe.matrice, &games[m].joueurs[i].x, &games[m].joueurs[i].y, games[m].labyrinthe.h, games[m].labyrinthe.w);
            char x[4];
            int_to_str_3(x, games[m].joueurs[i].x);
            x[3] = '\0';
            char y[4];
            int_to_str_3(y, games[m].joueurs[i].y);
            y[3] = '\0';
            sprintf(answer, "POSIT %s %s %s***", identifiant, x, y);
            if ((send(sockcomm, answer, strlen(answer), 0)) == -1)
            {
                printf("Erreur dans le send\n");
                return -1;
            }
            break;
        }
    }
    while ((r = recv(sockcomm, answer, sizeof(answer), 0)) > 0)
    {
        answer[r] = '\0';
        printf("|%s|\n",answer);

        if (games[m].partilance == 0)
        {
            sprintf(answer, "GOBYE***");
            if ((r = send(sockcomm, answer, 9, 0)) == -1)
            {
                printf("Erreur dans le send\n");
                return -1;
            }
            return -1;
        }
        if (games[m].labyrinthe.nbdefantome == 0 || games[m].nbdejoueur == 0)
        {
            // TODO envoie mess multidiffusé ENDGA id p+++
            char tampon[100];

            int pointmax=0;
            char *iden=malloc(9);
            for (int i=0; i<games[m].nbdejoueur; i++){
                if(games[m].joueurs[i].points>pointmax){
                    pointmax=games[m].joueurs[i].points;
                    strcpy(iden,games[m].joueurs[i].id);
                }
            }
            char points[6];
            int_to_str_4(points, pointmax);
            points[5] = '\0';

            sprintf(tampon, "ENDGA %s %s+++", iden, points);
            multidiffusion(games[m].ip, games[m].port, tampon);

            sprintf(answer, "GOBYE***");
            if ((r = send(sockcomm, answer, 9, 0)) == -1)
            {
                printf("Erreur dans le send\n");
                return -1;
            }
            games[m].partilance = 0;
            return -1;
        }

        char *strToken = malloc(5);
        strncpy(strToken, answer, 5);
        printf("Commande rentré : %s\n", strToken);
        if (strncmp(strToken, "UPMOV", 5) == 0)
        {
            if (strlen(answer) != 12)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *esp = malloc(2);
            char *fin = malloc(4);
            strncpy(esp, answer + 5, 1);
            strncpy(fin, answer + 9, 3);
            esp[1]='\0';
            fin[3]='\0';
            if (strcmp(esp, " ") != 0 || strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    free(esp);
                    free(fin);
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            free(esp);
            free(fin);
            char *d = malloc(4);
            strncpy(d, answer + 6, 3);
            d[3]='\0';
            int n = atoi(d);
            free(d);
            if (n == 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }

            for (int i = 0; i < games[m].nbdejoueur; i++)
            {
                if (strcmp(games[m].joueurs[i].id, identifiant) == 0)
                {
                    int capture = 0;
                    for (int l = 1; l <= n; l++)
                    {
                        int a = goTop((int *)games[m].labyrinthe.matrice, games[m].joueurs[i].x, games[m].joueurs[i].y, games[m].labyrinthe.h, games[m].labyrinthe.w);
                        if (a == 1)
                        {
                            games[m].joueurs[i].x -= 1;
                        }
                        else if (a == 2)
                        {
                            capture = 1;
                            games[m].joueurs[i].x -= 1;
                            games[m].joueurs[i].points++;
                            games[m].labyrinthe.nbdefantome--;
                            // TODO Multidiffusé SCORE games[m].joueurs[i].id games[m].joueurs[i].points games[m].joueurs[i].x games[m].joueurs[i].y+++
                            char tampon[100];

                            char x[4];
                            int_to_str_3(x, games[m].joueurs[i].x);
                            x[3] = '\0';
                            char y[4];
                            int_to_str_3(y, games[m].joueurs[i].y);
                            y[3] = '\0';
                            char points[6];
                            int_to_str_4(points, games[m].joueurs[i].points);
                            points[5] = '\0';

                            sprintf(tampon, "SCORE %s %s %s %s+++", games[m].joueurs[i].id, points, x, y);
                            multidiffusion(games[m].ip, games[m].port, tampon);
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (capture == 0)
                    {
                        char x[4];
                        int_to_str_3(x, games[m].joueurs[i].x);
                        x[3] = '\0';
                        char y[4];
                        int_to_str_3(y, games[m].joueurs[i].y);
                        y[3] = '\0';
                        sprintf(answer, "MOVE! %s %s***", x, y);
                        if ((r = send(sockcomm, answer, strlen(answer), 0)) == -1)
                        {
                            printf("Erreur dans le send\n");
                            return -1;
                        }
                    }
                    else
                    {
                        char x[4];
                        int_to_str_3(x, games[m].joueurs[i].x);
                        x[3] = '\0';
                        char y[4];
                        int_to_str_3(y, games[m].joueurs[i].y);
                        y[3] = '\0';
                        char points[6];
                        int_to_str_4(points, games[m].joueurs[i].points);
                        points[5] = '\0';
                        sprintf(answer, "MOVEF %s %s %s***", x, y, points);
                        if ((r = send(sockcomm, answer, 20, 0)) == -1)
                        {
                            printf("Erreur dans le send\n");
                            return -1;
                        }
                    }
                    break;
                }
            }
            int random_i, random_j;
            srand(time(NULL));

            random_i = rand() % games[m].labyrinthe.h;
            random_j = rand() % games[m].labyrinthe.w;
            int b = moveUnGhost((int *)games[m].labyrinthe.matrice, random_i, random_j, games[m].labyrinthe.h, games[m].labyrinthe.w);
            if (b == 1)
            {
                // TODO multidifusé GHOST random_i random_j+++
                char tampon[100];

                
                char x[4];
                int_to_str_3(x, random_i);
                x[3] = '\0';
                char y[4];
                int_to_str_3(y, random_j);
                y[3] = '\0';

                sprintf(tampon, "GHOST %s %s+++", x, y);
                multidiffusion(games[m].ip, games[m].port, tampon);
            }
        }
        else if (strncmp(strToken, "DOMOV", 5) == 0)
        {
            if (strlen(answer) != 12)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *esp = malloc(2);
            char *fin = malloc(4);
            strncpy(esp, answer + 5, 1);
            strncpy(fin, answer + 9, 3);
            esp[1]='\0';
            fin[3]='\0';
            if (strcmp(esp, " ") != 0 || strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    free(esp);
                    free(fin);
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            free(esp);
            free(fin);
            char *d = malloc(4);
            strncpy(d, answer + 6, 3);
            d[3]='\0';
            int n = atoi(d);
            free(d);
            if (n == 0)
            {
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }

            for (int i = 0; i < games[m].nbdejoueur; i++)
            {
                if (strcmp(games[m].joueurs[i].id, identifiant) == 0)
                {
                    int capture = 0;
                    for (int l = 1; l <= n; l++)
                    {
                        int a = goBot((int *)games[m].labyrinthe.matrice, games[m].joueurs[i].x, games[m].joueurs[i].y, games[m].labyrinthe.h, games[m].labyrinthe.w);
                        if (a == 1)
                        {
                            games[m].joueurs[i].x += 1;
                        }
                        else if (a == 2)
                        {
                            capture = 1;
                            games[m].joueurs[i].x += 1;
                            games[m].joueurs[i].points++;
                            games[m].labyrinthe.nbdefantome--;
                            // TODO Multidiffusé SCORE games[m].joueurs[i].id games[m].joueurs[i].points games[m].joueurs[i].x games[m].joueurs[i].y+++
                            char tampon[100];

                            char x[4];
                            int_to_str_3(x, games[m].joueurs[i].x);
                            x[3] = '\0';
                            char y[4];
                            int_to_str_3(y, games[m].joueurs[i].y);
                            y[3] = '\0';
                            char points[6];
                            int_to_str_4(points, games[m].joueurs[i].points);
                            points[5] = '\0';

                            sprintf(tampon, "SCORE %s %s %s %s+++", games[m].joueurs[i].id, points, x, y);
                            multidiffusion(games[m].ip, games[m].port, tampon);
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (capture == 0)
                    {
                        char x[4];
                        int_to_str_3(x, games[m].joueurs[i].x);
                        x[3] = '\0';
                        char y[4];
                        int_to_str_3(y, games[m].joueurs[i].y);
                        y[3] = '\0';
                        sprintf(answer, "MOVE! %s %s***", x, y);
                        if ((r = send(sockcomm, answer, strlen(answer), 0)) == -1)
                        {
                            printf("Erreur dans le send\n");
                            return -1;
                        }
                    }
                    else
                    {
                        char x[4];
                        int_to_str_3(x, games[m].joueurs[i].x);
                        x[3] = '\0';
                        char y[4];
                        int_to_str_3(y, games[m].joueurs[i].y);
                        y[3] = '\0';
                        char points[6];
                        int_to_str_4(points, games[m].joueurs[i].points);
                        points[5] = '\0';
                        sprintf(answer, "MOVEF %s %s %s***", x, y, points);
                        if ((r = send(sockcomm, answer, 20, 0)) == -1)
                        {
                            printf("Erreur dans le send\n");
                            return -1;
                        }
                    }
                    break;
                }
            }
            int random_i, random_j;
            srand(time(NULL));

            random_i = rand() % games[m].labyrinthe.h;
            random_j = rand() % games[m].labyrinthe.w;
            int b = moveUnGhost((int *)games[m].labyrinthe.matrice, random_i, random_j, games[m].labyrinthe.h, games[m].labyrinthe.w);
            if (b == 1)
            {
                // TODO multidifusé GHOST random_i random_j+++
                char tampon[100];
                char x[4];
                int_to_str_3(x, random_i);
                x[3] = '\0';
                char y[4];
                int_to_str_3(y, random_j);
                y[3] = '\0';

                sprintf(tampon, "GHOST %s %s+++", x, y);
                multidiffusion(games[m].ip, games[m].port, tampon);
            }
        }
        else if (strncmp(strToken, "LEMOV", 5) == 0)
        {
            if (strlen(answer) != 12)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *esp = malloc(2);
            char *fin = malloc(4);
            strncpy(esp, answer + 5, 1);
            strncpy(fin, answer + 9, 3);
            esp[1]='\0';
            fin[3]='\0';
            if (strcmp(esp, " ") != 0 || strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    free(esp);
                    free(fin);
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            free(esp);
            free(fin);
            char *d = malloc(4);
            strncpy(d, answer + 6, 3);
            d[3]='\0';
            int n = atoi(d);
            free(d);
            if (n == 0)
            {
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }

            for (int i = 0; i < games[m].nbdejoueur; i++)
            {
                if (strcmp(games[m].joueurs[i].id, identifiant) == 0)
                {
                    int capture = 0;
                    for (int l = 1; l <= n; l++)
                    {
                        int a = goLeft((int *)games[m].labyrinthe.matrice, games[m].joueurs[i].x, games[m].joueurs[i].y, games[m].labyrinthe.h, games[m].labyrinthe.w);
                        if (a == 1)
                        {
                            games[m].joueurs[i].y -= 1;
                        }
                        else if (a == 2)
                        {
                            capture = 1;
                            games[m].joueurs[i].y -= 1;
                            games[m].joueurs[i].points++;
                            games[m].labyrinthe.nbdefantome--;
                            // TODO Multidiffusé SCORE games[m].joueurs[i].id games[m].joueurs[i].points games[m].joueurs[i].x games[m].joueurs[i].y+++
                            char tampon[100];

                            char x[4];
                            int_to_str_3(x, games[m].joueurs[i].x);
                            x[3] = '\0';
                            char y[4];
                            int_to_str_3(y, games[m].joueurs[i].y);
                            y[3] = '\0';
                            char points[6];
                            int_to_str_4(points, games[m].joueurs[i].points);
                            points[5] = '\0';

                            sprintf(tampon, "SCORE %s %s %s %s+++", games[m].joueurs[i].id, points, x, y);
                            multidiffusion(games[m].ip, games[m].port, tampon);
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (capture == 0)
                    {
                        char x[4];
                        int_to_str_3(x, games[m].joueurs[i].x);
                        x[3] = '\0';
                        char y[4];
                        int_to_str_3(y, games[m].joueurs[i].y);
                        y[3] = '\0';
                        sprintf(answer, "MOVE! %s %s***", x, y);
                        if ((r = send(sockcomm, answer, strlen(answer), 0)) == -1)
                        {
                            printf("Erreur dans le send\n");
                            return -1;
                        }
                    }
                    else
                    {
                        char x[4];
                        int_to_str_3(x, games[m].joueurs[i].x);
                        x[3] = '\0';
                        char y[4];
                        int_to_str_3(y, games[m].joueurs[i].y);
                        y[3] = '\0';
                        char points[6];
                        int_to_str_4(points, games[m].joueurs[i].points);
                        points[5] = '\0';
                        sprintf(answer, "MOVEF %s %s %s***", x, y, points);
                        if ((r = send(sockcomm, answer, 20, 0)) == -1)
                        {
                            printf("Erreur dans le send\n");
                            return -1;
                        }
                    }
                    break;
                }
            }
            int random_i, random_j;
            srand(time(NULL));

            random_i = rand() % games[m].labyrinthe.h;
            random_j = rand() % games[m].labyrinthe.w;
            int b = moveUnGhost((int *)games[m].labyrinthe.matrice, random_i, random_j, games[m].labyrinthe.h, games[m].labyrinthe.w);
            if (b == 1)
            {
                // TODO multidifusé GHOST random_i random_j+++
                char tampon[100];

                
                char x[4];
                int_to_str_3(x, random_i);
                x[3] = '\0';
                char y[4];
                int_to_str_3(y, random_j);
                y[3] = '\0';

                sprintf(tampon, "GHOST %s %s+++", x, y);
                multidiffusion(games[m].ip, games[m].port, tampon);
            }
        }
        else if (strncmp(strToken, "RIMOV", 5) == 0)
        {
            if (strlen(answer) != 12)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *esp = malloc(2);
            char *fin = malloc(4);
            strncpy(esp, answer + 5, 1);
            strncpy(fin, answer + 9, 3);
            esp[1]='\0';
            fin[3]='\0';
            if (strcmp(esp, " ") != 0 || strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    free(esp);
                    free(fin);
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            free(esp);
            free(fin);
            char *d = malloc(4);
            strncpy(d, answer + 6, 3);
            d[3]='\0';
            int n = atoi(d);
            free(d);
            if (n == 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }

            for (int i = 0; i < games[m].nbdejoueur; i++)
            {
                if (strcmp(games[m].joueurs[i].id, identifiant) == 0)
                {
                    int capture = 0;
                    for (int l = 1; l <= n; l++)
                    {
                        int a = goRight((int *)games[m].labyrinthe.matrice, games[m].joueurs[i].x, games[m].joueurs[i].y, games[m].labyrinthe.h, games[m].labyrinthe.w);
                        if (a == 1)
                        {
                            games[m].joueurs[i].y += 1;
                        }
                        else if (a == 2)
                        {
                            capture = 1;
                            games[m].joueurs[i].y += 1;
                            games[m].joueurs[i].points++;
                            games[m].labyrinthe.nbdefantome--;
                            // TODO Multidiffusé SCORE games[m].joueurs[i].id games[m].joueurs[i].points games[m].joueurs[i].x games[m].joueurs[i].y+++
                            char tampon[100];

                            char x[4];
                            int_to_str_3(x, games[m].joueurs[i].x);
                            x[3] = '\0';
                            char y[4];
                            int_to_str_3(y, games[m].joueurs[i].y);
                            y[3] = '\0';
                            char points[6];
                            int_to_str_4(points, games[m].joueurs[i].points);
                            points[5] = '\0';

                            sprintf(tampon, "SCORE %s %s %s %s+++", games[m].joueurs[i].id, points, x, y);
                            multidiffusion(games[m].ip, games[m].port, tampon);
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (capture == 0)
                    {
                        char x[4];
                        int_to_str_3(x, games[m].joueurs[i].x);
                        x[3] = '\0';
                        char y[4];
                        int_to_str_3(y, games[m].joueurs[i].y);
                        y[3] = '\0';
                        sprintf(answer, "MOVE! %s %s***", x, y);
                        if ((r = send(sockcomm, answer, strlen(answer), 0)) == -1)
                        {
                            printf("Erreur dans le send\n");
                            return -1;
                        }
                    }
                    else
                    {
                        char x[4];
                        int_to_str_3(x, games[m].joueurs[i].x);
                        x[3] = '\0';
                        char y[4];
                        int_to_str_3(y, games[m].joueurs[i].y);
                        y[3] = '\0';
                        char points[6];
                        int_to_str_4(points, games[m].joueurs[i].points);
                        points[5] = '\0';
                        sprintf(answer, "MOVEF %s %s %s***", x, y, points);
                        if ((r = send(sockcomm, answer, 20, 0)) == -1)
                        {
                            printf("Erreur dans le send\n");
                            return -1;
                        }
                    }
                    break;
                }
            }
            int random_i, random_j;
            srand(time(NULL));

            random_i = rand() % games[m].labyrinthe.h;
            random_j = rand() % games[m].labyrinthe.w;
            int b = moveUnGhost((int *)games[m].labyrinthe.matrice, random_i, random_j, games[m].labyrinthe.h, games[m].labyrinthe.w);
            if (b == 1)
            {
                // TODO multidifusé GHOST random_i random_j+++
                char tampon[100];

            
                char x[4];
                int_to_str_3(x, random_i);
                x[3] = '\0';
                char y[4];
                int_to_str_3(y, random_j);
                y[3] = '\0';

                sprintf(tampon, "GHOST %s %s+++", x, y);
                multidiffusion(games[m].ip, games[m].port, tampon);
            }
        }
        else if (strncmp(strToken, "IQUIT", 5) == 0)
        {
            if (strlen(answer) != 8)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *fin = malloc(4);
            strncpy(fin, answer + 5, 3);
            fin[3]='\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    free(fin);
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            free(fin);
            int ok = 0;
            for (int j = 0; j < games[m].nbdejoueur; j++)
            {
                if (j + 1 >= games[m].nbdejoueur)
                {
                    games[m].nbdejoueur -= 1;
                    free(games[m].joueurs[j].id);
                    free(games[m].joueurs[j].port);
                    games[m].joueurs[j].points = 0;
                    games[m].joueurs[j].x = 0;
                    games[m].joueurs[j].y = 0;
                }
                else if ((strcmp(games[m].joueurs[j].id, identifiant) == 0) || ok == 1)
                {
                    games[m].joueurs[j].id = realloc(games[m].joueurs[j].id, strlen(games[m].joueurs[j + 1].id) + 1);
                    strcpy(games[m].joueurs[j].id, games[m].joueurs[j + 1].id);
                    games[m].joueurs[j].port = realloc(games[m].joueurs[j].port, strlen(games[m].joueurs[j + 1].port) + 1);
                    strcpy(games[m].joueurs[j].port, games[m].joueurs[j + 1].port);
                    games[m].joueurs[j].points = games[m].joueurs[j + 1].points;
                    games[m].joueurs[j].x = games[m].joueurs[j + 1].x;
                    games[m].joueurs[j].y = games[m].joueurs[j + 1].y;
                    ok = 1;
                }
            }
            free(identifiant);
            identifiant = NULL;

            // m (1 octet)
            sprintf(answer, "GOBYE***");
            if ((r = send(sockcomm, answer, 9, 0)) == -1)
            {
                printf("Erreur dans le send\n");
                return -1;
            }
            return -1;
        }
        else if (strncmp(strToken, "GLIS?", 5) == 0)
        {
            if (strlen(answer) != 8)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *fin = malloc(4);
            strncpy(fin, answer + 5, 3);
            fin[3]='\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    free(fin);
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            free(fin);
            sprintf(answer, "GLIS! %d***", games[m].nbdejoueur);
            if ((r = send(sockcomm, answer, 10, 0)) == -1)
            {
                printf("Erreur dans le send\n");
                return -1;
            }

            for (int i = 0; i < games[m].nbdejoueur; i++)
            {
                char x[4];
                int_to_str_3(x, games[m].joueurs[i].x);
                x[3] = '\0';
                char y[4];
                int_to_str_3(y, games[m].joueurs[i].y);
                y[3] = '\0';
                char points[6];
                int_to_str_4(points, games[m].joueurs[i].points);
                points[5] = '\0';
                sprintf(answer, "GPLYR %s %s %s %s***", games[m].joueurs[i].id, x, y, points);
                // id x y p
                if ((r = send(sockcomm, answer, 5 + 1 + 8 + 1 + 3 + 1 + 3 + 1 + 4 + 3, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
            }
        }
        else if (strncmp(strToken, "MALL?", 5) == 0)
        {
            if (strlen(answer) < 7)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *esp = malloc(2);
            strncpy(esp, answer + 5, 1);
            esp[1]='\0';
            if (strcmp(esp, " ") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    free(esp);
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            free(esp);
            char *a = strstr(answer + 6, "***");
            char *b = strstr(answer + 6, "+++");
            int positiondefinetoile = a - answer;
            int positiondefinplus = b - answer;
            if (a == NULL || positiondefinetoile + 3 != strlen(answer) || b != NULL || positiondefinetoile - 6 > 200)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *mess = malloc(200);
            strncpy(mess, answer + 6, positiondefinetoile - 6);

            char tampon[100];
            sprintf(tampon, "MESSA %s %s+++", identifiant, mess);
            multidiffusion(games[m].ip, games[m].port, tampon);

            sprintf(answer, "MALL!***");
            if ((r = send(sockcomm, answer, 9, 0)) == -1)
            {
                free(esp);
                printf("Erreur dans le send\n");
                return -1;
            }
        }
        else if (strncmp(strToken, "SEND?", 5) == 0)
        {
            if (strlen(answer) < 16)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *esp1 = malloc(2);
            strncpy(esp1, answer + 5, 1);
            esp1[1]='\0';
            char *id = malloc(9);
            strncpy(id, answer + 6, 8);
            id[8]='\0';
            char *esp2 = malloc(2);
            strncpy(esp2, answer + 14, 1);
            esp2[1]='\0';

            if (strcmp(esp1, " ") != 0 || strlen(id) != 8 || strcmp(esp2, " ") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    free(esp1);
                    free(esp2);
                    free(id);
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            free(esp1);
            free(esp2);
            char *a = strstr(answer + 14, "***");
            char *b = strstr(answer + 14, "+++");
            int positiondefinetoile = a - answer;
            int positiondefinplus = b - answer;
            if (a == NULL || positiondefinetoile + 3 != strlen(answer) || b != NULL || positiondefinetoile - 14 > 200)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
                continue;
            }
            char *mess = malloc(200);
            strncpy(mess, answer + 14, positiondefinetoile - 14);

            int envoye = 0;

            for (int i = 0; i < games[m].nbdejoueur; i++)
            {
                if (strcmp(id, games[m].joueurs[i].id) == 0)
                {
                    // TODO envoyer le mess UDP au joueur i MESSP identifiant mess+++
                    char udpmess[300];
                    sprintf(udpmess,"MESSP %s %s+++",identifiant,mess);
                    sendUDP(games[m].joueurs[i].port, udpmess);
                    sprintf(answer, "SEND!***");
                    if ((r = send(sockcomm, answer, 9, 0)) == -1)
                    {
                        free(id);
                        printf("Erreur dans le send\n");
                        return -1;
                    }
                    envoye=1;
                    continue;
                }
            }

            free(id);

            if(envoye==0){
                sprintf(answer, "NSEND***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");
                    return -1;
                }
            }
        }
        else
        {
            sprintf(answer, "DUNNO***");
            if ((r = send(sockcomm, answer, 9, 0)) == -1)
            {
                printf("Erreur dans le send\n");
                return -1;
            }
        }
        free(strToken);
    }
    return -1;
}

void *comm(void *arg)
{

    int sockcomm = *((int *)arg);

    char answer[100];
    // pour recevoir les messages
    int r;
    // struct Joueur joueur;

    uint8_t c = 0;
    for (int i = 0; i < nombrePartie; i++)
    {
        if (games[i].partidanssalon == 1)
        {
            c++;
        }
    }
    // envoie nombre de partie qui risque de commencer
    // GAMES n (sur 1 octet)
    sprintf(answer, "GAMES %d***\n", c); // %d=n
    if ((r = send(sockcomm, answer, 9 + sizeof(uint8_t), 0)) == -1)
    {
        printf("Erreur dans le send\n");

        close(sockcomm);
        free(arg);

        return NULL;
    }
    // envoie nombre de partie avec id et nombre de joueur
    for (int i = 0; i < nombrePartie; i++)
    {
        if (games[i].partidanssalon == 1)
        {
            // OGAMES m s (1 octet les deux)
            sprintf(answer, "OGAMES %d %d***", games[i].numeroPartie, games[i].nbdejoueur);
            if ((r = send(sockcomm, answer, 11 + sizeof(uint8_t) + sizeof(uint8_t), 0)) == -1)
            {
                printf("Erreur dans le send\n");

                close(sockcomm);
                free(arg);

                return NULL;
            }
        }
    }

    char *identifiant = NULL;
    uint8_t joueurdanspartie = -1;

    while ((r = recv(sockcomm, answer, sizeof(answer), 0)) > 0)
    {
        /*Pour voir les modifications sur les données du serveur
        for (int i = 0; i<MAX; i++){
            if (games[i].partiefini==1 || games[i].partilance==1){
                printf("numerodepartie: %d\n",games[i].numeroPartie);
                printf("nbdejoueur: %d\n",games[i].nbdejoueur);
                printf("nbdejoueurpossible: %d\n",games[i].nbdejoueurpossible);
                for (int j = 0; j<games[i].nbdejoueur; j++){
                    printf("joueur %d : id=%s port=%s\n",j,games[i].joueurs[j].id,games[i].joueurs[j].port);
                }
                printf("nbdejoueurpret: %d\n",games[i].joueurpret);
            }
        }
        */

        answer[r] = '\0';
        char *strToken = malloc(5);
        strncpy(strToken, answer, 5);
        printf("Commande rentré : %s\n", strToken);
        if (strncmp(strToken, "NEWPL", 5) == 0) // Création de partie
        {
            if (identifiant != NULL || (strncmp(answer + 5, " ", 1) != 0) || strlen(answer) < 6)
            {
                // send regno
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            //      8  4
            // NEWPL id port (id char[8] et  port char[4])
            char *id = malloc(9);
            strncpy(id, answer + 6, 8); // on recupere l'id
            id[8] = '\0';
            if (strlen(id) != 8 || strlen(answer) < 15)
            {
                // send regno
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *port = malloc(5);
            strncpy(port, answer + 15, 4); // on recupere le port
            port[4] = '\0';
            int test = atoi(port);
            int num = -1;
            for (int k = 1; k < MAX; k++)
            {
                if (games[k].partidanssalon == 0 && games[k].partilance == 0 && nombrePartie <= k)
                {
                    num = k;
                    nombrePartie += 1;
                    break;
                }
                else if (games[k].partidanssalon == 0 && games[k].partilance == 0)
                {
                    num = k;
                    break;
                }
            }
            if (num == -1 || test <= 999 || test >= 10000 || strlen(answer) < 15 + 4)
            {
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *fin = malloc(5);
            strncpy(fin, answer + 19, 3);
            fin[4] = '\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            struct Joueur j = {id, port};
            struct Labyrinthe l = {5, 5, 2};
            initialisation_5((int *)l.matrice);
            for (int c = 0; c < l.nbdefantome; c++)
            {
                while(setRandomPositionToGhost((int *)l.matrice, l.h, l.w)==1){
                    sleep(1);
                }
                sleep(2);
            }
            struct Game newgame = {1, 1, num, 2, 1, 0};
            strcpy(newgame.ip,"225.1.2.4");
            int portgame = PORT + nombrePartie;
            char portg[5];
            sprintf(portg, "%d", portgame);
            portg[4] = '\0';
            strcpy(newgame.port, portg);
            newgame.joueurs[newgame.nbdejoueur - 1] = j;
            newgame.labyrinthe = l;
            newgame.partidanssalon=1;
            games[num] = newgame;
            nombrePartie += 1;
            identifiant = malloc(9);
            strcpy(identifiant, id);
            joueurdanspartie = num;

            // send regok m (1 octet)
            sprintf(answer, "REGOK %d***", num);
            if ((r = send(sockcomm, answer, 9 + sizeof(uint8_t), 0)) == -1)
            {
                printf("Erreur dans le send\n");

                close(sockcomm);
                free(arg);

                return NULL;
            }
        }
        // REGIS id port m (char[8] char[4] 1 octet)
        else if (strncmp(strToken, "REGIS", 5) == 0) // inscription a partie
        {
            if (identifiant != NULL || (strncmp(answer + 5, " ", 1) != 0) || strlen(answer) < 6)
            {
                // send regno
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *id = malloc(9);
            strncpy(id, answer + 6, 8); // on recupere l'id
            id[8] = '\0';
            if (strlen(id) != 8 || strlen(answer) < 15)
            {
                // send regno
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *port = malloc(5);
            strncpy(port, answer + 15, 4); // on recupere le port
            port[4] = '\0';
            int test = atoi(port);
            if (test <= 999 || test >= 10000 || strlen(answer) < 15 + 4 + 1)
            {
                // send regno
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *str = malloc(2);
            strncpy(str, answer + 15 + 4 + 1, 1);
            str[1] = '\0';
            uint8_t m = atoi(str); // on recupere le num de la partie
            free(str);
            if (m == 0)
            {
                // send regno
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            test = 0;
            for (int i = 0; i < games[m].nbdejoueur; i++)
            {
                if (strcmp(id, games[m].joueurs[i].id) == 0)
                {
                    test = 1;
                    break;
                }
            }
            if (m > nombrePartie || games[m].partidanssalon == 0 || games[m].partilance == 0 || games[m].nbdejoueur >= games[m].nbdejoueurpossible || test == 1 || strlen(answer) < 21)
            {
                // send regno
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *fin = malloc(5);
            strncpy(fin, answer + 21, 3);
            fin[4] = '\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "REGNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            free(fin);
            struct Joueur j = {id, port};
            games[m].nbdejoueur += 1;
            games[m].joueurs[games[m].nbdejoueur - 1] = j;
            identifiant = malloc(9);
            strcpy(identifiant, id);
            joueurdanspartie = games[m].numeroPartie;
            // send regok (1 octet)
            sprintf(answer, "REGOK %d***", m);
            if ((r = send(sockcomm, answer, 9 + sizeof(uint8_t), 0)) == -1)
            {
                printf("Erreur dans le send\n");

                close(sockcomm);
                free(arg);

                return NULL;
            }
        }

        else if (strncmp(strToken, "UNREG", 5) == 0) // désinscription a partie
        {

            if (identifiant == NULL || strlen(answer) < 5)
            {
                // send dunno
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *fin = malloc(5);
            strncpy(fin, answer + 5, 3);
            fin[4] = '\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            free(fin);
            int ok = 0;
            for (int j = 0; j < games[joueurdanspartie].nbdejoueur; j++)
            {
                if (j + 1 >= games[joueurdanspartie].nbdejoueur)
                {
                    games[joueurdanspartie].nbdejoueur -= 1;
                    free(games[joueurdanspartie].joueurs[j].id);
                    free(games[joueurdanspartie].joueurs[j].port);
                }
                else if ((strcmp(games[joueurdanspartie].joueurs[j].id, identifiant) == 0) || ok == 1)
                {
                    games[joueurdanspartie].joueurs[j].id = realloc(games[joueurdanspartie].joueurs[j].id, strlen(games[joueurdanspartie].joueurs[j + 1].id) + 1);
                    strcpy(games[joueurdanspartie].joueurs[j].id, games[joueurdanspartie].joueurs[j + 1].id);
                    games[joueurdanspartie].joueurs[j].port = realloc(games[joueurdanspartie].joueurs[j].port, strlen(games[joueurdanspartie].joueurs[j + 1].port) + 1);
                    strcpy(games[joueurdanspartie].joueurs[j].port, games[joueurdanspartie].joueurs[j + 1].port);
                    ok = 1;
                }
            }
            free(identifiant);
            identifiant = NULL;
            if (games[joueurdanspartie].nbdejoueur == 0)
            {
                games[joueurdanspartie].partidanssalon = 0;
                games[joueurdanspartie].partilance = 0;
            }

            // m (1 octet)
            sprintf(answer, "UNROK %d***", joueurdanspartie);
            if ((r = send(sockcomm, answer, 9 + sizeof(uint8_t), 0)) == -1)
            {
                printf("Erreur dans le send\n");

                close(sockcomm);
                free(arg);

                return NULL;
            }
            joueurdanspartie = -1;
        }
        else if (strncmp(strToken, "SIZE?", 5) == 0) // taille labyrinthe
        {
            if ((strncmp(answer + 5, " ", 1) != 0) || strlen(answer) < 6)
            {
                // send dunno
                sprintf(answer, "DUNNO***");
                if ((r = (send(sockcomm, answer, 9, 0))) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            // SIZE? m (1 octet)
            char *str = malloc(2);
            strncpy(str, answer + 6, 1);
            str[1] = '\0';
            uint8_t m = atoi(str); // on recupere le num de la partie
            free(str);
            if (m == 0 || m > nombrePartie || games[m].partidanssalon == 0 || games[m].partilance == 0 || strlen(answer) < 7)
            {
                // send dunno
                sprintf(answer, "DUNNO***");
                if ((r = (send(sockcomm, answer, 9, 0))) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *fin = malloc(5);
            strncpy(fin, answer + 7, 3);
            fin[4] = '\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            free(fin);
            // SIZE! m h w (1 octet, 2 octet LE, 2 octet LE)
            // send SIZE! m games[m].labyrinthe.h games[m].labyrinthe.w***
            sprintf(answer, "SIZE! %d %d %d***", m, games[m].labyrinthe.h, games[m].labyrinthe.w);
            if ((r = send(sockcomm, answer, 11 + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t), 0)) == -1)
            {
                printf("Erreur dans le send\n");

                close(sockcomm);
                free(arg);

                return NULL;
            }
        }
        else if (strncmp(strToken, "LIST?", 5) == 0) // liste des joueurs
        {
            if ((strncmp(answer + 5, " ", 1) != 0) || strlen(answer) < 6)
            {
                // send dunno
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            // LIST? m (1 octet)
            char *str = malloc(2);
            strncpy(str, answer + 6, 1);
            str[1] = '\0';
            uint8_t m = atoi(str); // on recupere le num de la partie
            free(str);
            if (m == 0 || m > nombrePartie || games[m].partidanssalon == 0 || games[m].partilance == 0 || strlen(answer) < 7)
            {
                // send dunno
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *fin = malloc(5);
            strncpy(fin, answer + 7, 3);
            fin[4] = '\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            free(fin);
            // LIST! m s (1 octet, 1 octet)
            //  send LIST! m games[m].nbdejoueur***
            sprintf(answer, "LIST! %d %d***", m, games[m].nbdejoueur);
            if ((r = send(sockcomm, answer, 10 + sizeof(uint8_t) + sizeof(uint8_t), 0)) == -1)
            {
                printf("Erreur dans le send\n");

                close(sockcomm);
                free(arg);

                return NULL;
            }
            char *iden = malloc(8);
            for (int i = 0; i < games[m].nbdejoueur; i++)
            {
                // PLAYR id (char [8])
                //  send PLAYR games[m].joueurs[i].id
                printf("%s\n", games[m].joueurs[i].id);
                strncpy(iden, games[m].joueurs[i].id, 8);

                sprintf(answer, "PLAYR %s***", iden);
                printf("iden: %s| answer: %s\n", iden, answer);
                if ((r = send(sockcomm, answer, 5 + 1 + 8 + 3, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
            }
            free(iden);
        }
        else if (strncmp(strToken, "GAME?", 5) == 0) // liste partie pas commencé
        {
            if (strlen(answer) < 5)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *fin = malloc(5);
            strncpy(fin, answer + 5, 3);
            fin[4] = '\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            free(fin);
            uint8_t compteur = 0;

            for (int i = 0; i < nombrePartie; i++)
            {
                if (games[i].partidanssalon == 1)
                {
                    compteur++;
                }
            }
            // send GAMES compteur***
            // GAMES n (1 octet)
            sprintf(answer, "GAMES %d***", compteur);
            if ((r = send(sockcomm, answer, 9 + sizeof(uint8_t), 0)) == -1)
            {
                printf("Erreur dans le send\n");

                close(sockcomm);
                free(arg);

                return NULL;
            }
            for (uint8_t i = 0; i < nombrePartie; i++)
            {
                if (games[i].partidanssalon == 1)
                {
                    // send OGAME i games[i].nbdejoueur***
                    // OGAM m s (1 octet 1 octet)
                    sprintf(answer, "OGAME %d %d***", i, games[i].nbdejoueur);
                    if ((r = send(sockcomm, answer, 10 + sizeof(uint8_t) + sizeof(uint8_t), 0)) == -1)
                    {
                        printf("Erreur dans le send\n");

                        close(sockcomm);
                        free(arg);

                        return NULL;
                    }
                }
            }
        }
        else if (strncmp(strToken, "START", 5) == 0) // liste partie pas commencé
        {
            if (identifiant == NULL || strlen(answer) < 5)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            char *fin = malloc(5);
            strncpy(fin, answer + 5, 3);
            fin[4] = '\0';
            if (strcmp(fin, "***") != 0)
            {
                sprintf(answer, "DUNNO***");
                if ((r = send(sockcomm, answer, 9, 0)) == -1)
                {
                    printf("Erreur dans le send\n");

                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
                continue;
            }
            free(fin);

            for (int j = 0; j < games[joueurdanspartie].nbdejoueur; j++)
            {
                if (strcmp(games[joueurdanspartie].joueurs[j].id, identifiant) == 0)
                {
                    games[joueurdanspartie].joueurpret += 1;
                    while (games[joueurdanspartie].joueurpret != games[joueurdanspartie].nbdejoueurpossible)
                    {
                        sleep(1);
                    }
                    char ip[16];
                    ip_to_15(ip,games[joueurdanspartie].ip);
                    ip[15]='\0';
                    char h2[3];
                    int_to_str_2(h2,games[joueurdanspartie].labyrinthe.h);
                    h2[2]='\0';
                    char w2[3];
                    int_to_str_2(w2,games[joueurdanspartie].labyrinthe.w);
                    w2[2]='\0';

                    sprintf(answer, "WELCO %d %s %s %d %s %s***", joueurdanspartie, h2, w2, games[joueurdanspartie].labyrinthe.nbdefantome, ip, games[joueurdanspartie].port);
                    if ((r = send(sockcomm, answer, 39, 0)) == -1)
                    {
                        printf("Erreur dans le send\n");

                        close(sockcomm);
                        free(arg);

                        return NULL;
                    }
                    games[joueurdanspartie].partilance = 1;
                    games[joueurdanspartie].partidanssalon=0;
                    entre_dans_la_partie(sockcomm, identifiant, joueurdanspartie);
                    close(sockcomm);
                    free(arg);

                    return NULL;
                }
            }
        }
        else
        {
            sprintf(answer, "DUNNO***");
            if ((r = send(sockcomm, answer, 9, 0)) == -1)
            {
                printf("Erreur dans le send\n");

                close(sockcomm);
                free(arg);

                return NULL;
            }
            continue;
        }
        free(strToken);
    }
    close(sockcomm);
    free(arg);

    return NULL;
}

int main(int argc, char **argv)
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (argc != 2)
    {
        printf("Erreur il faut fournir un numero de port.\n");
        exit(-1);
    }
    int port = atoi(argv[1]);

    struct sockaddr_in sockaddress;
    sockaddress.sin_family = AF_INET;
    sockaddress.sin_port = htons(port);
    sockaddress.sin_addr.s_addr = htonl(INADDR_ANY);

    int r = bind(sock, (struct sockaddr *)&sockaddress, sizeof(struct sockaddr_in));
    if (r == 0)
    {
        r = listen(sock, 0);
        if (r == 0)
        {
            struct sockaddr_in caller;
            socklen_t size = sizeof(caller);
            while (1)
            {
                int *sock2 = (int *)malloc(sizeof(int));
                *sock2 = accept(sock, (struct sockaddr *)&caller, &size);
                if (sock2 >= 0)
                {

                    pthread_t th;
                    pthread_create(&th, NULL, comm, sock2);
                }
                else
                {
                    exit(-1);
                }
            }
        }
        else
        {
            printf("Problème dans le listen\n");
            exit(-1);
        }
    }
    else
    {
        printf("Erreur dans le bind\n");
        exit(-1);
    }
    close(sock);
    return 0;
}