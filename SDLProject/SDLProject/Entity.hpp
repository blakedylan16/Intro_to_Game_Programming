/// Dylan Blake
///  12.15.23
///  Intro to Game Programming CS 3113
///  Professor Sebastian Romero Cruz (they/them)
///

enum EntityType { PLATFORM, PLAYER, ENEMY };

class Entity {
private:
    bool m_isActive;
    
    int *m_animationRight = NULL, // move to the right
        *m_animationLeft  = NULL, // move to the left
        *m_animationUp    = NULL, // move upwards
        *m_animationDown  = NULL; // move downwards
    
    /* ----- PHYSICS ------ */
    glm::vec3   m_position,
                m_velocity,
                m_acceleration,
                m_defaultAcceleration;
    
    /* ----- TRANSFORMATIONS ----- */
    glm::vec3   m_movement,
                m_scale;
    glm::mat4   m_modelMatrix;
    float       m_speed;
    
    /* ----- TEXTURES ----- */
    GLuint m_textureID;
    
    float m_width     = 1,
          m_height    = 1;
    
public:
    EntityType type;
    
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

    float m_flyingPower = 0;
    
    bool m_collidedTop      = false,
         m_collidedBottom   = false,
         m_collidedLeft     = false,
         m_collidedRight    = false;
    
    bool    m_platformCollision = false,
            m_enemyCollision    = false;
    
    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void drawSprite(ShaderProgram *program, GLuint textureID, int index);
    void update(float deltaTime = 0.0f,
                Entity* platforms = NULL, int platformCount = 0,
                Entity* enemies = NULL, int enemyCount = 0);
    void render(ShaderProgram* program);
    
    void moveLeft()     { m_movement.x = -1.0f; };
    void moveRight()    { m_movement.x =  1.0f; };
    void moveUp()       { m_movement.y =  1.0f; };
    void moveDown()     { m_movement.y = -1.0f; };

    bool const checkCollision(Entity* other) const;
    void const checkCollisonX(Entity* collidables, int collidablesCount);
    void const checkCollisonY(Entity* collidables, int collidablesCount);
    
    void const activate()       { m_isActive = true; };
    void const deactivate()     { m_isActive = false; };
    bool const getActiveState() { return m_isActive; };
    
    /* ————— GETTERS ————— */
    glm::vec3 const getPosition()       const { return m_position; };
    glm::vec3 const getMovement()       const { return m_movement; };
    glm::vec3 const getScale()          const { return m_scale; };
    glm::vec3 const getVelocity()       const { return m_velocity; };
    glm::vec3 const getAcceleration()   const { return m_acceleration; };
    float const getSpeed()              const { return m_speed; };
    
    float const getHeight() const { return m_height; };
    float const getWidth()  const { return m_width; };
    
    bool const getPlatformCollision()   { return m_platformCollision; };
    bool const getEnemyCollison()       { return m_enemyCollision; };
    
    GLuint const getTextureID()         const { return m_textureID; };
    
    /* ————— SETTERS ————— */
    void const setPosition(glm::vec3 pos)       { m_position = pos; };
    void const setMovement(glm::vec3 mov)       { m_movement = mov; };
    void const setScale(glm::vec3 scale)        { m_scale = scale;}
    void const setVelocity(glm::vec3 vel)       { m_velocity = vel; };
    void const setAcceleration(glm::vec3 accel) { m_acceleration = accel; };
    void const setDefAccel(glm::vec3 accel)     { m_defaultAcceleration = accel; };
    void const setSpeed(float speed)            { m_speed = speed; };
    void const setFlyingPower(float power)      { m_flyingPower = power; };
    
    void const setHeight(float newHeight)   { m_height = newHeight; };
    void const setWidth(float newWidth)     { m_width = newWidth; };
    
    void const setPlatformCollision(bool col)   { m_platformCollision = col; };
    void const setEnemyCollision(bool col)      { m_enemyCollision = col; };
    
    void const setTextureID(GLuint newID)           { m_textureID = newID; };
    
};
