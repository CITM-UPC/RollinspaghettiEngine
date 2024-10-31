# Generl description

Engine made by the third year students of the Video Game Design and Development course. The engine is the first contact for what we will be working on in class and will be developed as the course progresses.
The link to the GitHub repo is the following: 
https://github.com/CITM-UPC/RollinspaghettiEngine.git

## Members
- Yiwei Ye --- https://github.com/Yeeway69
- Pablo Longaron --- https://github.com/PabloL2003
- Andrea Doña --- https://github.com/poderoculto5

# Trello link
https://trello.com/invite/b/671f5539e9e45eef32369bab/ATTIb1663741f6c55a31d3413ee318b1b78f85079D03/rollin-spaguetti


## The Engine
1. The engine starts with the FBX properly loaded and the texture loaded at the start, a focus on the GameObject method is working whenever the Model is imported. <br/><br/>
2. The Engine is structured with SOLID principles in mind, the House is a game object with MeshComponent, RendererComponent, and MaterialComponent, each is a different class. <br/><br/>
3. A default directional light is set, you can see one side of the house is significantly brighter due to the lighting. <br/><br/>
4. A primitive Generator is implemented, and it reflects on the UI, however, there is an unknown issue with the rendering pipeline regarding the primitive that prevents it from being visible, how you can see the feature is working with the related information in the console output when clicking generate "GameObjects" from the UI. <br/><br/>
5. An extensive debug log system for the console is in place, you can see the components or features of the Engine working even tho it may not reflect in the editor app. it prints information like UV coordinate, Texture resolution and channel, texture id, Mesh coordinates, etc. For example, When you drag an fbx to the engine you can observe in console how all the parameters of the fbx are loaded correctly but the model is not rendered on screen. <br/><br/>



## Extra feature
1. There is a material component that is working properly, with diffuse, AO, Specular, and Shineness channels.
2. A lighting system is working.

## The Controls
Movement:
- W to go up
- A to go left
- S to go down
- D to go right
Right mouse click to rotate the camera
1 to click on the space you want to look at
Shift + WASD to increase speed x2
Arrow keys to rotate the camera 

## UI: (from left to right)
- GameObject opens a menu where you can select one of the primitives you want to create (in the console you can see how the primitive is created but it is not rendered on screen yet).
- Hide Editor Windows to hide the Editor and Inspector window.
- Github Link Button to go directly to github.
- About Us to see the popup with information about the engine.
- Configuration Windows to show relevant information.
- Console opens a window with the Console Log (all logs will be printed in the future).
-Quit to close the program.

## Editor:
- Creates a window with a hierarchy that detects GameObjects (do not click on the gameobjects yet as it will close the engine).

Inspector Window:
- Creates a window with the Transform, Mesh, Texture menu. Which have their parameters and functionalities hardcoded at the moment for each of them.
