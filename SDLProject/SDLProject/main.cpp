/*
 Author: Dylan Blake
 Assignment: Simple 2D Scene
 I pledge that I have completed this assignment without collaborating w/
 anyone else, in conformance w/ the NYU School of Engineering Policies
 and Procedures on Academic Misconduct.
 */

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECIATION
#define GL_GLEXT_PROTOTYPES 1
#define NUMBER_OF_ENEMIES 2
#define FIXED_TIMESTEP 0.0166666f
#define ACC_OF_GRAVITY -2.0f
#define PLATFORM_COUNT 10

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Entity.hpp"
#include "cmath"
#include <ctime>
#include <vector>
#include <cstdlib>

// ----- STRUCTS AND ENUMS ----- //
struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;
};

const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED      = 0.1922f,
            BG_BLUE     = 0.549f,
            BG_GREEN    = 0.9059f,
            BG_OPACITY  = 1.0f;

const float ORTHO_HEIGHT    = 7.5f,
            ORTHO_WIDTH     = 10.0f;
const glm::vec3 ORTHO_DIM   = glm::vec3(ORTHO_WIDTH/2,
                                        ORTHO_HEIGHT/2,
                                        1.0f);

const int VIEWPORT_X        = 0,
          VIEWPORT_Y        = 0,
          VIEWPORT_WIDTH    = WINDOW_WIDTH,
          VIEWPORT_HEIGHT   = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

const char  SPRITESHEET_FILEPATH[]  = "assets/george_0.png",
            PLATFORM_FILEPATH[]     = "assets/platform_tile.png",
            ENEMY_FILEPATH[]        = "assets/Minotaur2.png",
            FONT_FILEPATH[]         = "assets/font1.png";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0,
            TEXTURE_BORDER  = 0;

/* ----- VARIABLES ------ */

GameState g_gameState;

bool    missionAccomplished = false,
        missionFailed       = false;

SDL_Window* g_displayWindow;
bool g_gameIsRunning = true;

ShaderProgram g_program;
glm::mat4 g_viewMatrix, g_projectionMatrix;

float g_previousTicks = 0.0f;
float g_timeAccumulator = 0.0f;

const glm::vec3 environAccel = glm::vec3(0.0f, ACC_OF_GRAVITY, 0.0f);

const int FONTBANK_SIZE = 16;
GLuint fontTextureID;

GLuint loadTexture(const char* filepath);

// Part1: Initialise our program
void Initialise();

// Part 2: Process any player input - pressed button or moved joystick
void ProcessInput();

// Part 3: Update game state given player input and previous state
void Update();
// Part 4: Once updated, render those changes onto the screen
void Render();
// Part 5: Shutdown protocol once game is over
void shutdown();

void playerInit();

void platformsInit();

void drawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float screenSize, float spacing, glm::vec3 position);

