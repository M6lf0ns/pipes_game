#include "Box.h"
#include <iostream>
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "Floor.h"


namespace gl3 {


    Box::Box(glm::vec3 position, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld)
        :Entity(Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            position,
            0,
            scale,
            color,
            physicsWorld),
    sprite(position, scale, Texture::LoadTexture("assets/textures/temp_box1.png"))

    {


        halfWidth = scale.x / 2.0f;
        halfHeight = scale.y / 2.0f;

        Box::createPhysicsBody();

        // Make sure sprite is initially at the physics body's position
        b2Vec2 bodyPos = b2Body_GetPosition(body);
        sprite.setPosition(glm::vec3(bodyPos.x, bodyPos.y, position.z));

    }

    void Box::createPhysicsBody() {

        //Define Body
        b2BodyDef boxBodyDef = b2DefaultBodyDef();
        boxBodyDef.type = b2_dynamicBody;
        boxBodyDef.position = b2Vec2{position.x,  position.y};
        boxBodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &boxBodyDef);

        auto bodyPos = b2Body_GetTransform(body);
        std::cout << "POSITION BODY BOX " << bodyPos.p.x << " " << bodyPos.p.y << " " <<  std::endl;
        glm::vec3 spritepos = sprite.getPosition();
        std::cout << "SPRITE POSITION BOX " << spritepos.x << spritepos.y << spritepos.z << std::endl;
        std::cout << "BOY HALFWIDTH " << halfWidth << "BOY HALFHEIGHT " << halfHeight << std::endl;
        glm::vec3 spriteScale = sprite.getScale();
        float spriteWidth = spriteScale.x;
        float spriteHeight = spriteScale.y;
        std::cout << " WIDTH SPRITE " << spriteWidth << "HEIGHT SPRITE  " << spriteHeight << std::endl;



        //Define box shape
        b2Polygon box = b2MakeBox(halfWidth, halfHeight);
        b2ShapeDef boxShapeDef = b2DefaultShapeDef();
        shape = b2CreatePolygonShape(body, &boxShapeDef, &box);
        b2Shape_SetFriction(shape, 0.8f);
        b2Shape_SetRestitution(shape, 0.1f);
        b2Shape_EnableContactEvents(shape,true);

        b2MassData massData;
        massData.mass = 1.5f;
        massData.center = b2Vec2{0.0f, 0.0f};
        massData.rotationalInertia = 0.0f;
        b2Body_SetMassData(body, massData);
        b2Body_SetLinearDamping(body, 0.5f);


    }

    void Box::updateBasedOnPhysics() {

        Entity::updateBasedOnPhysics();
        //setting the sprite´s position, can be avoided if we add Sprite to Entity
        //would not need to be overriden anymore
        auto bodyPos = b2Body_GetTransform(body);
        sprite.setPosition(glm::vec3(bodyPos.p.x, bodyPos.p.y, 0.0f));




    }

    float Box::GetHalfHeight() const {
        return halfHeight;
    }




    void Box::draw(Game *game) {
        sprite.draw(shader);
        DebugDraw();
        // glm::vec3 spriteScale = sprite.getScale();
        // std::cout << spriteScale.x << " " <<  spriteScale.y << " " << spriteScale.z << std::endl;
    }

    void Box::startContact() {
        std::cout << "Box::startContact" << std::endl;
    }

    void Box::DebugDraw() {
        // Get physics body transform
        auto bodyPos = b2Body_GetTransform(body);

        // Calculate the edges of the box
        float left   = bodyPos.p.x - halfWidth;
        float right  = bodyPos.p.x + halfWidth;
        float bottom = bodyPos.p.y - halfHeight;
        float top    = bodyPos.p.y + halfHeight;

        // Log the box information
        std::cout << "=== DEBUG BOX ===" << std::endl;
        std::cout << "Body Position: (" << bodyPos.p.x << ", " << bodyPos.p.y << ")" << std::endl;
        std::cout << "Half-Width: " << halfWidth << ", Half-Height: " << halfHeight << std::endl;
        std::cout << "Edges -> Left: " << left << ", Right: " << right
                  << ", Bottom: " << bottom << ", Top: " << top << std::endl;

        // AABB
        float minX = left;
        float maxX = right;
        float minY = bottom;
        float maxY = top;
        std::cout << "AABB -> Min: (" << minX << ", " << minY << "), Max: (" << maxX << ", " << maxY << ")" << std::endl;

        // Vertices for drawing
        std::vector<glm::vec3> vertices = {
            {left, bottom, 0.0f},
            {right, bottom, 0.0f},
            {right, top, 0.0f},
            {left, top, 0.0f}
        };

        // Log vertices
        for (int i = 0; i < vertices.size(); ++i) {
            std::cout << "Vertex " << i << ": (" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << ")" << std::endl;
        }

        // Draw box outline using your existing shader
        shader.use();
        shader.setVector("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

        GLuint VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }




}
