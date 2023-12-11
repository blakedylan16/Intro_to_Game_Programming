// The old stuff
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
#include <cmath>

enum Coordinate {
    x_coordinate,   // 0
    y_coordinate    // 1
};

// Our window dimensions
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const Coordinate X_COORDINATE = x_coordinate;
const Coordinate Y_COORDINATE = y_coordinate;

const float ORTHO_HEIGHT  = 7.5f,
            ORTHO_WIDTH = 10.0f;
const glm::vec3 ORTHO_DIM = glm::vec3(ORTHO_WIDTH/2, ORTHO_HEIGHT/2, 1.0f);

const glm::vec2 ZERO = glm::vec2(0.0f);

// Background color components
const float BG_RED     = 0.0f,
            BG_BLUE    = 0.0f,
            BG_GREEN   = 0.0f,
            BG_OPACITY = 0.0f;

// Our viewport—or our "camera"'s—position and dimensions
const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

SDL_Window* display_window;
bool game_is_running = true;

ShaderProgram g_program;

SDL_Joystick *player_one_controller;
SDL_Joystick *player_two_controller;

// Our shader filepaths; these are necessary for a number of things
// Not least, to actually draw our shapes
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
const char V_SHADER_PATH_SHAPES[] = "shaders/vertex.glsl";
const char F_SHADER_PATH2_SHAPES[] = "shaders/fragment.glsl";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL  = 0,
            TEXTURE_BORDER   = 0;

/*
 g_view_matrix - Defines the position of the camera
 model matrices - Defines any translation, rotation, and/or scaling applied to an object
 g_projection_matrix - Defines the characteristics of the camera
 */
glm::mat4 g_view_matr,
          paddle_1_mod_matr,
          paddle_2_mod_matr,
          ball_mod_matr,
          g_proj_matr,
          g_trans_matr;

/*
 g_view_matrix - Defines the position of the camera
 g_model_matrix - Defines any translation, rotation, and/or scaling applied to an object
 g_projection_matrix - Defines the characteristics of the camera
 */
glm::mat4 g_trans_matrix;


const float MILLISECONDS_IN_SECOND = 1000.0,
            DEGREES_PER_SECOND     = 90.0f;
float       g_prev_ticks           = 0.0f;

// INIT stuff
const float PADDLE_SCALE = 1.0f;

const glm::vec3 PADDLE_1_INIT_POS = glm::vec3(-4.25f, 0.0f, 0.0f),
                PADDLE_DIM      = glm::vec3( .25f, 1.0f, 0.0f),
                PADDLE_2_INIT_POS = glm::vec3( 4.25f, 0.0f, 0.0f),
                BALL_INIT_POS     = glm::vec3(0.0f, 0.0f, 0.0f),
                BALL_DIM          = glm::vec3(.2f, .2f, 0.0f);

const float PADDLE_RED      = 1.0f,
            PADDLE_BLUE     = 1.0f,
            PADDLE_GREEN    = 1.0f;

// general stuff
glm::vec3 paddle_1_pos  = PADDLE_1_INIT_POS,
          paddle_1_mov  = glm::vec3(0.0f, 0.0f, 0.0f),
          paddle_2_pos  = PADDLE_2_INIT_POS,
          paddle_2_mov  = glm::vec3(0.0f, 0.0f, 0.0f),
          ball_pos      = BALL_INIT_POS,
          ball_mov      = glm::vec3(0.0f, 0.0f, 0.0f);
const float SPEED = 6.0f;
const float MAX_REL_ANGLE = 60.0f;
const float MAX_REL_ANGLE_RAD = MAX_REL_ANGLE * (M_PI / 180.0f);
const float MAX_VERT_BALL_MOV = std::tan(MAX_REL_ANGLE_RAD);

const float TRAN_VALUE = 0.025f;
const float GROWTH_FACTOR = 1.01f;
const float SHRINK_FACTOR = .99f;
const int MAX_FRAME = 40;
bool g_is_growing = true;
int g_frame_counter = 0;
float g_triangle_x = 0.0f;

