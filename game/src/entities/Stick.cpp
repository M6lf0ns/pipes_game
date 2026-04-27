#include "../rendering/Texture.h"
#include <iostream>
#include "Stick.h"
#include "../PhysicsCategories.h"
#include "../Game.h"
#include "box2d/box2d.h"
#include "box2d/types.h"

namespace gl3 {

    Stick::Stick(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld) : Entity(
        shader,
        position,
        scale,
        physicsWorld,
        EntityType::Stick)
    {
        this -> sprite = new Sprite (position, scale, Texture::LoadTexture("assets/textures/temp_pipe.png"));
        this->setZOrder(4);
        Stick::createPhysicsBody();
        // 1. Create the IDs
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        // 2. Set up the "instructions" (VAO)
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Reserve space for 100 points
        glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);

        // Tell the shader: "Position is at location 0"
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }

    void Stick::update(Game *game, float deltaTime) {
        // 1. If we aren't in "flying mode," don't touch anything
        if (!hasBeenThrown) return;

        b2Vec2 vel = b2Body_GetLinearVelocity(body);
        float angularVel = b2Body_GetAngularVelocity(body);

        // 2. Threshold Check: Is it slow enough to "land"?
        // If it's falling (vel.y) or spinning (angularVel) fast, SKIP THIS.
        if (std::abs(vel.y) < 0.2f && std::abs(angularVel) < 0.8f) {

            // 3. Pick the closest flat angle (0 or 180)
            float snapAngle = (std::abs(zRotation) > 90.0f) ? 180.0f : 0.0f;

            // 4. Force the physics body to that flat angle
            b2Body_SetTransform(body, b2Body_GetPosition(body), b2MakeRot(glm::radians(snapAngle)));

            // 5. Kill all energy so it doesn't bounce back up
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
            b2Body_SetAngularVelocity(body, 0.0f);

            // 6. CRITICAL: Turn off the flag so this block stops running!
            hasBeenThrown = false;
            hasLanded = true;
            dawg->setStickHasLanded(hasLanded);
            dawg->setTargetLocation(position.x);

            std::cout << "STICK LANDED AND SNAPPED" << std::endl;
        }
    }

    void Stick::draw(Game *game) {
        Entity::draw(game);

        if (isAiming) {
            this->drawTrajectory(game);
        }

    }

    void Stick::createPhysicsBody() {

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        bodyDef.angularDamping = 0.9f;
        bodyDef.enableSleep = true; // Make sure this is TRUE
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_STICK;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_FLOOR |
                                 gl3::PhysicsCategories::CATEGORY_WALL |
                                 gl3::PhysicsCategories::CATEGORY_BOX |
                                     gl3::PhysicsCategories::CATEGORY_DOOR;


        // Use b2MakeBox with the correct half-extents
        float halfWidth = scale.x / 2.0f;
        float halfHeight = scale.y / 2.0f;

        b2Polygon box = b2MakeBox(halfWidth, halfHeight);
        shape = b2CreatePolygonShape(body, &shapeDef, &box);
        b2Shape_SetUserData(shape, this);
        b2Shape_SetFriction(shape, 0.7f);
        b2Shape_SetRestitution(shape, 0.1f);
        b2Shape_SetDensity(shape, 5.0f, true);
        // Inside Stick::createPhysicsBody



        b2MassData massData;
        massData.mass = 1.0f;
        mass = massData.mass;
        massData.center = b2Vec2{0.05f, 0.0f};
        massData.rotationalInertia = 0.9f;
        b2Body_SetMassData(body, massData);
        b2Body_SetLinearDamping(body, 0.5f);

    }


    void Stick::setAiming(bool isAiming) {
        this->isAiming = isAiming;
    }



    void Stick::simulateThrow(Game *game, glm::vec3 bodyPos, float direction) {
        float force = 4.0f;
        float x = bodyPos.x;
        float y = bodyPos.y;
        impulse = force * direction;

        float initialVelocityX = impulse / mass;
        float initialVelocityY = 4.0f / mass; // Match the 4.0f in throwStick

        this->trajectoryPoints.clear();

        // ADJUST THIS: Increase gravity by ~20% to account for Linear Damping (0.5f) and make the arch more accurate to the real throw
        float effectiveGravity = game->getGravity().y * 1.25f;

        for (float t = 0; t < 1.5f && trajectoryPoints.size() < 30; t += 0.05f) {
            float x_t = x + (initialVelocityX * t);

            // Use the effectiveGravity here
            float y_t = y + (initialVelocityY * t) + (0.5f * effectiveGravity * (t * t));

            this->trajectoryPoints.push_back(glm::vec3(x_t, y_t, 0.0f));
        }
    }

    void Stick::throwStick(Game *game, glm::vec3 pos, float direction) {
        // 1. Move it (0.2f is fine now with your maskBits!)
        this->setPosition(pos + glm::vec3(direction * 0.2f, 0.0f, 0.0f));

        // 2. THE FIX: Kill all existing momentum, Only necessary thing to fix stick having different trajectory
        b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
        b2Body_SetAngularVelocity(body, 0.0f);

        // 3. Force it awake so the engine re-evaluates its position
        b2Body_SetAwake(body, true);
        // Apply movement first
        b2Body_ApplyLinearImpulseToCenter(body, {impulse, 4.0f}, true); //no impulse for y because dir can be neg --> stick is pushed down -> big no no
        b2Body_ApplyAngularImpulse(body, 2.5f * -direction, true);

        // THEN turn on the update logic
        hasBeenThrown = true;

    }


    void Stick::drawTrajectory(Game *game) {
        if (trajectoryPoints.empty()) return;

        // 1. Prepare Shader (Using the pointer)
        shader->use();
        shader->setBool("useTexture", false);
        shader->setVector("spriteColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        // 2. Set the Matrix
        auto cameraMatrix = game->calculateMvpMatrix(glm::vec3(0.0f), 0.0f, glm::vec3(1.0f)); //we already have the positions calculated in simulateThrow, so no need to set new ones
        shader->setMatrix("mvp", cameraMatrix);

        // 3. Bind and Upload
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, trajectoryPoints.size() * sizeof(glm::vec3), trajectoryPoints.data());

        // 4. SET STATES (Save current state or explicitly set)
        glDisable(GL_DEPTH_TEST);
        glPointSize(8.0f); // For your "dots"

        // 5. DRAW
        glDrawArrays(GL_POINTS, 0, (GLsizei)trajectoryPoints.size());

        // 6. --- THE CLEANUP (The "Everything Disappears" Fix) ---
        glEnable(GL_DEPTH_TEST);   // Re-enable so the next frame's sprites work
        glBindVertexArray(0);      // Unbind Stick's VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Switch the SHARED shader back to texture mode for the next object
        shader->setBool("useTexture", true);
    }

    void Stick::startContact(Entity* other) {

            std::cout << "DEBUG: Stick started contact!" << std::endl;


    }

    void Stick::setDawg(Dog* dog) {
        this->dawg = dog;



    }

    void Stick::endContact() {
        if (hasBeenThrown) hasLanded = false;
    }


    Stick::~Stick() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }






}
