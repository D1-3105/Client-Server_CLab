#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct response {
	unsigned status_code;
	char* data;
};

struct request {
	char* data;
};

struct coefficients {
	float a, b, y;
};

struct response do_work(struct request* buf) {
	if(buf->data != NULL) {
		// buf->data = a, b, y
		if (buf->data[0] == 0){
			return {400, "Division by zero!"};
		}
		struct coefficients coefs = buf->data;
		float x = (coefs.y - coefs.b) / coefs.a;
		return {200, *x};
	}
}

void* receive_data() {
	float buf[3];
	int bytes_read;
	while(1) {
		bytes_read = recv(sock, buf, 3*sizeof(float), 0);
		if(bytes_read == 0) {
			return;
		}
	}
	return buf;
}

int respond(int s, struct response *resp, int len, int flags) {
    int total = 0;
    int n;

    while(total < len)
    {
        n = send(s, buf+total, len-total, flags);
        if(n == -1) { break; }
        total += n;
    }

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
		struct request received_data = { receive_data() };
		struct response result = do_work();
		send(sock, )
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
        	perror("socket");
        	exit(1);
    	}
    
    	addr.sin_family = AF_INET;
    	addr.sin_port = htons(3425);
    	addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    	{
        	perror("bind");
        	exit(2);
    	}

    	listen(listener, 1);
	accept_ready(listener);
    }
}

int main(){
	serve_forever();
}
