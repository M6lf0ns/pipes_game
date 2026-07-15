# How to make a game

This engine provides a lightweight framework for 2D development. It features a data-driven level loading system that allows for level configuration via text files. Additionally, the engine includes core utilities for asset management, sprite rendering, shader handling, and basic raycast collision detection.

Define Entities: Create your custom objects (e.g., Robot, Enemy) by inheriting from the gl3::Entity base class. Override update() for logic and createPhysicsBody() for collisions as well as any other virtual functions for custom behavior.

Create Levels using the Level loading system (currently based on text files).

Initialize: Create your game window and instantiate the main Game class.

The Game Loop: Call your engine's update/draw functions within your main loop. This ensures physics, entity logic, and rendering sync correctly.

Use the Scene´s Load() function to load spriteshaders.
Load your levels using the LoadLevelData() function.

Update: Call game.update() to process GameState behavior and player input.
        Call scene.update() to process Entity behaviors.

Render: Call game.draw() to push menus to the screen depending on the GameState.
        Call scene.draw() to push entities to the screen that use 2D Sprites.