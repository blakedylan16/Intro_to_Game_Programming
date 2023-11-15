// The old stuff
#define GL_SILENCE_DEPRECIATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"               // We'll talk about these later in the course

//#include "helper.cpp"
void print_matrix(glm::mat4 &matrix, int size);

const float MILLISECONDS_IN_SECOND = 1000.0;

// Our window dimensions
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

// Background color components
const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// Our shader filepaths; these are necessary for a number of things
// Not least, to actually draw our shapes
const char V_SHADER_PATH[] = "shaders/vertex.glsl";
const char F_SHADER_PATH[] = "shaders/fragment.glsl";

const float TRIANGLE_RED      = 1.0f,
          TRIANGLE_BLUE     = 0.4f,
          TRIANGLE_GREEN    = 0.4f,
          TRIANGLE_OPACITY  = 1.0f;

const float DEGREES_PER_SECOND = 90.0f;
const float TRAN_VALUE = 0.025f;
const float GROWTH_FACTOR = 1.01f;
const float SHRINK_FACTOR = .99f;
const int MAX_FRAME = 40;



SDL_Window* g_display_window;

bool g_game_is_running = true;
bool g_is_growing = true;
int g_frame_counter = 0;


ShaderProgram g_program;
/*
 g_view_matrix - Defines the position of the camera
 g_model_matrix - Defines any translation, rotation, and/or scaling applied to an object
 g_projection_matrix - Defines the characteristics of the camera
 */
glm::mat4 g_view_matrix,
          g_model_matrix,
          g_projection_matrix,
          g_trans_matrix;

float g_triangle_x = 0.0f;
float g_triangle_rotate = 0.0f;
float g_prev_ticks = 0.0f;

// Part1: Initialise our program
void initialise() {
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hello, Triangle!",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    // Initialise the cameras
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    // Load up shaders
    g_program.Load(V_SHADER_PATH, F_SHADER_PATH);
    
    /*
     Initialise our view, model, and projection matrices.
     Orthographic means perpendicular - meaning that the camera will be
        perpendicularly down to our triangle
     */
    g_view_matrix       = glm::mat4(1.0f);
//    g_model_matrix      = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
//    g_trans_matrix = g_model_matrix;
    
    g_program.SetViewMatrix(g_view_matrix);
    g_program.SetProjectionMatrix(g_projection_matrix);
    
    g_program.SetColor(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);
    glUseProgram(g_program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
}

// Part 2: Process any player input - pressed button or moved joystick
void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            g_game_is_running = false;
        }
    }
}
// Part 3: Update game state given player input and previous state
void update() {
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the curr # of ticks
    float delta_time = ticks - g_prev_ticks;        // time since last frame
    g_prev_ticks = ticks;
    
    g_triangle_x += 1.0f * delta_time;
    g_triangle_rotate += DEGREES_PER_SECOND * delta_time;
    g_model_matrix = glm::mat4(1.0f);
    
    
    g_model_matrix = glm::translate(g_model_matrix, glm::vec3(g_triangle_x, 0.0f, 0.0f));
    g_model_matrix = glm::rotate(g_model_matrix,
                                 glm::radians(g_triangle_rotate),
                                 glm::vec3(0.0f, 0.0f, 1.0f));
}
// Part 4: Once updated, render those changes onto the screen
void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_program.SetModelMatrix(g_model_matrix);
    
    float vertices [] =
    {
         0.5f, -0.5f,   // (x1, y1)
         0.0f,  0.5f,   // (x2, y2)
        -0.5f, -0.5f    // (x3, y3)
    };
    
    
    glVertexAttribPointer(g_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(g_program.positionAttribute);
    
    SDL_GL_SwapWindow(g_display_window);
}
// Part 5: Shutdown protocol once game is over
void shutdown() { SDL_Quit(); }


// The game will reside inside the main
int main(int argc, char* argv[]) {
    // Part 1
    initialise();
    
    while (g_game_is_running) {
        // Part 2
        process_input();
        // Part 3
        update();
        // Part 4
        render();
    }
    // Part 5
    shutdown();
    return 0;
}

