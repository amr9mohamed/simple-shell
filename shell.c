#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "unistd.h"
#include "sys/wait.h"
#include "string.h"

#define foreground 1
#define backgroud  0

void spawn(char** arg_list,unsigned char ground_status)
{
	pid_t child_pid;
	child_pid = fork();
	if (child_pid == 0)/* child process */
	{
		execvp(arg_list[0],arg_list);//return only if there is error
		fprintf(stderr, "ERROR IN COMMAND\n");
		abort();
	}
	else if (child_pid > 0)/* parent process */
	{	
		if (ground_status == foreground )
		{
			waitpid(child_pid,NULL,0);//TO WAIT FOR THE CHILD TO TERMINATE
		}
		else
		{
			;//will not wait
		}
	}
	else /* error in forking the process */
	{
		fprintf(stderr, "ERROR IN FORK\n");
	}
}

char* take_input(char* command)
{
	//char*check=malloc(1000*sizeof(char));	
	char*check = fgets(command,1000,stdin);
	while(command[0]==' ')
	{
		command++;
	}	
	while(command[1]=='\0')
	{
		printf("shell>>");		
		check = fgets(command,1000,stdin);
		while(command[0]==' ')
		{
			command++;
		}	
	}
	if(command != NULL && (strcmp(command,"")!=0) && (strlen(command)<=512))
	{		
		command[strlen(command)-1] = '\0';//to remove the \n char
	}
	else
	{
		return NULL;		
	}
	return check;
}

void tokenizer(char* command, unsigned char count, char** arg_list)
{
	int i;
	for (i = 0; i < count; ++i)
	{
		arg_list[i] = (char*)malloc(20*sizeof(char));
	}

	const char delim[2] = " ";
	char* token = strtok(command,delim);

	i = 0;
	while(token != NULL)
	{
		arg_list[i++] = token;
		token = strtok(NULL,delim);
	}
	arg_list[i]= NULL;
}

int main(void)
{
	printf("shell>>");
	while(1)
	{
		char command[1000];
		unsigned char ground_status, i = 0, count = 0;
		if ((take_input(command)) != NULL)//read successfully
		{
			ground_status = foreground;//assume all in foreground
			if(command[0]=='&')
			{
				fprintf(stderr, "ERROR\n");
				printf("shell>>");				
				continue;
			}			
			while(command[i] != '\0')
			{
				if (command[i] == ' ')
				{
					count++;
				}
				else if (command[i] == '&' && i == (strlen(command)-1) )//will run in backgroud
				{
					command[i] = '\0';// dont know yet what to do
					ground_status = backgroud;
				}
				i++;
			}
			count++;//the number of sub strings created
			count++;//for the null


			char** arg_list = (char**)malloc((count)*sizeof(char*));//this will hold the parameters 
			tokenizer(command, count, arg_list);//ground and arg_list are outputs of the function
			if (strcmp(arg_list[0],"exit") == 0)
			{
				break;
			}
			else if (strcmp(arg_list[0],"cd") == 0)
			{
				if (*(arg_list+1) == NULL || (int)strlen(arg_list[1]) == 1 && *(*(arg_list+1)) == '~' )
				{
					*(arg_list+1) = getenv("HOME");
				}
				char state = (char)chdir(*(arg_list+1));
				if (state == 0)
				{
					;//do nothing
				}
				else if (state == -1)
				{
					fprintf(stderr, "ERROR in cd\n");
				}
			}
			else if (strcmp(arg_list[0],"pwd") == 0)
			{
				char buff[512];
				if (strcmp(getcwd(buff,512),buff) == 0)
				{
					printf("%s\n",buff);
				}
				else
				{
					printf("ERROR in pwd\n");;
				}
			}
			else
			{
				spawn(arg_list,ground_status);
			}
			free(arg_list);//free the dynamically allocated memory
			printf("shell>>");
			
		}
		else //read unsuccessfully 
		{			
			fprintf(stderr, "ERROR\n");
			printf("shell>>");
		}
	}
	return 0;
}
