//
//  helper.cpp
//  SDLProject
//
//  Created by Dylan Blake on 11/15/23.
//  Copyright © 2023 ctg. All rights reserved.
//



#define GL_SILENCE_DEPRECIATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"

const char MAT_SEP = '\t';

void print_matrix(glm::mat4 &matrix, int size) {
    for (auto row = 0 ; row < size ; row++) {
        for (auto col = 0 ; col < size ; col++) {
            std::cout << matrix[row][col] << MAT_SEP;
            
        }
        std::cout << "\n";
    }
}
