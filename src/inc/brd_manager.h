// brd_manager.h - object binary tree manager template & classes

#ifndef __BRD_MANAGER
#include "brd_shared.h"
#define __BRD_MANAGER

// LOADER = int (*ContentLoad)(std::string path)
//template <typename TYPE, typename LOADER>
template <typename TYPE, bool (*LOADER)(std::string&, TYPE&), std::string *ROOT_CONTENT>
class binTree_Manager{
private:
    binTree_Manager* parent;
    binTree_Manager* leaves[2];
    unsigned int childCount;
    bool isFailure;
    std::string name;
    TYPE content;
    int CompareStrings(std::string word1, std::string word2){
            // 0 = word1 < word2; 1 = word1 > word2; -1 = ERROR (word1 = word2 [= NULL])
            // TODO: replace with std::string.compare(string&)
            // 0 - equal, <0: a > b; >0: a < b
            int i = 0;
            if (word1 == word2)
                return 2;
            if ((word1.length() == 0) && (word2.length() == 0))
                return -1;
            while(true){
                if ((word1[i] == '\0')||(word1[i] < word2[i]))
                    return 0;
                if ((word2[i] == '\0')||(word2[i] < word1[i]))
                    return 1;
                i++;
            }
    };
    binTree_Manager*    FindLeaf(std::string& path){
            if (path.length() == 0){
                std::cout << "binTree_Manager::FindLeaf: path is empty\n";
                return NULL;
            }
            if (isFailure == true){
                return NULL;
            }
            int i = CompareStrings(name, path);
            if (i == 2)
                return this;
            else if (leaves[i] != NULL)
                return leaves[i]->FindLeaf(path);
            std::cout << "binTree_Manager::FindLeaf - attempting to locate unloaded leaf \"" <<
            path << "\"" << std::endl;
            return NULL;
    }
    bool CheckIfExists(std::string& path){
        // TODO: Separate
        if (FILE* ifExists = fopen(path.c_str(), "r")){
            fclose(ifExists);
            return true;
        } else{
            std::cout << "Unable to load file \"" << path << "\"\n";
            return false;
        }
    }
    binTree_Manager*   AddElement(std::string& path){
        int i = CompareStrings(name, path);
        if (i != 2){
            binTree_Manager *leaf;
            if (leaves[i] != NULL)
                leaf = leaves[i]->AddElement(path);
            else
                leaf = leaves[i] = new binTree_Manager(this, path);
            if (leaf != NULL){
                if (leaf->isFailure == true)
                    // Something went wrong and the leaf failed to create
                    delete leaf;
                else
                    return leaf;
            }
        }
        if (path == name){
            //std::cout << path << "ALREADY LOADED\n";
            return this;
        }
        std::cout << "binTree_Manager::AddElement - failed to add new element.\n";
        return NULL;
    };
    void _GetAllElementsPtr(std::string& mask, std::vectors& elements){
        // return_root_when_failed is only used in the root leaf function
        if (mask.length() == 0){
            std::cout << "binTree_Manager::_GetAllElementsPtr: mask is empty\n";
            return;
        }
        int i = mask.compare(name.substr(0, mask.length()));
        if (i == 0){
            elements.push_back(&content);
        }
        for (int j = 0; j < 2; j++)
            if (leaves[j] != NULL)
                leaves[j]->_GetAllElementsPtr(mask, elements);
    }
public:
                        binTree_Manager(){
                                // Tree root constructor
                                std::cout << "CONSTRUCTING TREE ROOT\n";
                                parent = leaves[0] = leaves[1] = NULL;
                                if (LOADER(*ROOT_CONTENT, content) == false){
                                    std::cout << " Unable to load default file \"" << *ROOT_CONTENT << "\"!\n";
                                    system("pause");
                                    exit(1);
                                }
                                name = *ROOT_CONTENT; isFailure = false;
                        }
                        binTree_Manager(binTree_Manager* my_parent, std::string& path){
                                std::cout << "CONSTRUCTING TREE LEAF \"" << path << "\"\n";
                                parent = my_parent; name = path;
                                leaves[0] = leaves[1] = NULL;
                                if (LOADER(path, content) == false){
                                    std::cout << " Unable to correctly load file \"" << path << "\"!\n";
                                    isFailure = true;
                                    return;
                                }
                                isFailure = false;
                        };
                       ~binTree_Manager(){
                            for (int i = 0; i < 2; i++){
                                if (leaves[i] != NULL) delete leaves[i];
                                if (parent == NULL) break;
                                if (parent->leaves[i] == this) parent->leaves[i] = NULL;
                            }
                        };
    TYPE                GetElement(std::string& path, bool load_if_missing = false){
                            binTree_Manager* leaf = FindLeaf(path);
                            if (leaf != NULL)
                                return leaf->content;
                            if (load_if_missing == true){
                                leaf = AddElement(path);
                                if (leaf != NULL)
                                    return leaf->content;
                            }
                            return content;
                        };
    TYPE                GetElement(const char* path, bool load_if_missing = false){
                            std::string temp_path = std::string(path);
                            return GetElement(temp_path, load_if_missing);
                        }
    TYPE*               GetElementPtr(std::string& path, bool load_if_missing = false){
                            binTree_Manager* leaf = FindLeaf(path);
                            if (leaf != NULL)
                                return &(leaf->content);
                            if (load_if_missing == true){
                                leaf = AddElement(path);
                                if (leaf != NULL)
                                    return &(leaf->content);
                            }
                            return &content;
                        }
    TYPE*               GetElementPtr(const char* path, bool load_if_missing = false){
                            std::string temp_path = std::string(path);
                            return GetElementPtr(temp_path, load_if_missing);
                        }
    std::vector<TYPE*>  GetAllElementsPtr(std::string& mask, bool return_root_when_failed = false){
                            //return pointers to all elements whose names match the mask
                            std::vector<TYPE*> elements;
                            std::string mem;
                            // Recusrsive function (char* name, vector& elements), which compares first
                            // length(mask) chars of mask with the leaf name, and, if it fits, adds the pointer
                            // to it's contents to the vector
                            _GetAllElementsPtr(mask, elements);
                            if ((elements.size() == 0)&&(return_root_when_failed == true))
                                elements.push_back(&content);
                            return elements;
                        }
    void                PrintLeaves(int spacing = 0){
                            if (leaves[0] != NULL)
                                leaves[0]->PrintLeaves(spacing + 1);
                            for (int i = 0; i < spacing; i++)
                                std::cout << " ";
                            std::cout << "leaf: " << name << std::endl;
                            if (leaves[1] != NULL)
                                leaves[1]->PrintLeaves(spacing + 1);
                        }
    void                ChangeName(std::string& name_old, std::string& name_new){
                            binTree_Manager* leaf = FindLeaf(name_old);
                            if (leaf != NULL)
                                leaf->name = name_new;
                            else
                                std::cout << "binTree_Manager::ChangeName - unable to find element \"" << name_old << "\"\n";
                        }
    void                DeleteLeaf(std::string& name, bool force = false){
                            binTree_Manager* leaf = FindLeaf(name);
                            if (leaf != NULL){
                                if ((force == false)&&((leaf->leaves[0] != NULL)||(leaf->leaves[1] != NULL))){
                                    std::cout << "binTree_Manager::DeleteLeaf - leaf \"" << name << "\" has children!\n";
                                } else{
                                    // Determine, which child of a parent is the deleted node and redetermine
                                    // that child as the deleted node's child
                                    int child = (leaf->parent->leaves[0] == leaf) ? 0 : 1;
                                    if ((leaf->leaves[0] != NULL)&&(leaf->leaves[1] == NULL))
                                        leaf->parent->leaves[child] = leaf->leaves[0];
                                    else if ((leaf->leaves[1] != NULL)&&(leaf->leaves[0] == NULL))
                                        leaf->parent->leaves[child] = leaf->leaves[1];
                                    else {
                                        std::cout << "binTree_Manager::DeleteLeaf - leaf \"" << name
                                        << "\" has more than one child! Unable to delete leaf.\n";
                                        return;
                                    }
                                    leaf->leaves[0] = leaf->leaves[1] = NULL;
                                    delete leaf;
                                }
                            } else
                                std::cout << "binTree_Manager::DeleteLeaf - unable to find element \"" << name << "\"\n";
                        }
};

#endif // __BRD_MANAGER
