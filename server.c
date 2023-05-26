#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>
#include <math.h>

typedef struct {
	size_t content_len;
	unsigned status_code;
	char* data;
} response;

typedef struct {
	unsigned content_len;
	void* data;
} request;

int* endpoint_logic(request* req, int* answer_len, int* answer) {
	int cur_len = 0;
    	int* data = (int*) req->data;
    	int num_ints = req->content_len / sizeof(int);
    	for (int i = 0; i < num_ints - 1; i++) {
        	int cur_num = data[i];
        	unsigned is_prime = 1;
        	for (int delim = 2; delim < cur_num; delim++) {
			if (cur_num % delim == 0) {
                		is_prime = 0;
                		break;
          		}
        	}
        	if (is_prime) {
            		cur_len++;
            		answer = (int*) realloc(answer, sizeof(int) * cur_len);
            		answer[cur_len - 1] = cur_num;
        	}
    	}
    	*answer_len = cur_len;
}

response do_work(request* req) {
	if(req->data != NULL) {
		int* result_len = malloc(sizeof(int));
		int* result = malloc(100);
		endpoint_logic(req, result_len, result);
		response resp = {sizeof(int)*(*result_len)+sizeof(unsigned)*2, 200, (void*)result};
		return resp;
	}
	response resp = {sizeof(unsigned)*2, 500, NULL};
	return resp;
}

request receive_data(int sock) {
	float* buf = malloc(sizeof(float)*3);
	int bytes_read;
	unsigned content_length;
	recv(sock, &content_length, sizeof(unsigned), 0);
	recv(sock, buf, content_length-sizeof(unsigned), 0);
	request received = {content_length, buf};
	return received;
}

void respond(int sock, response *resp, int len, int flags) {
	send(sock, &resp->content_len, sizeof(unsigned), 0);
	send(sock, &resp->status_code, sizeof(unsigned), 0);
	send(sock, (void*)resp->data, resp->content_len - sizeof(unsigned)*2, 0);
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
		request received_request = receive_data(sock);
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
