/*
 * client.cpp
 */

#include "common.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;


int main( int argc, char *argv[] ) {

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	socklen_t addr_len;

	// 	Pour executer l'application client: l'utilisateur devrait saisir trois arguments: l'adresse IP du serveur, le port sollicit� sur le serveur et le nom du fichier a transferer
	// Remarque: le fichier utilis� en argument doit etre dans le m�me dossier que l'executable
	//Remarque: argv[] est un tableau dans lequel sont stock�s les arguments saisis. Le nombre des arguments saisis est stock� dans la variable argc
	

	//Consigne1: Affichez un message d'erreur lorsque le nombre d'arguments est insuffisant, (indication: pensez a utiliser la valeur de argc)

	
	if(argc<4){
		cout<< "mauvais nombre d'argument" << endl;
	}

	//Consigne 2: Verifiez la longueur du nom du fichier a�envoyer (ceci se trouve dans argv[3]) 
	//et affichez un message d'erreur lorsque la longeur de son nom depasse la taille maximale FILENAME_MAX_SIZE. 
	//Pensez a� utiliser la fonction strlen() pour 
	if(strlen(argv[3])>FILENAME_MAX_SIZE){
		cout<<"Erreur taille nom de fichier trop long" <<endl;

	}

	//Consigne 3: Initialisation des attributs de la variable structure serv_addr (sin_family, sin_addr et sin_port)
    //Inspirez-vous du code que vous avez fait en prelab
	
	                      // Votre code pour la consigne 3
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	serv_addr.sin_port=htons(atoi(argv[2]));
	

	// Consigne 4: Creez un socket en utilisant la fonction socket()
	
	                      //Votre code  pour la consigne 4
	int client_socket = socket(AF_INET,SOCK_STREAM,0);

	//Consigne 5: Verfiez la valeur de la variable retournee par socket() et affichez un message d'erreur lorsque la creation du socket a echoue
	
		                                    //Votre code pour la consigne 5
	if(client_socket==0){
		cout<<"Erreur creation socket client" << endl;
	}
	if(client_socket==1){
		cout<<"Creation socket client okay" << endl;
	}

	//Consigne 6: recupererz la taille de serv_addr et stockez-l� dans la variable pour une utilisation ulterieure avec les fonctions send(). etc,
	//(pensez � utiliser sizeof()) 
	
	                                  //Votre code pour la consigne 6
	long long int taille_serv_addr = sizeof(serv_addr);
									  
	//=========================Ouverture de la connexion entre le client et le serveur========================
	
	// Consigne 7: connectez le socket client au serveur en utilisant la fonction connect()
	
	                                //Votre code pour la consigne 7
	int reponse_connect = connect(client_socket, (sockaddr*) &serv_addr, sizeof(serv_addr));
	
	
	//Consigne 8: Verifiez la valeur de la variable retournée par la finction connect() et affichez un message d'erreur lorsque la connexion au serveur échoue
	 
	                                //Votre code pour la consigne 8
	if(reponse_connect!=0){
		perror("Connexino echoue");
		close(client_socket);
		exit(EXIT_FAILURE);
	}
	cout<<"Connecte au serveur" << endl;
	
	 
	// Consigne 9: Ouvrez un ifstream binaire (fichier en lecture binaire) associ� au fichier saisi dans les arguments
	//Remarque: le nom fichier se trouve dans argv[3]
	
	                                   //Votre code pour la consigne 9
	ifstream fichier;

	fichier.open(argv[3], ios::binary);
	if(fichier.fail()){
		cout<<"Erreur ouverture fichier" << endl;
	}

	//===============================initialisation du transfert du fichier======================================
	//Etape 1: creation du paquet d'initialisation du transfert des donness (il s'agit d'un fichier dans notre cas)
	
	init_packet ipkt;
	memset(&ipkt, 0, sizeof(init_packet));
	// ipkt est une variable de type structure (voir sa definition dans common.h). 
	//ipkt est utilise dans le reste du code pour definir le paquet d'initialisation du transfert des donnees
		
	// Consigne 10: Initialisez l'entete du paquet d'initialisation du trasnfert des donness ipkt  par MSG_INIT 
	
	                                       //Votre code pour la consigne 10
	ipkt.msg=MSG_INIT;
	
	
	// Consigne 11: Initialisez le champ ipkt.filesize par la taille du fichier en utilisant la fonction getFileSize() definie dans common.h et common.cpp
	
	                                       //Votre code pour la consigne 11
	ipkt.filesize=sizeof(fichier);
										   
	// Initialisation du champ ipkt.filename par le nom du fichier en utilisant la fonction copy()
	       string(argv[3]).copy(ipkt.filename, strlen(argv[3]));
	
	//Etape 2: Envoi du paquet d'initialisation du transfert des donness (il s'agit d'un fichier dans notre cas)
	//Consigne 12: Utilisez la fonction write() pour en envoyer le paquet au serveur 
	//Remarque: write() sur le socket TCP est equivalent � la fonction send() et read() sur un socket TCP est equivalent a receive())
	
	                                        //Votre code pour la consigne 12
	int reponse_write = write(client_socket, &ipkt.msg ,sizeof(ipkt.msg)); // PAS BUFFER SIZE, mais filesize(...)
	
														 
// Consigne 13: Verifier la valeur de la variable retournee par write() 
//Affichez un message d'erreur dans le cas où on n'est pas capable d'écrire dans le socket (ce cas correspond à une valeur négative retournée par write())

                                             //Votre code pour la consigne 13
	if(reponse_write < 0){
		cout << "erreur write" << endl;
	};

	//==================================Reception de l'acceptation du transfert du fichier de la part du serveur											  
	// Consigne 14: Reception de l'acceptetion du transfert du fichier de la part du serveur. Pensez � utiliser la fonction recv() et le flag  MSG_WAITALL
	MESSAGE msg;
	                                  //Votre code pour la consigne 14
	recv(client_socket,&msg,sizeof(MESSAGE),MSG_WAITALL);
	// ssize_t recv(int socket, void *buffer, size_t length, int flags);
	
	//Consigne 15: Affichez un message d'erreur dans le cas msg  n'est pas MSG_ACCEPT puis fermez immediatement le socket ouvert 
	
	                                   //Votre code pour la consigne 15
	if(msg!=MSG_ACCEPT){
		cout << "Erreur acceptation du transfert de fichier " << endl;
	}
									   
	//===================================Transfert du fichier===============================================

	// la variable bytes_sent stocke le nombre de bytes deja envoyes
	long long int bytes_sent = 0;
	// la variable buffer est un tampon pour stocket les morceaux lus � partir du fichier original qu'on desire envoyer au serveur. Il a un taille maximale BUFFER_SIZE
	char buffer[BUFFER_SIZE];
	
	// Tant qu'on n'a pas envoye tous les bytes du fichier, on va faire le traitement suivant
	while (bytes_sent != ipkt.filesize)
	// Tant qu'on n'a pas envoye tous les bytes du fichier, on va faire le traitement suivant
	{
		// Consigne 16: Lecture du fichier: la lecture se fait par fragment de taille BUFFER_SIZE en utilisant la fonction read() sur le ifstream 
		//et mettez cette quantit� de donn�es dans buffer
		
		
		                                   //Votre code pour la consigne 16
		fichier.read(buffer, BUFFER_SIZE);
		n = fichier.gcount();	
		
		
		// Consigne 17: On ecrit les bytes lus :a partir du fihier ifstream dans le stream TCP (ceci est equivalent a les envoyer)
		//===Attention: vous devez distinguer deux cas: 1) le nombre de bytes retants < BUFFER_SIZE et 2)le nombre de bytes retants >= BUFFER_SIZE. 
		//Adaptez les contenu de vos variables selon le cas
		// Indication: Le nombre de bytes est plus petit que la taille du tampon si on a atteint la fin du fichier au cours d'une lecture 
		//d'une portion du fichier de taille BUFFER_SIZE, Pensez � utiliser la fonction eof() 
		
                                  //Votre code pour la consigne 17
		if(n<BUFFER_SIZE){
			write(client_socket, &buffer, n);
			if(fichier.eof()){
				cout << "fin de transmission" << endl;	
			}
			else
				cout << "erreur lecture" << endl;
		}
		if(n=BUFFER_SIZE){
			write(client_socket, &buffer, BUFFER_SIZE);
		}

		// compteur de bytes envoyes est increment� par le nombre de bytes envoy�s
		bytes_sent += n;
	}
//==============================Fermeture de la connexion entre le client et le serveur=================================
	// Consigne 18: Le client envoie le message MSG_END pour fermer la connexion en utilisant la fonction write() sur le socket
	//Affichez un message d'erreur dans si la valeur retournee par write() est negative
	msg = MSG_END;

	                               //Votre code pour la consigne 18
	if(0>write(client_socket, &msg, sizeof(msg) )){
		cout << "erreur msg_end" << endl;
	}

	// Consigne 19: Fermez le ifstream et le socket en utilisant close
	
	                              //Votre code pour la consigne 19
	close(client_socket);
	fichier.close();

	return 0;
}