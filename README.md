Warped by GroundZero

 -------- GAME INFO --------

	Warped is a platformer game developed by two students at Universitat Politècnica de Catalunya for the Game Development subject. As of this version, the game has basic 	collisions with the environment, the player can walk left or right, jump, and if it falls on a pit, it's sent to the start of the level. The game reads the map and 	most of the gameplay values from .xml files, and .tmx files generated via Tiled.

	DISCLAIMER: The tileset used in this game is copyright free (credit to ansimuz, tileset URL: https://ansimuz.itch.io/warped-caves), but the sprites used for the main 	character are taken from the 2007 game "Mega Man ZX Advent", developed 	by Inti Creates and published by Capcom. We do not own the rights for these sprites, nor we 	claim to have made them. All rights belong to Capcom.

	--!!INNOVATION!!--

	For innovation (doing something that the assignment didn't require), we developed a system that can read all the animations of the player from a .tmx file, so that 		animations can later be easily edited from Tiled. The number of animations the player has, and their names, still has to be specified in the code, but all pushbacks 		(frame data) are read from the .tmx, and a designer can modify the .png spritesheet, and then set the animations from Tiled.

	-- Controls --

	- A and D: 	Move the character left or right.

	- J:		Jump. By inputting again in midair, the player can double-jump once before touching ground again.

	-- Debug Features --

	- F1:		Start again from the first level of the game.
	- F2:		Start again from the beginning of the current level.
	- F5:		Save current state of the game. (save file: "save_game.xml")
	- F6:		Load state from earlier save.
	- F9:		See colliders/hitboxes.
	- F10:		Godmode (can walk over pits, can double jump limitless).

 --------  GITHUB  --------

	As stated on the latter license, anyone can look at or modify the code of this project. It can be found here: https://github.com/Scarzard/Game_Development

 -------- LICENSING --------

	MIT License

	Copyright (c) 2018 Joel Cabaco Pérez and Víctor Chen Chen

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.