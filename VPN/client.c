/* ===================== */
/*      HEADER FILES     */
/* ===================== */
#include <stdlib.h>		/* atoi		   */
#include <fcntl.h>		/* O_RDWR		   */
#include <linux/if.h>		/* ifr			   */
#include <linux/if_tun.h>      /* IFF_TUN		   */
#include <sys/ioctl.h>         /* ioctl(), TUNSETIFF     */
#include <arpa/inet.h>         /* inet_addr 		   */
#include <openssl/ssl.h> 	/* ssl 		   */ 
#include <unistd.h>            /* close, access 	   */
#include <sys/select.h>	/* select		   */
#include <string.h>		/* strstr, strtok, strcmp */
#include <signal.h>		/* SIGINT 		   */

/* ===================== */
/*      DEFINITIONS      */
/* ===================== */
#define VNIC_NAME "tun0"
#define MTU 1400
#define BUFFER_SIZE 1500
#define MAX_LINE_LENGTH 4096
#define MIN_PORT 1024
#define MAX_PORT 65535
#define CMD_LINE_LENGTH 1024

/*** COMPILE WITH -lssl -lcrypto ***/
/********* RUN USING ROOT *********/

static volatile int keep_running = 1;
char server_host[16] = {'\0'};
int port = 0;
char ca_path[MAX_LINE_LENGTH] = {'\0'};


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
 * Function:  ValidateAndAssignServerHost 
 * --------------------
 *  validates and assigns the server host value extracted from the configuration file
 *
 *  value:            	server host value to validate and assign
 *
 *  returns:		0 if successful, -1 if an error occurred
 */
int ValidateAndAssignServerHost(char *value)
{
	struct in_addr addr;

	if (1 != inet_pton(AF_INET, value, &addr)) 
	{
		printf("Error: The provided SERVER_HOST '%s' is not a valid IPv4 address.\n", value);
    		return -1; 
    	}
	
    	strcpy(server_host, value);
    	return 0; 
}


/*		
 * Function:  ValidateAndAssignCA 
 * --------------------
 *  validates and assigns the CA path value extracted from the configuration file
 *
 *  value:            	CA path value to validate and assign
 *
 *  returns:		0 if successful, -1 if an error occurred
 */
