#include "inc/brd_scene.h"

// TODO: Read some theory on text parsers: there should be another way
// instead of writing, basically, separate parser for each content type. Maybe, something like rule-based system?

/* * * * * * * * *   Dummy loaders for manager trees   * * * * * * * * */

static bool CheckForExistence(std::string& path){
    if (FILE* ifExists = fopen(path.c_str(), "r")){
        fclose(ifExists);
        return true;
    } else{
        DEBUG_COUT << "Unable to load file \"" << path << "\"\n";
        return false;
    }
}

std::string dummyroot("textures/error.png");
bool TextureLoaderFunc(std::string& path, BRD_Texture2D& content){
    if (CheckForExistence(path) == false)
        return false;
    content = BRD_Texture2D(path.c_str());
    return true;
}

std::string material_root("ErrorMat");
bool MaterialLoaderFunc(std::string& name, BRD_Material& content){
    content = BRD_Material();
    return true;
}

std::string mesh_root("models/error.obj.vlist");
bool MeshLoaderFunc(std::string& path, BRD_Mesh& content){
    if (CheckForExistence(path) == false)
        return false;
    content = BRD_Mesh(path.c_str());
    return true;
}

bool EntityLoaderFunction(std::string& classname, BRD_Entity*& content){
    BRD_EntityMaker* em = ent_class_table.GetElementPtr(classname);
    if (classname != em->classname){
        DEBUG_COUT << "ERROR: " << classname << " - unknown classname\n";
        return false;
    }
    content = em->func();
    content->classmkr = em;
    return true;
}

std::string shader_root("DummyShader");

bool ShaderLoaderFunc(std::string& name, Shader& content){
    content = Shader();
    return true;
}
/* * * * * * * * *       Common parser functions       * * * * * * * * */

tokenVec StrToTokenVec(std::string str){
    // Search for substrings separated with whitespaces
    // Parts of string, enclosed in quotation marks, should be
    // treated as a single token, regardless of whitespace presence
    // TODO: One string spliced by pointers instead of separate strings for each token?
    tokenVec tkens;
    bool inQuotes = false;
    int i = 0;
    std::string pfx;
    while (str.length() > 0){
        int pos, quotePos;
        if (inQuotes == true){
            pos = str.find_first_of("\"");
            if (pos != std::string::npos)
                str.erase(str.begin()+pos);
            inQuotes = false;
        } else{
            pos = str.find_first_of(" \t"); // TODO: Standardize whitespace list
            if (pos == 0){
                str.erase(str.begin());
                continue;
            }
        }
        if (pos == std::string::npos){
            // Check for quotes: if whole string is a single, whitespace-less block, they should be erased
            while ((quotePos = str.find_first_of("\"")) != std::string::npos)
                str.erase(str.begin() + quotePos);
            // No more whitespaces/quotes? Pass the rest of the string as the last token
            tkens.push_back(str);
            return tkens;
        }
        // Generate token substring, check for quotation marks inside
        std::string subs = str.substr(0, pos);
        quotePos = subs.find_first_of("\"");
        if (quotePos != std::string::npos){
            inQuotes = true;
            str.erase(str.begin() + quotePos);
            continue;
        }
        tkens.push_back(subs);
        str = str.substr(pos);
    }
    return tkens;
}

/* * * * * * * * *       Scene parsing functions       * * * * * * * * */

static void TokenContents(tokenVec& tkens){
    DEBUG_COUT << "  broken into " << tkens.size() << " tokens!\n{\n";
    for (int i = 0; i < tkens.size(); i++)
        DEBUG_COUT << "\t" << tkens[i] << std::endl;
    DEBUG_COUT << "}\n\n";
}

class ParserInfo{
public:
    std::string name;
    bool *params_set;
    bool inDesc, closed, failed;
    void *object;

    ParserInfo(unsigned int params_num){
        name = "";
        params_set = new bool[params_num];
        for (int i = 0; i < params_num; i++)
            params_set[i] = false;
        inDesc = closed = failed = false;
    }
    void AddObject(std::string& n, void *obj){
        if (closed == true)
            return;
        name = n; object = obj;
    }
    bool HasName(){
        return (name.length() > 0);
    }
    bool IsComplete(unsigned int params_num){
        if ((HasName() == true)&&(closed == true)){
            for (int i = 0; i < params_num; i++)
                if (params_set == false)
                    return false;
            return true;
        }
        return false;
    }
};
static ParserInfo *info = NULL;

