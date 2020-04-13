#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "shell.h"

history_t* history;
int to_history;

    int main(int argc, char const *argv[])
    {
        printf("\n*************This is the SHELL**************\n***********by Olivia & Juan Carlos**********\n");

        history = Create_History();
        int count = 0;
        to_history = 1;

        while(1)
        {
            printf("$ ");
            int pc;
            int bg;
          
            command_t** commands = Get_Commands(&pc,&bg);
            Execute(commands,pc,bg);
        }

        return 0;
    }


    command_t** Get_Commands(int* pc, int* bg)
    {
        int count_commands = 1;
        char* line = Get_Line(&count_commands);

        line = Check_Exclamation(line);
        if(line == NULL) return NULL;


        if(to_history) Add_To_History(line);

        command_t** command = malloc(sizeof(command_t*) * (count_commands+1));
        count_commands = 0;

        command[count_commands] = malloc(sizeof(command_t));
        command[count_commands]->args = malloc(sizeof(char*) * 20);
        command[count_commands]->redir = malloc(sizeof(char*) * 5);

        int i = 0;
        int arg_index = 0;
        int redir_index = 0;
 
        while(line[i] != 0)
        {
            
            if(line[i] == ' ') SpaceKiller(line,&i);

            
            else if(line[i] == '|')
            {
                line[i] = 0;
                if(arg_index == 0) return NULL;

                command[count_commands]-> redir[redir_index] = NULL;
                command[count_commands]-> args[arg_index] = NULL;

                count_commands++;
                arg_index = redir_index = 0;

                command[count_commands] = malloc(sizeof(command_t));
                command[count_commands]->args = malloc(sizeof(char*) * 20);
                command[count_commands]->redir = malloc(sizeof(char*) * 5);
                i++;
            }
            
            else if(line[i] == '<' || line[i] == '>')
            {
                char* c = (line[i] == '<')? "<" : ">";

                if(line[i] == '>' && line[i+1] == '>')
                {
                    line[i] = 0;
                    i++;
                    c = ">>";
                }

                line[i] = 0;

                command[count_commands]-> redir[redir_index] = c;
                redir_index++;

                i++;
                if(line[i] == ' ') SpaceKiller(line,&i);
                

                command[count_commands]->redir[redir_index] = line + i;
                redir_index++;

                if(Get_Text(line,&i) == -1) return NULL;

                if(line[i] != 0 &&  line[i] != ' ') return NULL;
            }

            else
            {
                command[count_commands]->args[arg_index] = line + i;
                arg_index++;

                if(Get_Text(line,&i) == -1) return NULL;
            }

        }

        command[count_commands]->args[arg_index] = NULL;
        command[count_commands]->redir[redir_index] = NULL;
        command[count_commands + 1] == NULL;
        *pc = count_commands + 1;


        //Add_Command_To_History(command);
        return command;
    
    }
    
    // lee una linea y cuenta la cantidad de commandos.
    char* Get_Line(int* count_commands)
    {
        
        char* line = malloc(1024 * sizeof(char));
        char c;
        int j = 0;

        scanf("%c", &c);
        while(c != 10)
        {
            if(c == '|') *count_commands++;

            line[j] = c;
            j++;
            scanf("%c", &c);
        }

        line[j] = 0;
    
        //para no agregar comandos que empiecen con espacios.
        if(line[0] == ' ') to_history = 0;
        else to_history =1;

        int i = 0;
        while(line[i] == ' ')
        {
            i++;
            j--;
        }

        // poner al final de la linea un 0.
        line[i + j] = 0;

        while(line[j - 1] == ' ')
        {
            line[j - 1] = 0;
            j--;
        }

        char* aux = line;
        line = malloc(sizeof(char) * (j + 1));
        strcpy(line,aux + i);
        free(aux);
        
        return line;
    }

    history_t* Create_History()
    {
        history_t* history = malloc(sizeof(history_t));
        history->history = malloc(sizeof(char*) * 50);
        history->current = 0;
        return history;
    }

    void Add_To_History(char* command)
    {
        int len =  strlen(command);
        char* aux = malloc(sizeof(char)* len);
        strcpy(aux,command);


        if (aux[0] == ' ' || aux[0] == 0) return;

        if( history->current>0 && !strcmp(history->history[history-> current - 1],aux)) return;

        history->history[history->current % 50] = aux;
        history->current++;
    }

    void Print_History()
    {
        printf("\n*********Historial*********\n");
        int start;
        int current = history->current;

        if(current < 50) start = 0;
        else start = current % 50;

        int count = 0;
        int line_number = start + 1;

        for(int i = start; i < current && count < 50; count++)
        {
            printf("%d %s\n",line_number++,history->history[i]);
            if (i == 49) i = 0;
            else i++;
        }
        printf("*********Historial*********\n");
    }

    void SpaceKiller(char* line, int* i)
    {
        int aux = *i;
        line[aux] = 0;
        aux++;

        while(line[aux] == ' ')
        {
            line[aux] == 0;
            aux++;
        }
        *i = aux;

    }

    int Get_Text(char* line, int* pos)
    {
        int i = *pos;
        while(line[i] != 0 && line[i] != '<' && line[i] != '>' && line[i] != '|' &&  line[i] != ' ' )
        {
            if(line[i] == '"') 
            {
                i++;
                while(line[i] != '"' && line[i] != 0)
                {
                    line[i-1] = line[i];
                    i++;
                }

                if(line[i] == 0) return -1;

                i++;
                while(line[i] != ' ' && line[i] != 0)
                {
                    line[i-2] = line[i];
                    i++;
                }
                line[i-1] = line[i-2] = 0;
            }
            else i++;
        }
        *pos = i;
        return 0;
    }

    void Execute(command_t** commands, int pc, int bg)
    {
        // if(commands == NULL) return;
        if(commands == NULL || commands[0]->args[0] == NULL) return;

        int i = 0;
        int redir_index = 0;
        command_t* command;

        int pipes[pc -1][2];
        
        for(i = 0; i < pc -1; i++)
        {
            pipe(pipes[i]);
        }
        
        int status;

        
        for(i = 0; i < pc; i++)
        {
            command = commands[i];

            if(!strcmp(command->args[0],"cd"))
            {
                if(CD(command->args) == -1) return;
            }

            else if(!strcmp(command->args[0],"exit"))
            {
                exit(0);
            }


            int child_pid = fork();
            if(child_pid == 0)
            {
                if(i == 0 && pc > 1)
                    dup2(pipes[i][1],STDOUT_FILENO);

                else if(i == pc - 1 && pc > 1)
                    dup2(pipes[i-1][0],STDIN_FILENO);

                else if(pc > 1)
                {
                    dup2(pipes[i][1],STDOUT_FILENO);
                    dup2(pipes[i-1][0],STDIN_FILENO);
                }


                while(command->redir[redir_index] != NULL)
                {
                    if(!strcmp(command->redir[redir_index],"<"))
                    {
                        redir_index++;
                        int fd = open(command->redir[redir_index],O_RDONLY);
                        if(fd == -1)
                        {
                            perror(NULL);
                            exit(0);
                        }
                        dup2(fd,STDIN_FILENO);
                        redir_index++;
                    }
                    else
                    {
                        int flags = (!strcmp(command->redir[redir_index],">"))? O_WRONLY|O_CREAT|O_TRUNC:O_WRONLY|O_CREAT|O_APPEND;
                        redir_index++;
                        int fd = open(command->redir[redir_index],flags,0664);
                        if(fd == -1)
                        {
                            perror(NULL);
                            exit(0);
                        }
                        dup2(fd,STDOUT_FILENO);
                        redir_index++;

                    }

                }

                if(execvp(command->args[0],command->args) == -1)
                {
                    if(errno == ENOENT)
                    {
                        if(!strcmp(command->args[0],"history"))
                        {
                            Print_History();
                        }
     
                    }

                    else perror(command->args[0]);
                    exit(0);
                }

            }

            else
            {
                if(i == 0 && pc > 1)
                    close(pipes[i][1]);

                else if(i == pc - 1 && pc > 1)
                    close(pipes[i-1][0]);

                else if(pc > 1)
                {
                    close(pipes[i][1]);
                    close(pipes[i-1][0]);
                }

                 waitpid(child_pid,&status,0);

    
            }


        }
        

    }


    int CD(char** cd)
    {
        if(cd[1] == NULL) return 0;
        if(chdir(cd[1]) == -1)
        {
            perror("cd");
            return -1;

        }
        return 0;
    }


    char* Check_Exclamation(char* line)
    {

        int i = 0;
        while(line[i] != 0)
        {
            if(line[i] == '!')
            {
                line[i] = 0;
                if(line[i+1] == '!')
                {
                    if(history->current == 0) 
                    {
                        perror("ERROR: No hay comando previo");
                        return NULL;
                    }

                    line[i+1] = 0;
                    i+=2;
                    char* aux1 = line;
                    char* aux2 = line + i;

                    char* last_line = history->history[history->current -1];
                    int len1 = strlen(aux1);
                    int len2 = strlen(aux2);
                    int len3 = strlen(last_line);

                    
                    line = malloc(len1 + len2 + len3);

                    strcat(line,aux1);
                    strcat(line,last_line);
                    strcat(line,aux2);
                }

                else
                {
                    i++;
                    char* arg = malloc(sizeof(char) * 20);
                    char* aux = line + i;
                    int j = 0;

                    while(line[i] != 0 && line[i] != '<' && line[i] != '>' && line[i] != '|' &&  line[i] != ' ' )
                    {
                        arg[j] = line[i];
                        i++;
                        j++;

                    }


                    // si es un comando.
                    if(arg[0] > 57)
                    {
                        
                        int count = history->current - 1;
                        char* arg_hist = malloc(sizeof(char)*30);
                        char* aux = history->history[count];
                        
                        while(count >= 0)
                        {
                            aux = history->history[count];
                            arg_hist = malloc(sizeof(char)*30);
                            
                            int k =0;
                            int j =0;

                            while(aux[k] != 0 && aux[k] != '<' && aux[k] != '>' && aux[k] != '|' &&  aux[k] != ' ' && aux[k] != 10)
                            {
                                arg_hist[j] = history->history[count][k];
                                k++;
                                j++;
                                
                            }

                            if(!strcmp(arg_hist,arg))
                            {
                                printf("%s\n",arg_hist);
                                char* aux1 = line;
                                char* aux2 = line + i;
                                char* number_line = history->history[count];

                                int len1 = strlen(aux1);
                                int len2 = strlen(aux2);
                                int len3 = strlen(number_line);


                                line = malloc(len1 + len2 + len3);

                                strcat(line,aux1);
                                strcat(line,number_line);
                                strcat(line,aux2);
                                break;

                            }

                            count--;

                        }
                        
                        if(count == -1)
                        {
                            perror("ERROR: Comando no encontrado");
                            return NULL;
                        }
                        
                        
                        

                    }
                    // si es un numero.
                    else
                    {
                        char* aux1 = line;
                        char* aux2 = line + i;

                        int number = atoi(arg);
                        if(number > history->current)
                        {
                            perror("ERROR: No hay sufientes comandos");
                            return NULL;
                        }

                        char* number_line = history->history[number-1];

                        int len1 = strlen(aux1);
                        int len2 = strlen(aux2);
                        int len3 = strlen(number_line);


                        
                        line = malloc(len1 + len2 + len3);

                        strcat(line,aux1);
                        strcat(line,number_line);
                        strcat(line,aux2);
              
                    }
                    
                }

            }

            i++;

        }
        return line;
        
    }

