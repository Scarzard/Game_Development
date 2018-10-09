#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "SDL/include/SDL.h"

class Player
{
	Player();
	virtual ~Player() {};

	p2Point<int> position = { 0,0 };
	
	bool	alive		=	false;
	bool	jumping		=	false;

	Collider* playerCollider = nullptr;

	void SetPosition(int x, int y)
	{
		position = { x, y };
	};

	void Move(int horizontalDisplacement = 0, int verticalDisplacement = 0)
	{
		position += {horizontalDisplacement, verticalDisplacement};
	}

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

	// Called before quitting
	bool CleanUp();

private:

public:

	Player* player1 = nullptr;
	
};

#endif // __j1PLAYER_H__