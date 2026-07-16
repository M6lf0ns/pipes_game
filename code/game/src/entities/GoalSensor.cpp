#include "GoalSensor.h"

#include <iostream>

#include "../Game.h"
#include "../PhysicsCategories.h"
#include "box2d/box2d.h"
#include "box2d/types.h"



namespace gl3 {
   GoalSensor::GoalSensor(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld)
        :Entity( shader,
            position,
            scale,
            physicsWorld,
            EntityType::GoalSensor) {

        GoalSensor::createPhysicsBody();

   }
    void GoalSensor::createPhysicsBody() {
       b2BodyDef bodyDef = b2DefaultBodyDef();
       bodyDef.type = b2_staticBody;
       bodyDef.position = {position.x, position.y};
       bodyDef.userData = this;
       body = b2CreateBody(physicsWorld, &bodyDef);

       b2Polygon switchBox = b2MakeBox(scale.x/2.0f, scale.y / 2.0f);

       b2ShapeDef shapeDef = b2DefaultShapeDef();
       shapeDef.isSensor = true;
       shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_GOAL_SENSOR;
       shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER;
       shapeDef.userData = this;

       shape = b2CreatePolygonShape(body, &shapeDef, &switchBox);
       b2Shape_SetUserData(shape, this);
       b2Shape_EnableSensorEvents(shape, true);
   }

    void GoalSensor::update(Game *game, float deltaTime) {
       if (goalReached) {
           game->setGoalReached(true);



       }
   }

    void GoalSensor::onSensorEnter(Entity *other) {
       if (other && other->getType() == EntityType::Player) {
           std::cout << "GoalSensor::onSensorEnter: Player" << std::endl;
           goalReached = true;
       }

    }

    void GoalSensor::onSensorExit(Entity *other) {
       if (other && other->getType() == EntityType::Player) {
           std::cout << "GoalSensor::onSensorExit: Player" << std::endl;
       }
   }


   }