typedef bool (*ParseFunc) (BRD_Scene*, std::string&);

/* * * * * * * * *       Parse shader       * * * * * * * * */

static bool ParseShader(BRD_Scene* scene, std::string& str){
    DEBUG_COUT << "SHADER: " << str << std::endl;
    tokenVec tkens = StrToTokenVec(str);
    if (tkens.size() <= 0)
        return true;

    //TokenContents(tkens);

    if (info == NULL){
        info = new ParserInfo(0);
    }
    else if (info->closed == true){
        delete info;
        info = new ParserInfo(0);
    }
    Shader *shd = (Shader*)info->object;
    try{
        for (int i = 0; i < tkens.size(); i++){
            DEBUG_COUT << " DEBUG: token " << i << " = '" << tkens[0] << "'\n";
            if (tkens[i].length() == 0)
                continue;
            if (tkens[i].find("//") != std::string::npos)
                break;
            // Parse braces
            if (tkens[i][0] == '{'){
                if (info->inDesc == true){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: nested braces in the description!\n";
                    throw i;
                }
                if (info->HasName() == false){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: trying to register a material without a name!\n";
                    throw i;
                }
                info->inDesc = true;
                continue;
            }
            if (tkens[i][0] == '}'){
                if (info->inDesc == false){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: invalid placement of the closing brace!\n";
                    throw i;
                }
                info->inDesc = false;
                info->closed = true;
                break;
            }
            if (info->HasName() == false){
                shd = scene->ShdMan.GetElementPtr(tkens[i], true);
                info->AddObject(tkens[i], (void*)shd);
            } else{
                if (info->inDesc == false){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: token outside of a description!\n";
                    throw i;
                }
                if ((i == 0)&&(tkens[i][0] == '$')){
                    // Got a shader type: use the next token as a shader path
                    if (tkens.size() < 2){
                        DEBUG_COUT << " ERROR [token " << i+1 << "]: token without attached value: \"" << tkens[1] << "\"!\n";
                        break;
                    }
                    GLint tmp;
                    if (tkens[0] == "$vertex")
                        tmp = shd->CompileShaderCode(tkens[1].c_str(), GL_VERTEX_SHADER);
                    else if (tkens[0] == "$fragment")
                        tmp = shd->CompileShaderCode(tkens[1].c_str(), GL_FRAGMENT_SHADER);
                    else{
                        DEBUG_COUT << " ERROR [token " << i+1 << "]: unrecognized token: \"" << tkens[0] << "\"!\n";
                        break;
                    }
                    if (tmp == -1)
                        DEBUG_COUT << " ERROR [token " << i+1 << "]: unable to compile shader: \"" << tkens[1] << "\"!\n";
                    break;
                }
            }
        }
    } catch (int i){
        DEBUG_COUT << " Shader loading failed: description improperly formatted. (error on line " << i << ")\n";
        info->closed = true;
        scene->ShdMan.DeleteLeaf(info->name);
        return false;
    }
    // After filling out parse info, finalize material by filling blank spaces
    if (info->closed == true){
        DEBUG_COUT << "Shader \"" << info->name << "\" ";
        if (shd->CompileProgram() == true)
            DEBUG_COUT << " was successfully loaded!\n";
        else{
            DEBUG_COUT << " failed to load!\n";
            scene->ShdMan.DeleteLeaf(info->name);
        }
    }
    return true;
}

/* * * * * * * * *       Parse material       * * * * * * * * */

#define PARAMS_IN_MATERIAL 4

