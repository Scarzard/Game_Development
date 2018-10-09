#include "p2Log.h"
#include "j1Player.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"


j1Player::j1Player()
{
}


j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node &)
{


	return true;
}

bool j1Player::Start()
{
	player1 = new Player;

	player1->position_x = 50;
	player1->position_y = 50;

	player1->playerTexture = App->tex->Load("textures/ashe.png");
	return true;
}

bool j1Player::PreUpdate()
{
	return true;
}

bool j1Player::Update(float dt)
{
	App->render->Blit(player1->playerTexture, player1->position_x, player1->position_x, player1->spriteSection);
	return true;
}

bool j1Player::CleanUp()
{
	return true;
}
