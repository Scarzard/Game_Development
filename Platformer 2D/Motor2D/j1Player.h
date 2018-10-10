#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"
#include "PugiXml/src/pugixml.hpp"

struct Player
{
	iPoint position{ 75,350 };
	iPoint speed = { 0,0 };
	int playerSpeed = 3;
	int jumpStrength = 10;
	int gravity = 8;


	bool	alive;
	bool	jumping = false;
	

	bool	facingLeft	=	false;
	
	Collider*		playerCollider		= nullptr;
	SDL_Texture*	playerTexture		= nullptr;

	Animation* currentAnimation = nullptr;


	Animation idle;
	Animation run;
	Animation jump;

public:

	Collider* playerHitbox;
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
	bool PostUpdate(float dt); //needead to update the player position that was changed in update(float dt);

	// Called before quitting
	bool CleanUp();

	void LoadAnimations();

private:

public:

	Player* player1 = nullptr;

private:

	pugi::xml_node		animFinder;
	pugi::xml_document	storedAnims;
	
};

#endif // __j1PLAYER_H__