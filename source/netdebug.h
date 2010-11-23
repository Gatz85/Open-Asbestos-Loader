// This is based on the sample application will connect to a specified IP using the PS3's
// network and send a small message from psl1ght. It will try sending it with TCP .
// It has been changed a litte my me to work as a quick and dirty debug setup.
// look below on how to use

// i could not file what this was 
#include <psl1ght/lv2/net.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Configure these (especially the IP) to your own setup.
// Use netcat to receive the results on your PC:
// TCP: nc -l -p 4000
// UDP: nc -u -l -p 4000
// For some versions of netcat the -p option may need to be removed.
#define TESTIP		"192.168.2.7"
#define TESTPORT	4000


#define TESTSTRING  "Debug system Up and Running\n"
int tcp_debug()
{
	
	// Beginning debug TCP socket 
	int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s < 0) {
		//Unable to create a socket
		return -1;
	}
	//Socket created
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_len = sizeof(server);
	server.sin_family = AF_INET;
	inet_pton(AF_INET, TESTIP, &server.sin_addr);
	server.sin_port = htons(TESTPORT);

	int ret = connect(s, (struct sockaddr*)&server, sizeof(server));
	if (ret) {
		// Unable to connect to server 
		return -1;
	}
	// Connected to server successfully!
	ret = write(s, TESTSTRING, strlen(TESTSTRING));
	if (ret < 0)
		{
		// Unable to send to server
		shutdown(s, SHUT_RDWR);
		close(s);
		return -1;
		}

	return (s);
}

//I cliped this out of the above code made it as a seprate function
static void printX(char *arg, int s)
{
	if (s <0)
	return;
int len = strlen(arg);
 write(s, arg, len);
}


//Shutdown in case we get a error
static void tcp_shutdown( int s)
{
 shutdown(s, SHUT_RDWR);
}
