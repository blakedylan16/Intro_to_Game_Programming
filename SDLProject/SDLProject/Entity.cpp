//
//  Entity.cpp
//  SDLProject
//
//  Created by Dylan Blake on 12/12/23.
//  Copyright © 2023 ctg. All rights reserved.
//
#define GL_SILENCE_DEPRECIATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"               // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp" // Matrix transformation methods
#include "ShaderProgram.h"
#include "Entity.hpp"

Entity::Entity() {
    m_position      = glm::vec3(0);
    m_acceleration  = glm::vec3{0.0f};
    m_velocity      = glm::vec3{0.0f};
    m_speed = 1.0f;
    
    m_modelMatrix   = glm::mat4(1.0f);
}

Entity::~Entity() {
    delete [] m_animationUp;
    delete [] m_animationDown;
    delete [] m_animationLeft;
    delete [] m_animationRight;
    delete [] m_walking;
}

void Entity::drawSprite(ShaderProgram *program,
                        GLuint textureID, int index) {
    // Calculate the UV location of the indexed frame
    float UCoord = (float) (index % m_animationCols) / (float) m_animationCols,
          VCoord = (float) (index / m_animationCols) / (float) m_animationRows;
    // Calculate its UV size
    float width  = 1.0f / (float) m_animationCols,
          height = 1.0f / (float) m_animationRows;
    // Match the texture coordinates to the vertices
    float texCoords[] = {
        UCoord, VCoord + height,
        UCoord + width, VCoord + height,
        UCoord + width, VCoord,
        UCoord, VCoord + height, 
        UCoord + width, VCoord,
        UCoord, VCoord
    };
    float vertices[] = {
        -0.5, -0.5,
         0.5, -0.5,
         0.5,  0.5,
        -0.5, -0.5,
         0.5,  0.5,
        -0.5,  0.5
    };
    
    // Render
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::update(float deltaTime, Entity* collidables, int collidablesCount) {
    if (m_animationIndices) {
        if (glm::length(m_movement) != 0) {
            m_animationTime += deltaTime;
            float framesPerSecond = (float) 1 / SECONDS_PER_FRAME;
        
            if (m_animationTime >= framesPerSecond) {
                m_animationTime = 0.0f;
                m_animationIndex++;
                
                if (m_animationIndex >= m_animationFrames)
                    m_animationIndex = 0;
            }
        }
    }
    
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * deltaTime;
    m_position += m_velocity * deltaTime;
    
    m_modelMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::translate(m_modelMatrix, m_position);
    
    for (size_t i = 0; i < collidablesCount; i++){
        
        Entity* collidable = &collidables[i];
        
        if (checkCollison(collidable)) {
            float yDistance = fabs(m_position.y - collidable->m_position.y);
            float yOverlap = fabs(yDistance - (m_height / 2.0f) - (collidable->m_height / 2.0f));
        }
    }
}

void Entity::render(ShaderProgram *program) {
    
    program->SetModelMatrix(m_modelMatrix);
    
    if (m_animationIndices) {
        drawSprite(program, m_textureID,
                   m_animationIndices[m_animationIndex]);
        return;
    }
    
    float vertices[] = {
        -0.5f, -0.5, 
        0.5, -0.5,
        0.5, 0.5,
        -0.5f, -0.5f, 
        0.5,  0.5,
        -0.5, 0.5
    };
    float texCoords[] = {
        0.0f, 1.0f, 
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
        
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
        
    glDrawArrays(GL_TRIANGLES, 0, 6);
        
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

bool const Entity::checkCollison(Entity* other) const {
    float xDistance =
        fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float yDistance =
        fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);
    
    return xDistance < 0.0f && yDistance < 0.0f;
}

//void draw_text(ShaderProgram *program, GLuint fontTextureID, std::string text, float screenSize, float spacing, glm::vec3 position) {
//    // Scale the size of the fontbank in the UV-plane
//    // We will use this for spacing and positioning
//    float width = 1.0f / FONTBANK_SIZE;
//    float height = 1.0f / FONTBANK_SIZE;
//
//    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
//    // Don't forget to include <vector>!
//    std::vector<float> vertices;
//    std::vector<float> textureCoordinates;
//
//    // For every character...
//    for (int i = 0; i < text.size(); i++) {
//        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
//        //    relative to the whole sentence)
//        int spritesheetIndex = (int) text[i];  // ascii value of character
//        float offset = (screenSize + spacing) * i;
//
//        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
//        float UCoordinate = (float) (spritesheetIndex % FONTBANK_SIZE) / FONTBANK_SIZE;
//        float VCoordinate = (float) (spritesheetIndex / FONTBANK_SIZE) / FONTBANK_SIZE;
//
//        // 3. Inset the current pair in both vectors
//        vertices.insert(vertices.end(), {
//            offset + (-0.5f * screenSize), 0.5f * screenSize,
//            offset + (-0.5f * screenSize), -0.5f * screenSize,
//            offset + (0.5f * screenSize), 0.5f * screenSize,
//            offset + (0.5f * screenSize), -0.5f * screenSize,
//            offset + (0.5f * screenSize), 0.5f * screenSize,
//            offset + (-0.5f * screenSize), -0.5f * screenSize,
//        });
//
//        textureCoordinates.insert(textureCoordinates.end(), {
//            UCoordinate, VCoordinate,
//            UCoordinate, VCoordinate + height,
//            UCoordinate + width, VCoordinate,
//            UCoordinate + width, VCoordinate + height,
//            UCoordinate + width, VCoordinate,
//            UCoordinate, VCoordinate + height,
//        });
//    }
//
//    glm::mat4 modelMatr = glm::mat4(1.0f);
//    modelMatr = glm::translate(modelMatr, position);
//
//    program->SetModelMatrix(modelMatr);
//    glUseProgram(program->programID);
//
//    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
//    glEnableVertexAttribArray(program->positionAttribute);
//    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());
//    glEnableVertexAttribArray(program->texCoordAttribute);
//
//    glBindTexture(GL_TEXTURE_2D, fontTextureID);
//    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));
//
//    glDisableVertexAttribArray(program->positionAttribute);
//    glDisableVertexAttribArray(program->texCoordAttribute);
//}
