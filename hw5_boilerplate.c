	/*
Homework 5: Pipes

The goal of this homework is to understand the pipe command in shell. The
given routine is trying to implement ‘‘ls | wc -l’’. You need to use close
and dup system calls to redirect the STDOUT of ls to the STDIN of wc. The effect
of running your modified code should be same as running the command ‘‘ls
| wc -l’’.
*/
#include <stdio.h>
#include <unistd.h>
//ls | wc -l
int main(){
	int pid;
	int fd[2];
	int ret;
	ret = pipe (fd);
	if (ret == -1) {
		printf ("Unable to create pipe\n");
		return 0;
	}
	pid = fork ();

	if (pid == 0) {
		/* Verify that ls exists at /bin/ls */
		/* to verify run: which ls */
		char* const args[] = {"/bin/ls", NULL};
		/* write your code here */
		close(1);
		dup(fd[1]);// child dups the write end onto the write end of the pipe
		close(fd[0]);
		close(fd[1]);
		ret = execv (args[0], args);
		/* NOT REACHED*/
		printf ("failed to exec ls\n");
	} else if (pid > 0) {
		/* Verify that wc exists at /usr/bin/wc */
		
		/* to verify run: which wc */
		char* const args[] = {"/usr/bin/wc", "-l", NULL};
		/* write your code here */
		close(0);
		dup(fd[0]);
		close(fd[1]);
		execv (args[0], args);
		/* NOT REACHED*/
		printf ("failed to exec wc\n");
	} else {
		printf ("Unable to fork\n");
	}
	return 0;
}

/*
Submission
----------
Upload a pdf file with your answers on the Backpack. This is a project group
homework. Only one project member needs to upload. You must follow the
naming convention as group id.pd
*/