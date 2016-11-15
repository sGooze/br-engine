#include "inc/brd_obj.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>

// TODO: Rewrite everything! Use vector to substitute reallocs

void OBJ_Model::ParseFace(char* obj_line, obj_face* face){
    char* token, *temp;
    token = strtok(obj_line, " \n\t\r");
    int vcount = 0;
    if (face == NULL)
        return;
    while (token = strtok(NULL, " \n\t\r")){
        //printf(" token: %s\n", token);
        face->vertex_index[vcount] = atoi(token) - 1;
        if(strstr(token, "//") != NULL){
			temp = strchr(token, '/');
			temp++;
			face->texture_index[vcount] = -1;
			face->normal_index[vcount++] = atoi(++temp) - 1;
		}
		else if(strstr(token, "/") != NULL){
			temp = strchr(token, '/');
			face->texture_index[vcount] = atoi(++temp) - 1;
			if(strstr(temp, "/") != NULL){
				temp = strchr(temp, '/');
				face->normal_index[vcount] = atoi(++temp) - 1;
			}
			vcount++;
		}
		if (vcount >= MAX_VERTEX_COUNT) break;
    }
    face->vertex_count = vcount;
    //DebugPrintFaceContent(face);
}

OBJ_Model::OBJ_Model(const char* path){
    FILE* obj = fopen(path, "r");
    if (obj == NULL){
        fprintf(stderr, "Unable to load model \"%s\"!\n", path);
    } else{
        int i, *cntr = NULL;
        vec3** vc = NULL, temp;
        char obj_line[OBJ_LINE_SIZE], *fmt;

        face_count = 0;
        vertex_count = 0;
        vertex_normal_count = 0;
        vertex_texture_count = 0;
        face_list = NULL;
        vertex_list = NULL;
        vertex_normal_list = NULL;
        vertex_texture_list = NULL;

        while (fgets(obj_line, OBJ_LINE_SIZE, obj)){
            obj_line[strlen(obj_line)-1] = '\0';
            switch (obj_line[0]){
            case 'v':
                cntr = NULL; vc = NULL;
                switch (obj_line[1]){
                case 'n':
                    cntr = &(vertex_normal_count);
                    vertex_normal_list = realloc(vertex_normal_list, sizeof(struct Vector3S*)*(++vertex_normal_count));
                    vc = vertex_normal_list;
                    fmt = "vn %lf %lf %lf";
                    break;
                case 't':
                    cntr = &(vertex_texture_count);
                    vertex_texture_list = realloc(vertex_texture_list, sizeof(struct Vector3S*)*(++vertex_texture_count));
                    vc = vertex_texture_list;
                    fmt = "vt %lf %lf %lf";
                    break;
                case ' ':
                    cntr = &(vertex_count);
                    vertex_list = realloc(vertex_list, sizeof(struct Vector3S*)*(++vertex_count));
                    vc = vertex_list;
                    fmt = "v %lf %lf %lf";
                    break;
                default:
                    break;
                }
                if ((cntr != NULL)&&(vc != NULL)){
                    i = (*cntr)-1;
                    vc[i] = malloc(sizeof(struct Vector3S));
                    if (vc[i] == NULL){
                        exit(-86541);
                    }
                    sscanf(obj_line, fmt, &temp.c[0], &temp.c[1], &temp.c[2]);
                    int j;
                    for (j = 0; j < 3; j++)
                        vc[i]->c[j] = temp.c[j];
                    //printf("    v %lf %lf %lf\n", vc[i]->c[0], vc[i]->c[1], vc[i]->c[2]);
                    vc = NULL;
                    cntr = NULL;
                }
                break;
            case 'f':
                i = face_count++;
                obj_face** new_list = realloc(face_list, sizeof(struct OBJ_Face*) * (face_count));
                if (new_list == NULL)
                    exit(1337);
                face_list = new_list;
                face_list[i] = malloc(sizeof(struct OBJ_Face));
                ParseFace(obj_line, face_list[i]);
                break;
            default:
                break;
            }
        }
        printf("Model loading complete!\n    v: %i\n    vt: %i\n    vn: %i\n    face: %i\n",
                vertex_count, vertex_texture_count, vertex_normal_count, face_count);
        fclose(obj);
        printf("Model filename: %s\n", cur->path);
    }
}
