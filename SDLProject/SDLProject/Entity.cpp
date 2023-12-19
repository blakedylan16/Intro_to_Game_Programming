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
    /* ----- PHYSICS ----- */
    m_position      = glm::vec3(0);
    m_acceleration  = glm::vec3{0.0f};
    m_velocity      = glm::vec3{0.0f};
    
    /* ----- TRANSLATION -----*/
    m_movement  = glm::vec3(0.0f);
    m_scale     = glm::vec3(1.0f);
    
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
    float UCoord = (float)(index % m_animationCols) / (float)m_animationCols,
          VCoord = (float)(index / m_animationCols) / (float)m_animationRows;
    // Calculate its UV size
    float width  = 1.0f / (float)m_animationCols,
          height = 1.0f / (float)m_animationRows;
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

void Entity::update(float deltaTime,
                    Entity* platforms, int platformCount,
                    Entity* enemies, int enemyCount) {
    
    if (!m_isActive) return;
    
    m_collidedTop = false;
    m_collidedBottom = false;
    m_collidedLeft = false;
    m_collidedRight = false;
    
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
    
    /* ----- GRAVITY ----- */
    /*
     pretending application of force is instant, therefore
     change in acceleration is instant
     */
    
    m_acceleration = m_movement * m_flyingPower + m_defaultAcceleration;
    m_velocity += m_acceleration * deltaTime;
    
    if (type == ENEMY) {
        checkCollisonY(platforms, platformCount);
        checkCollisonX(platforms, platformCount);
        
        checkCollisonY(enemies, enemyCount);
        checkCollisonX(enemies, enemyCount);
    }
    
    for (size_t i = 0; i < platformCount; i++) {
        Entity* platform = &platforms[i];
        if (checkCollision(platform))
            setPlatformCollision(true);
    }
    
    for (size_t i = 0; i < enemyCount; i++) {
        Entity* enemy = &enemies[i];
        if (checkCollision(enemy))
            setEnemyCollision(true);
    }
    
    if (type == PLAYER and (getPlatformCollision() or getEnemyCollison())) {
        deactivate();
        return;
    }
    
    m_position += m_velocity * deltaTime;
    
    /* ----- TRANSFORMATIONS ----- */
    m_modelMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::translate(m_modelMatrix, m_position);
    m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
}


bool const Entity::checkCollision(Entity* other) const {
    
    if (other == this) return false;
    
    if (!m_isActive or not other->m_isActive) return false;

    float xDistance =
        fabs(m_position.x - other->m_position.x) -
        ((m_width + other->m_width) / 2.0f);
    float yDistance =
        fabs(m_position.y - other->m_position.y) -
        ((m_height + other->m_height) / 2.0f);
    
    return xDistance < 0.0f and yDistance < 0.0f;
}

void const Entity::checkCollisonX(Entity* collidables,
                                  int collidablesCount) {
    
    for (size_t i = 0; i < collidablesCount; i++){
        
        Entity* collidable = &collidables[i];
        
        if (checkCollision(collidable)) {
            float xDistance = fabs(m_position.x - collidable->m_position.x);
            float xOverlap =
                fabs(xDistance - (m_width / 2.0f) - (collidable->m_width / 2.0f));
            
            if (m_velocity.x > 0.1f) {
                m_position.x -= xOverlap;
                m_velocity.x = 0;
                m_collidedRight = true;
            } else if (m_velocity.x < 0.1f) {
                m_position.x += xOverlap;
                m_velocity.x = 0;
                m_collidedLeft = true;
            }
        }
    }
}

void const Entity::checkCollisonY(Entity* collidables,
                                  int collidablesCount) {
    
    for (size_t i = 0; i < collidablesCount; i++){
        
        Entity* collidable = &collidables[i];
        
        if (checkCollision(collidable)) {
            
            float yDistance = fabs(m_position.y - collidable->m_position.y);
            float yOverlap =
                fabs(yDistance - (m_height / 2.0f) - (collidable->m_height / 2.0f));
            
            if  (m_velocity.y > 0.1f) {
                m_position.y -= yOverlap;
                m_velocity.y = 0;
                m_collidedTop = true;
            } else if (m_velocity.y < 0.1f) {
                m_position.y += yOverlap;
                m_velocity.y = 0;
                m_collidedBottom = true;
            }
        }
    }
}

void Entity::render(ShaderProgram *program) {
    
    program->SetModelMatrix(m_modelMatrix);
    
    if (!m_isActive) return;
    
    if (m_animationIndices) {
        drawSprite(program, m_textureID, m_animationIndices[m_animationIndex]);
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

