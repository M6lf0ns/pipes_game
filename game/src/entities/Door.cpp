#include "Door.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include <iostream>

#include "../Game.h"
#include "../PhysicsCategories.h"

namespace gl3 {

    Door::Door(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld)
      :Entity(shader,
          position,
          scale,
          physicsWorld,
          EntityType::Door)

    {
        this->closedTex = Texture::LoadTexture("assets/textures/door.png");
        this->openTex = Texture::LoadTexture("assets/textures/door_open.png");
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/door.png"));
        this->setZOrder(1);//MIDGROUND
        halfWidth = scale.x / 2.0f;
        halfHeight = scale.y / 2.0f;

        Door::createPhysicsBody();

        //Align sprite center with physicsbody
        sprite->setPosition(glm::vec3(position.x,position.y,0.0f));
        sprite->setScale(glm::vec3(halfWidth * 2, halfHeight * 2, 1.0f)); //full width and height
    }

    void Door::createPhysicsBody() {

        b2BodyDef doorBodyDef = b2DefaultBodyDef();
        doorBodyDef.type = b2_staticBody;
        doorBodyDef.position = b2Vec2{position.x,position.y};
        doorBodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &doorBodyDef);

        b2Polygon doorShape = b2MakeBox(halfWidth, halfHeight);
        b2ShapeDef doorShapeDef = b2DefaultShapeDef();
        doorShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_DOOR;
        doorShapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                               gl3::PhysicsCategories::CATEGORY_SEC_BOT |
                                   gl3::PhysicsCategories::CATEGORY_DOG  |
                                       gl3::PhysicsCategories::CATEGORY_STICK |
                                           gl3::PhysicsCategories::CATEGORY_BOX |
                                               gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;
        // 2. Ensure contact events are enabled (Box2D sometimes needs this for static bodies)
        doorShapeDef.enableContactEvents = true;
        shape = b2CreatePolygonShape(body, &doorShapeDef, &doorShape);
        b2Shape_SetFriction(shape, 0.0f);

        // ✅ ADDED: Set user data and enable events
        b2Shape_SetUserData(shape, this);



    }

    void Door::draw(Game *game) {
        Entity::draw(game);
    }
    void Door::update(Game *game, float deltaTime) {
        Entity::update(game, deltaTime);

        // Only play if it just changed from CLOSED to OPEN
        if (isDoorOpen && !wasDoorOpen) {
            game->PlaySfx(game->getDoorOpenCloseSFX(), 0.3f);
            wasDoorOpen = true; // IMPORTANT: Mark as played
        }
        // Handle the closing sound too if you want one
        else if (!isDoorOpen && wasDoorOpen) {
            game->PlaySfx(game->getDoorOpenCloseSFX(), 0.3f);
            wasDoorOpen = false; // IMPORTANT: Mark as played
        }
    }


    void Door::openDoor() {
        if (isDoorOpen) return;

        // Create a filter that masks out everything (0 bits)
        b2Filter openFilter = b2DefaultFilter();
        openFilter.maskBits = 0x0000;

        b2Shape_SetFilter(shape, openFilter);

        isDoorOpen = true;
        sprite->setTexture(openTex);
        wasDoorOpen = false;

    }

    void Door::closeDoor() {
        if (!isDoorOpen) return;

        // Restore the original filter so door can hit the bot/other maskBits again
        b2Filter closedFilter = b2DefaultFilter();
        closedFilter.categoryBits = gl3::PhysicsCategories::CATEGORY_DOOR;
        closedFilter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                                gl3::PhysicsCategories::CATEGORY_SEC_BOT |
                                gl3::PhysicsCategories::CATEGORY_DOG |
                                    gl3::PhysicsCategories::CATEGORY_BOX |
                                        gl3::PhysicsCategories::CATEGORY_RAY_SENSOR ;

        b2Shape_SetFilter(shape, closedFilter);

        isDoorOpen = false;
        wasDoorOpen = true;

        sprite->setTexture(closedTex);
        sprite->setPosition(glm::vec3(position.x, position.y, 0.0f));
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


    void Door::startContact(Entity* other) {

    }

    float Door::getHalfHeight() {
        return halfHeight;
    }






}
