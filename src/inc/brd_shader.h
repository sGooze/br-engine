#ifndef __SHADER_H
#define __SHADER_H

#include "brd_shared.h"
#include "brd_manager.h"

// Lighting information - SYNCHRONIZE THIS WITH FRAGMENT SHADER!
#define MAX_DIR 2
#define MAX_POINT 16
#define MAX_SPOT 16
#define MAX_LIGHTS MAX_DIR+MAX_POINT+MAX_SPOT

class Shader{
private:
	// Shaders IDs
	GLuint vertex = -1, fragment = -1;
public:
  	// Program ID
	GLuint program;
	// Compilation status
	GLint compile_status = false;
	// Lighting information
	GLuint light_dir, max_light_dir;
	GLuint light_point, max_light_point;
	GLuint light_spot, max_light_spot;
	std::string pfx_dir, pfx_point, pfx_spot;

	// Constructor reads and builds the shader
	Shader(){program = glCreateProgram();}
	std::string LoadShaderCode(const GLchar* path){
        std::ifstream codeFile;
        std::stringstream codeStream;
        codeFile.exceptions(std::ifstream::badbit);
        try
        {
            // Open files
            codeFile.open(path);
            // Read file's buffer contents into streams
            codeStream << codeFile.rdbuf();
            // close file handlers
            codeFile.close();
        }
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR: PROGRAM ID: [" << program << "] - SHADER FILE WAS NOT SUCCESFULLY READ!" << std::endl;
        }
        // Convert stream into string
        // TODO: Optimize - we don't need to create two temp strings to load one file
        return codeStream.str();
	}

	GLint CompileShaderCode(const GLchar* path, GLenum type){
	    GLint success;
        GLchar infoLog[512];

        if ((type != GL_VERTEX_SHADER)&&(type != GL_FRAGMENT_SHADER)){
            std::cout << "ERROR: PROGRAM ID: [" << program << "] - UNKNOWN SHADER TYPE FOR SHADER \"" << path << "\"\n";
            return -1;
        }
        std::string tempCode = LoadShaderCode(path);
        //const GLchar* shaderCode = LoadShaderCode(path).c_str();
        const GLchar* shaderCode = tempCode.c_str();
        GLint temp = glCreateShader(type);
        glShaderSource(temp, 1, &shaderCode, NULL);
        glCompileShader(temp);
        // Print compile errors if any
        glGetShaderiv(temp, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(temp, 512, NULL, infoLog);
            std::cout << "ERROR: PROGRAM ID: [" << program << "] - ";
            switch (type){
            case GL_VERTEX_SHADER:
                std::cout << "VERTEX "; break;
            case GL_FRAGMENT_SHADER:
                std::cout << "FRAGMENT "; break;
            }
            std::cout << "SHADER (\"" << path << "\") FAILED TO COMPILE!\n" << infoLog << std::endl;
            std::cout << "CODECODECODE\n" << shaderCode << std::endl;
            glDeleteShader(temp);
            return -1;
        } else {
            glGetShaderInfoLog(temp, 512, NULL, infoLog);
            std::cout << "SUCC: PROGRAM ID: [" << program << "] - ";
            switch (type){
            case GL_VERTEX_SHADER:
                std::cout << "vertex "; vertex = temp; break;
            case GL_FRAGMENT_SHADER:
                std::cout << "fragment "; fragment = temp; break;
            }
            std::cout << "shader (\"" << path << "\") attached!\n";
        }
        return temp;
	}

	bool CompileProgram(){
        GLchar infoLog[512];

        // Shader Program
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        // Print linking errors if any
        glGetProgramiv(program, GL_LINK_STATUS, &compile_status);
        if(!compile_status)
        {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cout << "ERROR: PROGRAM ID: [" << program << "] - SHADER PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            std::system("pause");
        }

        // Delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        max_light_dir = 2;
        max_light_point = 16;
        max_light_spot = 16;
        pfx_dir = "dir"; pfx_point = "point"; pfx_spot = "spot";
        return (bool)compile_status;
	}

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath){
	    program = glCreateProgram();
        // 1. Retrieve the vertex/fragment source code from filePath
        std::cout << "Loading program\n Vertex: \"" << vertexPath << "\"\n Fragment: \"" << fragmentPath << "\"\n";

        // 2. Compile shaders
        vertex = CompileShaderCode(vertexPath, GL_VERTEX_SHADER);
        fragment = CompileShaderCode(fragmentPath, GL_FRAGMENT_SHADER);

        // Shader Program
        CompileProgram();
    }
  	// Use the program for rendering
  	void use(){
  	    if (!compile_status){
            std::cout << "WARNING: PROGRAM ID: [" << program << "] - ATTEMPTING TO LINK UNSUCCESSFULLY LINKED PROGRAM!\n";
            return;
  	    }
        light_dir = light_point = light_spot = 0;
        glUseProgram(program);
    }
    // Update light sources quantity
    void update_light(){
        glUniform3f(glGetUniformLocation(program, "light_sources"), light_dir, light_point, light_spot);
    }
};

// Shader Data Manager
bool ShaderLoaderFunc(std::string&, Shader&);
extern std::string shader_root;
typedef binTree_Manager<Shader, ShaderLoaderFunc, &shader_root> BRD_Shader_Manager;

#endif
