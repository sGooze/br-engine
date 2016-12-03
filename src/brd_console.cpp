#include "inc/brd_console.h"

 std::string recordname_blank = "invalid";
 CON_Console consoleGlobal;
 std::vector< std::vector<std::string> > consoleArgs;

bool CON_Record_Register(std::string& name, void *var_ptr, paramType var_data_type, std::string cmd_help){
    if ((name.length() == 0)||(var_ptr == NULL))
        return false;
    CON_Record *rec = consoleGlobal.recordMan.GetElementPtr(name, true);
    if (rec->data_ptr != NULL){
        // Element already exists
        DEBUG_COUT << "Con_Record_Register: console record named \"" << name << "\" already exists!\n";
        return false;
    }
    rec->data_ptr = var_ptr;
    rec->data_type = var_data_type;
    rec->help_text = cmd_help;
    return true;
}

bool ConRecordLoaderFunc(std::string& name, CON_Record& record){
    record = CON_Record();
    return true;
}

void CMD_Invalid(){
    // TODO: May cause confusion if invalid command is located inside the macro?
    // TODO: Separate input parsing into a different object - to ease things out when a nested parse is necessary
    DEBUG_COUT << "Invalid command: " << consoleGlobal.consoleArgs[0] << std::endl;
}
