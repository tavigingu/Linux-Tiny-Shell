#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 1000

char* get_line()
{
    char* buffer=(char*)malloc(sizeof(char) * BUFFER_SIZE);

    if (fgets(buffer, BUFFER_SIZE, stdin)== NULL){
        perror("Eroare la citirea comenzii");
        exit(EXIT_FAILURE);
    }

    buffer[strlen(buffer)]= '\0';
    return buffer;
    
}

char** parse_line(char* line)
{   
    char* line_copy = strdup(line);
    const char delimitator[]=" \n";

    char* token = strtok(line_copy, delimitator);

    char** words;
    words=(char**)malloc(sizeof(char*)*32);

    int index=0;
    while(token!=NULL)
    {
        words[index]=strdup(token);

        if(index%32==0) {
            words=(char**)realloc(words, sizeof(char)*(index+32));
        }

        token = strtok(NULL, delimitator);
        index++;
    }
    words[index]=NULL;

    return words;
}

void expand_variable(char** command)
{
    for(int i=0; command[i] !=NULL; i++)
    {
        if(command[i][0] == '$')
        {
            char* var_name = command[i] + 1;
            char* var_value = getenv(var_name);
           
            if(var_value!=NULL)
            {
                free(command[i]);
                command[i] = var_value;
            }
        }
    }
}

void simple_command(char** command, int in_fd, int out_fd)
{
    int pid = fork();
    if(pid<0){
        perror("Eroare la crearea procesului copil");
        exit(EXIT_FAILURE);
    }
    
    if(pid==0){    
        if(in_fd != STDIN_FILENO){   
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        if(out_fd != STDOUT_FILENO){
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        expand_variable(command);
        execvp(command[0], command);
        perror("Error");
        exit(EXIT_FAILURE);
    }
    else 
    {
        wait(NULL);
    }

}

int assignment_command(char* line)
{
    char* equal_sign = strchr(line, '=');
    if(equal_sign != NULL)
    {
        *equal_sign = '\0';
        char* var_name = line;
        char* var_value = equal_sign + 1;
        setenv(var_name, var_value, 1);
        return 1;
    }
    else
        return 0;
}

int contains_redirect(const char* line) {
    return strstr(line, " > ") != NULL ;
}

int redirect_output(char** command)
{   
    int i=0;
    while(command[i] != NULL)
    {
        if(strcmp(command[i], ">") == 0)
        {
            char* filename = command[i+1];

            int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
            if(fd<0) {
                perror("Eroare la deschiderea fisierului");
                return -1;
            }

            if(dup2(fd, STDOUT_FILENO)<0){
                perror("Eroare la redirectarea outputului");
                return -1;
            }

            close(fd);

            while(command[i]!=NULL){
                free(command[i]);
                command[i]=NULL;
                i++;
            }
        }

        i++;
    }
}

char** split_by_pipe(char* line, int* num_pipes)
{
    char** commands = (char**)malloc(sizeof(char*) * 32);
    char* token = strtok(line, "|");
    int index = 0;

    while(token!=NULL)
    {
        commands[index++]=strdup(token);
        token=strtok(NULL, "|");
    }

    commands[index]=NULL;

    
    *num_pipes = --index;
    return commands;
}

int main()
{   

    int original_stdin = dup(STDIN_FILENO);

    while(1){
        printf("> ");
        char *line = get_line();
        
        if(strcmp(line, "exit\n") ==0) {
            break;
        }

        int num_pipes =0;
        char** commands = split_by_pipe(line, &num_pipes);
        int in_fd = STDIN_FILENO;


        for(int index_command=0; index_command<=num_pipes; index_command++)
        {   
            int pipe_fd[2];
            if(index_command < num_pipes && pipe(pipe_fd) < 0){
                perror("Error creating pipe");
                exit(EXIT_FAILURE);
            }

            char **command = parse_line(commands[index_command]);

            if(index_command == num_pipes) // last command
                simple_command(command, in_fd, STDOUT_FILENO);
            else 
                simple_command(command, in_fd, pipe_fd[1]);

            if(contains_redirect(line))
                redirect_output(command);

          
            close(pipe_fd[1]); // close writing head
            if(in_fd != STDIN_FILENO)
                close(in_fd);

            in_fd = pipe_fd[0]; // update reading head for next command
           
        }

        dup2(original_stdin, STDIN_FILENO);
    }

    close(original_stdin);
    
}
