# Projet Reseau GhostLab

Projet du cours de Programmation Réseau 2022 (L3) 2021-2022.

Ce dépôt contient : 

- [L'énoncé](projet-reseau-ghostlab.pdf) du projet  
- Un [Client](Client.java) qui joue le rôle du Client.
- Un [server](server.c) qui joue le rôle du Serveur
- Le fichier [jeu](jeu.c) qui s'occuppe du fonctionnement du jeu
- Un [Makefil](Makefile) pour compiler le tout.

## Mode d'emploi

Pour commencer, il faut compilé le projet avec la commande ```make```  
Ensuite, il faut démarrer le serveur avec ```./server PORT ``` avec PORT comme numéro de port.
Ensuite, il faut lancer un ou plusieur client(s) avec ```java Client PORT``` avec PORT, le numéro de port du serveur.

Pour créer une partie :   
```
NEWPL id port***
```  
Exemple :   
```
NEWPL cecitest 1234***
```   
Pour rejoindre une partie en cours:  
```
REGIS id port num_partie***
```  
Exemple :  
```
REGIS cesttest 4444 1***
```  
Pour afficher les parties non commencées :  
```
GAME?***
```  
Pour afficher les joueurs d'une partie non commencées : 
```
LIST? num_partie***
```
Exemple :
```
LIST? 1***
```
Pour se désinscrire d'une partie :
```
UNREG***
```
Pour demander la taille du labyrinthe : 
```
SIZE? num_partie***
```
Exemple : 
```
SIZE? 1***
```
Pour mettre prêt après avoir rejoint la partie :
```
START***
```
Pour se déplacer dans le labyrinthe :
```
UPMOV 001*** // RIMOV 001*** // LEMOV 001*** // DOMOV 001***
```
Pour afficher la liste des joueurs de la partie 
```
GLIS?***
```
Pour envoyer un message a tout les joueurs : 
```
MALL? message***
```
Pour envoyer un message a un joueur :
```
SEND? id mess***
```
avec id, l'identifiant du joueur a qui on veut envoyé le message.
