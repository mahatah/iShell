/***********************
* Program: iShell
* Author: Matthew Price
* February 19, 2009
************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

extern char *const *environ;			/*ALL GLOBAL VARIABLES ARE TO BE DEFINED HERE! */

int storedi = 0;
int command_numb = 0;
int lex_loop = 0;
int times_ran = 0;
int HIST_SIZE;
char *ptr_HIST_SIZE;

struct var{					/*structure used to hold all the user commands*/
	char main_command[100];
	char arguement[100];
	int num_arg;
}user_command[100];



int putenv(char *);				/*These three functions exist inside the libraries listed above, however, minix needs them to be specifically declared somewhere else to be happy*/
char * strtok_r(char *, const char *, char **);
int fileno(FILE *);



/*Precondition: At minimum one command has been entered and stored in the struct*/
/*Postcondition: Displays all the used structs in order from when the user typed them in up to the size of the environment variable HISTSIZE*/
int history(void);

/*Precondition: Environment variable is not NULL*/
/*Postcondition: Enviroment variables are displayed for the user*/
int environ_disp(char *);

/*Precondition: User has selected an environment variable to be changed or added to the environment and has provided sufficent number of arguements*/
/*Postcondition: The user's input is tokenized into seperate pieces and sent to setenv_minix for further processing*/
int set_environ(void);

/*Precondition: User has selected a directory they would like to change to*/
/*Postcondition: User's directory is change to the user's input, if directory exists, otherwise do not change directory. Afterward tell the user the present working directory.*/
int change_dir(void);

/*Precondition: User has selected an environment variable that they would like removed*/
/*Postcondition: The user's input is tokenized and sent to unputenv_minix for further processing*/
int unset_env(void);

/*Precondition: User has entered a command that was not picked up by any of the built in functions*/
/*Postcondition: User's program is executec if found or an error is returned if not found*/
int check_paths(void);

/*Precondtion: A function call was made such that the orignal function determined that there were not enough arguements to process the command and now we will inform the user*/
/*Postcondition: User is informed that the command requested was not processed due to too few arguements*/
int no_arguement_error(void);

/*Precondtion: A function has called that needs to make sure that there is at least two arguements in the arguement array in order for it to continue to process*/
/*Postcondition: If there are at least two arguements, the the function returns zero, otherwise it will came a call to no_arguement_error and return 1*/
int check_two_argue(void);

/*Precondtion: At least one command has been entered into the struct and a function call has been made to find_num_argue*/
/*Postcondition: The number of arguements will be calculated and placed inside of the member of the struct num_arg*/
int find_num_argue(void);

/*Precondition: A pointer containing the arguements (not the main command) is sent to this function that will be used to set an enviornment variable. At least two arguements must exist (a name of the varibale and the value to be assigned to it)*/
/*Postcondition: If the environment variable that the user wants to set exists, the value will be modified. However, if it does not exist the variable and value are added to the environment.*/
int setenv_minix(const char *, const char *, int);

/*Precondition: A pointer containing the arguements (not the main command) is sent to this function that will be used to delete an environment variable. At least one arguement must exist, which is the name of the variable to be deleted*/
/*Postcondition: The environment variable that the user have chosen is reomved from the environment, if found. Otherwise, a notice that the environment varible could not be found is displayed.*/
int unputenv_minix(const char *);



