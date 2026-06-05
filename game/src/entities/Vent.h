#pragma once
#include "Entity.h"


namespace gl3 {
    // Forward Declaration
    class Stick;

    class Vent: public Entity{
        public:
        explicit Vent(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld, int linkID, bool isExit);
        void onSensorEnter(Entity* other) override;
        void onSensorExit(Entity* other) override;
        void Open();
        [[nodiscard]] int getVentId() const { return ventId; }
        void setIsOpen(bool isOpen) {ventIsOpen = isOpen; }
        bool getIsOpen() {return ventIsOpen;}
        void setConnectedTwin(Vent* twin) { this->connectedTwin = twin; }
        Vent* getConnectedTwin() { return connectedTwin; }
        void initConnectedTwin(Vent* twin);
        void setStick(Stick* stick) {correspStick = stick; }

    protected:
        void createPhysicsBody() override;

    private:
        int ventId;
        Vent* connectedTwin = nullptr;
        bool ventIsOpen = false;
        bool isExit = false;
        Stick* correspStick = nullptr;

    };
}