static void ParseMaterial_LoadTex(BRD_Scene* scene, std::string& param, std::string& next_param){
    // Loads and links textures to the correct parameters of the material.
    // If texture loading fails, default texture (tex. manager root) is linked instead.
    BRD_Material *mat = (BRD_Material*)info->object;
    // TODO: Replace this monstrosity with something more comprehensive
    DEBUG_COUT << "  LoadTex pars: p = " << param << "\n              : n = " << next_param << std::endl;
    switch (param[2]){
    case 'i': // $dIffuse
        if (info->params_set[0] != false) break;
        mat->diffuse  = scene->TexMan.GetElementPtr(next_param, true); info->params_set[0] = true; return;
    case 'p': // $sPecular
        if (info->params_set[1] != false) break;
        mat->specular = scene->TexMan.GetElementPtr(next_param, true); info->params_set[1] = true; return;
    case 'm': // $eMissive
        if (info->params_set[2] != false) break;
        mat->emission = scene->TexMan.GetElementPtr(next_param, true); info->params_set[2] = true; return;
    case 'h': // $sHininess
        if (info->params_set[3] != false) break; DEBUG_COUT << " Adding SHININESS = " << atoi(next_param.c_str()) << std::endl;
        mat->shininess = atoi(next_param.c_str()); info->params_set[3] = true; return;
    default:
        DEBUG_COUT << " Warning: unrecognized parameter \"" << param << "\"\n"; return;
    }
    DEBUG_COUT << " Warning: skipped duplicate of parameter \"" << param << "\"\n"; return;
}

static void ParseMaterial_LoadDefault(BRD_Scene* scene, int param){
    BRD_Material *mat = (BRD_Material*)info->object;
    info->params_set[param] = true;
    switch (param){
    case 0:
        mat->diffuse = scene->TexMan.GetElementPtr(dummyroot); return;
    case 1:
        mat->specular = scene->TexMan.GetElementPtr("textures/blankw.png", true); return;
    case 2:
        mat->emission = scene->TexMan.GetElementPtr("textures/blank.png", true); return;
    case 3:
        mat->shininess = 32; return;
    }
}

static bool ParseMaterial(BRD_Scene* scene, std::string& str){
    /* Material description is structured like the following template:

	"material_name"
	{
		$diffuse "diffuse_tex_path"       // Param 0, default = TexMan root
		$specular "specular_tex_path"     // Param 1, default = "textures/blank.png"
		$emissive "emissive_tex_path"     // Param 2, default = "textures/blank.png"
		$shininess 64                     // Param 3, default = 0
	}

       For each material parameter an appropriate token is found.
       If there is no such token, default value is passed.
    */
    DEBUG_COUT << "MAT: " << str << std::endl;
    tokenVec tkens = StrToTokenVec(str);
    if (tkens.size() <= 0)
        return true;

    //TokenContents(tkens);

    if (info == NULL){
        info = new ParserInfo(PARAMS_IN_MATERIAL);
    }
    else if (info->closed == true){
        delete info;
        info = new ParserInfo(PARAMS_IN_MATERIAL);
    }
    BRD_Material *mat = (BRD_Material*)info->object;
    try{
        for (int i = 0; i < tkens.size(); i++){
            DEBUG_COUT << " DEBUG: token " << i << " = '" << tkens[i] << "'\n";
            if (tkens[i].length() == 0)
                continue;
            if (tkens[i].find("//") != std::string::npos)
                break;
            // Parse braces
            if (tkens[i][0] == '{'){
                if (info->inDesc == true){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: nested braces in the description!\n";
                    throw i;
                }
                if (info->HasName() == false){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: trying to register a material without a name!\n";
                    throw i;
                }
                info->inDesc = true;
                continue;
            }
            if (tkens[i][0] == '}'){
                if (info->inDesc == false){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: invalid placement of the closing brace!\n";
                    throw i;
                }
                info->inDesc = false;
                info->closed = true;
                break;
            }
            if (info->HasName() == false){
                mat = scene->MatMan.GetElementPtr(tkens[i], true);
                info->AddObject(tkens[i], (void*)mat);
            } else{
                if (info->inDesc == false){
                    if (tkens[i][0] == ':'){
                        // We're still outside of the description, but the next token is valid and contains shader name
                        if (tkens.size() < i+1){
                            DEBUG_COUT << "  WARN [token " << i+1 << "]: no shader name specified for material \""
                            << info->name << "\"; using default shader\n";
                            break;
                        }
                        DEBUG_COUT << "total: " << tkens.size();
                        DEBUG_COUT << " Got :, next token is " << tkens[i+1];
                        DEBUG_COUT << "\n";
                        mat->shader = scene->ShdMan.GetElementPtr(tkens[i+1]);
                        if (mat->shader == NULL){
                            // Something horrible happened: shader manager is empty! This should never happen...
                            DEBUG_COUT << "  ERROR [token " << i+1 << "]: invalid shader name \"" << tkens[i+1]
                            << "\"specified for material \"" << info->name << "\"; shader manager is NULL\n";
                            throw i;
                        }
                        break;
                    } else{
                        DEBUG_COUT << " ERROR [token " << i+1 << "]: token outside of a description!\n";
                        throw i;
                    }
                }
                if ((i == 0)&&(tkens[i][0] == '$')){
                    // Got a parameter name: use the next token as a texture name
                    if (tkens.size() < 2)
                        break;
                    ParseMaterial_LoadTex(scene, tkens[i], tkens[i+1]);
                }
            }
        }
    } catch (int i){
        DEBUG_COUT << " Material loading failed: description improperly formatted. (error on line " << i << ")\n";
        info->closed = true;
        // Remove material
        // TODO: What kinda worries me is skipping remaining lines in a definition block that was
        //       considered inappropriate.
        //delete mat;
        scene->MatMan.DeleteLeaf(info->name);
        return false;
    }
    // After filling out parse info, finalize material by filling blank spaces
    if (info->closed == true){
        for (int i = 0; i < PARAMS_IN_MATERIAL; i++)
            if (info->params_set[i] == false)
                ParseMaterial_LoadDefault(scene, i);
        if (mat->shader == NULL)
            mat->shader = scene->ShdMan.GetElementPtr(shader_root);
        DEBUG_COUT << "Material \"" << info->name << "\" ";
        if (info->IsComplete(PARAMS_IN_MATERIAL) == true){
            DEBUG_COUT << " was successfully loaded!\n";
            DEBUG_COUT << "Diffuse = " << mat->diffuse->tpath << std::endl;
            DEBUG_COUT << "Specular = " << mat->specular->tpath << std::endl;
            DEBUG_COUT << "Emissive = " << mat->emission->tpath << std::endl;
            DEBUG_COUT << "Shininess = " << mat->shininess << std::endl;
        }
        else{
            DEBUG_COUT << " failed to load!\n";
            //delete mat;
            scene->MatMan.DeleteLeaf(info->name);
        }
    }
    return true;
}

