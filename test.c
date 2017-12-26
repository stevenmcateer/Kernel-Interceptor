/*
  CS3013 Project 2
  Ethan Schutzman && Steven McAteer
*/

#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
#define __NR_cs3013_syscall3 379

struct ancestry{
  pid_t parent[10];
  pid_t children[100];
  pid_t sibling[100];
} ;

long testCall2 ( unsigned short* target_pid) {
      
	struct ancestry* response = malloc(sizeof(struct ancestry));

        long ret =  (long) syscall(__NR_cs3013_syscall2, target_pid, response);
      
	if (ret == -1){
                //if error return
        	return ret;
        }

	//loop for children
        printf("looking at children\n");
	for (int i = 0; i < 100; i++) {
		if (response->children[i] < 1) {
		   break;
		} else {
		   printf("Child PID is: %d\n", response->children[i]);
		}

	}
        //loop for siblings
	printf("looking at siblings\n");
	for (int i = 0; i < 100; i++) {
		if (response->sibling[i] < 1 ) {
		   break;
		} else {
		   printf("Sibling PID is: %d\n", response->sibling[i]);
		}

	}

	//loop for parents
	printf("looking at parents\n");
	for (int i = 0; i < 10; i++) {
		if (response->parent[i] < 1) {
		   break;
		} else {
		   printf("Parent PID is: %d\n", response->parent[i]);
		}

	}
        return ret;
}


int main () {
	printf("Calling Test 1\n");
        unsigned short testPid = getpid();
        unsigned short* testPoint= &testPid;
       
        testCall2(testPoint);

        unsigned short pid2 = fork();
	if(pid2 == 0){
		unsigned short pid3 = fork();
		if(pid3 == 0){
			unsigned short pid4 = fork();
			if(pid4 == 0){
				sleep(3);
				_exit(3);
			}
			else{
				printf("Calling test 3\n");
				unsigned short testPid3 = getpid();
				unsigned short* testPid3Addr = &testPid3;
				testCall2(testPid3Addr);	
				_exit(3);	
			}
		}
		else{
			printf("Calling test 2\n");
			unsigned short testPid2 = getpid();
			unsigned short* testPid2Addr = &testPid2;
			testCall2(testPid2Addr);
			_exit(3);

		}
			
	}
	else{
	   wait(NULL);
	}
      
        return 0;
}
