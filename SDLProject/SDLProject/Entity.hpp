/// Dylan Blake
///  12.15.23
///  Intro to Game Programming CS 3113
///  Professor Sebastian Romero Cruz (they/them)
///

enum EntityType { PLATFORM, PLAYER, ENEMY };

class Entity {
private:
    int *m_animationRight = NULL, // move to the right
        *m_animationLeft  = NULL, // move to the left
        *m_animationUp    = NULL, // move upwards
        *m_animationDown  = NULL; // move downwards
    
    /* ----- TRANSFORMATIONS ------ */
    glm::vec3   m_position,
                m_movement,
                m_velocity,
                m_acceleration;
    glm::mat4   m_modelMatrix;
    float       m_speed;
    
    /* ----- TEXTURES ----- */
    GLuint m_textureID;
    
    int m_width     = 1,
        m_height    = 1;
    
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

    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void drawSprite(ShaderProgram *program, GLuint textureID, int index);
    void update(float deltaTimem, Entity* collidables, int collidablesCount);
    void render(ShaderProgram* program);
    
    void moveLeft()     { m_movement.x = -1.0f; };
    void moveRight()    { m_movement.x =  1.0f; };
    void moveUp()       { m_movement.y =  1.0f; };
    void moveDown()     { m_movement.y = -1.0f; };
    
    bool const checkCollison(Entity *other) const;
    
    // ————— GETTERS ————— //
    glm::vec3 const getPosition()       const { return m_position; };
    glm::vec3 const getMovement()       const { return m_movement; };
    glm::vec3 const getVelocity()       const { return m_velocity; };
    glm::vec3 const getAcceleration()   const { return m_acceleration; };
    float const getSpeed()              const { return m_speed; };
    
    GLuint const getTextureID()         const { return m_textureID; };
    
    // ————— SETTERS ————— //
    void const setPosition(glm::vec3 newPos)        { m_position = newPos; };
    void const setMovement(glm::vec3 newMov)        { m_movement = newMov; };
    void const setVelocity(glm::vec3 newVel)        { m_velocity = newVel; };
    void const setAcceleration(glm::vec3 newAccel)  { m_acceleration = newAccel; };
    void const setSpeed(float newSpeed)             { m_speed = newSpeed; };
    
    void const setTextureID(GLuint newID)           { m_textureID = newID; };
};
