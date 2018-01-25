/* symperilipsi aparaititwn vivlio8ikwn */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#define MAX_READ_BYTES 256
#define PORT 9380        // default port 9000+(omada=39-1)*10 dld 9380

/* Dilwsi file descriptor pou epistrefetai ap tin klisi sistimatos socket */
int sockfd = -1;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/* h synartisi toDie xeirizetai ta signals SIGTERM kai SIGINT. Kaleitai otan plhktrologisoume Ctrl+C sto terminal tou server */
void toDie(int signal)
{
	// Katharo termatisma. Kleisimo tou socket.
	printf("Die!\n");
	close(sockfd);
	exit(0);
}

/* H synartisi parse_name() kaleitai otan o server lavei mhnyma me Service: res_name */
void parse_name(char *name, int newsockfd)
{
	struct hostent *hp;
	struct in_addr **addr_list;
	char reply_buf [ MAX_READ_BYTES];
	char **pAlias;
	int x = 0, i = 0;
	
	memset(reply_buf, 0, MAX_READ_BYTES);	// arxikopoiisi tou reply_buf me midenika

	printf("Hostname: %s\n", name);

	hp = gethostbyname(name);

	if (hp == NULL) { // 8a vre8oume se auto to if se periptwsi "leitourgikou sfalmatos"
		printf("%d %d %d %d\n", HOST_NOT_FOUND, TRY_AGAIN, NO_RECOVERY, NO_DATA);
		/* Xrhsimopoioume ti sinartisi hstrerror() gia na anaktisoume to minima la8ous vasi tis timis tou h_errno */
		sprintf(reply_buf, "gethostbyname() failed. Error message: %s\n", hstrerror(h_errno));
		write(newsockfd, reply_buf, strlen(reply_buf));
	} else { /* Epistrefoume ola ta sxetika stoixeia tou host, mesw tis domis hostent */
		/* official name of the host, alias list, host address type, length of address, list of addresses for the host */
		sprintf(reply_buf, "\nOfficial name: %s\n", hp->h_name);
		write(newsockfd, reply_buf, strlen(reply_buf));

		// alias list
		for (pAlias = hp->h_aliases; *pAlias != 0; pAlias++) {
			sprintf(reply_buf, "Alternate name #%d: %s\n", ++x, *pAlias);				
			write(newsockfd, reply_buf, strlen(reply_buf));
		}		

		// host address type, length of address
		switch (hp->h_addrtype) {
			case AF_INET:
				sprintf(reply_buf, "Address type: AF_INET\nAddress length: %d bytes\n", hp->h_length);
				write(newsockfd, reply_buf, strlen(reply_buf));
				break;
			case AF_INET6:
				sprintf(reply_buf, "Address type: AF_INET6\nAddress length: %d bytes\n", hp->h_length);
				write(newsockfd, reply_buf, strlen(reply_buf));
				break;
			default:
				sprintf(reply_buf, "Address type: %d\nAddress length: %d bytes\n", hp->h_addrtype, hp->h_length);
				write(newsockfd, reply_buf, strlen(reply_buf));
				break;
		}		

		// list of addresses for the host
		addr_list = (struct in_addr **)hp->h_addr_list;
		for ( i = 0; addr_list[i] != NULL; i++) {
			sprintf(reply_buf, "Address #%d:\t%s\n", i+1, inet_ntoa(*addr_list[i]));
			write(newsockfd, reply_buf, strlen(reply_buf));
		}	
	}
}

/* H synartisi parse_ip() kaleitai otan o server lavei mhnyma me Service: res_ip */
void  parse_ip(char *ip, int newsockfd)
{
	struct hostent *hp;
	struct in_addr **addr_list;
	char reply_buf [ MAX_READ_BYTES ];
	in_addr_t data;
	char **pAlias;
	int x = 0, i = 0;
		
	memset(reply_buf, 0, MAX_READ_BYTES);	// arxikopoiisi tou reply_buf me midenika

	printf("IP address: %s\n", ip);
	data = inet_addr(ip);

	hp = gethostbyaddr(&data, sizeof(data), AF_INET);

	if (hp == NULL) { // 8a vre8oume se auto to if se periptwsi "leitourgikou sfalmatos"
		printf("%d %d %d %d\n", HOST_NOT_FOUND, TRY_AGAIN, NO_RECOVERY, NO_DATA);
		sprintf(reply_buf, "gethostbyaddr() failed. Error message: %s\n", hstrerror(h_errno));
		write(newsockfd, reply_buf, strlen(reply_buf));
	} else { /* Epistrefoume ola ta sxetika stoixeia tou host, mesw tis domis hostent */
		/* official name of the host, alias list, host address type, length of address, list of addresses for the host */
		sprintf(reply_buf, "\nOfficial name: %s\n", hp->h_name);
		write(newsockfd, reply_buf, strlen(reply_buf));

		// alias list
		for (pAlias = hp->h_aliases; *pAlias != 0; pAlias++) {
			sprintf(reply_buf, "Alternate name #%d: %s\n", ++x, *pAlias);				
			write(newsockfd, reply_buf, strlen(reply_buf));
		}

		// host address type, length of address
		switch (hp->h_addrtype) {
			case AF_INET:
				sprintf(reply_buf, "Address type: AF_INET\nAddress length: %d bytes\n", hp->h_length);
				write(newsockfd, reply_buf, strlen(reply_buf));
				break;
			case AF_INET6:
				sprintf(reply_buf, "Address type: AF_INET6\nAddress length: %d bytes\n", hp->h_length);
				write(newsockfd, reply_buf, strlen(reply_buf));
				break;
			default:
				sprintf(reply_buf, "Address type: %d\nAddress length: %d bytes\n", hp->h_addrtype, hp->h_length);
				write(newsockfd, reply_buf, strlen(reply_buf));
				break;
		}	

		// list of addresses for the host
		addr_list = (struct in_addr **)hp->h_addr_list;
		for ( i = 0; addr_list[i] != NULL; i++) {
			sprintf(reply_buf, "Address #%d:\t%s\n", i+1, inet_ntoa(*addr_list[i]));
			write(newsockfd, reply_buf, strlen(reply_buf));
		}
	}
}

