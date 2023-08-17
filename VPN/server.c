/* ===================== */
/*      HEADER FILES     */
/* ===================== */
#include <stdlib.h>
#include <fcntl.h>		/* O_RDWR		*/
#include <linux/if.h>		/* ifr			*/
#include <linux/if_tun.h>      /* IFF_TUN		*/
#include <sys/ioctl.h>         /* ioctl(), TUNSETIFF  */
#include <arpa/inet.h>         /* inet_addr		*/
#include <openssl/ssl.h> 	/* ssl			*/
#include <unistd.h>            /* close, read, access */
#include <sys/select.h>	/* select 		*/
#include <string.h>		/* strstr 		*/
#include <signal.h>		/* SIGINT 		*/
#include <stdio.h>		/* printf 		*/  
#include <ctype.h>		/* isalnum 		*/

/* ===================== */
/*      DEFINITIONS      */
/* ===================== */
#define VNIC_NAME "tun0"
#define PORT 1688
#define MTU 1400
#define BUFFER_SIZE 1500
#define MAX_LINE_LENGTH 4096
#define MIN_PORT 1024
#define MAX_PORT 65535
#define CMD_LINE_LENGTH 1024

/*** COMPILE WITH -lssl -lcrypto IN THE END ***/
/********* RUN USING ROOT *********/

static volatile int keep_running = 1;
int port = 0;
char interface[16] = {'\0'};
char server_crt[MAX_LINE_LENGTH] = {'\0'};
char server_key[MAX_LINE_LENGTH] = {'\0'};


/* ===================== */
/*    UTILITY FUNCTIONS  */
/* ===================== */
/*		
 * Function:  max 
 * --------------------
 *  returns the maximum value between two integers (x, y)
 */
int max(int x, int y)
{
   	return x > y ? x : y;
}


/* ============================ */
/*    CONFIGURATION FUNCTIONS   */
/* ============================ */
/*		
 * Function:  ValidateAndAssignPort 
 * --------------------
 *  validates and assigns the port value extracted from the configuration file
 *
 *  value:		port value to validate and assign
 *
 *  returns:	      	0 if successful, -1 if an error occurred
 */
int ValidateAndAssignPort(int value)
{ 
	if(value <= MIN_PORT || value > MAX_PORT)
	{
		printf("Error: Invalid PORT. Port should be in the range 1024-65535.\n");
		return -1;
	}
	
	port = value;
	return 0;
}


/*		
 * Function:  ValidateAndAssignInterface 
 * --------------------
 *  validates and assigns the interface value extracted from the configuration file
 *
 *  value:		port value to validate and assign
 *
 *  returns:	      	0 if successful, -1 if an error occurred
 */
int ValidateAndAssignInterface(char* value)
{ 
	int i = 0;
	int length = strlen(value);
    
    	if(length < 1 || length > 15) 
    	{
    		printf("Error: Invalid interface. Interface length should be between 1 and 15 characters.\n");
        	return -1;
    	}
    
    	for(i = 0; i < length; i++) 
    	{
        	if (!isalnum(value[i]) && value[i] != '_') 
        	{
            		printf("Error: Invalid character in interface. Only alphanumeric characters and underscores are allowed.\n");
            return -1; 
        	}
    	}
    
    	strcpy(interface, value);
	return 0;
}


/*		
 * Function:  ValidateAndAssignServerCrt 
 * --------------------
 *  validates and assigns the server.crt path value extracted from the configuration file
 *
 *  value:            	server_crt path value to validate and assign
 *
 *  returns:		0 if successful, -1 if an error occurred
 */
int ValidateAndAssignServerCrt(char* value)
{ 
	if(access(value, F_OK) == -1) 
	{
		printf("Error: SERVER_CRT file not found or inaccessible at the specified path.\n");
		return -1;
    	} 
        
        strcpy(server_crt, value);
        return 0;
}


/*		
 * Function:  ValidateAndAssignServerKey 
 * --------------------
 *  validates and assigns the server.key path value extracted from the configuration file
 *
 *  value:            	server_key path value to validate and assign
 *
 *  returns:		0 if successful, -1 if an error occurred
 */
