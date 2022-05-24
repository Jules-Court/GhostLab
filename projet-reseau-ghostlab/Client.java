import java.net.*;
import java.io.*;
import java.util.Scanner;

import javax.swing.border.StrokeBorder;

public class Client {
  private static int portdonne = -1;


  public static void multiCast(String adresse, int port) { // reception multicast
    try {
      MulticastSocket mso = new MulticastSocket(port);
      mso.joinGroup(InetAddress.getByName(adresse));
      byte[] data = new byte[100];
      DatagramPacket paquet = new DatagramPacket(data, data.length);
      while (true) {
        mso.receive(paquet);
        String st = new String(paquet.getData(), 0, paquet.getLength());
        String pseudo = st.substring(6, 14);
        String msg = st.substring(15, st.length() - 3);
        System.out.println("\u001B[33m[" + pseudo + "]\u001B[0m : " + msg);
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  public static void receiveUDP(int port) { // reception UDP
    try {
      DatagramSocket dso = new DatagramSocket(port);
      byte[] data = new byte[100];
      while (true) {
        DatagramPacket paquet = new DatagramPacket(data, data.length);
        dso.receive(paquet);
        String st = new String(paquet.getData(), 0, paquet.getLength());
        String pseudo = st.substring(6, 14);
        String msg = st.substring(15, st.length() - 3);
        System.out.println("\033[0;92m[" + pseudo + "]\u001B[0m : " + msg);
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
  }



  public static void main(String[] args) {

    if (args.length != 1) {
      System.err.println("Erreur il faut fournir un numero de port.");
      System.exit(1);
    }

    int port = Integer.parseInt(args[0]);

    try {

      Socket socket = new Socket("localhost", port);

      BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
      PrintWriter pw = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));

      // remplit buf
      char[] buf = new char[20];
      int offset = 0;
      int length = 10;
      br.read(buf, offset, length);

      // affiche GAMES n
      System.out.println("Il y a " + buf[6] + " partie(s) en attente : ");

      int n = Integer.parseInt(String.valueOf(buf[6]));

      // affiche les différentes parties
      for (int i = 0; i < n; i++) {
        br.read(buf, offset, length + 3);
        System.out.println("- la partie " + buf[7] + " compte " + buf[9] + " joueur(s)");
      }


      
      

      // Intéractions avant le START

      String rep="";
      char[] strToken = new char[5];// les 5 premiers char de reacS
      char[] reacS = new char[100];

      while(true){
          // gestion des réponses du serveur
          
        
          System.out.println("\nVous pouvez toujours vous désinscrire, demander la taille du labyrinthe,\nla liste des joueurs, la liste des parties non commencées.\nSi vous êtes prêt.e envoyez START*** ");
          Scanner sc= new Scanner(System.in);
          if(sc.hasNext()){
              rep= sc.nextLine();
              pw.print(rep);
              pw.flush();
          }
          
          if(rep.equals("exit")){
              System.exit(1);
          }
         
          //récupération du token suite à la demande du client 
          br.read(strToken,0,5);
          if(String.valueOf(strToken).equals("WELCO")){
            br.read(reacS, 0, 34);

            System.out.println(reacS);

            
            // RECUPERER ADRESSM ET PORT POUR MULTIDIFFUSION
            // 4 05 05 2 225.1.2.4###### 1239***
              
            String adressM=""+reacS[11]+reacS[12]+reacS[13]+reacS[14]+reacS[15]+reacS[16]+reacS[17]+reacS[18]+reacS[19];
            String portMulti=""+reacS[27]+reacS[28]+reacS[29]+reacS[30];
            int portM=Integer.parseInt(portMulti);

            System.out.println("Bienvenue dans la partie "+reacS[1]+".\nLe labyrinthe dans lequel vous jouez a pour dimensions "+reacS[3]+reacS[4]+"x"+reacS[6]+reacS[7]+".\nLa partie compte "+reacS[9]+" fantôme(s).");
            br.read(reacS, 0, 25);
      
            System.out.println(String.valueOf(reacS).substring(0,25));
            System.out.println("\nVous vous trouvez à la position "+reacS[15]+reacS[16]+reacS[17]+"x"+reacS[19]+reacS[20]+reacS[21]+"");
            Thread t = new Thread(new Runnable() {

              @Override
              public void run() {
                // TODO Auto-generated method stub
      
                multiCast(adressM, portM);
              }
            });
            Thread t2 = new Thread(new Runnable() {

              @Override
              public void run() {
                // TODO Auto-generated method stub
      
                receiveUDP(portdonne);
              }
            });
          
            t2.start();
      
            t.start();
      
            break;
          }
          System.out.println("Token du serveur :");
          System.out.println(strToken);
          //br.read(reacS, 0, 23);

          
          switch(String.valueOf(strToken)){
            case "REGOK" :
            // REGOK 1***
                br.read(reacS, offset, 9);
                String portUDP = rep; 
                portUDP=""+rep.charAt(15)+rep.charAt(16)+rep.charAt(17)+rep.charAt(18);
                portdonne=Integer.parseInt(portUDP);

                System.out.println("\nVous êtes bien enregistré.e ! Votre partie est la numéro "+reacS[1]+".");
                break;
            case "REGNO" :
                System.out.println("\nDésolé vous n'avez pas pu être enregistré.e.\nVérifiez que votre pseudo comporte 8 caractères alphanumériques et que votre port est correct.\nVous pouvez retenter ou nous quitter."); // gérer le nous quitter
                
                br.read(strToken,0,5);
                break;
                
            case "UNROK" : //TO DO : mieux faire ça
                portdonne=-1;
                System.out.println("Vous êtes bien désincrit.e, vous pouver rejoindre une nouvelle partie ou nous quitter");
                br.read(strToken, 0, 5);
                break;

            case "SIZE!" :
                br.read(reacS,offset,11);
                char m=reacS[1];
                String h=String.valueOf(reacS).substring(3,5);
                String w=String.valueOf(reacS).substring(5,6);
                System.out.println("Le labyrinthe de la partie "+m+" a pour hauteur "+h+" et pour largeur "+w+"\n");
                break;

            case "LIST!":
                System.out.println("Voici la liste des joueurs inscrits à cette partie :");
                br.read(reacS,offset,7);
                int s=Integer.parseInt(String.valueOf(reacS[3]));
                char[] idj=new char[17];
                for(int i=0; i<s; i++){
                    br.read(idj,offset,17);
                    String j=String.valueOf(idj).substring(6,14);
                    System.out.println(j);
                }
                break;

            case "GAMES":
                System.out.println("La liste des parties pour lesquelles des joueurs se sont inscrits :");
                br.read(reacS,offset,5);
                int np=Integer.parseInt(String.valueOf(reacS[1]));
                char[] ogame=new char[12];
                for(int i=0; i<np; i++){
                    br.read(ogame,offset,12);
                    char mi=ogame[6];
                    char si=ogame[8];
                    System.out.println("La partie "+mi+" compte "+si+" joueurs");
                }
                break;

            case "DUNNO":
                System.out.println("Votre demande n'a pas de sens");
                //peut être rajouter un read pour déplacer jsp l'offset mais c'est peut être une solution dég
                br.read(strToken,0,5);
                break;

            default:
                System.err.println("Erreur sur le token");
                System.exit(1);
          }           
        
      }


      // Début de partie

      //réception du welcome du seveur

      // MULTI DIFFUSION
      
     
      
      // Déroulement de la partie


      while(true){// fin de la partie quand il n'y a plus de fantôme ou quand tous les joueurs inscrit ont quitté
        System.out.println("Vous êtes en pleine partie, que voulez vous faire ?");
        Scanner sc = new Scanner(System.in);
        if(sc.hasNext()){
          rep= sc.nextLine();
        }

        pw.print(rep);
        pw.flush();

        //récupération du token suite à la demande du client 
        br.read(strToken,0,5);
        System.out.println("Token du serveur :");
        System.out.println(strToken);
        //System.out.print("REACS : ");
        //System.out.println(reacS);

        switch(String.valueOf(strToken)){
            case "MOVE!":
                br.read(reacS,offset,15);
                //System.out.println(reacS);
                String x=String.valueOf(reacS).substring(1,4);
                String y=String.valueOf(reacS).substring(5,8);
                System.out.println("Vous êtes arrivé.e à la position ("+x+", "+y+" )");
                break;

            case "MOVEF":
                br.read(reacS,offset,15);
                String xf=String.valueOf(reacS).substring(1,4);
                String yf=String.valueOf(reacS).substring(6,9);
                String p=String.valueOf(reacS).substring(9,13);
                System.out.println("Vous êtes arrivé.e à la position ("+xf+", "+yf+") et vous avez eu un fantôme !\nBravo votre nombre de points s'élève maintenant à "+p);
                break;
            
            case "GLIS!":
                br.read(reacS,offset,11);
                int s=Integer.parseInt(String.valueOf(reacS[1]));
                //System.out.println("1er : "+ reacS[0]);
                char[] gplyr=new char[21];
                for(int i=0; i<s; i++){
                  //12345678 001 003 0003***  // 29
                    br.read(gplyr,offset,21);
                    br.read(reacS,offset,9);
                    String id=String.valueOf(gplyr).substring(0,8);
                    String xid=String.valueOf(gplyr).substring(9,12);
                    String yid=String.valueOf(gplyr).substring(13,16);
                    String pj=String.valueOf(gplyr).substring(17,21);

                    System.out.print("Le joueur "+id+ " est à la position ("+xid+ ";"+yid+ "), avec "+ pj+" points.\n");
                    
                }
                break;
            case "MALL!": 
                br.read(strToken,0,5);
                break;
            case "SEND!": 
            br.read(strToken,0,5);
                break;
            case "NSEND":
                System.out.println("Votre message ne peut pas etre envoye");
                br.read(strToken, 0, 5);
                break;
            case "DUNNO":
                System.out.println("requête impossible");
                br.read(strToken,offset,5);
                break;
        }

        if(String.valueOf(strToken).equals("GOBYE")){
            System.out.println("Bye bye vous avez bien été déconecté.e");
            break;
          
        }
      

    }
    pw.close();
    br.close();
    socket.close();

  } catch (Exception e) {
    System.out.println(e);
    e.printStackTrace();
  }
    
  
}
}