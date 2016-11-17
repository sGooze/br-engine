#include "inc/brd_console.h"

extern CON_Record_Manager consoleGlobal;

bool CON_Record_Register(std::string& name, void *var_ptr, paramType var_data_type, std::string cmd_help = ""){
    if ((name.length() == 0)||(var_ptr == NULL))
        return false;
    CON_Record *rec;
    if (consoleGlobal.GetElementPtr(name, true)->data_ptr != NULL){
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