int ValidateAndAssignServerKey(char* value)
{ 
	if(access(value, F_OK) == -1) 
	{
		printf("Error: SERVER_KEY file not found or inaccessible at the specified path.\n");
		return -1;
    	} 
        
        strcpy(server_key, value);

        return 0;
}


/*		
 * Function:  ParseConfigFile 
 * --------------------
 *  parses the client configuration file to extract key-value pairs
 *  representing server host and port information
 *
 *  config_file:    	pointer to the opened client configuration file
 *
 *  returns:	       0 if successful, -1 if an error occurred
 */
int ParseConfigFile(FILE *config_file)
{
	char line[MAX_LINE_LENGTH];
	char *key;
	char *value;
	
	while(fgets(line, sizeof(line), config_file))
	{
		key = strtok(line, "=");
		value = strtok(NULL, "\n");

		if(0 == strcmp(key, "PORT"))
		{
			if(-1 == ValidateAndAssignPort(atoi(value)))
			{
				return -1;
			}
		}
		else if(0 == strcmp(key, "INTERFACE"))
		{
			if(-1 == ValidateAndAssignInterface(value))
			{
				return -1;
			}
		}
		else if(0 == strcmp(key, "SERVER_CRT"))
		{
			if(-1 == ValidateAndAssignServerCrt(value))
			{
				return -1;
			}
		}
		else if(0 == strcmp(key, "SERVER_KEY"))
		{
			if(-1 == ValidateAndAssignServerKey(value))
			{
				return -1;
			}
		}
		else
		{
			printf("Error: Invalid configuration in 'client_config_file.txt'.\n");
			return -1;
		}
	}
	
	return 0;
}


/*		
 * Function:  GetConfiguration 
 * --------------------
 *  reads and parses a client configuration file to retrieve port and interface information 
 *
 *  returns:	 0 if successful, -1 if an error occurred or the configuration is invalid
 */
int GetConfiguration()
{
	FILE *config_file = NULL;
	
	config_file = fopen("server_config_file.txt", "r");
	if(NULL == config_file)
	{
		printf("Error: Unable to open 'server_config_file.txt'. Ensure the file exists and has the appropriate permissions.\n");
		return -1;
	}
	
	if(-1 == ParseConfigFile(config_file))
	{
		fclose(config_file);
		return -1;
	}
	
	fclose(config_file);
	return 0;
}


/* ========================== */
/*    NETWORK SETUP FUNCTIONS */
/* ========================== */
/*		
 * Function:  SetUpVictualNIC 
 * --------------------
 *  initializes a virtual network interface (TUN device) with the specified name
 *
 *  vnic_name:	the name of the virtual network interface to be created
 *
 *  returns: 	the file descriptor associated with the TUN device if successful,
 *              or -1 if an error occurred during setup
 */