/* H synartisi parse_service() kaleitai otan o server lavei mhnyma me Service: disc_serv */
void parse_service(char *servname, int newsockfd)
{
	struct servent *sp_tcp, *sp_udp;
	char reply_buf [ MAX_READ_BYTES ];
	char **pAlias;
	int x = 0;
	
	memset(reply_buf, 0, MAX_READ_BYTES);	// arxikopoiisi tou reply_buf me midenika

	sp_udp = getservbyname(servname, "udp");	// servname using protocol "udp"
	sp_tcp = getservbyname(servname, "tcp");	// servname using protocol "tcp"	

	if (sp_tcp == NULL && sp_udp == NULL) { // 8a vre8oume se auto to if se periptwsi "leitourgikou sfalmatos"
		sprintf(reply_buf, "getservbyname() failed. Unknown application %s\n", servname);

		write(newsockfd, reply_buf, strlen(reply_buf));
	} else { /* Epistrefoume tis plirofories tou hostname sxetika me tin ipiresia, mesw tis domis servent */
		/* service name, port, protocols, alias list */
		if (sp_tcp != NULL) {
			sprintf(reply_buf, "\nService name: %s\nPort: %d\nProtocol(s): tcp", sp_tcp->s_name, ntohs(sp_tcp->s_port));
			if (sp_udp != NULL) 
				sprintf(reply_buf, "%s, udp\n", reply_buf);
			else
				sprintf(reply_buf, "%s\n", reply_buf);

		} else if (sp_udp != NULL) {
			sprintf(reply_buf, "\nService name: %s\nPort: %d\nProtocol(s): udp\n", sp_udp->s_name, ntohs(sp_udp->s_port));
			// eimaste edo mono an to sp_tcp einai null. den exei noima na xanakanoume elegxo
		}

		write(newsockfd, reply_buf, strlen(reply_buf));
		
		for (pAlias = sp_tcp->s_aliases; *pAlias != 0; pAlias++) {
			sprintf(reply_buf, "Alternate name #%d: %s\n", ++x, *pAlias);				
			write(newsockfd, reply_buf, strlen(reply_buf));
		}
	}
}

