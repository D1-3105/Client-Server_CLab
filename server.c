#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>

typedef struct {
	size_t content_len;
	unsigned status_code;
	char* data;
} response;

typedef struct {
	char* data;
} request;

struct coefficients {
	float a, b, y;
};

response do_work(request* buf) {
	if(buf->data != NULL) {
		// buf->data = a, b, y
		if (((float*) buf->data)[0] == 0){
			response resp = {18+sizeof(unsigned)*2, 400, "Division by zero!"};
			return resp;
		}
		struct coefficients coefs ={ ((float*) buf->data)[0], ((float*) buf->data)[1], ((float*) buf->data)[2] };
		float *x = (float*) malloc(sizeof(float)*1);
		*x = (coefs.y - coefs.b) / coefs.a;
		response resp = {sizeof(float)+sizeof(unsigned)*2, 200, (char*)x};
		return resp;
	}
	response resp = {sizeof(unsigned)*2, 500, NULL};
	return resp;
}

void* receive_data(int sock) {
	float* buf = malloc(sizeof(float)*3);
	int bytes_read;
	bytes_read = recv(sock, buf, 3*sizeof(float), 0);
	if(bytes_read == 0) {
		return;
	}
	printf("\ns:A: %f \n", buf[0]);
	printf("\ns:B: %f \n", buf[1]);
	printf("\ns:Y: %f \n", buf[2]);
	return buf;
}

int respond(int sock, response *resp, int len, int flags) {
    	int total = 0;
	int n;
	printf("%u , %u \n", resp->status_code, resp->content_len);
	printf("s:X: %f", *resp->data);
	send(sock, &resp->content_len, sizeof(unsigned), 0);
	send(sock, &resp->status_code, sizeof(unsigned), 0);
	//if (resp->status_code == 200)
	send(sock, resp->data, resp->content_len - sizeof(unsigned)*2, 0);
	//for debug 

		//float test_n = 777;
		//send(sock, &test_n, resp->content_len - sizeof(unsigned)*2, 0);



//else
		//send(sock, resp->data, resp->content_len - sizeof(unsigned)*2, 0);
	return (n==-1 ? -1 : total);
}

void accept_ready(int listener){
	int sock;
	while(1)
        {
                sock = accept(listener, NULL, NULL);
                if(sock < 0){
                        perror("accept");
                        exit(3);
                }
		printf("\ns:ACCEPTED CONNECTION\n");
		request received_request = { receive_data(sock) };
		printf("\ns:ACCEPTED DATA\n");
		response resp = do_work(&received_request);
		printf("\ns:WORK DONE\n");
		respond(sock, &resp, resp.content_len, 0);
		printf("\ns:RESPONDED\n");
		free(received_request.data);
	}
	close(sock);
}

void serve_forever(){
	int listener;
    	struct sockaddr_in addr;
    	int bytes_read;
    	listener = socket(AF_INET, SOCK_STREAM, 0);
    	if(listener < 0)
    	{
        	perror("s:socket");
        	exit(1);
    	}
	printf("s:Listening socket...");
    	addr.sin_family = AF_INET;
    	addr.sin_port = htons(8080);
    	addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        	perror("s:bind");
        	exit(2);
    	}

    	listen(listener, 1);
	printf("\ns:BINDED 8080\n");
	accept_ready(listener);
}

int main(){
	serve_forever();
}