int SetUpVictualNIC(char *vnic_name)
{
	struct ifreq ifr;
	int fd = 0;
	char cmd[CMD_LINE_LENGTH];

	system("ip tuntap add mode tun tun0");

	fd = open("/dev/net/tun", O_RDWR);
	if (-1 == fd)
	{
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI; 
	strncpy(ifr.ifr_name, vnic_name, IFNAMSIZ);

	if (-1 == ioctl(fd, TUNSETIFF, (void *)&ifr))
	{
		close(fd);
		return -1;
    	}
    
	system("ip link set dev tun0 up");
	snprintf(cmd, sizeof(cmd), "ifconfig tun0 10.8.0.1/24 mtu %d up", MTU);
	system(cmd);
	
	return fd;
}


/*		
 * Function:  SetUpTCPSocketWithTLS 
 * --------------------
 *  sets up a TCP socket and initializes an SSL/TLS context for secure communication
 *  
 *  this function creates a socket, binds it to an IP address and port, and sets up
 *  an SSL/TLS context with the server's certificate and private key
 *
 *  ctx:	a pointer to a pointer for storing the SSL/TLS context
 *  ssl:	a pointer to a pointer for storing the SSL/TLS session
 *
 *  returns:	the socket file descriptor if successful, or -1 if an error occurred
 */
int SetUpTCPSocketWithTLS(SSL_CTX **ctx, SSL **ssl)
{
	int sockfd = 0;
	struct sockaddr_in server_addr;

	*ctx = SSL_CTX_new(TLS_server_method());
	if(SSL_CTX_use_certificate_file(*ctx, server_crt, SSL_FILETYPE_PEM) != 1)
	{
		printf("Error: Failed to use the provided certificate file.\n");
		return -1;
	}
	if(SSL_CTX_use_PrivateKey_file(*ctx, server_key, SSL_FILETYPE_PEM) != 1)
	{
		printf("Error: Failed to use the provided key file.\n");
		return -1;
	} 

	if((sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
	{
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		return -1;
	}

	if(listen(sockfd, 1) < 0)
	{
		return -1;
	}

	return sockfd;
}


/*		
 * Function:  CreateConnection 
 * --------------------
 *  accepts an incoming client connection, sets up an SSL/TLS session, 
 *  and performs SSL/TLS handshake with the client
 *
 *  socket_fd:  a pointer to the socket file descriptor for the listening socket
 *  ctx:        a pointer to the SSL/TLS context
 *  ssl:        a pointer to a pointer for storing the SSL/TLS session
 *
 *  returns:    the socket file descriptor for the established client connection
 *              if successful, or -1 if an error occurred during connection
 */
int CreateConnection(int *socket_fd, SSL_CTX **ctx, SSL **ssl)
{
	int conn_fd = 0;
	int result = 0;
	socklen_t len;
	struct sockaddr_in client_addr;

	len = sizeof(client_addr);

	conn_fd = accept(*socket_fd, (struct sockaddr *)&client_addr, &len);
	if(-1 == conn_fd)
	{
		return -1;
	}

	*ssl = SSL_new(*ctx);
	SSL_set_fd(*ssl, conn_fd);

	if(SSL_accept(*ssl) != 1)
	{
		printf("Error: SSL handshake failed with the client.\n");
		return -1;
	}

	printf("Client successfully connected.\n");
	return conn_fd;
}


/*		
 * Function:  RouteTraffic 
 * --------------------
 *  configures network routing to enable IP forwarding
 *  and sets up NAT rules to masquerade outgoing traffic
 *
 *  returns:    no return value
 */
void RouteTraffic()
{
	char cmd[CMD_LINE_LENGTH];
		
	system("sysctl -w net.ipv4.ip_forward=1");				/* enable IP forwarding */
	snprintf(cmd, sizeof(cmd), "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE", interface);
	system(cmd);								/* masquerade outgoing traffic */
}


/*		
 * Function:  HandleTrafficFromClient 
 * --------------------
 *  reads data from an SSL connection and writes it to a virtual NIC
 *
 *  virtual_nic_fd:   file descriptor of the virtual NIC
 *  ssl:              pointer to the SSL connection
 *
 *  returns:          0 on success, -1 on error
 */
int HandleTrafficFromClient(int virtual_nic_fd, SSL* ssl)
{
	int read_result = 0;
	char buffer[BUFFER_SIZE];

	read_result = SSL_read(ssl, buffer, sizeof(buffer));
	if(0 >= read_result)
	{
		return -1;
	}
    
	if(-1 == write(virtual_nic_fd, (const void *)buffer, read_result))
	{
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));
	return 0;
}


/*		
 * Function:  HandleTrafficToClient 
 * --------------------
 *  reads data from a virtual NIC and writes it to an SSL connection
 *
 *  virtual_nic_fd:   file descriptor of the virtual NIC
 *  ssl:              pointer to the SSL connection
 *
 *  returns:          0 on success, -1 on error
 */
int HandleTrafficToClient(int virtual_nic_fd, SSL *ssl)
{
	int read_result = 0;
	char buffer[BUFFER_SIZE];

	read_result = read(virtual_nic_fd, buffer, sizeof(buffer));
	if(-1 == read_result)
	{
		return -1;
	}
    
	if(0 >= SSL_write(ssl, buffer, read_result))
	{
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));
	return 0;
}


/*		
 * Function:  ClearRoutingTable 
 * --------------------
 *  clears the routing table by removing the NAT rule that masquerades outgoing traffic
 *
 *  returns:  no return value
 */
void ClearRoutingTable()
{
	system("iptables -t nat -D POSTROUTING -o ens33 -j MASQUERADE"); 
}


/*		
 * Function:  RemoveVirtualNic 
 * --------------------
 *  removes the virtual network interface (TUN device) from the system
 *
 *  returns:  no return value
 */
void RemoveVirtualNic()
{
	system("ip link delete tun0");
}


/*		
 * Function:  CleanUp 
 * --------------------
 *  closes the network connections, SSL context, and releases resources
 *
 *  virtual_nic_fd:  file descriptor of the virtual network interface (TUN device)
 *  socket_fd:       file descriptor of the TCP socket
 *  conn_fd:         file descriptor of the client connection
 *  ctx:             SSL context
 *  ssl:             SSL session
 *
 *  returns:	      no return value
 */
void CleanUp(int virtual_nic_fd, int socket_fd, int conn_fd, SSL_CTX *ctx, SSL *ssl)
{
	close(virtual_nic_fd);
	close(socket_fd);
	close(conn_fd);
	SSL_free(ssl);
	SSL_CTX_free(ctx); 
	RemoveVirtualNic();
	ClearRoutingTable();
}


/*		
 * Function:  HandleCtrlC 
 * --------------------
 *  handles the Ctrl+C signal by updating the keep_running flag to terminate the program
 *
 *  dummy:	unused parameter (required by signal handler)
 *
 *  Returns:	no return value
 */
void HandleCtrlC(int dummy)
{
	keep_running = 0;
}


/*		 
 * Function:  main 
 * --------------------
 *  the entry point of the VPN server application. sets up the virtual NIC, TCP socket,
 *  and SSL context, and then enters a loop to handle incoming and outgoing traffic
 */
int main()
{
	int virtual_nic_fd = -1;
	int socket_fd = -1;
	int conn_fd = -1;
	int maxfdp = 0;
	fd_set read_fds;
	SSL_CTX *ctx;
	SSL *ssl;
	
	if(-1 == GetConfiguration())
	{
		return -1;
	}
	
	/* set up the virtual network interface (TUN device) */
	virtual_nic_fd = SetUpVictualNIC(VNIC_NAME);
	if (-1 == virtual_nic_fd)
	{
		return -1;
	}

	/* set up the Ctrl+C signal handler */ 
	signal(SIGINT, HandleCtrlC);
	
	/* route traffic using IP tables */
	RouteTraffic();
    
    	/* set up the TCP socket with TLS/SSL */
	socket_fd = SetUpTCPSocketWithTLS(&ctx, &ssl);
	if (-1 == socket_fd)
	{
		close(virtual_nic_fd);
		ClearRoutingTable();
		RemoveVirtualNic();
		printf("Error: Failed to set up the TCP socket with TLS/SSL.\n");
		return -1;
	}
    

	while(keep_running)
	{
	    	/* accept incoming client connection */
		conn_fd = CreateConnection(&socket_fd, &ctx, &ssl);
		
		/* main loop for handling traffic */
		while(-1 != conn_fd)
		{
			FD_ZERO(&read_fds);
			FD_SET(virtual_nic_fd, &read_fds);
			FD_SET(conn_fd, &read_fds);

			maxfdp = max(virtual_nic_fd, conn_fd);
			select(maxfdp + 1, &read_fds, NULL, NULL, NULL);
		
			if(FD_ISSET(virtual_nic_fd, &read_fds))	/* outgoing */
			{
				if(-1 == HandleTrafficToClient(virtual_nic_fd, ssl))
				{
					break;
				}
			}
			
			if(FD_ISSET(conn_fd, &read_fds))		/* incoming */
			{
				if(-1 == HandleTrafficFromClient(virtual_nic_fd, ssl))
				{
					break;
				}
			}
		}	
	}
	
	CleanUp(virtual_nic_fd, socket_fd, conn_fd, ctx, ssl);
	return 0;
}
