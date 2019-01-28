#include <stdio.h>
#include <unistd.h>
//ls | wc -l
/*
0 - STDIN
1 - STDOUT
2 - STDERR

You need to use close
and dup system calls to redirect the STDOUT of ls to the STDIN of wc

dup -> https://www.geeksforgeeks.org/dup-dup2-linux-system-call/

The dup() system call creates a copy of a file descriptor.

It uses the lowest-numbered unused descriptor for the new descriptor.
If the copy is successfully created, then the original and copy file descriptors may be used interchangeably.
They both refer to the same open file description and thus share file offset and file status flags.
int dup(int oldfd);
oldfd: old file descriptor whose copy is to be created.



close -> http://pubs.opengroup.org/onlinepubs/009696899/functions/close.html
*/
int main(){
	int pid; //Process ID
	int fd[2]; //File Descriptor, to store the ends of pipe I believe
	/* 0 - Read End
	   1 - Write End*/

	int ret; //Return value ?

	// fd currently has garbage values
	// printf("fd[] = %d, %d\n", fd[0],fd[1]);
	// fd[] = -308746176, 32766
	
	/* Cannot find much about this function from the man page and Clion.
	From what I understand it just tries to create a pipe

	it also sets the values in fd
	returns -1 when pipe cannot be created*/
	ret = pipe (fd); 
	if (ret == -1) {
		printf ("Unable to create pipe\n");
		return 0;
	}
	// printf("pipe(fd) returned %d\n", ret);
	// returned 0

	// printf("fd[] = %d, %d\n", fd[0],fd[1]);
	// fd[] = 3, 4

	pid = fork ();
	//Trying to create a child process, 
	//so the execution splits/forks here and two instances 
	//start executing from the very next line

	if (pid == 0) { //CHILD, writes to the pipe
		/* Verify that ls exists at /bin/ls */
		/* to verify run: which ls */
		char* const args[] = {"/bin/ls", NULL};
		/* write your code here 
		Close STDOUT*/
		close(fd[0]);//Closing read end of the pipe
		//non-essential, good coding, so that fd table entry can be used for other purposes

		close(1);//Closing STDOUT
		dup(fd[1]);//Duplicating Write end of pipe (to 1)
		
		close(fd[1]);//good coding, so that fd table entry can be used for other purposes
		//Closing write end of the pipe

		ret = execv (args[0], args);
		/* NOT REACHED*/
		printf ("failed to exec ls\n");
	} else if (pid > 0) { //Parent reads from the pipe
		/* Verify that wc exists at /usr/bin/wc */
		/* to verify run: which wc */
		char* const args[] = {"/usr/bin/wc", "-l", NULL};
		/* write your code here 
		Close STDIN*/
		close(fd[1]);//ESSENTIAL, OTHERWISE CODE WONT STOP
		//Closing write end of the pipe
		
		close(0); //Closing STDIN
		dup(fd[0]);//Duplicating Read end of pipe (to 0)
		
		close(fd[0]);//good coding, so that fd table entry can be used for other purposes
		//Closing read end of the pipe

		execv (args[0], args);
		// close(fd[0]);//no point of this
		
		/* NOT REACHED*/
		printf ("failed to exec wc\n");
	} else { //Probably the fork() did not work properly
		printf ("Unable to fork\n");
	}
	return 0;
}