int ValidateAndAssignCA(char *value)
{
	if(access(value, F_OK) == -1) 
	{
		printf("Error: CA_CRT file not found or inaccessible at the specified path.\n");
		return -1;
    	} 
        
        strcpy(ca_path, value);
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
		
		if(0 == strcmp(key, "SERVER_HOST"))
		{
			if(-1 == ValidateAndAssignServerHost(value))
			{
				return -1;
			}
		}
		else if(0 == strcmp(key, "PORT"))
		{
			if(-1 == ValidateAndAssignPort(atoi(value)))
			{
				return -1;
			}
		}
		else if(0 == strcmp(key, "CA_PATH"))
		{
			if(-1 == ValidateAndAssignCA(value))
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
 *  reads and parses a client configuration file to retrieve server host and port information
 *
 *  returns:	 0 if successful, -1 if an error occurred or the configuration is invalid
 */
int GetConfiguration()
{
	FILE *config_file = NULL;
	
	config_file = fopen("client_config_file.txt", "r");
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
int SetUpVirtualNIC(char *vnic_name)
{
	struct ifreq ifr;
	int fd = 0;
	char cmd[CMD_LINE_LENGTH];

    	system("sudo ip tuntap add mode tun tun0");

    	fd = open("/dev/net/tun", O_RDWR);
    	if (-1 == fd)
    	{
        	return -1;
    	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI; /* IFF_NO_PI = dudce IP protocol version from packet, IFF_TUN = tun device (no ethernet header) */
	strncpy(ifr.ifr_name, vnic_name, IFNAMSIZ);

    	/* ioctl will use ifr_name as the name of TUN interface to open: "tun0", etc. */
    	if (-1 == ioctl(fd, TUNSETIFF, (void *)&ifr))
    	{
    	    close(fd);
    	    return -1;
    	}
    	/* After the ioctl call the fd is "connected" to tun device specified by vnic_name */

    	system("sudo ip link set dev tun0 up");
    	snprintf(cmd, sizeof(cmd), "ifconfig tun0 10.8.0.2/24 mtu %d up", MTU);
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
    
    	*ctx = SSL_CTX_new(TLS_client_method());
    	if(SSL_CTX_use_certificate_file(*ctx, ca_path, SSL_FILETYPE_PEM) != 1)
    	{
    		printf("Error: Failed to use the provided certificate file.\n");
    		return -1;
    	}
    	

    	/* Set the address and port of the server to connect to */
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_port = htons(port);
    	server_addr.sin_addr.s_addr = inet_addr(server_host);

    	/* Create a socket and SSL session */
    	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	{
     	   return -1;
   	}
    	
    	*ssl = SSL_new(*ctx);
    	SSL_set_fd(*ssl, sockfd);
    
    	if (-1 == connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    	{
    		printf("Error: Unable to connect to server at '%s:%d'.\n", server_host, port);
        	return -1;
    	}
    
    	if(0 > SSL_connect(*ssl))
    	{
        	printf("Error: SSL handshake with server failed.\n");
        	return -1;
    	}
    
    	printf("Client connected to server successfully.\n");
    	return sockfd;
}


/*		
 * Function:  RouteTrafficToVirtualNIC 
 * --------------------
 *  configures routing rules for directing traffic to the virtual network interface (TUN)
 *  
 *  this function enables IP forwarding, sets up iptables rules to allow traffic to flow
 *  between the TUN interface and the physical network interface, and adds IP route rules
 *  for proper routing of traffic through the TUN interface
 *
 *  returns:	no return value
 */
void RouteTrafficToVirtualNIC()
{
	char cmd[CMD_LINE_LENGTH];

	system("sysctl -w net.ipv4.ip_forward=1");							/* enable IP forwarding			*/
	system("iptables -I FORWARD 1 -i tun0 -m state --state RELATED,ESTABLISHED -j ACCEPT");	/* allow incoming traffic			*/
	system("iptables -I FORWARD 1 -o tun0 -j ACCEPT");						/* route traffic to tun0			*/
	snprintf(cmd, sizeof(cmd), "iptables -I FORWARD 1 ! -d %s -o tun0 -j ACCEPT", server_host);	/* route all non-local traffic to tun0	*/
	system(cmd);											/* (same VLAN traffic isn't handled)		*/
	/* total ip range coverage - equivelant to 0.0.0.0/0 (maybe unsupposted) */
	system("ip route add 0/1 dev tun0");   							/* 0.0.0.0/1 -> 000000000... - 011111111...	*/
	system("ip route add 128/1 dev tun0");							/* 128.0.0.0/1 -> 10000000... - 111111111...	*/
}


/*		
 * Function:  HandleTrafficToServer 
 * --------------------
 *  handles outgoing traffic from the virtual network interface (TUN) to the server
 *  
 *  this function reads data from the virtual network interface, then writes the
 *  data to the SSL/TLS session for secure transmission to the server
 *
 *  virtual_nic_fd:	file descriptor of the virtual network interface (TUN)
 *  ssl:		pointer to the SSL/TLS session
 *
 *  returns:		0 if successful, or -1 if an error occurred
 */
int HandleTrafficToServer(int virtual_nic_fd, SSL *ssl)
{
    	int result = 0;
    	char buffer[BUFFER_SIZE];
    
    	result = read(virtual_nic_fd, buffer, sizeof(buffer));
    	if(-1 == result)
    	{
    	    	return -1;
    	}
    
    	if(-1 == SSL_write(ssl, buffer, result))
    	{
    		return -1;
    	}
   
    	memset(buffer, 0, sizeof(buffer));
    	return 0;
}


/*		
 * Function:  HandleTrafficFromServer 
 * --------------------
 *  handles incoming traffic from the server to the virtual network interface (TUN)
 *  
 *  this function reads data from the SSL/TLS session, then writes the
 *  data to the virtual network interface (TUN) for further processing
 *
 *  virtual_nic_fd:	file descriptor of the virtual network interface (TUN)
 *  ssl:             	pointer to the SSL/TLS session
 *
 *  returns:		0 if successful, or -1 if an error occurred
 */
int HandleTrafficFromServer(int virtual_nic_fd, SSL *ssl)
{
	int result = 0;
    	char buffer[BUFFER_SIZE];
    
    	result = SSL_read(ssl, buffer, sizeof(buffer));
    	if(0 >= result)
    	{
    		return -1;
    	}
    
    	if(-1 == write(virtual_nic_fd, (const void *)buffer, result))
    	{
    		return -1;
    	}
   
    	memset(buffer, 0, sizeof(buffer));
    	return 0;
}


/*		
 * Function:  ClearRoutingTable 
 * --------------------
 *  clears the routing table and iptables rules related to traffic routing
 *
 *  returns:  no return value
 */
void ClearRoutingTable()
{
	char cmd[CMD_LINE_LENGTH];

	system("iptables -D FORWARD -i tun0 -m state --state RELATED,ESTABLISHED -j ACCEPT");
	snprintf(cmd, sizeof(cmd), "iptables -D FORWARD ! -d %s -o tun0 -j ACCEPT", server_host);
	system(cmd);
	system("ip route del 0/1");
	system("ip route del 128/1");
}


/*		
 * Function:  RemoveVirtualNic 
 * --------------------
 *  Removes the virtual network interface (TUN device) from the system
 *
 *  returns:  no return value
 */
void RemoveVirtualNic()
{
	system("sudo ip link delete tun0");
}


/*		
 * Function:  CleanUp 
 * --------------------
 *  closes the network connections, removes the virtual network interface, and releases resources
 *
 *  virtual_nic_fd:  file descriptor of the virtual network interface 
 *  socket_fd:       file descriptor of the TCP socket
 *  ctx:             SSL context
 *  ssl:             SSL session
 *
 *  returns:	      no return value
 */
void CleanUp(int virtual_nic_fd, int socket_fd, SSL_CTX *ctx, SSL *ssl)
{
	ClearRoutingTable();
	close(virtual_nic_fd);
	close(socket_fd);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	RemoveVirtualNic();
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
 *  the entry point of the client program. sets up the virtual network interface (tun0), 
 *  establishes a TCP connection with the server using SSL/TLS encryption,
 *  and handles incoming and outgoing traffic through the virtual network interface
 */
int main()
{
	int virtual_nic_fd = 0;
	int socket_fd = 0;
	int maxfdp = 0;
	fd_set read_fds;
	SSL_CTX *ctx;
	SSL *ssl;
	
	if(-1 == GetConfiguration())
	{
		return -1;
	}

	/* set up virtual network interface (tun0) */
	virtual_nic_fd = SetUpVirtualNIC(VNIC_NAME);
	if (-1 == virtual_nic_fd)
	{
		return -1;
	}
    	
	signal(SIGINT, HandleCtrlC);
	
	/* route traffic through the virtual network interface */
	RouteTrafficToVirtualNIC();

	/* set up TCP socket and SSL/TLS connection */
	socket_fd = SetUpTCPSocketWithTLS(&ctx, &ssl);
	if (-1 == socket_fd)
	{
	    	close(virtual_nic_fd);
		ClearRoutingTable();
	    	RemoveVirtualNic();
		return -1;
	}
    
	while(keep_running)
	{
		FD_ZERO(&read_fds);			
		FD_SET(virtual_nic_fd, &read_fds);
		FD_SET(socket_fd, &read_fds);

		maxfdp = max(virtual_nic_fd, socket_fd);
		
		/* wait for activity on the file descriptors */
		select(maxfdp + 1, &read_fds, NULL, NULL, NULL);

		if(FD_ISSET(virtual_nic_fd, &read_fds))	/* outgoing */
		{
			if(-1 == HandleTrafficToServer(virtual_nic_fd, ssl))
			{
				CleanUp(virtual_nic_fd, socket_fd, ctx, ssl);
				printf("Error: Failed to handle outgoing traffic to the server.\n");
				return -1;
            		}
        	}
        
        	if(FD_ISSET(socket_fd, &read_fds))		/* incoming */
        	{
			if(-1 == HandleTrafficFromServer(virtual_nic_fd, ssl))
            		{
                		CleanUp(virtual_nic_fd, socket_fd, ctx, ssl);
            			printf("Error: Failed to handle incoming traffic from the server.\n");
            			return -1;
            		}
        	}
    	}

	CleanUp(virtual_nic_fd, socket_fd, ctx, ssl);
	
	return 0;
}