#undef PARAMS_IN_MATERIAL

/* * * * * * * * *       Parse entity       * * * * * * * * */

static BRD_EntityMaker *mkr = NULL;

static glm::vec3 TokenToVec3(std::string& token){
    // Vectors definitions should be formatted as following:
    // $vector_field "0.0 0.0 0.0"
    DEBUG_COUT << "|TokenToVec3 - got token \"" << token << "\"\n";
    tokenVec vec_fields = StrToTokenVec(token);
    TokenContents(vec_fields);
    if (vec_fields.size() != 3){
        DEBUG_COUT << "WARNING: Improperly formatted vec3 definition: \"" << token << "\"\n";
        return glm::vec3(0.0, 0.0, 0.0);
    }
    //return glm::vec3((float)atof(vec_fields[0].c_str()), (float)atof(vec_fields[1].c_str()), (float)atof(vec_fields[2].c_str()));
    glm::vec3 fin = glm::vec3((float)atof(vec_fields[0].c_str()), (float)atof(vec_fields[1].c_str()), (float)atof(vec_fields[2].c_str()));
    DEBUG_COUT << "Vector: " << fin.x << " " << fin.y << " " << fin.z << std::endl;
    return fin;
}

static void ParseTokenToVec3(std::string& token, void *addr){
    // TokenToVec3 approach crashes and burns, so this will be used instead - at least, for now
    DEBUG_COUT << "|ParseTokenToVec3 - got token \"" << token << "\"\n";
    tokenVec vec_fields = StrToTokenVec(token);
    TokenContents(vec_fields);
    glm::vec3 *fin = static_cast<glm::vec3*>(addr);
    if (vec_fields.size() != 3){
        DEBUG_COUT << "WARNING: Improperly formatted vec3 definition: \"" << token << "\"\n";
        fin->x = fin->y = fin->z = 0;
    }
    fin->x = (float)atof(vec_fields[0].c_str());
    fin->y = (float)atof(vec_fields[1].c_str());
    fin->z = (float)atof(vec_fields[2].c_str());
    DEBUG_COUT << "Vector: " << fin->x << " " << fin->y << " " << fin->z << std::endl;
}

