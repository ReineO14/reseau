/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h> 	/* pour les sockets */
#include <sys/socket.h>
#include <netdb.h> 		/* pour hostent, servent */
#include <string.h> 		/* pour bcopy, ... */  
#define TAILLE_MAX_NOM 256
#define NOMBRE_MAX_CO 4

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;


const char* message_a_renvoyer = "";
int sockets[NOMBRE_MAX_CO];
int nbConnexion=0;

typedef struct structUtilisateur {
	
	char* pseudo;
	char* extension;
	
}Utilisateur;

typedef struct structListUtilisateur {

//	int nbUtilisateur;
	
}ListeUtilisateurs;

typedef struct {
	int sock;
	int *ptrTab;
	int nbCo;
} infosThread ;


/*------------------------------------------------------*/


void renvoi (int sock) {

    char buffer[256];
    int longueur;
   
    if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0) 
    	return;
    
    printf("message lu : %s \n", buffer);
    
    buffer[0] = 'R';
    buffer[1] = 'E';
    buffer[longueur] = '#';
    buffer[longueur+1] ='\0';
    
    //printf("message apres traitement : %s \n", buffer);
    
    //printf("renvoi du message traite.\n");

    /* mise en attente du prgramme pour simuler un delai de transmission */
    //sleep(3);
    
    write(sock,buffer,strlen(buffer)+1);
    
    //printf("message envoye. \n");
        
    return;
    
}



char* traiter_message(char* message){					
	char* nouveau_message;
	nouveau_message = (char *)malloc(sizeof(message));	
	return nouveau_message;
}

char* lecture_message(int socket) {

	int longueur;
	char* buffer = malloc(4096);
	char* retour = malloc(4096);

	if ((longueur = read(socket, buffer, 4096)) <= 0){
		printf("Erreur de longueur : %d\n",longueur);
    		return;
	} 
	//buffer = traiter_message(buffer);
	return buffer;
}

void* threadLecture(int *socket) {
	int socket_descriptor;
	socket_descriptor = (int) socket;
	int longueur;
	int i ;
	char mssg[4096];
	while(1){
		message_a_renvoyer = lecture_message(socket_descriptor);
		if (strstr(message_a_renvoyer, "/nb" ) != NULL ) {
			printf("Le client %d demande le nombre de personnes\n",socket_descriptor);
			message_a_renvoyer=mssg;
			if (strstr(message_a_renvoyer, "/exit" ) != NULL) {
				pthread_exit(NULL);
				return EXIT_SUCCESS;
			}						
		}
		printf("Client %d reception d'un message : %s\n",socket_descriptor,message_a_renvoyer);
	}
}

void repondre (int sock,const char* retour) {
	write(sock,retour,strlen(retour)+1);
}



void* threadEcriture(int *structure) {
	infosThread* infos;
	int i = 0;	
	int nouv_socket_descriptor;
	int sockets[NOMBRE_MAX_CO];
	char* rep = malloc(4096);
	infos  = (infosThread*) structure;		
	
	while(1){
		//Si il y a un message, on l'envoie a tous les connectes
		if (strcmp(message_a_renvoyer,"") != 0){
	
			for (i=0; i< nbConnexion; i++){
				if(sockets[i] != 0){
					repondre(sockets[i], message_a_renvoyer);
				}
			}	
			message_a_renvoyer = "";
		}
	}
}


int accepterCo(int socket_descriptor){
	int 	nouv_socket_descriptor;
	int longueur_adresse_courante;
	sockaddr_in 	adresse_client_courant; 	//adresse client courant 

	longueur_adresse_courante = sizeof(adresse_client_courant);
	
	// adresse_client_courant sera renseigne par accept via les infos du connect 
	if ((nouv_socket_descriptor = 
		accept(socket_descriptor, 
		       (sockaddr*)(&adresse_client_courant),
		       &longueur_adresse_courante))
		 < 0) {
		perror("erreur : impossible d'accepter la connexion avec le client.");
		exit(1);
	}
	printf("client connecte sur le socket %d depuis %s:%d\n"
		,nouv_socket_descriptor
		,(char *)inet_ntoa(adresse_client_courant.sin_addr)
		,htons(adresse_client_courant.sin_port));	
	

	return nouv_socket_descriptor;
}


