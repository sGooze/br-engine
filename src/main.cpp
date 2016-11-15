#include "inc/main.h"
#include "imgui/imgui_internal.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
BRD_Scene *cscene;

        long sectors = 0;
static  bool keys[1024]  = {false};
static  bool render_gui = true;

bool init(){
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

    //Use OpenGL 3.3 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    //Create window
    glWindow = SDL_CreateWindow("BEnGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if( glWindow == NULL ){
        std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    //Create context
    glContext = SDL_GL_CreateContext( glWindow );
    if( glContext == NULL ){
        std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    //Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK ){
        std::cout << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
        return false;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    BRD_Texture2D_InitBW();

	ImGui_ImplSdlGL3_Init(glWindow);
	atexit(close);
	return true;
}

void close(){
	//Destroy window
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow( glWindow );
	glWindow = NULL;
	ImGui_ImplSdlGL3_Shutdown();

	//Quit SDL subsystems
	SDL_Quit();
}

bool BRD_IsKeyPressed(int scancode){
    bool res = keys[scancode];
    if (res == true) keys[scancode] = false;
    return res;
}

BRD_Mesh LoadTestObject(){
    GLfloat vertices[] = {
        // Positions          // Colors           // Texture Coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left
    };

    GLuint indices[] = {  // Note that we start from 0!
        3, 1, 0, 3, 2, 1
    };

    BRD_Mesh testmesh(vertices, 4, 3, indices, 6);
    return testmesh;
}

//************ Main function ************//

int main( int argc, char* args[] ){
    // TODO: Destructors for each OpenGL container class
    // INITIALIZATION STEP
    // Some of that code will go into the main engine initialization function, some - to the scene constructor
	//Start up SDL and create a window
	if(!init()){
		std::cout <<  "Failed to initialize!" << std::endl;
		return -1;
	}

	InitClassTable();
    cscene = new BRD_Scene("test.scene");

	// Scene debug
	std::cout << "Scene contents:\nTexture list:\n\n";
	cscene->TexMan.PrintLeaves();
	std::cout << "\nMaterial list:\n\n";
	cscene->MatMan.PrintLeaves();
	std::cout << "\n";
	std::cout << "Scene loading step: completed\n";
    system("pause");

	// OpenGL parameters
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/*glEnable(GL_SCISSOR_TEST);
	glScissor(250, 150, 300, 300);*/

	// SCENE LOADING STEP
	// Load and precache all the data needed for the scene, then load the object & scenery data

	// Mesh loading
    BRD_Mesh skycube("models/skycube.vlist", 3);
    BRD_Mesh renderplane = LoadTestObject();

    // Shader object loading
    //
        Shader skybox("shaders/skybox.vert", "shaders/skybox.frag");
        Shader postprocess("shaders/postprocess.vert", "shaders/postprocess.frag");

    // Texture loading
    BRD_Texture2D blank ("textures/blank.png");
    BRD_Texture2D blankw ("textures/blankw.png");

    // Skybox texture loading: several skybox textures may be loaded and changed on the fly.
    // Dynamic cubemap reflection maps may also be used.
    //BRD_TextureCube skyboxtex("textures/skybox/2desert", ".bmp");
    BRD_TextureCube skyboxtex("textures/skybox/sky_trainstation01", ".png");

    // Setup objects
    // .. materials/models

    // .. cameras
    double fov = 45.0;
    BRD_Camera cam = BRD_Camera(glm::vec3(0.0, 0.0, 5.0)), altcam = BRD_Camera(), *curcam = &cam;

    // .. lights

    // Framebuffer
    BRD_Framebuffer screen_render(SCREEN_WIDTH, SCREEN_HEIGHT);

    GLfloat xshift, yshift;
    // TODO: make global - for usage in separate functions
    GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
    GLfloat lastFrame = 0.0f;  	// Time of last frame

    glm::vec3 inv = glm::vec3(-1.0, -1.0, -1.0);
	bool isquit;
	SDL_Event event;
	SDL_bool mouse_grab = SDL_TRUE;
	while (!isquit){
        // Frame timer update
        GLfloat currentFrame = (GLfloat)SDL_GetTicks() / 1000.0;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input processing
        // TODO: Separate into different functions. Will need console cmds implementation (?)
        // Event processor inside BRD_Input.
        SDL_SetRelativeMouseMode(mouse_grab);
        xshift = yshift = 0;
        while (SDL_PollEvent(&event) != 0){
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            if (mouse_grab == SDL_TRUE){
                // BRD_ProcessGameInputEvents
                switch (event.type){
                    case SDL_KEYDOWN:
                        keys[event.key.keysym.scancode] = true;  break;
                    case SDL_KEYUP:
                        keys[event.key.keysym.scancode] = false; break;
                    case SDL_MOUSEMOTION:
                            xshift = event.motion.xrel;
                            yshift = event.motion.yrel;
                            xshift *= curcam->sensitivity;
                            yshift *= curcam->sensitivity;
                        break;
                    case SDL_QUIT:
                        isquit = true; break;
                }
            }
        }
        // .. keyboard controls
        // TODO: Separate into standalone functions for key/mouse processing & camera translation/rotation
        //       Also, refine to avoid key rapid-fire
        GLfloat   cameraSpeed = 5.0f * deltaTime;
        // Key state is reset every frame. The speed of the key re-trigger depends on the OS settings.
        if (BRD_IsKeyPressed(SDL_SCANCODE_1))
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (BRD_IsKeyPressed(SDL_SCANCODE_2))
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (BRD_IsKeyPressed(SDL_SCANCODE_G))
            render_gui = !render_gui;
        if (BRD_IsKeyPressed(SDL_SCANCODE_ESCAPE)){
            mouse_grab = (SDL_bool)(mouse_grab == SDL_FALSE);
            if ((mouse_grab == SDL_FALSE)&&(render_gui == false))
                render_gui = true;
        }
        if (BRD_IsKeyPressed(SDL_SCANCODE_F10))
            isquit = true;
        // Key state is changed only when the corresponding event happens. Effects linked to those keys
        // are re-triggered every frame.
        if (keys[SDL_SCANCODE_W])
            curcam->translate(cameraSpeed * curcam->cameraFront);
        if (keys[SDL_SCANCODE_S])
            curcam->translate(cameraSpeed * curcam->cameraFront * inv);
        if (keys[SDL_SCANCODE_A])
            curcam->translate(glm::normalize(glm::cross(curcam->cameraFront, curcam->cameraUp)) * cameraSpeed * inv);
        if (keys[SDL_SCANCODE_D])
            curcam->translate(glm::normalize(glm::cross(curcam->cameraFront, curcam->cameraUp)) * cameraSpeed);
        if (keys[SDL_SCANCODE_SPACE])
            curcam->translate(cameraSpeed * glm::vec3(0.0, 1.0, 0.0));
        if (keys[SDL_SCANCODE_LCTRL])
            curcam->translate(cameraSpeed * glm::vec3(0.0, 1.0, 0.0) * inv);
        // .. mouse
        curcam->rotate(xshift, yshift);

        // Start new GUI frame
        //BRD_GUI_NewFrame();
        ImGui_ImplSdlGL3_NewFrame(!mouse_grab);

        GUI_FpsCounter();

        // Update spotlight

        // Update pointlight
        GLfloat ticks = (GLfloat)SDL_GetTicks();
        ImGui::End(); // GUI_FpsCounter

        // Matrices calculation

        // .. projection
        glm::mat4 project;
        double aspect = (double)SCREEN_WIDTH/(double)SCREEN_HEIGHT;
        project = glm::perspective(glm::radians(fov), aspect, 0.1, 100.0);

        // .. view
        glm::mat4 view;
        view = curcam->lookat();

        // <- RENDERING STARTS HERE
        // Everything from here till the end of the rendering will go into the method Scene::Render()

        // Change framebuffer to the main render framebuffer.
        // In total, every scene will use two framebuffers at minimum: render framebuffer and screen framebuffer.
        // Scenes with planar reflection will use one additional framebuffer for every reflection plane:
        // their total amount may be controlled by sector parameters
        // In addition, dynamic shadow rendering may too be dependent on additional framebuffers:
        // it will be determined when the shadow rendering technique will be decided on
        screen_render.use();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // Clear z-buffer and color buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // First pass - render skybox
        // Note: if the height fog will be implemented, then skybox should be affected by it
        // USING SKYBOX SHADER
        // All shaders used in the scene rendering are defined in the .scene file
        // Default shaders should always be supplied if the custom shader fields are left blank
        skybox.use();
        // .. draw skybox
        glDepthMask(GL_FALSE);
        glm::mat4 view_s;
        view_s = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(glGetUniformLocation(skybox.program, "project"),
                            1, GL_FALSE, glm::value_ptr(project));
        glUniformMatrix4fv(glGetUniformLocation(skybox.program, "view"),
                            1, GL_FALSE, glm::value_ptr(view_s));
        skyboxtex.use(0);
        skycube.DrawObject();
        glDepthMask(GL_TRUE);

        // USING MODEL SHADER
        // .. setup material

        // .. setup light parameters
        // All light objects will be stored in an easily accessible data structure.
        // VisDef functions will determine, which lights affect the lighting in the current frame, and update
        // the model shader with their parameters
        // Ambient lighting is determined by a separate entity (scene parameter?). It always affects the rendered
        // frame and can be changed on the fly by activating special triggers.
        // Same thing for the directional light

        // Object rendering
        // Visibility determination functions will compile the list of objects to be rendered
        // Engine debug objects will be rendered only in special cases
        // .. draw lightbulbs

        // Change framebuffer back to the default one
        screen_render.use_default();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        postprocess.use();
        glDisable(GL_DEPTH_TEST);
        screen_render.tex->use(0);
        renderplane.DrawObject();

        // TODO: Change to default fbuf and render first quad with postprocess texture, then GUI
        // ...   functions to draw gui
        if (mouse_grab == SDL_FALSE)
            ImGui::ShowTestWindow();

        if (render_gui == true){
            GUI_MainMenu();
            //GUI_ManagerViewer(Scene.TexMan);
            //point2.ShowProperties();
            ImGui::Render();
        }
        else
            ImGui::EndFrame();

        // Update window
        SDL_GL_SwapWindow(glWindow);
	}

	//Free resources and close SDL
	close();
	return 0;
}
