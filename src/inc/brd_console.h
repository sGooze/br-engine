// brd_console.h - console interpreter and text parser

#ifndef __BRD_CONSOLE
#include "brd_shared.h"
#include "brd_manager.h"
#define __BRD_CONSOLE

class CON_Macro{
public:
    std::string macrocommand;
};

class CON_Record{
public:
    void *data_ptr;         // Either a variable pointer or a void(void) func pointer
    std::string help_text;
    paramType data_type;

    /*CON_Record(void(*CommandFuncPtr)(void), std::string cmd_help = "") : help_text(cmd_help){
        data_ptr = CommandFuncPtr;
        data_type = TYPE_FUNC;
    }*/
    CON_Record(void *var_ptr, paramType var_data_type, std::string cmd_help = "") : help_text(cmd_help){
        data_ptr = CommandFuncPtr;
        data_type = var_data_type;
    }
    CON_Record(void *data){
        data_type = TYPE_NULL;
        data_ptr = data;
    }
    CON_Record(){data_ptr = NULL;}
};

// Add new record to the record manager (use instead of constructors)
bool CON_Record_Register(std::string& name, void *var_ptr, paramType var_data_type, std::string cmd_help = ""); //bool local = false

bool ConRecordLoaderFunc(std::string& name, CON_Record&);
extern std::string recordname_blank = "wait";
typedef binTree_Manager<CON_Record, ConRecordLoaderFunc, &recordname_blank> CON_Record_Manager;

extern CON_Record_Manager consoleGlobal;

void ConsoleExampleWindow();

#endif // __BRD_CONSOLE