// collision
//const float COLLISION_FACTOR = .6f;
bool collision;

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

void draw_object(glm::mat4 &object_model_matrix);
/*
 Bounces ball after hitting a paddle. Keeps magnitude of horizontal movement
 constant and calculates new vertical speed.
 
 @param ball_mov movement vector for ball
 @param relative_y ball's y position relative to the paddle at collision
 */
void bounce();

void bind(glm::vec3 &paddle_pos);

// The game will reside inside the main
int main(int argc, char* argv[]) {
    
    initialise();
    
    while (game_is_running) {
        process_input();
        update();
        render();
    }
    shutdown();
    return 0;
}

float get_screen_to_ortho(float coordinate, Coordinate axis) {
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

// Part1: Initialise our program
void initialise() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    display_window = SDL_CreateWindow("Pong",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    // Initialise the cameras
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    // Load up shaders
    g_program.Load(V_SHADER_PATH_SHAPES, F_SHADER_PATH2_SHAPES);
    
    g_view_matr       = glm::mat4(1.0f);
    g_proj_matr = glm::ortho(-ORTHO_DIM.x, ORTHO_DIM.x,
                                     -ORTHO_DIM.y, ORTHO_DIM.y,
                                     -ORTHO_DIM.z, ORTHO_DIM.z);
    g_program.SetColor(PADDLE_RED, PADDLE_BLUE, PADDLE_GREEN, 1.0f);
    
    /* ---- PADDLE 1 ---- */
    paddle_1_mod_matr = glm::mat4(1.0f);
    paddle_1_mod_matr = glm::translate(paddle_1_mod_matr, PADDLE_1_INIT_POS);
    paddle_1_mod_matr = glm::scale(paddle_1_mod_matr, PADDLE_DIM);
    /* ---------------- */
    
    /* ---- PADDLE 2 ---- */
    paddle_2_mod_matr = glm::mat4(1.0f);
    paddle_2_mod_matr = glm::translate(paddle_2_mod_matr, PADDLE_2_INIT_POS);
    paddle_2_mod_matr = glm::scale(paddle_2_mod_matr, PADDLE_DIM);
    /* ---------------- */
    
    /* ---- BALL ---- */
    ball_mod_matr = glm::mat4(1.0f);
    ball_mod_matr = glm::translate(ball_mod_matr, BALL_INIT_POS);
    ball_mod_matr = glm::scale(ball_mod_matr, BALL_DIM);
    ball_mov.x = 1.0f;
    /* ---------------- */
    
    g_program.SetViewMatrix(g_view_matr);
    g_program.SetProjectionMatrix(g_proj_matr);
    
    glUseProgram(g_program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
//    // enable blending
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Part 2: Process any player input - pressed button or moved joystick
void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                game_is_running = false;
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_UP] && not key_state[SDL_SCANCODE_DOWN])
        paddle_2_mov.y = 1.0f;
    else if (key_state[SDL_SCANCODE_DOWN] && not key_state[SDL_SCANCODE_UP])
        paddle_2_mov.y = -1.0f;
    else paddle_2_mov.y = 0;

    if (key_state[SDL_SCANCODE_W] && not key_state[SDL_SCANCODE_S])
        paddle_1_mov.y = 1.0f;
    else if (key_state[SDL_SCANCODE_S] && not key_state[SDL_SCANCODE_W])
        paddle_1_mov.y = -1.0f;
    else paddle_1_mov.y = 0;

    paddle_1_mov.x = 0;
    paddle_2_mov.x = 0;

    // This makes sure that the player can't "cheat" their way into moving faster
//    if (glm::length(paddle_1_mov) > 1.0f)
//        paddle_1_mov = glm::normalize(paddle_1_mov);
//    if (glm::length(paddle_2_mov) > 1.0f)
//        paddle_2_mov = glm::normalize(paddle_2_mov);
}

