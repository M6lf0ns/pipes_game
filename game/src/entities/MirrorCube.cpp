#include "MirrorCube.h"

#include "../Game.h"

#include "../EntityType.h"

#include "../rendering/Texture.h"

#include "box2d/box2d.h"

#include "../PhysicsCategories.h"

#include <iostream>

#include "../physics/PhysicsUtils.h"


namespace gl3 {
    MirrorCube::MirrorCube(Shader *shader, glm::vec3 position, glm::vec3 scale, float rotation, int linkid, int floorid, b2WorldId physicsWorld)

        : Entity(shader,

                 position,

                 scale,

                 physicsWorld,

                 EntityType::MirrorCube),

                 rotationAngle(rotation),
    linkID(linkid),
    floorID(floorid)

 {
        this->setZOrder(1); // MIDGROUND

        // 1. Calculate half extents
        halfWidth = scale.x / 2.0f;
        halfHeight = scale.y / 2.0f;

        pipeHalfWidth = halfWidth;
        pipeHalfHeight = 0.15;

        // 2. Allocate separate sprites
        this->backgroundSprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/mirror_background.png"));

        glm::vec3 pipeScale = glm::vec3(pipeHalfWidth * 2.0f, pipeHalfHeight * 2.0f, 1.0f);
        this->pipeSprite = new Sprite(position, pipeScale, Texture::LoadTexture("assets/textures/pipe.png"));

        // Create the dual physics bodies (This creates 'body' at 0 rotation initially)
        MirrorCube::createPhysicsBody();
        // --- SYNCHRONIZE INITIAL ROTATION ---
        this->rotatePipe(rotationAngle, false);


        std::cout << "SET ROTATION AT: " <<  rotationAngle << std::endl;

    }


    void MirrorCube::update(Game *game, float deltaTime) {
        Entity::update(game, deltaTime);
        if (this->game == nullptr) this->game = game; // grab it once
    }


    void MirrorCube::createPhysicsBody() {
        // ==========================================

        // 1. THE BACKDROP BODY (Collides with Player, ignored by Laser)

        // ==========================================

        b2BodyDef backdropDef = b2DefaultBodyDef();

        backdropDef.type = b2_staticBody;

        backdropDef.position = b2Vec2{position.x, position.y};

        backdropDef.userData = this;

        b2BodyId backdropBody = b2CreateBody(physicsWorld, &backdropDef);


        // Full sized box matching the background sprite dimensions

        b2Polygon backdropBox = b2MakeBox(halfWidth, halfHeight);

        b2ShapeDef backdropShapeDef = b2DefaultShapeDef();


        // Identity: What this shape is

        backdropShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_MIRROR_BACKDROP;

        // Interaction: It can only touch the player (and maybe boxes), NOT the laser ray!

        backdropShapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                                           gl3::PhysicsCategories::CATEGORY_BOX;


        b2CreatePolygonShape(backdropBody, &backdropShapeDef, &backdropBox);


        // ==========================================

        // 2. THE PIPE BODY (Collides with Laser, ignored by Player)

        // ==========================================

        b2BodyDef pipeDef = b2DefaultBodyDef();

        pipeDef.type = b2_staticBody;
        // Change to dynamic/kinematic if moving via velocities, or keep static and use b2Body_SetTransform to rotate

        pipeDef.position = b2Vec2{position.x, position.y};

        pipeDef.userData = this;

        body = b2CreateBody(physicsWorld, &pipeDef); // Assigning to your class 'body' variable for tracking


        // Tightly cropped box matching ONLY the pipe's physical width/height

        b2Polygon pipeBox = b2MakeBox(pipeHalfWidth / 2.0f, pipeHalfHeight);

        b2ShapeDef pipeShapeDef = b2DefaultShapeDef();


        // Identity: What this shape is

        pipeShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_MIRROR_PIPE;

        // Interaction: It ONLY registers hits from the laser ray sensor!

        pipeShapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_RAY_SENSOR  |

                                       gl3::PhysicsCategories::CATEGORY_PLAYER;

        pipeShapeDef.enableSensorEvents = true;

        pipeShapeDef.enableContactEvents = true;


        shape = b2CreatePolygonShape(body, &pipeShapeDef, &pipeBox);

        b2Shape_SetUserData(shape, this);
    }


    void MirrorCube::draw(Game *game) {
        // 1. Draw the static background cube (Rotation is always 0.0f)

        glm::vec3 bgScale = glm::vec3(halfWidth * 2.0f, halfHeight * 2.0f, 1.0f);

        glm::mat4 bgMvp = game->calculateMvpMatrix(position, 0.0f, bgScale);

        backgroundSprite->draw(*shader, bgMvp);


        // 2. Draw the rotating pipe on top (Passes the actual puzzle rotation angle)

        glm::vec3 pipeScale = glm::vec3(pipeHalfWidth * 2.0f, pipeHalfHeight * 2.0f, 1.0f);

        glm::mat4 pipeMvp = game->calculateMvpMatrix(position, rotationAngle, pipeScale);

        pipeSprite->draw(*shader, pipeMvp);
    }

    RayCastResult MirrorCube::castRay(b2Vec2 reflectionVec, b2Vec2 hitPoint) {
        // 1. Grab the exact physical center position of this MirrorCube block
        b2Vec2 dir = b2Normalize(reflectionVec);

        // 2. Project a reasonable distance for the raycast (e.g., 20.0f)
        b2Vec2 origin = { hitPoint.x + (dir.x * 0.05f), hitPoint.y + (dir.y * 0.05f) };
        b2Vec2 end = { origin.x + (dir.x * 10.0f), origin.y + (dir.y * 10.0f) };
        RayCastResult result;

        uint64_t category = gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;
        uint64_t mask = gl3::PhysicsCategories::CATEGORY_PLAYER |
                        gl3::PhysicsCategories::CATEGORY_WALL |
                        gl3::PhysicsCategories::CATEGORY_DOOR |
                        gl3::PhysicsCategories::CATEGORY_BOX |
                       // gl3::PhysicsCategories::CATEGORY_MIRROR |
                        //gl3::PhysicsCategories::CATEGORY_MIRROR_PIPE |
                            gl3::PhysicsCategories::CATEGORY_LASER_EYES;

        result = PhysicsUtils::CastRay(physicsWorld, origin, end, mask, category);
        std::cout << "LASER ORIGIN FROM PIPE AND ENDr " << origin.x << " " << origin.y << " " << end.x << " " << end.y << std::endl;

        // If the secondary beam hits open sky space, project its end coordinate manually
        if (!result.didHit) {
            result.hitPoint = end;
            result.didHit = true; // Force true so LaserGuy registers this secondary path segment exists!



        }

        // Log for verification
        std::cout << "DEBUG: Cast from " << origin.x << " to " << result.hitPoint.x << std::endl;
        return result;
    }



    void MirrorCube::rotatePipe(float angleInDegrees, bool playSound) {
        this->rotationAngle = angleInDegrees;
        float radians = glm::radians(angleInDegrees);
        b2Vec2 bodyPos = b2Body_GetPosition(body);
        b2Body_SetTransform(body, bodyPos, b2MakeRot(radians));
        pipeSprite->setRotation(radians);

        if (playSound && game != nullptr) {
            game->PlaySfx(game->getPipeRotatedSFX());
        }
    }
}

