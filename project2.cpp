//ESE 333 Project 2
//Shahzodjon Ismatov 110518374

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define N 100
#define EMPTY 0
#define FULL  1
#define MUTEX 2


int shm;    //shared memory
int front=0;  //two shared memory indices, consumer gets a bye from shared memory's front
int back=0;   //producer gets a byte from shared memory's back
vector<char> table;   //monitoring table

int pidp;   //process ID of producer
int pidc;   //process ID of consumer
int sem;    //semaphore
int status;  
char *buffer;


void handler(int);
void producer_proc(string source);
void consumer_proc(string destination);

int main(int argc, char* argv[]) {
	//srand(time(0)); //seed random based on CurrentTimeMillis
	
 	string source;
  	cout<<"Enter path of source file : ";
  	cin>>source;
  	string destination;
  	cout<<"Enter path of destination file : ";
  	cin>>destination;
	
	// 1. create shared memory. hint: use shmget
	//Give r/w permissions after creating new segment 
  	shm=shmget(IPC_PRIVATE, N, IPC_CREAT|0666);

	// 2. create semaphore. hint: use semget, semop
	sem=semget(IPC_PRIVATE, 1, IPC_CREAT);
  	struct sembuf func[3]={{EMPTY,N,0 },{FULL, 0,0},{MUTEX,1,0 }};
  	semop(sem, func, 3); 

	// 3. set handler functions for signals from producer and consumer process
  	signal(SIGUSR1, handler);
  	signal(SIGUSR2, handler);
	
	// 4. deal with processes
	
	pidp=fork();

	 // 4.1 producer child process
	if(pidp==0){
		//create a buffer
		//buffer=(char*)shmat(shm, NULL, 0);
		producer_proc(source);
		exit(0);
	}

	// 4.2 consumer child process

	else{
		pidc=fork();
		if(pidc==0){
		//buffer=(char*)shmat(shm,NULL,0);
		consumer_proc(destination);
		exit(0);
		}

	  // 4.3 main process
		else{
		waitpid(pidc,&status, 0);
                	cout<< "\n Total number of operations: " << table.size()<< endl;
		//vector<char>::iterator i;
		cout <<  "Table: "; 
		// in main process, print monitoring table
		for(int i = 0; i <table.size(); i++){
		cout<<table[i];
		}	
		cout << endl; 
		 // in main process, destroy shared memory and semaphore. hint: use shmctl, semctl

		
		shmdt(buffer);
		shmctl(shm, IPC_RMID,0); //kill shm
		shmctl(sem, IPC_RMID, 0); //kill sem
	}
}
}

/* handler function for signals from producer and consumer process */

void handler(int signum){
  	if (signum==SIGUSR1){
    		table.push_back('P');
  	}
  	else if (signum=SIGUSR2){
    		table.push_back('C');
  	}
}


void producer_proc(string source) {
	// 1. down & up operation defination
  	struct sembuf down[]={{EMPTY,-1,SEM_UNDO},{MUTEX,-1, SEM_UNDO}};
  	struct sembuf up[]={{MUTEX, 1, SEM_UNDO},{FULL, 1, SEM_UNDO}};

	//2.open file1
  	FILE *fp1;
  	char *file=&source[0];
  	fp1=fopen(file, "r");  
    	cout << "Producer begins..." << endl;
	//3. move bytes from file1 to shared memory
    	char ch;
	buffer = (char*)shmat(shm, NULL, 0);
    	
	while (true) {
	//sleep random time
        	usleep((rand()%3+1)*100000);
	
	semop(sem,down,2);
	//refer to Fig. 2-28
	
        	ch=fgetc(fp1);
	buffer[back] = ch;
	back++;

	if(ch == EOF){ 
	 break;
	}
	else{
		if(ch == '\n')
		cout << "Producer[" << source << "]: '\\n' into buffer" <<endl;
		else
		cout << "Producer[" << source << "]: '" << ch << "' into buffer" << endl;

	}
		//send signal to main process
	semop(sem,up, 2); 
	kill(getppid(), SIGUSR1);
    	}
	cout<<"Producer ends."<<endl;
}


/* consumer process function */
void consumer_proc(string destination) { 
	// 1. down & up operation defination
	struct sembuf up[]={{FULL,-1,SEM_UNDO},{MUTEX,-1,SEM_UNDO}};
  	struct sembuf down[]={{EMPTY,1,SEM_UNDO},{MUTEX,1,SEM_UNDO}}; 

	//2.open file2   
  	FILE *fp2;
  	char *file=&destination[0];
	char ch;
	fp2=fopen(file,"wb");
    	cout << "Consumer begins..." << endl;

	//3. move bytes shared memory to file2
    	while (true){
		// sleep random time
		usleep((rand()%4+1)*100000); 

	
		semop(sem,down, 2);
		buffer = (char*)shmat(shm, NULL, 0);
		ch=buffer[front];


		if(ch =='\0'){
		semop(sem,up,2);
		cout<<"Empty buffer. Nothing to consume."<<endl;
		continue;
		} 
		if(ch!= EOF){
		    putc(ch, fp2);
		    if(ch == '\n')
		    cout<<"Consumer ["<<destination<< "]: '\\n into destination" << endl;
		    else
                        cout<<"Consumer [" << destination<< "] copied char '"<<ch<<"' into destination"<<endl;
		front++;
		}
		else break;
		semop(sem,up,2); 
                    kill(getppid(),SIGUSR2);
		}
	cout << "Consumer ends." << endl;
}