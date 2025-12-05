#include "Door.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include <iostream>

namespace gl3 {

    Door::Door(glm::vec3 position, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld)
      :Entity(Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
          position,
          0,
          scale,
          color,
          physicsWorld,
          EntityType::Door),
        sprite(position, scale, Texture::LoadTexture("assets/textures/door_square.png"))

    {
        halfWidth = scale.x / 2.0f;
        halfHeight = scale.y / 2.0f;

        Door::createPhysicsBody();

        //Align sprite center with physicsbody
        sprite.setPosition(glm::vec3(position.x,position.y,0.0f));
        sprite.setScale(glm::vec3(halfWidth * 2, halfHeight * 2, 1.0f)); //full width and height
    }

    void Door::createPhysicsBody() {

        b2BodyDef doorBodyDef = b2DefaultBodyDef();
        doorBodyDef.type = b2_staticBody;
        doorBodyDef.position = b2Vec2{position.x,position.y};
        doorBodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &doorBodyDef);

        b2Polygon doorShape = b2MakeBox(halfWidth, halfHeight);
        b2ShapeDef doorShapeDef = b2DefaultShapeDef();
        shape = b2CreatePolygonShape(body, &doorShapeDef, &doorShape);
        b2Shape_SetFriction(shape, 0.5f);

        // ✅ ADDED: Set user data and enable events
        b2Shape_SetUserData(shape, this);


    }

    void Door::draw(Game *game) {
        Entity::draw(game);
        sprite.draw(shader, game);
    }



    void Door::openDoor() {
        if (isDoorOpen) return;

        std::cout << "Opening door..." << std::endl;

        // Destroy the physics body (removes collision)
        destroyPhysicsBody();

        isDoorOpen = true;

        // Optional: Change sprite to show open door
        // sprite = Sprite(position, scale, Texture::LoadTexture("assets/textures/door_open.png"));
        // Switch back to closed texture
        sprite.setTexture(Texture::LoadTexture("assets/textures/open_door.png"));

    }

    void Door::closeDoor() {

        if (!isDoorOpen) return;  // Already closed

        std::cout << "Closing door..." << std::endl;

        // Recreate the physics body (re-enables collision)
        createPhysicsBody();


        isDoorOpen = false;
        // Switch back to closed texture
        sprite.setTexture(Texture::LoadTexture("assets/textures/door_square.png"));

        // Optional: Change sprite back
        // sprite = Sprite(position, scale, Texture::LoadTexture("assets/textures/door_closed.png"));
    }



    void Door::destroyPhysicsBody() {
        //destroy shape first
        if (b2Shape_IsValid(shape)) {
            b2DestroyShape(shape, true);
        }


        // Then destroy body
        if (b2Body_IsValid(body)) {
            b2DestroyBody(body);
        }
    }


    void Door::startContact() {

    }

    float Door::getHalfHeight() {
        return halfHeight;
    }






}
