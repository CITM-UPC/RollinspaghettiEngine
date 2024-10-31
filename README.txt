Engine made by the third year students of the Video Game Design and Development course. The engine is a first contact for what we will be working on in class and will be developed as the course progresses.
The link to the github is the following: 
https://github.com/CITM-UPC/RollinspaghettiEngine.git

The members of the group are the following:
- Yiwei Ye
- Pablo Longaron
- Andrea Doña

The Controls
Movement:
- W to go up
- A to go left
- S to go down
- D to go right
Right mouse click to rotate the camera
1 to click on the space you want to look at
Shift + WASD to increase speed x2
Arrow keys to rotate the camera 

UI: (from left to right)
- GameObject opens a menu where you can select one of the primitives you want to create (in the console you can see how the primitive is created but it is not rendered on screen yet).
- Hide Editor Windows to hide the Editor and Inspector window.
- Github Link Button to go directly to github.
- About Us to see the popup with information about the engine.
- Configuration Windows to show relevant information.
- Console opens a window with the Console Log (all logs will be printed in the future).
-Quit to close the program.

Editor:
- Creates a window with a hierarchy that detects GameObjects (do not click on the gameobjects yet as it will close the engine).

Inspector Window:
- Creates a window with the Transform, Mesh, Texture menu. Which have their parameters and functionalities hardcoded at the moment for each of them.

Additional comments:
- When you drag an fbx to the engine you can observe in console how all the parameters of the fbx are loaded correctly but the model is not rendered on screen.