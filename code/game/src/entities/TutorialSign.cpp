#include "TutorialSign.h"

#include <iostream>

#include "../PhysicsCategories.h"
#include "../Game.h"



namespace gl3 {

    TutorialSign::TutorialSign(Shader* shader, glm::vec3 position, glm::vec3 scale, int linkID, b2WorldId physicsWorld)
      :Entity(shader,
          position,
          scale,
          physicsWorld,
          EntityType::TutorialSign)


    {

        backgroundSprite = new Sprite(position, scale, 0); // 0 = no texture

        // Use linkID to pick the right tutorial texture
        std::string texPath;
        switch (linkID) {
            case 1: texPath = "assets/textures/Level_1_Hint.png"; break;
            case 2: texPath = "assets/textures/Level_1_Tutorial_1.png"; break;
            case 3: texPath = "assets/textures/Level_1_Tutorial_2.png"; break;
            case 4: texPath = "assets/textures/Level_3_Hint_.png"; break;
            case 5: texPath = "assets/textures/Level_3_Tutorial.png"; break;
            case 6: texPath = "assets/textures/Level_5_Hint.png"; break;
            case 7: texPath = "assets/textures/Level_5_Tutorial.png"; break;
            case 8: texPath = "assets/textures/Level_6_Tutorial.png"; break;
            case 9: texPath = "assets/textures/Level_7_Hint.png"; break;
            case 10: texPath = "assets/textures/Level_7_Hint_2.png"; break;
            case 11: texPath = "assets/textures/Level_11_Hint_1.png"; break;
            case 12: texPath = "assets/textures/Level_12_Tutorial_1.png"; break;
            default:  break;
        }

        // Content - changes per sign
        GLuint contentTexture = Texture::LoadTexture(texPath);
        contentSprite = new Sprite(position, glm::vec3(scale.x, scale.y, 1.0f), contentTexture);
        this->sprite = contentSprite;
        this->setZOrder(3);
        halfWidth = scale.x / 2.0f;
        halfHeight = scale.y / 2.0f;

        TutorialSign::createPhysicsBody();

        // //Align sprite center with physicsbody
        // sprite->setPosition(glm::vec3(position.x,position.y,0.0f));
        // sprite->setScale(glm::vec3(halfWidth * 2, halfHeight * 2, 1.0f)); //full width and height
    }
    void TutorialSign::update(Game* game, float deltaTime) {
        if (!isVisible) return;

        displayTimer += deltaTime;

        if (displayTimer >= displayDuration) {
            isFading = true;
        }

        if (isFading) {
            alpha -= deltaTime * 1.5f; // tune fade speed
            if (alpha <= 0.0f) {
                alpha = 0.0f;
                isVisible = false;
                isFading = false;
                displayTimer = 0.0f;
            }
        }
    }

    void TutorialSign::createPhysicsBody() {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        // Make the sensor height exactly the same as the ladder.
        // If the floor is 0.1 thick and sits exactly at the ladder's top,
        // the player's feet will still be "touching" the top edge of the sensor.
        sensorHeight = (halfHeight * 2.0f) + 0.6f;
        b2Polygon sensorBox = b2MakeBox(halfWidth, sensorHeight / 2.0f);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true;
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_TUTORIAL_SIGN;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER;
        shapeDef.userData = this;

        shape = b2CreatePolygonShape(body, &shapeDef, &sensorBox);
        b2Shape_SetUserData(shape, this);
        b2Shape_EnableSensorEvents(shape, true);
    }


    void TutorialSign::onSensorEnter(Entity* other) {
        if (other->getType() == EntityType::Player && !hasBeenShown) {
            std::cout << "Player shown" << std::endl;
            isVisible = true;
            hasBeenShown = true;

        }
    }

    void TutorialSign::onSensorExit(Entity* other) {
        // if (other->getType() == EntityType::Player) {
        //     isVisible = false;
        // }
    }

    void TutorialSign::draw(Game* game) {
        if (!isVisible) return;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader->use();

        // 1. Backdrop FIRST (underneath)
        shader->setBool("useTexture", false);
        backgroundSprite->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.6f * alpha));
        glm::mat4 backdropMvp = game->calculateMvpMatrix(
            position,
            0.0f,
            this->sprite->getScale()
        );
        backgroundSprite->draw(*shader, backdropMvp);

        // 2. Text ON TOP
        shader->setBool("useTexture", true);
        shader->setInt("spriteTexture", 0);
        Entity::draw(game);

        glEnable(GL_DEPTH_TEST);
    }
    }

