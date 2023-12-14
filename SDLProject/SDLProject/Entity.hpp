//
//  Entity.hpp
//  SDLProject
//
//  Created by Dylan Blake on 12/12/23.
//  Copyright © 2023 ctg. All rights reserved.
//


class Entity {
private:
    int *m_animationRight = NULL, // move to the right
        *m_animationLeft  = NULL, // move to the left
        *m_animationUp    = NULL, // move upwards
        *m_animationDown  = NULL; // move downwards
    
    glm::vec3 m_position;
public:
    // ————— STATIC VARIABLES ————— //
    static const int SECONDS_PER_FRAME = 4;
    static const int LEFT  = 0,
                     RIGHT = 1,
                     UP    = 2,
                     DOWN  = 3;
    
    // ————— ANIMATION ————— //
    int **m_walking = new int*[4] {
        m_animationLeft,
        m_animationRight,
        m_animationUp,
        m_animationDown
    };
    
    int m_animationFrames = 0,
        m_animationIndex  = 0,
        m_animationCols   = 0,
        m_animationRows   = 0;
    
    int  *m_animationIndices = NULL;
    float m_animationTime    = 0.0f;
    
    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    glm::mat4 m_modelMatrix;
    
    GLuint    m_textureID;

    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void drawSprite(ShaderProgram *program, GLuint textureID, int index);
    void update(float deltaTime);
    void render(ShaderProgram *program);
    
    // ————— GETTERS ————— //
    glm::vec3 const getPosition() const { return m_position; };
    glm::vec3 const getMovement() const { return m_movement; };
    
    // ————— SETTERS ————— //
    void const setPosition(glm::vec3 newPosition) { m_position = newPosition; };
    void const setMovement(glm::vec3 newMovement) { m_movement = newMovement; };
    void const setSpeed(float newSpeed) { m_speed = newSpeed; };
    void const setTextureID(GLuint newID) { m_textureID = newID; };
};
