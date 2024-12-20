//import some c libraries
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>


//jobs are tracked by the shell whereas processes are tracked by the OS.
// job table that stores PID and command
typedef struct job{
	pid_t pid;
	char command[1024];
} job_table;

#define MAX_JOBS 64
job_table jobs[MAX_JOBS];
int job_count = 0;

void sigint(int sig){
	printf("\nCaught SIGINT. Use 'kill' for specific job management.\n");
}

//1. Command Line Prompt
void displayPrompt() {
	printf("time to shellabrate ~$ ");
}

//2. Parser
char** parser(char *input) { //takes raw string from user and returns a pointer to an array of strings
	int bufsize = 64; //buffer size for tokens
	int curr = 0; //track current index of tokens
	char **tokens = malloc(bufsize * sizeof(char*)); //allocate mem for array of char*
	char *token; //temp

	if (!tokens) {
		fprintf(stderr, "shell nah: allocation error\n");
		exit(EXIT_FAILURE); //if tokens is null then allocation failed so print error message
	}
	token = strtok(input, " \t\r\n"); //split strings in tokens based on spaces, tabs, returns, and newlines. Then returns pointer to first token in input
	
	while (token != NULL) {
		tokens[curr] = token;
		curr++; //place curr token into tokens array at curr
		if (curr >= bufsize) { //if index reaches the bufsize then allocate more mem
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) { //if realloc fails handle error
				fprintf(stderr, "shell nah: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, " \t\r\n"); //get next token
	}
	tokens[curr] = NULL; //stop loop
	return tokens;
}

//3. Executor

int executor(char *input) {

   	char **args = parser(input);
   	pid_t pid;
   	pid_t wpid;
   	int status;
   	int background = 0;

   	if (args[0] == NULL){


        	// If empty
       		return 1;
   	}

    	if (strncmp(args[0], "exit", 4) == 0){
       		printf("leaving the shellabration ... goodbye!");
       		return 0; // exit
    	}

    for (int i = 0; args[i] != NULL; i++){
        	// & for background processing
    	if (strcmp(args[i], "&") == 0){
            background = 1;
            args[i] = NULL;
            break;
        }
    }

    // Handle jobs command here
    if (strncmp(args[0], "jobs", 4) == 0){
        for(int i = 0; i < job_count; i++){
            printf("[%d] PID: %d Command: %s\n", i + 1, jobs[i].pid, jobs[i].command);
        }
        return 1;
    }

    if (strncmp(args[0], "pwd", 3) == 0){
        // Print working directory
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("shell nah error");
        }
        return 1;
    } else if (strncmp(args[0], "ls", 2) == 0) {
        // List directory
        pid = fork();
        if (pid == 0) {
            // Child process
            if (execvp("ls", args) == -1) {
                perror("shell nah error");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0){
            perror("shell nah error");
        } else {
            // Parent process
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        return 1;
    } else if (strncmp(args[0], "gcc", 3) == 0) {
        pid = fork();
        if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                perror("shell nah error");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            // Fork error
            perror("shell nah error");
        } else {
            // Parent process
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        return 1;

    } else if (strncmp(args[0], "vim", 3) == 0) {

        pid = fork();

        if (pid == 0) {

            // Child process
            if (execvp("vim", args) == -1) {

                perror("shell nah error");
                exit(EXIT_FAILURE);

            }

        } else if (pid < 0) {

            // Fork error
            perror("shell nah error");

        } else {
            // Parent process
            do {

                wpid = waitpid(pid, &status, WUNTRACED);

            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        return 1;

    } else if (strncmp(args[0], "less", 4) == 0) {

    	pid = fork();

    	if (pid == 0) {
        // Child process
        	if (execvp("less", args) == -1) {

            		perror("shell nah error");
            		exit(EXIT_FAILURE);

        	}

    	} else if (pid < 0) {

        	// Fork error
        	perror("shell nah error");

    	} else {

        	// Parent process
        	do {

            		wpid = waitpid(pid, &status, WUNTRACED);

        	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
    	}
    	return 1;

	} else if (strncmp(args[0], "mkdir", 5) == 0) {
    		pid = fork();

    		if (pid == 0) {
        		// Child process
        		if (execvp("mkdir", args) == -1) {
            		perror("shell nah error");
            		exit(EXIT_FAILURE);
        	}
    	} else if (pid < 0) {
        	// Fork error
        	perror("shell nah error");
    	} else {
        	// Parent process
        	do {
            		wpid = waitpid(pid, &status, WUNTRACED);
        	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
    	}
    	return 1;
}

    // Load processes
    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1){
            perror("shell nah error");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("shell nah error");
    } else {
        // Parent process
        if(background){
            // Add background process to job table here
            if(job_count < MAX_JOBS){
                jobs[job_count].pid = pid;
                strncpy(jobs[job_count].command, input, sizeof(jobs[job_count].command) - 1);
                jobs[job_count].command[sizeof(jobs[job_count].command) - 1] = '\0';
                job_count++;
                printf("[Background Process] PID: %d Command: %s\n", pid, input);
            } else {
                printf("Job table is full. Cannot track more background jobs.\n");
            }
        } else {
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
    return 1;
}


//4. Clean up table
void clearJobs() {
	int status;
	for (int i = 0; i < job_count; i++){
		if (waitpid(jobs[i].pid, &status, WNOHANG) > 0){ //is job done?
			printf("[Job Done] PID: %d Command: %s\n", jobs[i].pid, jobs[i].command);
			for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
			}
			job_count--; //clear
			i--; //adjust idx
		}
	}
}

//5. Main
int main(){
	char input[1024]; //current input limit
	int status = 1;
	signal(SIGINT, sigint); //ignore sigint so that shell itself doesn't terminate
	while (status) {
		clearJobs();
		displayPrompt();
		if (fgets(input, sizeof(input), stdin) == NULL){
			break; //exit on EOF
		}
		input[strcspn(input, "\n")] = '\0'; 
		status = executor(input);
	}
	return 0;

}