void update() {
    /* ------ DELTA TIME ------ */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the curr # of ticks
    float delta_time = ticks - g_prev_ticks;        // time since last frame
    g_prev_ticks = ticks;
    /* ------------------------ */
    
    
    
    /* ---- PADDLE 1 ---- */
    paddle_1_mod_matr = glm::mat4(1.0f);
    paddle_1_pos += SPEED * paddle_1_mov * delta_time;
    bind(paddle_1_pos);
    paddle_1_mod_matr = glm::translate(paddle_1_mod_matr, paddle_1_pos);
    paddle_1_mod_matr = glm::scale(paddle_1_mod_matr, PADDLE_DIM);
    /* ---------------- */

    /* ---- PADDLE 2 ---- */
    paddle_2_mod_matr = glm::mat4(1.0f);
    paddle_2_pos += SPEED * paddle_2_mov * delta_time;
    bind(paddle_2_pos);
    paddle_2_mod_matr = glm::translate(paddle_2_mod_matr, paddle_2_pos);
    paddle_2_mod_matr = glm::scale(paddle_2_mod_matr, PADDLE_DIM);
    /* ---------------- */
    
    /* ---- BALL ---- */
    ball_mod_matr = glm::mat4(1.0f);
    ball_pos += 5.0f * ball_mov * delta_time;
    bounce();
    ball_mod_matr = glm::translate(ball_mod_matr, ball_pos);
    ball_mod_matr = glm::scale(ball_mod_matr, BALL_DIM);
    /* ---------------- */
    
}
// Part 4: Once updated, render those changes onto the screen
void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = {
            -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
            -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };
    
    //    // Textures
    //    float texture_coordinates[] = {
    //        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
    //        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    //    };
    
    glVertexAttribPointer(g_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_program.positionAttribute);
    // Bind texture
    draw_object(paddle_1_mod_matr);
    draw_object(paddle_2_mod_matr);
    draw_object(ball_mod_matr);
    
    SDL_GL_SwapWindow(display_window);
}

void draw_object(glm::mat4 &object_model_matrix) {
    g_program.SetModelMatrix(object_model_matrix);
//    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void shutdown() {
    SDL_JoystickClose(player_one_controller);
    SDL_JoystickClose(player_two_controller);
    SDL_Quit();
}

void bind(glm::vec3 &paddle_pos) {
    if (std::abs(paddle_pos.y) > (ORTHO_HEIGHT - PADDLE_DIM.y)/2)
        paddle_pos.y = paddle_pos.y > 0 ?
            (ORTHO_HEIGHT - PADDLE_DIM.y)/2 :
            (-ORTHO_HEIGHT + PADDLE_DIM.y)/2;
}

void bounce() {
    // ball-wall bounce mechanics
    if (std::abs(ball_pos.y) > (ORTHO_HEIGHT - BALL_DIM.y)/2) {
        ball_mov.y *= -1;
        ball_pos.y = ball_pos.y > 0 ?
            (ORTHO_HEIGHT - BALL_DIM.y)/2 :
            (-ORTHO_HEIGHT + BALL_DIM.y)/2;
    }
    if (std::abs(ball_pos.x) > (ORTHO_WIDTH - BALL_DIM.x)/2) {
        ball_mov = glm::vec3(1.0f, 0.0f, 0.0f);
        ball_pos = BALL_INIT_POS;
    }
    // ball-paddle bounce mechanics
    glm::vec2 dist_1 =
        glm::vec2(std::abs(paddle_1_pos.x - ball_pos.x) - (PADDLE_DIM.x + BALL_DIM.x) / 2,
                  std::abs(paddle_1_pos.y - ball_pos.y) - (PADDLE_DIM.y + BALL_DIM.y) / 2);
    glm::vec2 dist_2 =
        glm::vec2(std::abs(paddle_2_pos.x - ball_pos.x) - (PADDLE_DIM.x + BALL_DIM.x) / 2,
                  std::abs(paddle_2_pos.y - ball_pos.y) - (PADDLE_DIM.y + BALL_DIM.y) / 2);
    
    if(glm::all(glm::lessThanEqual(dist_1, ZERO))) {
        ball_mov.x *= -1;
        ball_mov.y = MAX_VERT_BALL_MOV *
            ((ball_pos.y - paddle_1_pos.y) / (PADDLE_DIM.y/2));
        ball_pos.x = paddle_1_pos.x + (PADDLE_DIM.x + BALL_DIM.x) / 2;
    }
    else if (glm::all(glm::lessThanEqual(dist_2, ZERO))) {
        ball_mov *= -1;
        ball_mov.y = MAX_VERT_BALL_MOV *
            ((ball_pos.y - paddle_2_pos.y) / (PADDLE_DIM.y/2));
        ball_pos.x = paddle_2_pos.x - (PADDLE_DIM.x + BALL_DIM.x)/2;
    }
    
    
}




