#include "inc/brd_object.h"
#include "inc/brd_manager.h"

std::string class_root("ent_null");
BRD_EntityMaker_Manager ent_class_table;

template<typename T, typename U> constexpr size_t offsetOfClassMember(U T::*member){
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

bool EntMakerLoaderFunc(std::string& name, BRD_EntityMaker& content){
    content = BRD_EntityMaker(name, NULL);
    content.params = BRD_EntityParameterTable();
    return true;
}

static BRD_Entity* ent_null(){
    return static_cast<BRD_Entity*>(new BRD_Entity);
}

void RegisterClass_ClassRoot(){
    DEBUG_COUT << " class \"" << class_root << "\"\n";
    BRD_EntityMaker *ent = ent_class_table.GetElementPtr(class_root, true);
    ent->func = ent_null;
    DEBUG_COUT << " /class \"" << class_root << "\"\n";
}

void RegisterClass_MdlStatic(){
    DEBUG_COUT << " class \"mdl_static\"\n";
    //std::string tempname = std::string("mdl_static");
    BRD_EntityMaker *ent = ent_class_table.GetElementPtr("mdl_static", true);
    ent->func = mdl_static;
    ent->params.add(TYPE_FILE, "mesh", offsetOfClassMember(&BRD_Model::mesh));
    ent->params.add(TYPE_MTL, "material", offsetOfClassMember(&BRD_Model::material));
    ent->params.add(TYPE_VEC3, "start_pos", offsetOfClassMember(&BRD_Model::position));
    ent->params.add(TYPE_VEC3, "rotation", offsetOfClassMember(&BRD_Model::angles));
    DEBUG_COUT << " /class \"mdl_static\"\n";
}

void BRD_InitClassTable(){
    RegisterClass_ClassRoot();
    RegisterClass_MdlStatic();
    DEBUG_COUT << "Class table initialized\n";
}