/*------------------------------------------------------*/
main(int argc, char **argv) {
  
	int 		socket_descriptor, 		/* descripteur de socket */
			nouv_socket_descriptor, 	/* [nouveau] descripteur de socket */
			longueur_adresse_courante; 	/* longueur d'adresse courante d'un client */
	sockaddr_in 	adresse_locale, 		/* structure d'adresse locale*/
			adresse_client_courant; 	/* adresse client courant */
	hostent*		ptr_hote; 			/* les infos recuperees sur la machine hote */
	servent*		ptr_service; 			/* les infos recuperees sur le service de la machine */
	char 		machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */
	infosThread * infos = malloc(sizeof(int)+sizeof(int)+4096);
    
	gethostname(machine,TAILLE_MAX_NOM);		/* recuperation du nom de la machine */

	int i;
    
    	pthread_t thread_lecture; 
	pthread_t thread_ecriture;

	char message_arrive[256];
    
    /* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
    }
    
    /* initialisation de la structure adresse_locale avec les infos recuperees */			
    
    /* copie de ptr_hote vers adresse_locale */
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family		= ptr_hote->h_addrtype; 	/* ou AF_INET */
    adresse_locale.sin_addr.s_addr	= INADDR_ANY; 			/* ou AF_INET */


    adresse_locale.sin_port = htons(5000);

    
    printf("numero de port pour la connexion au serveur : %d \n", 
		   ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);
    
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le client.");
		exit(1);
    }

    /* association du socket socket_descriptor à la structure d'adresse adresse_locale */
    if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de lier la socket a l'adresse de connexion.");
		exit(1);
    }
    
    /* initialisation de la file d'ecoute */
    listen(socket_descriptor,5);


	if (pthread_create(&thread_ecriture, NULL, threadEcriture, (void*)infos)  ==  0){
				printf("thread ecriture lance\n");
	} else printf("erreur \n");

    /* attente des connexions et traitement des donnees recues */
	while(1) {
	//for(;;) {
    
		longueur_adresse_courante = sizeof(adresse_client_courant);
		
		// adresse_client_courant sera renseigné par accept via les infos du connect 
		if ((nouv_socket_descriptor = 
			accept(socket_descriptor, 
			       (sockaddr*)(&adresse_client_courant),
			       &longueur_adresse_courante))
			 < 0) {
			perror("erreur : impossible d'accepter la connexion avec le client.");
			exit(1);
		}


			int attribue = 0;
			//on regarde si il n'y a pas deja une case libre
		for (i=0; i< NOMBRE_MAX_CO; i++){		
			if (sockets[i] == 0)	{
				sockets[i]=nouv_socket_descriptor;
				attribue = 1;
				nbConnexion++;
				break;
			}		
		}		
			//si aucune case n'est libre, on ajoute a la fin
		if (attribue == 0) {
			sockets[nbConnexion]=nouv_socket_descriptor;
			nbConnexion++;
		}
		
		//traitement du message 
		printf("reception d'un message.\n");
		//printf("Numero socket %d",nouv_socket_descriptor);
		printf("Le client %d est connecte\n",socket_descriptor);
		
		renvoi(nouv_socket_descriptor);
		
		printf("apres renvoi.\n");
		
		if (pthread_create(&thread_lecture, NULL, threadLecture, (void *)nouv_socket_descriptor)  ==  0){
			printf("thread lecture \n");
		} else printf("erreur \n");

		sprintf(message_arrive,"Une nouvelle personne entre sur le chat\n %d personne(s) dans le chat\n",nbConnexion);
		
		for (i=0; i< nbConnexion; i++){
				if(sockets[i] != 0){
			repondre(sockets[i],message_arrive);
			}
		}				
		//close(nouv_socket_descriptor);
		
    }
    
}