///*
// Author: Dylan Blake
// Assignment: Simple 2D Scene
// I pledge that I have completed this assignment without collaborating w/
// anyone else, in conformance w/ the NYU School of Engineering Policies
// and Procedures on Academic Misconduct.
// */
//
//#define GL_SILENCE_DEPRECIATION
//#define STB_IMAGE_IMPLEMENTATION
//#define GL_GLEXT_PROTOTYPES 1
//#define LOG(argument) std::cout << argument << '\n'
//
//#ifdef _WINDOWS
//#include <GL/glew.h>
//#endif
//
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include "glm/mat4x4.hpp"               // 4x4 Matrix
//#include "glm/gtc/matrix_transform.hpp" // Matrix transformation methods
//#include "ShaderProgram.h"
//#include "stb_image.h"
//#include <cmath>
//
//
//enum Coordinate {
//    x_coordinate,   // 0
//    y_coordinate    // 1
//};
//
//// Our window dimensions
//const int WINDOW_WIDTH  = 640,
//          WINDOW_HEIGHT = 480;
//
//const Coordinate X_COORDINATE = x_coordinate;
//const Coordinate Y_COORDINATE = y_coordinate;
//
//const float ORTHO_WIDTH  = 10.0f,
//            ORTHO_HEIGHT = 7.5f;
//const glm::vec3 ORTHO_DIM = glm::vec3(ORTHO_WIDTH/2, ORTHO_HEIGHT/2, 1.0f);
//
//// Background color components
//const float BG_RED     = 0.0f,
//            BG_BLUE    = 0.0f,
//            BG_GREEN   = 0.0f,
//            BG_OPACITY = 0.0f;
//
//// Our viewport—or our "camera"'s—position and dimensions
//const int VIEWPORT_X      = 0,
//          VIEWPORT_Y      = 0,
//          VIEWPORT_WIDTH  = WINDOW_WIDTH,
//          VIEWPORT_HEIGHT = WINDOW_HEIGHT;
//
//SDL_Window* g_display_window;
//bool g_game_is_running = true;
//
//ShaderProgram g_program;
//
//SDL_Joystick *player_one_controller;
//SDL_Joystick *player_two_controller;
//
//// Our shader filepaths; these are necessary for a number of things
//// Not least, to actually draw our shapes
//const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
//const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
//
//const int NUMBER_OF_TEXTURES = 1;
//const GLint LEVEL_OF_DETAIL = 0,
//            TEXTURE_BORDER  = 0;
//
////const char FLOWER_FILEPATH[] =
////    "/Users/dylanblake/Developer/IntroGameProg/SDLProject/SDLProject/assets/flower.png";
//const char CUP_FILEPATH[] =
//    "/Users/dylanblake/Developer/IntroGameProg/SDLProject/SDLProject/assets/cup.png";
//GLuint g_flower_texture_id;
//GLuint g_cup_texture_id;
//
///*
// g_view_matrix - Defines the position of the camera
// model matrices - Defines any translation, rotation, and/or scaling applied to an object
// g_projection_matrix - Defines the characteristics of the camera
// */
//glm::mat4 g_view_matrix,
//          g_flower_model_matrix,
//          g_cup_model_matrix,
//          g_projection_matrix,
//          g_trans_matrix;
//
///*
// g_view_matrix - Defines the position of the camera
// model matrices - Defines any translation, rotation, and/or scaling applied to an object
// g_projection_matrix - Defines the characteristics of the camera
// */
//glm::mat4 g_view_matr,
//          paddle_1_mod_matr,
//          paddle_2_mod_matr,
//          ball_mod_matr,
//          g_proj_matr,
//          g_trans_matr;
//
//const float MILLISECONDS_IN_SECOND = 1000.0;
//const float DEGREES_PER_SECOND = 90.0f;
//float g_prev_ticks = 0.0f;
//
//// INIT stuff
//
//// INIT stuff
//const float PADDLE_SCALE = 1.0f;
//
//const glm::vec3 PADDLE_1_INIT_POS = glm::vec3(-4.25f, 0.0f, 0.0f),
//                PADDLE_DIM      = glm::vec3( .25f, 1.0f, 0.0f),
//                PADDLE_2_INIT_POS = glm::vec3( 4.25f, 0.0f, 0.0f),
//                BALL_INIT_POS     = glm::vec3(0.0f, 0.0f, 0.0f),
//                BALL_DIM          = glm::vec3(.25f, .25f, 0.0f);
//
//const float PADDLE_RED      = 1.0f,
//            PADDLE_BLUE     = 1.0f,
//            PADDLE_GREEN    = 1.0f;
//
//const glm::vec3 FLOWER_INIT_POS = glm::vec3(0.0f, 1.0f, 0.0f),
//                FLOWER_INIT_DIM = glm::vec3(1.0f, 1.0f, 0.0f);
//
//const float cup_scale = 3.0f;
//const glm::vec3 CUP_INIT_POS = glm::vec3(0.0f, -1.6f, 0.0f),
//                CUP_INIT_DIM = glm::vec3(cup_scale, cup_scale, 0.0f);
//
//// general stuff
//glm::vec3 paddle_1_pos  = PADDLE_1_INIT_POS,
//          paddle_1_mov  = glm::vec3(0.0f, 0.0f, 0.0f),
//          paddle_2_pos  = PADDLE_2_INIT_POS,
//          paddle_2_mov  = glm::vec3(0.0f, 0.0f, 0.0f),
//          ball_pos      = BALL_INIT_POS,
//          ball_mov      = glm::vec3(0.0f, 0.0f, 0.0f);
//bool paddle_1_top_bound = false,
//     paddle_1_bott_bound = false,
//     paddle_2_top_bound = false,
//     paddle_2_bott_bound = false;
//const float SPEED = 5.0f;
//const float MAX_REL_ANGLE = 60.0f;
//const float MAX_REL_ANGLE_RAD = MAX_REL_ANGLE * (M_PI / 180.0f);
//const float MAX_VERT_BALL_MOV = std::tan(MAX_REL_ANGLE_RAD);
//
//glm::vec3 g_flower_pos      = FLOWER_INIT_POS,
//          g_flower_movement = glm::vec3(0.0f, 0.0f, 0.0f);
//float flower_speed = 5.0f;
//
//// oscillation effect of flower
//float flower_rotation = 0.0f;
//
//// post-collision effect
//const float COLLISION_FACTOR = .6f;
//bool collision = false;
//const float shrinkage = .5f;
//float scale = 1.0f;
//glm::vec3 flower_scale_vector = glm::vec3(1.0f, 1.0f, 0.0f);
//
//
//float get_screen_to_ortho(float coordinate, Coordinate axis);
//
//GLuint load_texture(const char* filepath);
//
//// Part1: Initialise our program
//void initialise();
//
//// Part 2: Process any player input - pressed button or moved joystick
//void process_input();
//
//// Part 3: Update game state given player input and previous state
//void update();
//// Part 4: Once updated, render those changes onto the screen
//void render();
//// Part 5: Shutdown protocol once game is over
//void shutdown();
//
//void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id);
//
//void draw_object(glm::mat4 &object_model_matrix);
//
//// The game will reside inside the main
//int main(int argc, char* argv[]) {
//    
//    initialise();
//    
//    while (g_game_is_running) {
//        process_input();
//        update();
//        render();
//    }
//    
//    shutdown();
//    return 0;
//}
//
//float get_screen_to_ortho(float coordinate, Coordinate axis)
//{
//    switch (axis) {
//        case x_coordinate:
//            return ((coordinate / WINDOW_WIDTH) * 10.0f ) - (10.0f / 2.0f);
//        case y_coordinate:
//            return (((WINDOW_HEIGHT - coordinate) / WINDOW_HEIGHT) * 7.5f) - (7.5f / 2.0);
//        default: return 0.0f;
//    }
//}
//
//GLuint load_texture(const char* filepath)
//{
//    // STEP 1: Loading the image file
//    int width, height, num_components;
//    unsigned char* image = stbi_load(filepath, &width, &height, &num_components, STBI_rgb_alpha);
//    
//    if (!image) {
//        LOG("Unable to load image. Make sure the path is correct.");
//        LOG(filepath);
//        assert(false);
//        exit(1);
//    }
//    
//    // STEP 2: Generating and binding a texture ID to our image
//    GLuint textureID;
//    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
//    
//    // STEP 3: Setting our texture filter parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    
//    // STEP 4: Releasing our file from memory and returning our texture id
//    stbi_image_free(image);
//    
//    return textureID;
//}
//
//void initialise() {
//    // Initialise video subsystem
//    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
//    
//    // Open the 1st controller found. Returns null on error
//    player_one_controller = SDL_JoystickOpen(0);
//    player_two_controller = SDL_JoystickOpen(1);
//
//    g_display_window = SDL_CreateWindow("PONG",
//                                        SDL_WINDOWPOS_CENTERED,
//                                        SDL_WINDOWPOS_CENTERED,
//                                        WINDOW_WIDTH, WINDOW_HEIGHT,
//                                        SDL_WINDOW_OPENGL);
//    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
//    SDL_GL_MakeCurrent(g_display_window, context);
//    
//#ifdef _WINDOWS
//    glewInit();
//#endif
//    // Initialise the cameras
//    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
//    // Load up shaders
//    g_program.Load(V_SHADER_PATH, F_SHADER_PATH);
//    
//    g_view_matr = glm::mat4(1.0f);
//    g_proj_matr = glm::ortho(-ORTHO_DIM.x, ORTHO_DIM.x,
//                             -ORTHO_DIM.y, ORTHO_DIM.y,
//                             -ORTHO_DIM.z, ORTHO_DIM.z);
//    g_program.SetColor(PADDLE_RED, PADDLE_BLUE, PADDLE_GREEN, 1.0f);
//    
//    /* ---- FLOWER ---- */
//    g_flower_model_matrix = glm::mat4(1.0f);
//    g_flower_model_matrix = glm::translate(g_flower_model_matrix,
//                                        PADDLE_1_INIT_POS);
//    g_flower_model_matrix = glm::scale(g_flower_model_matrix, FLOWER_INIT_DIM);
//    
////    g_flower_texture_id = load_texture(FLOWER_FILEPATH);
//    /* ---------------- */
//    
//    /* ---- CUP ---- */
//    g_cup_model_matrix = glm::mat4(1.0f);
//    g_cup_model_matrix = glm::translate(g_cup_model_matrix, CUP_INIT_POS);
//    g_cup_model_matrix = glm::scale(g_cup_model_matrix, CUP_INIT_DIM);
//    
//    g_cup_texture_id = load_texture(CUP_FILEPATH);
//    /* ------------- */
//    
//    g_program.SetViewMatrix(g_view_matrix);
//    g_program.SetProjectionMatrix(g_projection_matrix);
//    
//    glUseProgram(g_program.programID);
//    
//    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
//    
//    // enable blending
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//}
//
//void process_input() {
//    
//    SDL_Event event;
//    while (SDL_PollEvent(&event)) {
//        switch (event.type) {
//            case SDL_QUIT:
//            case SDL_WINDOWEVENT_CLOSE:
//                g_game_is_running = false;
//                break;
//        }
//    }
//    
//    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
//
//    if (key_state[SDL_SCANCODE_A]) g_flower_movement.x = -1.0f;
//    else if (key_state[SDL_SCANCODE_D]) g_flower_movement.x = 1.0f;
//    else if (key_state[SDL_SCANCODE_W]) g_flower_movement.y = 1.0f;
//    else if (key_state[SDL_SCANCODE_S]) g_flower_movement.y = -1.0f;
//    else {
//        g_flower_movement.x = 0;
//        g_flower_movement.y = 0;
//    }
//    
//    
//    // This makes sure that the player can't "cheat" their way into moving faster
//    if (glm::length(g_flower_movement) > 1.0f)
//        g_flower_movement = glm::normalize(g_flower_movement);
//}
//
//
//void update() {
//    /* ------ DELTA TIME ------ */
//    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the curr # of ticks
//    float delta_time = ticks - g_prev_ticks;        // time since last frame
//    g_prev_ticks = ticks;
//    /* ------------------------ */
//    
//    
//    /* ----- COLLISION ----- */
//    float x_dist = fabs(g_flower_pos.x - CUP_INIT_POS.x) -
//        (COLLISION_FACTOR * (FLOWER_INIT_DIM.x + CUP_INIT_DIM.x) / 2.0f);
//    float y_dist = fabs(g_flower_pos.y - CUP_INIT_POS.y) -
//        (COLLISION_FACTOR * (FLOWER_INIT_DIM.y + CUP_INIT_DIM.y) / 2.0f);
//    
//    if (x_dist < 0.0f && y_dist < 0.0f && scale > 0) {
//        LOG("COLLISION!");
//        collision = true;
//        scale -= shrinkage * delta_time;
//        flower_scale_vector *= scale;
//    }
//    /* --------------------- */
//    
//    /* ----- RESET MODEL MATRICES ----- */
//    g_flower_model_matrix   = glm::mat4(1.0f);
//    
//    g_cup_model_matrix = glm::mat4(1.0f);
//    g_cup_model_matrix = glm::translate(g_cup_model_matrix, CUP_INIT_POS);
//    g_cup_model_matrix = glm::scale(g_cup_model_matrix, CUP_INIT_DIM);
//    /* -------------------------------- */
//    
//    if (not collision) {
//        g_flower_pos += g_flower_movement * flower_speed * delta_time;
//    }
//    g_flower_model_matrix   = glm::translate(g_flower_model_matrix, g_flower_pos);
//    flower_rotation += 90.0f * delta_time;
//    g_flower_model_matrix   = glm::rotate(g_flower_model_matrix,
//                                          glm::radians(flower_rotation),
//                                          glm::vec3(0.0f, 1.0f, 0.0f));
//    g_flower_model_matrix = glm::scale(g_flower_model_matrix, flower_scale_vector);
//}
//
//void render() {
//    glClear(GL_COLOR_BUFFER_BIT);
//    
//    // Vertices
//    float vertices[] = {
//        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
//        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
//    };
//
//    // Textures
//    float texture_coordinates[] = {
//        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
//        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
//    };
//        
//    glVertexAttribPointer(g_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
//    glEnableVertexAttribArray(g_program.positionAttribute);
//        
//    glVertexAttribPointer(g_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
//    glEnableVertexAttribArray(g_program.texCoordAttribute);
//    
//    // Bind texture
//    draw_object(g_flower_model_matrix);
//    draw_object(g_cup_model_matrix, g_cup_texture_id);
//        
//    // We disable two attribute arrays now
//    glDisableVertexAttribArray(g_program.positionAttribute);
//    glDisableVertexAttribArray(g_program.texCoordAttribute);
//        
//    SDL_GL_SwapWindow(g_display_window);
//}
//
//void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id) {
//    g_program.SetModelMatrix(object_model_matrix);
//    glBindTexture(GL_TEXTURE_2D, object_texture_id);
//    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
//}
//
//void draw_object(glm::mat4 &object_model_matrix) {
//    g_program.SetModelMatrix(object_model_matrix);
////    glBindTexture(GL_TEXTURE_2D, object_texture_id);
//    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
//}
//
//void shutdown() {
//    SDL_JoystickClose(player_one_controller);
//    SDL_Quit();
//}
