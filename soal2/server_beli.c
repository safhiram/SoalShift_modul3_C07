#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#define PORT 8000

int *stok;
char message[1024]={0};
int status=0;
int new_socket;

void* kurangstok(void* sock){
	int valread;
	int yap=*(int*)sock;
	char buffer[1024]={0};
	while(valread=read(yap,buffer,1024))
	{
	    	if(strcmp(buffer,"beli")==0){
			if(*stok==0)
			{
				strcat(message,"Transaksi Gagal!");
				status=1;
				send(yap,message,strlen(message),0);
			}
			else
			{
				*stok-=1;
				strcat(message,"Transaksi Berhasil");
				send(yap,message,strlen(message),0);
				status=1;
			}
	    	}
		memset(buffer,0,1024);
		memset(message,0,1024);
	}
}

int main(int argc, char const *argv[]) {
	int err;
	key_t key = 1234;
	int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
	stok = shmat(shmid,NULL,0);
    int server_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
	char buffer[1024]={0};
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
	int *now=malloc(sizeof(*now));
	*now=new_socket;
	pthread_t tid;
	if(new_socket>0)
	{
       	err=pthread_create(&(tid),NULL,kurangstok,now);
		if(err!=0) //cek error
		{
			printf("\n can't create thread : [%s]",strerror(err));
		}
		else
		{
			printf("\n create thread success\n");
		}
	}
}

    return 0;
}