// The game will reside inside the main
int main(int argc, char* argv[]) {
    
    Initialise();
    
    while (g_gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    shutdown();
    return 0;
}
             
GLuint loadTexture(const char* filepath) {
    int width, height, numComponents;
    unsigned char* image = stbi_load(filepath, &width, &height,
                                     &numComponents, STBI_rgb_alpha);
    
    if (!image) {
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert(false);
        exit(1);
    }
    
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height,
                 TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    return textureID;
}

void playerInit() {
    
    g_gameState.player = new Entity();
    g_gameState.player->type = PLAYER;
    g_gameState.player->activate();
    
    g_gameState.player->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
    g_gameState.player->setMovement(glm::vec3(0.0f));
    g_gameState.player->setAcceleration(environAccel);
    g_gameState.player->setDefAccel(environAccel);
    g_gameState.player->setFlyingPower(6.0f);
    
    g_gameState.player->setTextureID(loadTexture(SPRITESHEET_FILEPATH));
        
    g_gameState.player->m_walking[g_gameState.player->LEFT] =
        new int[4] { 1, 5,  9, 13 };
    g_gameState.player->m_walking[g_gameState.player->RIGHT] =
        new int[4] { 3, 7, 11, 15 };
    g_gameState.player->m_walking[g_gameState.player->UP] =
        new int[4] { 2, 6, 10, 14 };
    g_gameState.player->m_walking[g_gameState.player->DOWN] =
        new int[4] { 4, 8, 12, 16 };
        
    g_gameState.player->m_animationIndices =
        g_gameState.player->m_walking[g_gameState.player->DOWN];
    g_gameState.player->m_animationFrames   = 4;
    g_gameState.player->m_animationIndex    = 0;
    g_gameState.player->m_animationTime     = 0.0f;
    g_gameState.player->m_animationCols     = 4;
    g_gameState.player->m_animationRows     = 4;
    
    g_gameState.player->setHeight(0.8f);
    g_gameState.player->setWidth(0.9f);
}

void platformsInit() {
    GLuint platformTextureID = loadTexture(PLATFORM_FILEPATH);
    
    g_gameState.platforms = new Entity[PLATFORM_COUNT];
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        
        
        g_gameState.platforms[i].type = PLATFORM;
        g_gameState.platforms[i].activate();
        
        g_gameState.platforms[i].setTextureID(platformTextureID);
        g_gameState.platforms[i].setHeight(1.0f);
        g_gameState.platforms[i].setWidth(0.5f);
        g_gameState.platforms[i].setScale(glm::vec3(1.0f, .75f, 1.0f));
        g_gameState.platforms[i].setPosition(glm::vec3(i - 4.5f, -3.6f, 0.0f));
        g_gameState.platforms[i].update();
    }
}

void enemiesInit() {
    GLuint enemiesTextureID = loadTexture(ENEMY_FILEPATH);
    
    g_gameState.enemies = new Entity[NUMBER_OF_ENEMIES];
    
    for (int i = 0; i < NUMBER_OF_ENEMIES; i++ ) {
        g_gameState.enemies[i].type = ENEMY;
        g_gameState.enemies[i].activate();
        g_gameState.enemies[i].setTextureID(enemiesTextureID);
        g_gameState.enemies[i].setHeight(.9f);
        g_gameState.enemies[i].setWidth(.55f);
        g_gameState.enemies[i].setScale(glm::vec3(2.0f, 2.0f, 1.0f));
        g_gameState.enemies[i].setMovement(glm::vec3(0.0f));
        g_gameState.enemies[i].setAcceleration(environAccel);
        g_gameState.enemies[i].setDefAccel(environAccel);
    }
    
    g_gameState.enemies[0].setPosition(glm::vec3(-2.0f, -2.0f, 0.0f));
    g_gameState.enemies[1].setPosition(glm::vec3(3.0f, -2.0f, 0.0f));
}