int main(int argc, char *argv[])
{
	/* Dilwsi file descriptor pou epistrefetai ap tin klisi sistimatos accept */
	int newsockfd = -1;
	int bd, portno;
	char *pch;
	char c;
	char *opt_value = NULL;

	struct sockaddr_in serv_addr, remote;	// Dilwsi diey8ynsewn tou server kai tou client antistoixa
	socklen_t clilen;			// Dilwsi mikous diey8ynsews tou client
	char buf[ MAX_READ_BYTES ];	// Dilwsi buffer opou apo8hkeyontai xarakthres gia xrisi stis synarthseis sistimatos read, write
	pid_t childpid;

    /* an o xristis den exei dosei kanena orisma dinoume ena default port stin metavliti portno */
	portno = PORT;
	while ((c = getopt (argc, argv, "p:")) != -1)
		switch (c) {
			case 'p':
				opt_value = optarg;
				/* an o xristis dosei orisma to metatrepoume se int mesw tis atoi() kai to pername sti metavliti portno */
				portno = atoi(opt_value);
				break;
			default:
				portno = PORT;
				break;
		}
	printf("Listening on %d\n", portno);
	
	/* Dhmiourgia tou end point tou server */ 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	printf("To socket dhmiourgh8hke epityxws!\n");

	/* Orisoume handler gia ta signals SIGTERM kai SIGINT */
	signal(SIGTERM, toDie);
        signal(SIGINT, toDie);	

	bzero((char *) &serv_addr, sizeof(serv_addr));		// arxikopoiisi tis serv_addr me midenika

	serv_addr.sin_family = AF_INET;		// IPv4 internet domain
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portno);	// arxikopoihsh tou port pou akouei casted se network byte order

	/* sindesi socket me to dosmeno port */
	bd = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	/* Elegxos sindesis socket me to dosmeno port kai ektiposi minimatos sfalmatos se periptwsi sfalmatos */
	if ( bd < 0)
	{
		perror("Bind failed:");
		close(sockfd);
		exit(1);
	}

	/* Dhmiourgia mias listas aitisewn gia tous clients me mikos 5. To socket akouei gia syndeseis */
	if (listen(sockfd, 5) < 0)
		error("Server listen failure");

	/* Atermonos vrogxos pou periexei ton kwdika me ton opoio ginetai i syndesi me ton client kai h exyphretisi tou */
	while( 1 ) {
		clilen = sizeof(remote);	// Ka8orismos mege8ous diey8ynsis tou client

		// Dexomaste mia syndesi. 
		// Antigrafi tis epomenis aitisis apo tin oura aitisewn sti metavliti newsockfd kai diagrafi tis apo tin oura.
		newsockfd = accept(sockfd, (struct sockaddr *) &remote, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");

		printf("Connected! New socket : %d\n", newsockfd);

		/* H diergasia-pateras kalei tin klisi systimatos fork() mesw tis opoias dimiourgeitai mia diergasia-paidi.Se ayti tin periptwsi 
		oi dyo diergasies exoyn ta idia antigrafa tou xwrou mnimis. Ektos apo tin timi tis metavlitis childpid. H diergasia-paidi pairnei 
		timi childpid=0. Me auto ton tropo mporoume na diaxwrisoume tis ergasies pou kanei i diergasia-paidi kai i diergasia-pateras. */

		childpid = fork();

		/*To parakatw kommati kwdika ekteleitai mono apo ti diergasia-paidi, i opoia exei childpid=0. Oysiastika oi diergasies paidia
		 exypiretoun tis aitiseis pou erxontai ston server enw i diergasia-pateras perimenei nees aitiseis */

		if ( childpid == 0 ) {	// child process 
			close(sockfd);	// close listening socket
			char reply_buf [ MAX_READ_BYTES ];

			while (1) {
				memset(buf, 0, MAX_READ_BYTES);		// arxikopoiisi tou buffer me midenika 
				if (read(newsockfd, buf, MAX_READ_BYTES) == 0) { // otan i read epistrefei 0 simainei oti ekleise to connection
					printf("closing\n");
					close(newsockfd);
					break;
				}
				
				// to telnet prosthetei \r \n sto telos kathe string
				// ksekiname apo to telos tou string kai oso exei \r \n ta antikathistoume me \0 (string terminate)
				int c = strlen(buf) - 1;

				while ( c > 0 && (buf[c] == '\r' || buf[c] == '\n') ) {
					buf[c] = '\0';
					c--;
				}
					
				if ( strlen(buf) == 1 && buf[0] == '\r') {
					// elegxoume an einai apla \r dld apla o client patise enter
					continue;// me continue epistrefoume kateutheian sti read
				}

				printf("%s ---> length: %zu\n", buf, strlen(buf));
				
				// Kanoume split to periexomeno tou buf vasi tou kenou, xrhsimopoiwntas tin strtok()
				// Prokyptoun 2 tokens. To 1o token einai to Service (res_name/res_ip/disc_serv)	
				pch = strtok(buf, " ");

				// Vriskoume poio Service dw8hke ap ton client xrisimopoiwntas tin strcmp() 
				// Pairnoume k to 2o token mesw tis strtok()
				// Kaloume tin antistoixi synartisi me orismata to 2o token kai to newsockfd
				if ( strcmp(pch, "res_name") == 0 ) { // Dw8hke to "res_name"
					pch = strtok(NULL, " ");
					if (pch != 0)
						parse_name(pch, newsockfd);
					else {
						sprintf(reply_buf, "You forgot to give a hostname. Try again!\n");
                                      		write(newsockfd, reply_buf, strlen(reply_buf));
					}	
				} else if ( strcmp(pch, "res_ip") == 0 ) { // Dw8hke to "res_ip"
					pch = strtok(NULL, " ");
					if (pch != 0)
						parse_ip(pch, newsockfd);
					else {
                                                sprintf(reply_buf, "You forgot to give an ip address. Try again!\n");
                                                write(newsockfd, reply_buf, strlen(reply_buf));
                                        } 
				} else if ( strcmp(pch, "disc_serv") == 0) { // Dw8hke to "disc_serv"
					pch = strtok(NULL, " ");
					if (pch != 0)
						parse_service(pch, newsockfd);
					else {
                                                sprintf(reply_buf, "You forgot to give a service name. Try again!\n");
                                                write(newsockfd, reply_buf, strlen(reply_buf));
                                        }
				} else if ( strcmp(pch, "exit") == 0) { // Dw8hke i entoli "exit". Diakopi syndesis.
					printf("closing\n");
					close(newsockfd);
					exit(0);
				} else { // Se periptwsi syntaktikou la8ous zhtame to swsto orismo twn parametrwn	
					printf("reply\n");
					sprintf(reply_buf, "Give the correct parameters!\n"); 
					write(newsockfd, reply_buf, strlen(reply_buf));
				}
			}
			close(newsockfd);
			exit(0);  
		} else {
			close(newsockfd);
		}
	}
	exit(0);
}
