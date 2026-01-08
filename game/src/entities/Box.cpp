#include "Box.h"
#include <iostream>
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "Floor.h"
#include "box2d/src/joint.h"
#include "Ship.h"
#include "box2d/src/body.h"
#include "../Game.h"


namespace gl3 {


    Box::Box(glm::vec3 position, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld)
        :Entity(Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            position,
            0,
            scale,
            color,
            physicsWorld,
            EntityType::Box),
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

        std::cout << "Box: physicsWorld.index1 = " << physicsWorld.index1 << std::endl;

        if (physicsWorld.index1 < 1 || physicsWorld.index1 > 32) {
            std::cout << "ERROR: Box has invalid physicsWorld!" << std::endl;
            throw std::runtime_error("Invalid physics world in Box");
        }
        //Define Body
        b2BodyDef boxBodyDef = b2DefaultBodyDef();
        boxBodyDef.enableSleep = true;
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
        //b2Shape_SetUserData(shape, this);             // <<< IMPORTANT: let contact system identify this Box
        b2Shape_SetFriction(shape, 0.1f); // friction of box sides/bottom
        b2Shape_SetRestitution(shape, 0.05f);
        b2Shape_SetDensity(shape, 500.0f, true);
        b2Shape_EnableContactEvents(shape,true);
        b2Shape_EnableSensorEvents(shape, true);
        b2Shape_SetUserData(shape, this);


