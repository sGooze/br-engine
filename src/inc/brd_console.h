// brd_console.h - console interpreter and text parser

#ifndef __BRD_CONSOLE
#include "brd_shared.h"
#include "brd_manager.h"
#define __BRD_CONSOLE

// Parser stuff - will be here for a while
typedef std::vector<std::string> tokenVec;

class CON_Macro{
public:
    std::string macrocommand;
};

typedef void (*voidfunc(void));
extern  void CMD_Invalid();

// C STANDARD DOES NOT ALLOW CONVERSION BETWEEN DATA AND FUNCTION POINTERS

class CON_Record{
private:
    void *data_ptr;         // Either a variable pointer or a void(void) func pointer
    std::string help_text;
    paramType data_type;

public:
    CON_Record(void *var_ptr, paramType var_data_type, std::string cmd_help = "") : help_text(cmd_help){
        data_ptr = var_ptr;
        data_type = var_data_type;
    }
    CON_Record(void *data){
        data_type = TYPE_NULL;
        data_ptr = data;
    }
    CON_Record(){data_ptr = CMD_Invalid; data_type = TYPE_FUNC;}
    void Execute(){
        if (data_type == TYPE_FUNC)
            (*(voidfunc)data_ptr)();
        else
            DEBUG_COUT << *data_ptr;
    }
    paramType GetDataType(){return data_type;}
    std::string& GetHelpText(){return help_text;}
    // Add new record to the record manager (use instead of constructors)
    // TODO: bool local = false - registers new record in the scene manager instead of a global one
    friend bool CON_Record_Register(std::string& name, void *var_ptr, paramType var_data_type, std::string cmd_help = "");
};

bool ConRecordLoaderFunc(std::string& name, CON_Record&);
extern std::string recordname_blank;
typedef binTree_Manager<CON_Record, ConRecordLoaderFunc, &recordname_blank> CON_Record_Manager;


class CON_Console{
private:

public:
    CON_Record_Manager recordMan;
    //std::vector< std::vector<std::string> > consoleArgs;
    std::vector< std::string > consoleArgs;
    short cmdCurrent = 0;
    tokenVec GetTokens(std::string&);
    bool read(std::string& input){
        if (input.length() == 0){
            consoleArgs.clear();
            return true;
        }
        consoleArgs = GetTokens(input);
        CON_Record* rec = recordMan.GetElementPtr(consoleArgs[0]);
        if (rec->GetDataType() != TYPE_FUNC)
            DEBUG_COUT << " " << consoleArgs[0] << ": ";
        rec->Execute();
        DEBUG_COUT << std::endl;
    }
};

extern CON_Console consoleGlobal;
//extern CON_Record_Manager consoleGlobal;
//extern std::vector< std::vector<std::string> > consoleArgs;

void ConsoleExampleWindow();

// Typedefs for variables registered as cvars
typedef int cvar_int;
typedef char cvar_char;
typedef float cvar_float;
typedef double cvar_double;
typedef std::string cvar_string;

#endif // __BRD_CONSOLE
