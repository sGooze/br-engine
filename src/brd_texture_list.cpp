#include "inc/brd_texture.h"

BRD_TextureList::find_leaf(const char* path){
    if (!strcmp(path, tex_path))
        return this;
    // Check if searched path is leftmost or rightmost in relation to tex_path
    if (left != NULL)
        return left->find_leaf(path);
    if (right != NULL)
        return right->find_leaf(path);
    else return NULL;
}

BRD_TextureList::BRD_TextureList(const char* path){
    // Called explicitly only for root texture
    tex_path = path;
    lst->tex = new BRD_Texture2D(path);
    deleted = false;
    left = right = NULL;
}

BRD_TextureList* BRD_TextureList::load(const char* path){
    BRD_TextureList* leaf = find_leaf(path);
    if (leaf != NULL)
        return leaf;

}
