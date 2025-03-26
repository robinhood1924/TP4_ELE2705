/*
 * main.cpp
 */



#include "common.h"
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;


int main( int argc, char *argv[] ) {

	int sockfd, newsockfd, portno;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t addr_len;
	int n;
    //Pour executer l'application serveur, l'utilisateur devrait saisir un seul argument qui est le num�ro de port sollicit� sur le serveur
	// Remarque: le fichier re�u de la part du client se trouvera dans le m�me dossier que l'executable Server
	//Remarque: argv[] est un tableau dans lequel sont stock�s les arguments saisis. Le nombre des arguments saisis est stock� dans la variable argc
	//Consigne 1: Afin de guider l'utilisateur quant au nombre d'arguments requis, affichez un message d'erreur si le nombre d'arguments saisis est incorrect
	
	if (argc < 2) {
		perror("Mauvais nombre d'argument");
		exit(EXIT_FAILURE);
	}

	
	// Consigne 2: Convertissez la chaine de caractere argv[1] (le numero de port sollicit� sur le serveur) en un entier, pensez � utiliser la fonction atoi()

	portno = atoi(argv[1]);
										 
	// Consigne 3: Initialisation des diff�rents atrributs sin_family, sin_addr.s_addr et sin_port de la variable serv_addr de type structure. Inspirez-vous de votre code de prelab
	memset(&serv_addr, 0, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

    addr_len = sizeof(serv_addr);
	// Consigne 4: Creez le socket en utilisant la fonction socket()
	
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
								  
    // Consigne 5: Verifiez la variable retournee par la fonction socket() et affichez le message d'erreur correspondant, 
	//Indication: vous pouvez utiliser la fonction perror() 

	if (server_socket < 0) {
		perror("Erreur lors de la cr�ation du socket");
		exit(EXIT_FAILURE);
	}

	// Consigne 6: On lie le socket creer � la structure definie au d�but du code.�Pensez a utiliser la fonction bind()
	
	int bind_resultat = bind(server_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	//Consigne 7: Affichez un message d'erreur lorsque le bind ne reussit pas
	
	if (bind_resultat < 0) {
		perror("Erreur lors du bind");
		close(server_socket);  
		exit(EXIT_FAILURE);
	}

	// Consigne 8: Mettez le serveur en �coute en utilisant la fonction listen()
	
	int ecoute = listen(server_socket, 5);

	// Consigne 9: Le serveur devrait accepter la connexion du client au serveur
	//Attention: la fonction accept() cree un nouveau File descriptor (FD) qui sera utilis� dans la suite du code pour d�signer le socket du serveur
	//socklen_t cli_len = sizeof(cli_addr);
	socklen_t cli_len = sizeof(cli_addr);
	newsockfd = accept(server_socket, (struct sockaddr *)&cli_addr, &cli_len);
	
	 // Consigne 10: Verifiez la valeur du nouveau FD retourn�e par la fonction accept() 
	 //et affichez un message d'erreur dans le cas ou le serveur refuse la connexion du client
	 
	if (newsockfd < 0) {

		perror("Le serveur � refus� la connexion");
		exit(EXIT_FAILURE);
	}

	else {
		cout << "Connexion accept�e" << endl;
	}

	// Consigne 11: Le serveur re�oit le paquet d'initialisation du transfert de fichier, 
	//Vous pouvez utiliser la fonction recv() avec le flag MSG_WAITALL 

	init_packet ipkt;

	int bytes_recv = recv(newsockfd, &ipkt, sizeof(init_packet), MSG_WAITALL);
	
	
	// Consigne 12: Affichez un message d'erreur dans le cas o� le paquet re�u ne correspoend pas � un paquet d'initialisation
	// Indication: vous pouvez simplement v�rifier si l'ent�te du paquet re�u ipkt.msg est diff�rente au MSG_INIT
	//Dans le cas d'erreur fermez le socket (le FD r�sultant de l'excustion de la focntion par accept())
	
	if (ipkt.msg != MSG_INIT) {
		perror("Le paquet re�u n'est pas un paquet d'initialisation");
		exit(EXIT_FAILURE);
	}
	close(newsockfd);

	// un ofstream binaire ouvert en mode out ervira pour l'ecriture des octets re�us par chaque fragment
	
	
	ofstream ofs(string(ipkt.filename).c_str(), ios::out | ios::binary);
	
	
	// Consigne 13: Apr�s la reception du paquet d'initialisation du transfert des du fichier, le serveur accepte le transfert de fichier en renvoyant un 
	//message MSG_ACCEPT au client. Pensez � utiliser la fonction write() sur le nouveau socket cree par accept()
	//Remarque: write() sur un socket est �quivalent � l'envoi des donn�es
	
	 MESSAGE msg = MSG_ACCEPT;
	
	 int bytes_sent = write(newsockfd, &msg, sizeof(msg));

	 if (bytes_sent < 0) {
		 perror("Erreur lors de l'envoi de MSG_ACCEPT");
		 close(newsockfd);
		 exit(EXIT_FAILURE);
	 }
	
    // bytes_received est une variable pour stocker le nombre de bytes recus par le serveur 
	long long int bytes_received = 0;
	// buffer est une memoire tampon pour stocker le fragment de donn�es re�u
	char buffer[BUFFER_SIZE];
	
	while (	bytes_received != ipkt.filesize)
	{
		// On lit le stream TCP: 
		
		
		
		//Vous devriez distinguez deux cas:
		if( BUFFER_SIZE> ipkt.filesize - bytes_received) {
         //Consigne 14: Cas 1: le nombre de bytes restants est inf�rieur � BUFFER_SIZE 
		 //Vous pouvez utiliser la fonction read() sur le socket du serveur. read() sur un socket est equivalent � la reception des donnees
		
			long long int bytes_restant = ipkt.filesize - bytes_received;
			n = read(newsockfd, buffer, bytes_restant);
			if (n < 0) {
				perror("Erreur lors de la r�ception des donn�es (cas 1)");
				close(newsockfd);
				ofs.close();
				exit(EXIT_FAILURE);
			}
			ofs.write(buffer, n);
		}
					
			
		else {
         //Consigne 15: Cas 2: le nombre de bytes restants est inf�rieur � BUFFER_SIZE 
		 //Vous pouvez utiliser la fonction read() sur le socket su serveur
		 
			n = read(newsockfd, buffer, BUFFER_SIZE);
			if (n < 0) {
				perror("Erreur lors de la r�ception des donn�es (cas 2)");
				close(newsockfd);
				ofs.close();
				exit(EXIT_FAILURE);
			}
			ofs.write(buffer, n);
		}
		                
		// Consigne 16: �crivez les bytes re�us dans le fichier binare ofstream en utilisant ofs.write()
		
		ofs.write(buffer, n);
		
		// Le compteur du nombre de bytes re�u est incr�ment� par le nombre de bytes re�us. 
		//Pensez � mettre � jour la valeur de la variable n dans chacun des cas �tudi� ci-haut
		 bytes_received += n;
	}

	// Consign 17: Le serveur re�oit le paquet qui indique la fin du transfert des donnees. Vous pouvez utiliser la fonction recv() avec le flag MSG_WAITALL 
	
	MESSAGE fin_msg;
	int fin_recv = recv(newsockfd, &fin_msg, sizeof(MESSAGE), MSG_WAITALL);

	if (fin_recv <= 0) {
		perror("Erreur lors de la r�ception du MSG_END");
		close(newsockfd);
		ofs.close();
		exit(EXIT_FAILURE);
	}
								 
	//Consigne 18: Affichez un message d'erreur si l'entete du paquet re�u est diff�rente MSG_END  
	
	if (fin_msg != MSG_END) {
		perror("Le message re�u n'est pas MSG_END");
		close(newsockfd);
		ofs.close();
		exit(EXIT_FAILURE);
	}

	// Consigne 19: Fermez le ofstream et le socket
	                    
	cout << "Closing socket..." << endl;

	ofs.close();         
	close(newsockfd);   
	close(server_socket);
	cout << "Socket ferm�, serveur termin�." << endl;
	return 0;
}


