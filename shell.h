
typedef struct Command
{
    char** args;
    int args_count;
    char** redir;
    int redir_count;
}command_t;

typedef struct History
{
    char** history;
    int current;

}history_t;


///////FUNCTIONS///////

    command_t** Get_Commands(int* pc, int* bg);
    command_t** Get_Commands2(int* pc,char* line1);
    history_t* Create_History();
    char* Get_Line(int* command_count);
    char* Get_Line2(int* command_count,char* line1);
    void Add_To_History();
    void Print_History();
    void SpaceKiller(char* line, int* i);
    int Get_Text(char* line, int* i);
    void Execute(command_t** commands, int pc, int bg);
    int CD(char** cd);
    void Add_Command_To_History(command_t* command);
    char* Check_Exclamation(char* line);
    
///////FUNCTIONS///////

