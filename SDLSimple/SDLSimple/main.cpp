#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif


const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;
const float BG_RED      = .1922f,
            BG_BLUE     = .549f,
            BG_GREEN    = .9059f,
            BG_OPACITY  = 1.0f;

SDL_Window* displayWindow;
bool gameIsRunning = true;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow =
        SDL_CreateWindow("Hello, World!", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                         SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    SDL_Event event;
    while (gameIsRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                gameIsRunning = false;
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
