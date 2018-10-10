#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"
#include "PugiXml/src/pugixml.hpp"

class Player
{
public:

	Player() {};
	virtual ~Player() {};

	p2Point<int> position = { 0,0 };
	
	bool	alive		=	false;
	bool	jumping		=	false;
	
	Collider*		playerCollider		= nullptr;
	SDL_Texture*	playerTexture		= nullptr;

	SDL_Rect*		spriteSection		= nullptr;

	Animation* currentAnimation = nullptr;

	Animation idle;
	Animation run;
	Animation jump;

	int			runningSpeed;
	iPoint		dashValue;
	iPoint		jumpStrength;

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

	void LoadAnimations();

private:

public:

	Player* player1;

private:

	pugi::xml_node		animFinder;
	pugi::xml_document	storedAnims;
	
};

#endif // __j1PLAYER_H__