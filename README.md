# iShell
iShell is a type of shell (much like BASH), which is a command interpreter and interface for the Minix operating system. Although it is written specifically for Minix it is possible it will work for many Unix based operating systems.

Installation:
In order to run iShell, a user simply needs to run the command "make" inside the directory the iShell file have been extracted to. This does not add any additional folders but a few temporary files will be created along with an executable named iShell.

To run iShell in the Minix environment:
Simply navigate to the folder with the iShell executable, if you have not already done so by completing the step above, and type ". iShell" without the quotes and press enter.

Implementations and Design:
----------------------------------------------------------------------------------------------------------------------------------------------------------
iShell was designed around a the flex parser which takes in all the user submitted commands and tokenizes them for further processing. iShell has built in commands that are as follows:

-env: Displays all environment variables in current environment
-setenv [variable name] [variable value]: Changes an environment variable's value or adds it to the environment if it is not found
-unsetenv [variable name]: Deletes the variable from the environment
-cd [variable]: Changes directory to desired directory
-history: Displays the history of the last user submitted commands up to the value of the HISTSIZE environment variabible.
-exit: Exits iShell

iShell will search the PATH environment values for a the location of any command that is not listed above. If the user types "ls" and the command is located inside one of the paths in the PATH environment variable, iShell will run that command. However, if it is not found, an error will be displayed such as,
"iShell: [command]: command not found". Also, iShell will check the current working directory for the command as well.

Input/Output redirection has been implemented in iShell. The use of "<" for input or ">" for output is the standard for requesting a redirect. For example:
"ls > temp.dat" will run the "ls" command and output all the results in a new file "temp.dat" created by the redirection command. Likewise, 
"myprog < input.dat" will allow input.dat to be the input variables for the program myprog and then run myprog using those variables.

Piping is also implemented for iShell. However, as of this release, iShell currently is only operable with one pipe per command like. The underlying code structure exists to implement more than one pipe per command line, but this has not been fully developed as of now. Some precautions were taken to disallow the user to attempt redirecting input or output away from a pipe that was necessary to allow the pipe to function. An output redirect after the last argument to the right of the pipe is acceptable (example: "ls | grep x > temp.dat"). However, an input redirect on the right side of a pipe is currently unacceptable (example "ls | myprog < temp.dat").

Special notes:
----------------------------------------------------------------------------------------------------------------------------------------------------------
-No "clear history" command was implemented. In order to accomplish this, every time HISTSIZE is changed in the environment variable, the history is subsequently cleared. The history will begin recording from the very next command.
-History is limited to 100 entries. If the user attempts to set the history above 100, a message will be displayed explaining that this is unacceptable and that HISTSIZE is now set to 100.


Special thanks:
opengroup.org - for all the documentation on all these functions I had never seen before.
raspberryginger.com - Amazing resources for the programmer that gets stuck and needs another way to look at things. Especcially in Minix. Thanks.
Vern Paxon - For creating lex. No way I could have written this without it.
