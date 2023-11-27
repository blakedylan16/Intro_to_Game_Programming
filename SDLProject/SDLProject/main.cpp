/*
 Author: Dylan Blake
 Assignment: Simple 2D Scene
 I pledge that I have completed this assignment without collaborating w/
 anyone else, in conformance w/ the NYU School of Engineering Policies
 and Procedures on Academic Misconduct.
 */

#define GL_SILENCE_DEPRECIATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"               // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp" // Matrix transformation methods
#include "ShaderProgram.h"
#include "stb_image.h"


enum Coordinate {
    x_coordinate,   // 0
    y_coordinate    // 1
};

// Our window dimensions
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const Coordinate X_COORDINATE = x_coordinate;
const Coordinate Y_COORDINATE = y_coordinate;

const float ORTHO_WIDTH  = 7.5f,
            ORTHO_HEIGHT = 10.0f;

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

SDL_Joystick *player_one_controller;

// Our shader filepaths; these are necessary for a number of things
// Not least, to actually draw our shapes
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const float DEGREES_PER_SECOND = 90.0f;

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0,
            TEXTURE_BORDER  = 0;

const char SPRITE_1_FILEPATH[] =
    "/Users/dylanblake/Developer/IntroGameProg/SDLProject/SDLProject/assets/Minotaur1.png";


SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_program;
/*
 g_view_matrix - Defines the position of the camera
 model matrices - Defines any translation, rotation, and/or scaling applied to an object
 g_projection_matrix - Defines the characteristics of the camera
 */
glm::mat4 g_view_matrix,
          g_player_model_matrix,
          g_projection_matrix,
          g_trans_matrix;

float g_prev_ticks = 0.0f;

GLuint sprite_texture_id;
GLuint sprite_2_texture_id;

// overall position
glm::vec3 g_player_position = glm::vec3(0.0f, 0.0f, 0.0f);

// movement tracker
glm::vec3 g_player_movement;
float player_speed = 3.0f;
//glm::vec3 sprite_2_movement;
float sprite_rotate = 0.0f;

float scale_speed = 1.0f;
float scale = 1.0f;
bool growing = true;


float get_screen_to_ortho(float coordinate, Coordinate axis);

GLuint load_texture(const char* filepath);

// Part1: Initialise our program
void initialise();

// Part 2: Process any player input - pressed button or moved joystick
void process_input();

// Part 3: Update game state given player input and previous state
void update();
// Part 4: Once updated, render those changes onto the screen
void render();
// Part 5: Shutdown protocol once game is over
void shutdown();

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id);

// The game will reside inside the main
int main(int argc, char* argv[]) {
    
    initialise();
    
    while (g_game_is_running) {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}

float get_screen_to_ortho(float coordinate, Coordinate axis)
{
    switch (axis) {
        case x_coordinate:
            return ((coordinate / WINDOW_WIDTH) * 10.0f ) - (10.0f / 2.0f);
        case y_coordinate:
            return (((WINDOW_HEIGHT - coordinate) / WINDOW_HEIGHT) * 7.5f) - (7.5f / 2.0);
        default: return 0.0f;
    }
}

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, num_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &num_components, STBI_rgb_alpha);
    
    if (!image) {
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert(false);
        exit(1);
    }
    
    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    
    return textureID;
}

void initialise() {
    // Initialise video subsystem
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    
    // Open the 1st controller found. Returns null on error
    player_one_controller = SDL_JoystickOpen(0);

    
    g_display_window = SDL_CreateWindow("Simple 2D!",
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
    
    g_view_matrix           = glm::mat4(1.0f);
    g_player_model_matrix   = glm::mat4(1.0f);
//    sprite_2_model_matrix   = glm::mat4(1.0f);
    g_projection_matrix     = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_player_movement = glm::vec3(0.0f, 0.0f, 0.0f);
//    sprite_2_movement = glm::vec3(-0.5f, 0.0f, 0.0f);
    
    g_program.SetViewMatrix(g_view_matrix);
    g_program.SetProjectionMatrix(g_projection_matrix);
    
    glUseProgram(g_program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    sprite_texture_id = load_texture(SPRITE_1_FILEPATH);
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT]) g_player_movement.x = -1.0f;
    else if (key_state[SDL_SCANCODE_RIGHT]) g_player_movement.x = 1.0f;
    
    
    // This makes sure that the player can't "cheat" their way into moving faster
    if (glm::length(g_player_movement) > 1.0f)
        g_player_movement = glm::normalize(g_player_movement);
}


void update() {
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the curr # of ticks
    float delta_time = ticks - g_prev_ticks;        // time since last frame
    g_prev_ticks = ticks;
    
    g_player_position += g_player_movement * player_speed * delta_time;
    g_player_model_matrix = glm::mat4(1.0f);
    
    
    g_player_model_matrix = glm::translate(g_player_model_matrix, g_player_position);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };
        
    glVertexAttribPointer(g_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_program.positionAttribute);
        
    glVertexAttribPointer(g_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_program.texCoordAttribute);
        
    // Bind texture
    draw_object(g_player_model_matrix, sprite_texture_id);
//    draw_object(sprite_2_model_matrix, sprite_2_texture_id);
        
    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_program.positionAttribute);
    glDisableVertexAttribArray(g_program.texCoordAttribute);
        
    SDL_GL_SwapWindow(g_display_window);
}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id) {
    g_program.SetModelMatrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void shutdown() {
    SDL_JoystickClose(player_one_controller);
    SDL_Quit();
}
