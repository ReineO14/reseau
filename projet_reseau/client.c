//cd M1_MIAGE/Reseau/Projet/v3

/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

typedef struct {
	int sock;
	char* message_client;
}infosClient;

const char* pseudo = "";

void recevoir_message(int socket_serveur, char message[], int taille_message){
	read(socket_serveur, message, taille_message);	
}

char* ecriture_message() {
	char* msg = malloc(128);
	int i;
	for(i=0;i<strlen(msg);i++){
		msg[i]='\0';		
	}
	printf("Entrez votre message: \n");
	scanf("%s",msg);
	if (strstr(msg, "/exit" ) != NULL) {
		pthread_exit(NULL);
		return EXIT_SUCCESS;
	}
	//scanf("%99[^\n]",msg);
	// %99[^\n]
	return msg;
}


//Envoie un message sur la socket passee en parametre
void envoi_message(int socket_descriptor, char* mesg){
    if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
	perror("erreur : impossible d'ecrire le message destine au serveur.");
	exit(1);
    }
}


int str_istr (const char *cs, const char *ct)
{
   int index = -1;

   if (cs != NULL && ct != NULL)
   {
      char *ptr_pos = NULL;

      ptr_pos = strstr (cs, ct);
      if (ptr_pos != NULL)
      {
         index = ptr_pos - cs;
      }
   }
   return index;
}

//Lit un message depuis la socket passee en parametre
void lire_message(int socket_descriptor){
	char buffer [512];
	char *tmp;
	char *msg;
	int index;
	int longueur;
	 /* lecture de la reponse en provenance du serveur */
	if((longueur = read(socket_descriptor, buffer, 512)) > 0) {
		tmp = buffer;
		//index = str_istr(tmp, "$$");
		/*if (index > 0) {
			tmp = strstr(tmp, "$$");
		}*/
	}		
}



void* threadLecture(void * socket){	
	int socket_descriptor;
	socket_descriptor = (int) socket;

	while(1){
		lire_message(socket_descriptor);		
	}
}



void* threadEcriture(void *structure){
	infosClient* infos;
	infos  = (infosClient*) structure;
	char * 	message = malloc(512);
	//char * pseudoTmp = malloc(512);
	char * a_envoyer = malloc(512);

	int socket_descriptor;
	socket_descriptor = infos->sock;
	while(1) {
		message="";
		message = ecriture_message();
		if (strstr(message, "/exit" ) != NULL) {
			close(socket_descriptor);
			pthread_exit(NULL);
			return EXIT_SUCCESS;
		}
		a_envoyer = message;
		envoi_message(socket_descriptor,a_envoyer);
	}
}

//void recevoir_messge()


int main(int argc, char **argv) {
  
    int socket_descriptor, 	/* descripteur de socket */
		longueur; 		/* longueur d'un buffer utilisé */
    sockaddr_in adresse_locale; 	/* adresse de socket local */
    hostent *	ptr_host; 		/* info sur une machine hote */
    servent *	ptr_service; 		/* info sur service */
    char 	buffer[256];
    char *	prog; 			/* nom du programme */
    char *	host; 			/* nom de la machine distante */
    //char *	mesg; 			/* message envoyé */
	char *      tmppseudo = malloc(50);
     
	pthread_t thread1; 
	pthread_t thread2; 

     
   /* if (argc != 3) {
	perror("usage : client <adresse-serveur> <message-a-transmettre>");
	exit(1);
    }*/
   
    prog = argv[0];
    host = argv[1];
    //mesg = argv[2];

	//printf("Entrez un nom pour acceder au chat: ");
	//scanf("%s",tmppseudo);
	//pseudo = strcat(tmppseudo, " : ");

	//printf("nom de l'executable : %s \n", prog);
	//printf("adresse du serveur  : %s \n", host);
	//printf("message envoye      : %s \n", mesg);
    
    if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
    }
    
    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
    

    adresse_locale.sin_port = htons(5000);

    
    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));

	
    
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
    }
    
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
    }
    
	printf("connexion etablie avec le serveur. \n");
    
  //  printf("envoi d'un message au serveur. \n");

	infosClient * infosCl = malloc(sizeof(int)+512);	
	infosCl->sock = socket_descriptor;
      
    /* envoi du message vers le serveur */
    if (pthread_create(&thread1 , NULL , threadEcriture, (void *) infosCl) == 0){
		 printf("ecriture client\n");
		 //return EXIT_SUCCESS;
	}
	else {
		perror("erreur ecriture");	
	}

    
    /* mise en attente du prgramme pour simuler un delai de transmission */
 //   sleep(3);
     
	//printf("message envoye au serveur. \n");
                
    /* lecture de la reponse en provenance du serveur */
 	if (pthread_create(&thread2 , NULL , threadLecture,  (void *) socket_descriptor) == 0){
		 printf("lecture client\n");
		 //return EXIT_SUCCESS;
	}
	else {
		perror("erreur lecture");
	}
	
	
	while(1) {

	}
    
   /* printf("\nfin de la reception.\n");
    
    close(socket_descriptor);
    
    printf("connexion avec le serveur fermee, fin du programme.\n");*/
    
    exit(0);
    
}