        b2Body_SetLinearDamping(body, 5.0f);


    }

    void Box::updateBasedOnPhysics() {

        Entity::updateBasedOnPhysics();
        // static int counter = 0;
        // std::cout << "Box updateBasedOnPhysics call #" << ++counter << std::endl;
        // //setting the sprite´s position, can be avoided if we add Sprite to Entity
        // //would not need to be overriden anymore
        //
        // static int frameCount = 0;
        b2Vec2 bodyPos = b2Body_GetPosition(body);
        //
        // // Log only when not touching player and ship is moving
        // if (!isTouchingPlayer && std::abs(Ship::playerVelX) > 0.1f) {
        //     if (frameCount++ % 30 == 0) { // Every 30 frames
        //         std::cout << "BOX JITTER CHECK - Pos: (" << bodyPos.x << ", " << bodyPos.y
        //                   << ") Vel: " << Ship::playerVelX
        //                   << " Touching: " << isTouchingPlayer << std::endl;
        //     }
        // }

        sprite.setPosition(glm::vec3(bodyPos.x, bodyPos.y, 0.0f));



    }

    void Box::update(Game *game, float deltaTime)
    {
        if (ignoreFirstContacts) ignoreFirstContacts = false;
        std::cout << "Is box touching player? " << playerContactCount << std::endl;
        if (playerContactCount > 0 && Ship::push_PullKeyPressed && isActuallyTouchingPlayer(game->getShip()))
        {


            b2Body_SetAwake(body, true);

            float playerVelX = Ship::playerVelX;
            float playerPosX = Ship::playerPos.x;
            b2Vec2 boxPos = b2Body_GetPosition(body);
            b2Vec2 currentVel = b2Body_GetLinearVelocity(body);

            // Determine if pushing or pulling
            float playerToBoxX = boxPos.x - playerPosX;
            bool pushing = (playerVelX * playerToBoxX > 0); // moving toward box
            bool pulling = (playerVelX * playerToBoxX < 0); // moving away from box

            if (pulling)
            {
                // PULLING: Instantly match player velocity (no lag)
                b2Body_SetLinearVelocity(body, {playerVelX, currentVel.y});
            }
            else if (pushing)
            {
                // PUSHING: Smooth interpolation feels more natural
                float smoothFactor = 15.0f;
                float newVelX = currentVel.x + (playerVelX - currentVel.x) * smoothFactor * deltaTime;
                b2Body_SetLinearVelocity(body, {newVelX, currentVel.y});
            }

            // Low friction for smooth movement
            b2Shape_SetFriction(shape, 0.1f);
            stillTime = 0.0f;
        }
        else
        {

            // RELEASE: Stop immediately
            b2Vec2 vel = b2Body_GetLinearVelocity(body);
            vel.x *= 0.95f;  // Slow down naturally
            b2Body_SetLinearVelocity(body, vel);
            b2Shape_SetFriction(shape, 0.8f);
        }

        // Sleep when stationary
        b2Vec2 vel = b2Body_GetLinearVelocity(body);
        float speed = sqrtf(vel.x * vel.x + vel.y * vel.y);

        if (speed < 0.01f) {
            stillTime += deltaTime;
            if (stillTime >= 0.5f) {
                b2Body_SetAwake(body, false);
                stillTime = 0.0f;
            }
        } else {
            stillTime = 0.0f;
        }
    }

    float Box::GetHalfHeight() const {
        return halfHeight;
    }




    void Box::draw(Game *game) {
        Entity::draw(game);
        sprite.draw(shader,game);
        // std::cout << "BOX - World Pos: " << position.x << ", " << position.y
        //   << " Screen Pos: " << sprite.getPosition().x << ", " << sprite.getPosition().y << std::endl;
        //
        // // glm::vec3 spriteScale = sprite.getScale();
        // // std::cout << spriteScale.x << " " <<  spriteScale.y << " " << spriteScale.z << std::endl;
    }

    void Box::startContact() {
        if (ignoreFirstContacts) return;
       playerContactCount++;
        std::cout << "[Box::startContact] this=" << this
              << " bodyType=" << b2Body_GetType(body)
              << " pos=(" << b2Body_GetPosition(body).x << "," << b2Body_GetPosition(body).y << ")"
              << std::endl;


    }

    void Box::endContact() {
        if (playerContactCount > 0) playerContactCount--;
        std::cout << "[Box::endContact] this=" << this << std::endl;
    }
    // ← Add this
    bool Box::isActuallyTouchingPlayer(Ship* player) {
        glm::vec3 diff = this->position - player->getPosition();
        float allowedX = 0.5f;
        float allowedY = 0.5f;
        return abs(diff.x) <= allowedX && abs(diff.y) <= allowedY;
    }

    // void Box::DebugDraw() {
    //     // Get physics body transform
    //     auto bodyPos = b2Body_GetTransform(body);
    //
    //     // Calculate the edges of the box
    //     float left   = bodyPos.p.x - halfWidth;
    //     float right  = bodyPos.p.x + halfWidth;
    //     float bottom = bodyPos.p.y - halfHeight;
    //     float top    = bodyPos.p.y + halfHeight;
    //
    //     // Log the box information
    //     std::cout << "=== DEBUG BOX ===" << std::endl;
    //     std::cout << "Body Position: (" << bodyPos.p.x << ", " << bodyPos.p.y << ")" << std::endl;
    //     std::cout << "Half-Width: " << halfWidth << ", Half-Height: " << halfHeight << std::endl;
    //     std::cout << "Edges -> Left: " << left << ", Right: " << right
    //               << ", Bottom: " << bottom << ", Top: " << top << std::endl;
    //
    //     // AABB
    //     float minX = left;
    //     float maxX = right;
    //     float minY = bottom;
    //     float maxY = top;
    //     std::cout << "AABB -> Min: (" << minX << ", " << minY << "), Max: (" << maxX << ", " << maxY << ")" << std::endl;
    //
    //     // Vertices for drawing
    //     std::vector<glm::vec3> vertices = {
    //         {left, bottom, 0.0f},
    //         {right, bottom, 0.0f},
    //         {right, top, 0.0f},
    //         {left, top, 0.0f}
    //     };
    //
    //     // Log vertices
    //     for (int i = 0; i < vertices.size(); ++i) {
    //         std::cout << "Vertex " << i << ": (" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << ")" << std::endl;
    //     }
    //
    //     // Draw box outline using your existing shader
    //     shader.use();
    //     shader.setVector("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    //
    //     GLuint VBO, VAO;
    //     glGenVertexArrays(1, &VAO);
    //     glGenBuffers(1, &VBO);
    //
    //     glBindVertexArray(VAO);
    //     glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //     glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
    //
    //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    //     glEnableVertexAttribArray(0);
    //
    //     glDrawArrays(GL_LINE_LOOP, 0, 4);
    //
    //     glDeleteBuffers(1, &VBO);
    //     glDeleteVertexArrays(1, &VAO);
    // }




}