static bool ParseEntity_ParseTokens(tokenVec& tkens, BRD_Scene* scene){
    // Expects tokens to be grouped by pairs ($key "value")
    // Remove '$' and try to find key in the current entity maker table
    // ADD EVERYTHING REGARDING ENTITY GENERATION FROM THE NOTES
    // Use the data type of the appropriate field in the switch/case to correctly construct
    // the pointer to the data
    // Return false if key does not correspond to any fields in the table
    DEBUG_COUT << "ParseTokens: " << tkens[0];
    BRD_Entity *ent = (BRD_Entity*)info->object;
    tkens[0].erase(0, 1);
    BRD_EntityParameter *par = ent->classmkr->params.find(tkens[0]);
    if (par == NULL){
        DEBUG_COUT << " - unknown token \n";
        return false;
    }
    void *ent_offset = ent + par->offset;
    switch (par->type){
    case TYPE_FILE:
        // TODO: File type detector function
        DEBUG_COUT << " - FILE field, offset = " << par->offset << std::endl;
        return true;
    case TYPE_FLOAT:
        DEBUG_COUT << " - FLOAT field, offset = " << par->offset << std::endl;
        *static_cast<float*>(ent_offset) = (float)atof(tkens[1].c_str());
        return true;
    case TYPE_INT:
        DEBUG_COUT << " - INT field, offset = " << par->offset << std::endl;
        *static_cast<int*>(ent_offset) = (int)atoi(tkens[1].c_str());
        return true;
    case TYPE_STRING:
        DEBUG_COUT << " - STRING field, offset = " << par->offset << std::endl;
        *static_cast<std::string*>(ent_offset) = tkens[1];
    case TYPE_VEC3:
        DEBUG_COUT << " - VEC3 field, offset = " << par->offset << std::endl;
        //*static_cast<glm::vec3*>(ent_offset) = TokenToVec3(tkens[1]);
        ParseTokenToVec3(tkens[1], ent_offset);
        return true;
    case TYPE_MTL:
        DEBUG_COUT << " - MATERIAL field, offset = " << par->offset << std::endl;
        *static_cast<BRD_Material**>(ent_offset) = scene->MatMan.GetElementPtr(tkens[1]);
        return true;
    }
    return false;
}

static bool ParseEntity(BRD_Scene* scene, std::string& str){
//    BRD_EntityMaker* mkr = ent_class_table.GetElementPtr(/*classname token*/);
    tokenVec tkens = StrToTokenVec(str);
    if (tkens.size() <= 0)
        return true;

    // Entity parsing will not use params table - all default params should be set in the constructor
    if (info == NULL){
        //info = new ParserInfo(mkr.params.table.size());
        info = new ParserInfo(0);
    }
    else if (info->closed == true){
        delete info;
        info = new ParserInfo(0);
    }

    TokenContents(tkens);
    BRD_Entity *ent = (BRD_Entity*)info->object;
    try{
        for (int i = 0; i < tkens.size(); i++){
            DEBUG_COUT << " DEBUG: token " << i << " = '" << tkens[0] << "'\n";
            if (tkens[i].length() == 0)
                continue;
            if (tkens[i].find("//") != std::string::npos)
                break;
            // Parse braces
            if (tkens[i][0] == '{'){
                if (info->inDesc == true){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: nested braces in the description!\n";
                    throw i;
                }
                if (info->HasName() == false){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: trying to register an entity without a name!\n";
                    throw i;
                }
                info->inDesc = true;
                continue;
            }
            if (tkens[i][0] == '}'){
                if (info->inDesc == false){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: invalid placement of the closing brace!\n";
                    throw i;
                }
                info->inDesc = false;
                info->closed = true;
                break;
            }
            // Parse classname
            if (tkens[i].find(":") != std::string::npos){
                if (tkens.size() < i+1){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: classname isn't specified!\n";
                    throw i;
                }
                // If token[i] is the colon, then [i+1] is the classname, and [i-1] is the entity name
                if (info->HasName() == false){
                    // Add new item using classname as the name, but save the real name to the ParseInfo,
                    // so that at the end of item generation we could correctly rename it
                    ent = scene->EntMan.GetElement(tkens[i+1], true);
                    info->AddObject(tkens[i-1], (void*)ent);
                    return true;
                }
            }

            if (info->HasName() == true){
                if (info->inDesc == false){
                    DEBUG_COUT << " ERROR [token " << i+1 << "]: token outside of the description!\n";
                    throw i;
                }
                if ((i == 0)&&(tkens[i][0] == '$')&&(tkens.size() >= 2)){
                    return ParseEntity_ParseTokens(tkens, scene);
                }
            }
        }
    } catch (int i){
        DEBUG_COUT << " Entity loading failed: (error on line " << i << ")\n";
        info->closed = true;
        mkr = NULL;
        // Remove entity
        // Don't forget to manually delete ent, since entities are stored in leaves as pointers
        delete ent;
        scene->EntMan.DeleteLeaf(ent->classmkr->classname);
        return false;
    }
    // After filling out parse info, finalize entity by naming it
    if (info->closed == true){
        scene->EntMan.ChangeName(ent->classmkr->classname, info->name);
        DEBUG_COUT << "New entity added: \"" << info->name << "\" ";
    }
    return true;
}

