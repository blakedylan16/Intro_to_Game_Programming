////
////  Lunar_Landing.cpp
////  SDLProject
//// REMAKING OF THE ONE IN CLASS
//// 
////  Created by Dylan Blake on 12/16/23.
////  Copyright © 2023 ctg. All rights reserved.
////
//
//#include "Lunar_Landing.hpp"
//
//#define LOG(argument) std::cout << argument << '\n'
//#define STB_IMAGE_IMPLEMENTATION
//#define GL_SILENCE_DEPRECIATION
//#define GL_GLEXT_PROTOTYPES 1
//#define NUMBER_OF_ENEMIES 3
//#define FIXED_TIMESTEP 0.0166666f
//#define ACC_OF_GRAVITY -9.81f
//#define PLATFORM_COUNT 5
//
//#ifdef _WINDOWS
//#include <GL/glew.h>
//#endif
//
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include "glm/mat4x4.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "ShaderProgram.h"
//#include "stb_image.h"
//#include "Entity.hpp"
//#include "cmath"
//#include <ctime>
//#include <vector>
//#include <cstdlib>
//
//// ----- STRUCTS AND ENUMS ----- //
//struct GameState {
//    Entity* rocket;
//    Entity* platforms;
//};
//
//const int WINDOW_WIDTH  = 640,
//          WINDOW_HEIGHT = 480;
//
//const float BG_RED      = 0.1922f,
//            BG_BLUE     = 0.549f,
//            BG_GREEN    = 0.9059f,
//            BG_OPACITY  = 1.0f;
//
//const float ORTHO_HEIGHT    = 7.5f,
//            ORTHO_WIDTH     = 10.0f;
//const glm::vec3 ORTHO_DIM   = glm::vec3(ORTHO_WIDTH/2,
//                                        ORTHO_HEIGHT/2,
//                                        1.0f);
//
//const int VIEWPORT_X        = 0,
//          VIEWPORT_Y        = 0,
//          VIEWPORT_WIDTH    = WINDOW_WIDTH,
//          VIEWPORT_HEIGHT   = WINDOW_HEIGHT;
//
//const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
//const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
//
//const float MILLISECONDS_IN_SECOND = 1000.0;
//
//const char  SPRITESHEET_FILEPATH[]  = "assets/george_0.png",
//            PLATFORM_FILEPATH[]      = "assets/platform_tile.png";
//
//const int NUMBER_OF_TEXTURES = 1;
//const GLint LEVEL_OF_DETAIL = 0,
//            TEXTURE_BORDER  = 0;
//
///* ----- VARIABLES ------ */
//
//GameState g_gameState;
//
//SDL_Window* g_displayWindow;
//bool g_gameIsRunning = true;
//
//ShaderProgram g_program;
//glm::mat4 g_viewMatrix, g_projectionMatrix;
//
//float g_previousTicks = 0.0f;
//float g_timeAccumulator = 0.0f;
//
//GLuint loadTexture(const char* filepath);
//
//// Part1: Initialise our program
//void Initialise();
//
//// Part 2: Process any player input - pressed button or moved joystick
//void ProcessInput();
//
//// Part 3: Update game state given player input and previous state
//void Update();
//// Part 4: Once updated, render those changes onto the screen
//void Render();
//// Part 5: Shutdown protocol once game is over
//void shutdown();
//
//void playerInit(Entity** player);
//
//void platformsInit(Entity** platforms);
//
//// The game will reside inside the main
//int main(int argc, char* argv[]) {
//    
//    Initialise();
//    
//    while (g_gameIsRunning) {
//        ProcessInput();
//        Update();
//        Render();
//    }
//    
//    shutdown();
//    return 0;
//}
//             
//GLuint loadTexture(const char* filepath) {
//    int width, height, numComponents;
//    unsigned char* image = stbi_load(filepath, &width, &height,
//                                     &numComponents, STBI_rgb_alpha);
//    
//    if (!image) {
//        LOG("Unable to load image. Make sure the path is correct.");
//        LOG(filepath);
//        assert(false);
//        exit(1);
//    }
//    
//    GLuint textureID;
//    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height,
//                 TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
//    
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    
//    stbi_image_free(image);
//    
//    return textureID;
//}
//
//void playerInit(Entity** player) {
//    
//    *player = new Entity();
//    (*player)->type = PLAYER;
//    
//    (*player)->setPosition(glm::vec3(0.0f));
//    (*player)->setMovement(glm::vec3(0.0f));
//    (*player)->setAcceleration(glm::vec3(0.0f, ACC_OF_GRAVITY, 0.0f));
//    (*player)->setSpeed(1.0f);
//    (*player)->m_jumpingPower = 4.0f;
//    
//    (*player)->setTextureID(loadTexture(SPRITESHEET_FILEPATH));
//        
//    (*player)->m_walking[(*player)->LEFT] =
//        new int[4] { 1, 5,  9, 13 };
//    (*player)->m_walking[(*player)->RIGHT] =
//        new int[4] { 3, 7, 11, 15 };
//    (*player)->m_walking[(*player)->UP] =
//        new int[4] { 2, 6, 10, 14 };
//    (*player)->m_walking[(*player)->DOWN] =
//        new int[4] { 4, 8, 12, 16 };
//        
//    (*player)->m_animationIndices =
//        (*player)->m_walking[(*player)->DOWN];
//    (*player)->m_animationFrames   = 4;
//    (*player)->m_animationIndex    = 0;
//    (*player)->m_animationTime     = 0.0f;
//    (*player)->m_animationCols     = 4;
//    (*player)->m_animationRows     = 4;
//    
//    (*player)->setHeight(0.8f);
//    (*player)->setWidth(0.9f);
//}
//
//void platformsInit(Entity** platforms) {
//    GLuint platformTextureID = loadTexture(PLATFORM_FILEPATH);
//    
//    *platforms = new Entity[PLATFORM_COUNT];
//    
//    int limit = PLATFORM_COUNT;
//    int randomInt = rand() % limit;
//    
//    for (int i = 0; i < PLATFORM_COUNT; i++) {
//        
//        (*platforms)[i].setTextureID(platformTextureID);
//        (*platforms)[i].setWidth(0.2f);
//        (*platforms)[i].setPosition(glm::vec3(i - 2.0f, -3.0f, 0.0f));
//        (*platforms)[i].update(0.0f, NULL, 0);
//        
////        if (i == randomInt) (*platforms)[i].type = ENEMY;
////        else (*platforms)[i].type = PLATFORM;
//        
//    }
//}
//
//void Initialise() {
//    SDL_Init(SDL_INIT_VIDEO);
//   g_displayWindow = SDL_CreateWindow("Hello, Entities",
//                                     SDL_WINDOWPOS_CENTERED,
//                                     SDL_WINDOWPOS_CENTERED,
//                                     WINDOW_WIDTH, WINDOW_HEIGHT,
//                                     SDL_WINDOW_OPENGL);
//    SDL_GLContext context = SDL_GL_CreateContext(g_displayWindow);
//    SDL_GL_MakeCurrent(g_displayWindow, context);
//    
//#ifdef _WINDOWS
//    glewInit();
//#endif
//    
//    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
//    
//    g_program.Load(V_SHADER_PATH, F_SHADER_PATH);
//    
//    g_viewMatrix        = glm::mat4(1.0f);
//    g_projectionMatrix  = glm::ortho(-ORTHO_DIM.x, ORTHO_DIM.x,
//                                     -ORTHO_DIM.y, ORTHO_DIM.y,
//                                     -ORTHO_DIM.z, ORTHO_DIM.z);
//    
//    g_program.SetViewMatrix(g_viewMatrix);
//    g_program.SetProjectionMatrix(g_projectionMatrix);
//    
//    glUseProgram(g_program.programID);
//    
//    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
//    
//    platformsInit(&g_gameState.platforms);
//    playerInit(&g_gameState.player);
//
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//}
//
//void ProcessInput() {
//    
//    g_gameState.player->setMovement(glm::vec3(0.0f));
//    
//    SDL_Event event;
//    while (SDL_PollEvent(&event)) {
//        switch (event.type) {
//            case SDL_QUIT:
//            case SDL_WINDOWEVENT_CLOSE:
//                g_gameIsRunning = false;
//                break;
//            case SDL_KEYDOWN:
//                switch (event.key.keysym.sym) {
//                    case SDLK_SPACE:
//                        if (g_gameState.player->m_collidedBottom)
//                            g_gameState.player->m_isJumping = true;
//                        break;
//                        
//                    case SDLK_q:
//                        g_gameIsRunning = false;
//                        break;
//                    default: break;
//                }
//            default: break;
//        }
//    }
//    
//    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
//    
//    if (keyState[SDL_SCANCODE_LEFT]) {
//        g_gameState.player->moveLeft();
//        g_gameState.player->m_animationIndices =
//            g_gameState.player->m_walking[g_gameState.player->LEFT];
//    } else if (keyState[SDL_SCANCODE_RIGHT]) {
//        g_gameState.player->moveRight();
//        g_gameState.player -> m_animationIndices =
//            g_gameState.player->m_walking[g_gameState.player->RIGHT];
//    }
//    
//    if (glm::length(g_gameState.player->getMovement()) > 1.0f)
//        g_gameState.player->setMovement(glm::normalize(g_gameState.player->getMovement()));
//}
//
//void Update() {
//    /* ----- DELTA TIME ----- */
//    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
//    float deltaTime = ticks - g_previousTicks;
//    g_previousTicks = ticks;
//    
//    /* ----- FIXED TIMESTEP ----- */
//    deltaTime += g_timeAccumulator;
//    if (deltaTime < FIXED_TIMESTEP) {
//        g_timeAccumulator = deltaTime;
//        return;
//    }
//    
//    while (deltaTime >= FIXED_TIMESTEP)
//    {
//        g_gameState.player->update(FIXED_TIMESTEP, g_gameState.platforms, PLATFORM_COUNT);
//        deltaTime -= FIXED_TIMESTEP;
//    }
//    
//    g_timeAccumulator = deltaTime;
//}
//
//void Render() {
//    glClear(GL_COLOR_BUFFER_BIT);
//    
//    for (size_t i = 0; i < PLATFORM_COUNT; i++)
//        g_gameState.platforms[i].render(&g_program);
//    
//    g_gameState.player->render(&g_program);
//    
//    SDL_GL_SwapWindow(g_displayWindow);
//}
//
//void shutdown() { SDL_Quit(); }
//
//
//
