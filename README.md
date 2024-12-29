# ⚠️ BEFORE OPENING ENGINE ⚠️
To try the new features with the new scene, you have to unzip the Scene.zip in Engine/Engine/Assets/Scenes and put the Scene.scene in Scenes folder.

# Rockstallica Engine
![655497EC-52E8-4703-9758-D0CD6AA57566_4_5005_c](https://github.com/user-attachments/assets/dd874f3d-9a8e-4075-8042-3d03edb3c761)

## Description
**Rockstallica Engine** is a 3D game engine developed in **C++** with **OpenGL**, currently capable of rendering geometry with a range of controls and information displays.

This Engine is forked from another CITM 3D Engine, you can check the original repesitory here:

Original repository: [SpectraEngine](https://github.com/CITM-UPC/SpectraEngine)

The project is under active development, and the code is available on [GitHub](https://github.com/ClaseAltaGames/RockstallicaEngine).

## Members
- [Edgar Mesa Domínguez](github.com/edgarmd1)
- [Pau Mena Torres](github.com/PauMenaTorres)

## Original Members
- [Guillem Alqueza Mayench](https://github.com/guillemalqueza)
- [Miguel Iglesias Abarca](https://github.com/MiguelIglesiasAbarca)
- [Lluc Cid](https://github.com/Lluccib)

## Viewer Controls
- **Right-click**: Move the current view.
- **Right-click + WASD**: Fly around the scene.
- **Alt + Right-click or Mouse Wheel**: Zoom in or out.
- **Alt + Left-click**: Orbit the camera around a pivot point.
- **Alt + Mouse Wheel**: Pan the camera.
- **F**: Frame the selected object.

## Original Features
- **Model Import**: Import models in FBX format.
- **Texture Import**: Supports PNG and DDS texture formats.
- **Drag and Drop**: Allows importing files by dragging them directly.
- **Transform Controls**: Apply transformations like position, rotation, and scale to models.
- **Logging System**: Provides detailed logging.
- **Custom File Library**: Manage files with a custom file format.
- **Shading Modes**: Swap between Shaded, Wireframe & Shaded Wireframe options.
- **Engine Monitor**: Visualize monitoring information.

## New Features
- **Scene Serialization**: Scenes can be serialized in .scene format.
- **Frustum Culling Camera**: The camera implements the frustum that optimizes the engine.
- **Bounding Boxes**: Each gameobject has his own AABB.
- **Clickable Objects**: You can click and select every object on the scene with a ray cast.
- **Hierarchy Improvements**: You can delete gameobjects and add a camera component.
- **Play / Pause**: You can play and move in the scene with a player, and then pause the iteration.
- **New Scene**: A new scene "Street Enviroment" is loaded with .scene format.

## Panels
- **Hierarchy**: Displays all game objects currently present in the scene. Includes a search feature to locate specific game objects and provides options to create empty game objects and basic primitive shapes.
- **Inspector**: Allows viewing and editing the properties of a currently selected game object, such as transform, mesh renderer, and material options.
- **Scene**: Allows visualizing and interacting with the world you create in the editor.
- **Project**: Access various project folders, manage files, and import assets. Displays all assets within the project, including models, textures, and other engine resources.
- **Console**: View errors, warnings, and information related to the engine. Allows filtering by type and clearing the console.
- **Preferences**: Offers settings for the user, including screen, grid, and rendering options.
- **Performance**: Displays information about engine performance and its impact on system resources.
- **About**: Displays basic information about the engine.

## Libraries and Dependencies
- [SDL2](https://github.com/libsdl-org/SDL): Cross-platform development library.
- [OpenGL](https://www.opengl.org/): Graphics API for rendering graphics.
- [ImGui](https://github.com/ocornut/imgui): Immediate mode GUI for C++.
- [GLM](https://github.com/g-truc/glm): C++ mathematics library for graphics software based on OpenGL.
- [Assimp](https://github.com/assimp/assimp): Library to load various 3D file formats into a shared, in-memory immediate format.
- [DeviL](https://github.com/DentonW/DevIL): Image library supporting loading, saving, and manipulating various image formats.