/* * * * * * * * *       Scene parsing constructor       * * * * * * * * */

typedef enum{PARSE_SKIP, PARSE_MATERIAL, PARSE_ENTITY, PARSE_SHADER} ParseMod;
static std::string _ptkens[] = {"skip", "materials", "entities", "shaders"};
static ParseFunc pfptrs[] = {NULL, ParseMaterial, ParseEntity, ParseShader};

BRD_Scene::BRD_Scene(const char* path, bool addin){
    // Reads the scene file, formats it into token array (or, at least, cuts off leading whitespace),
    // and transfers that array to the line parser function, specified by the current parse mode
    // TODO: scene should be loaded from scratch only when specified to do so! By default all data from the scene file
    //  should be loaded alongside previously loaded data to enable loading scenes divided into multiple files.
    //  This probably means that all this code should not be located in the constructor, but in the separate scene
    //  parser function instead.
    // addin = false -> unload current scene before loading new data
    // addin = true  -> load new data
    std::ifstream sceneFile;
    std::stringstream sceneStream;
    std::string sceneString;
    sceneFile.exceptions(std::ifstream::badbit);
    init = false;
    try{
        DEBUG_COUT << "Loading scene file \"" << path << "\"\n";
        sceneFile.open(path);
        sceneStream << sceneFile.rdbuf();
        sceneFile.close();

        ParseMod parseMod = PARSE_SKIP;

        int line = 0;
        while (std::getline(sceneStream, sceneString)){
            line++;
            int pos = sceneString.find_first_not_of(" \t\r\n");
            if (pos > 0)
                sceneString = sceneString.substr(pos);
            if (sceneString[0] == '#'){
                // Parse label encountered: change parse mode
                sceneString.erase(0, 1);
                bool func_found = false;
                int i;
                for (i = 0; i < (sizeof(pfptrs)/sizeof(ParseFunc)); i++)
                    if (sceneString == _ptkens[i])
                        {func_found = true; break;}
                if ((func_found == false)||(i == 0))
                    parseMod = PARSE_SKIP;
                else
                    parseMod = (ParseMod)i;
                continue;
            }
            // Skip comment and empty lines prematurely
            if (((sceneString[0] == '/')&&(sceneString[1] == '/'))||(sceneString.empty())||(parseMod == PARSE_SKIP))
                continue;
            if (pfptrs[(int)parseMod](this, sceneString) == false)
                DEBUG_COUT << "[" << line << "]: line parsed unsuccessfully\n";
        }
    } catch (std::ifstream::failure f){
        DEBUG_COUT << "BRD_Scene::BRD_Scene: An I/O error has occurred while trying to parse the file \"" << path << "\"\n";
        // TODO: Scene loading halts the loading procedure and returns game into main menu/console
    }
    DEBUG_COUT << "\n\n\n";
    init = true;
}
