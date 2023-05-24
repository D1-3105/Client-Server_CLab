#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


struct request {
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
	float a = 2123, b = 211, y = 37654;
	float data[] = {a, b, y};
	struct request on_send = { data };
    	send(sock, on_send.data, sizeof(float)*3, 0);
    	printf("Sended data \n");
	struct response resp;
	unsigned cl;
	recv(sock, &cl, 4, 0);
	printf("RECEIVED CONTENT LEN: %u\n", cl);
	resp.content_len = cl;
	printf("RECEIVED CONTENT\n");
	recv(sock, &resp.status_code, sizeof(unsigned), 0);
	//recv(sock, resp.data, cl-2*sizeof(unsigned), 0);
	float fl_n=-1;
	//printf("c: %f", fl_n);
//printf("c: %x %f", resp.data, ((float *)resp.data)[0]);
	if(resp.status_code == 200) {
		//float answer = ((float*) resp.data)[0];
		recv(sock, &fl_n, cl-2*sizeof(unsigned), 0);
		printf("Answer: %f\n", fl_n);
	}
	else {

		printf("Status: %i", resp.status_code);
		printf("Detail: Division by zero");
	}
    	close(sock);

	return 0;
}
