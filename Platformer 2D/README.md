Warped by GroundZero

Warped is a platformer game developed by two students at Universitat Politècnica de Catalunya for the Game Development subject. As of this version, the game has basic collisions with the environment, the player can walk left or right, jump, and if it falls on a pit, it's sent to the start of the level. The game reads the map and most of the gameplay values from .xml files, and .tmx files generated via Tiled. 

--!!INNOVATION!!--

For innovation (doing something that the assignment didn't require), we developed a system that can read all the animations of the player from a .tmx file, so that animations can later be easily edited from Tiled. The number of animations the player has, and their names, still has to be specified in the code, but all pushbacks are read from the .tmx, and a designer can modify the .png spritesheet, and then set the animations from Tiled.

-- Controls

	- A and D: 	Move the character left or right.

	- J:		Jump. By inputting again in midair, the player can double-jump once before touching ground again.