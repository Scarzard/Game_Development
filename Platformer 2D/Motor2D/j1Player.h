#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "SDL/include/SDL.h"

class Player
{
public:

	Player() {};
	virtual ~Player() {};

	int position_x = 0;
	int position_y = 0;
	
	bool	alive		=	false;
	bool	jumping		=	false;
	
	Collider*		playerCollider		= nullptr;
	SDL_Texture*	playerTexture		= nullptr;

	SDL_Rect*		spriteSection		= nullptr;

public:


};

class j1Player : public j1Module
{
public:

	j1Player();

	// Destructor
	virtual ~j1Player();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);


	// Called before quitting
	bool CleanUp();

private:

public:

	Player* player1 = nullptr;
	
};

#endif // __j1PLAYER_H__