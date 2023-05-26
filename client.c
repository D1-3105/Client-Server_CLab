#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


struct request {
	unsigned content_len;
	char *data;
};

struct response {
	unsigned content_len;
	unsigned status_code;
	char *data;
};

int main()
{
    	int sock;
    	struct sockaddr_in addr;

    	sock = socket(AF_INET, SOCK_STREAM, 0);
    	if(sock < 0) {
        	perror("socket");
        	exit(1);
    	}

    	addr.sin_family = AF_INET;
    	addr.sin_port = htons(8080);
    	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
   	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
       		perror("connect");
        	exit(2);
	}
	printf("Connected \n");
	int data[] = {3, 1, 5, 6, 7};
	struct request on_send = { 5*sizeof(int)+sizeof(unsigned), data };
    	send(sock, &on_send.content_len, sizeof(on_send.content_len), 0);
    	send(sock, on_send.data, on_send.content_len, 0);
	printf("Sended data \n");
	struct response resp = {0, 0, malloc(10)};
	recv(sock, &resp.content_len, sizeof(unsigned), 0);
	printf("RECEIVED CONTENT LEN: %u\n", resp.content_len);
	recv(sock, &resp.status_code, sizeof(unsigned), 0);
	printf("RECEIVED STATUS CODE: %u\n", resp.status_code);
	size_t datalen = resp.content_len - 2*sizeof(unsigned);
	recv(sock, resp.data, datalen, 0);
	if(resp.status_code == 200) {

		printf("Obtained result: [");
		for(int i = 0; i < datalen/sizeof(int); i++){
			printf("%d, ", ((int*) resp.data)[i]);
		}
		printf("]\n");
	}
	else {
		printf("Error status code: %d", resp.status_code);
	}
    	close(sock);

	return 0;
}