void Initialise() {
    SDL_Init(SDL_INIT_VIDEO);
   g_displayWindow = SDL_CreateWindow("Hello, Entities",
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     WINDOW_WIDTH, WINDOW_HEIGHT,
                                     SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(g_displayWindow);
    SDL_GL_MakeCurrent(g_displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_program.Load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_viewMatrix        = glm::mat4(1.0f);
    g_projectionMatrix  = glm::ortho(-ORTHO_DIM.x, ORTHO_DIM.x,
                                     -ORTHO_DIM.y, ORTHO_DIM.y,
                                     -ORTHO_DIM.z, ORTHO_DIM.z);
    
    g_program.SetViewMatrix(g_viewMatrix);
    g_program.SetProjectionMatrix(g_projectionMatrix);
    
    glUseProgram(g_program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    platformsInit();
    enemiesInit();
    playerInit();
    
    fontTextureID = loadTexture(FONT_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
    
    g_gameState.player->setMovement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_gameIsRunning = false;
                        break;
                    default: break;
                }
            default: break;
        }
    }
    
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    
    if (keyState[SDL_SCANCODE_LEFT]) {
        g_gameState.player->moveLeft();
        g_gameState.player->m_animationIndices =
            g_gameState.player->m_walking[g_gameState.player->LEFT];
    } else if (keyState[SDL_SCANCODE_RIGHT]) {
        g_gameState.player->moveRight();
        g_gameState.player -> m_animationIndices =
            g_gameState.player->m_walking[g_gameState.player->RIGHT];
    } else if (keyState[SDL_SCANCODE_SPACE]) {
        g_gameState.player->moveUp();
        g_gameState.player -> m_animationIndices =
            g_gameState.player-> m_walking[g_gameState.player->DOWN];
    }
    
//    if (glm::length(g_gameState.player->getMovement()) > 1.0f)
//        g_gameState.player->setMovement(glm::normalize(g_gameState.player->getMovement()));
}

void Update() {
    /* ----- DELTA TIME ----- */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float deltaTime = ticks - g_previousTicks;
    g_previousTicks = ticks;
    
    /* ----- FIXED TIMESTEP ----- */
    deltaTime += g_timeAccumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        g_timeAccumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        g_gameState.player->update(FIXED_TIMESTEP,
                                   g_gameState.platforms, PLATFORM_COUNT,
                                   g_gameState.enemies, NUMBER_OF_ENEMIES);
        for (int i = 0; i < NUMBER_OF_ENEMIES; i++)
            g_gameState.enemies[i].update(FIXED_TIMESTEP,
                                          g_gameState.platforms, PLATFORM_COUNT);
        deltaTime -= FIXED_TIMESTEP;
    }
    
    g_timeAccumulator = deltaTime;
    
    if (not g_gameState.player->getActiveState()) {
        if (g_gameState.player->getEnemyCollison())
            missionFailed = true;
        else
            missionAccomplished = true;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (size_t i = 0; i < PLATFORM_COUNT; i++)
        g_gameState.platforms[i].render(&g_program);
    
    for (size_t i = 0; i < NUMBER_OF_ENEMIES; i++)
        g_gameState.enemies[i].render(&g_program);
    
    g_gameState.player->render(&g_program);
    
    
    if (missionFailed) {
        drawText(&g_program, fontTextureID, std::string("Mission Failed"),
                 .4f, .0f, glm::vec3(-2.3f, 1.0f, 0.0f));
    } else if (missionAccomplished) {
        drawText(&g_program, fontTextureID, std::string("Mission Accomplished"),
                 .4f, .0f, glm::vec3(-3.5f, 1.0f, 0.0f));
    }
    
    SDL_GL_SwapWindow(g_displayWindow);
}

void shutdown() { SDL_Quit(); }

void drawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float screenSize, float spacing, glm::vec3 position) {
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
    // Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> textureCoordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheetIndex = (int) text[i];  // ascii value of character
        float offset = (screenSize + spacing) * i;
        
        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float UCoordinate = (float) (spritesheetIndex % FONTBANK_SIZE) / FONTBANK_SIZE;
        float VCoordinate = (float) (spritesheetIndex / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screenSize), 0.5f * screenSize,
            offset + (-0.5f * screenSize), -0.5f * screenSize,
            offset + (0.5f * screenSize), 0.5f * screenSize,
            offset + (0.5f * screenSize), -0.5f * screenSize,
            offset + (0.5f * screenSize), 0.5f * screenSize,
            offset + (-0.5f * screenSize), -0.5f * screenSize,
        });

        textureCoordinates.insert(textureCoordinates.end(), {
            UCoordinate, VCoordinate,
            UCoordinate, VCoordinate + height,
            UCoordinate + width, VCoordinate,
            UCoordinate + width, VCoordinate + height,
            UCoordinate + width, VCoordinate,
            UCoordinate, VCoordinate + height,
        });
    }

    // 4. And render all of them using the pairs
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

