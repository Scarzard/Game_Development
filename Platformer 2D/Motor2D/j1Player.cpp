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
	player1 = new Player();

	// Create all animations (HARDCODED!!!!! NEED TO CHANGE LOADING TO TMX READING)

	player1->idle.PushBack({ 0,0,38,50 });
	player1->idle.PushBack({ 38,0,38,50 });
	player1->idle.PushBack({ 76,0,38,50 });
	player1->idle.speed = 0.03f;
	player1->idle.loop = true;


	return true;
}

bool j1Player::Start()
{


	player1->position = { 50,50 };

	player1->playerTexture = App->tex->Load("textures/main_character.png");
	return true;
}

bool j1Player::PreUpdate()
{
	return true;
}

bool j1Player::Update(float dt)
{
	//player1->currentAnimation = &(player1->idle);
	App->render->Blit(player1->playerTexture, player1->position.x, player1->position.x, &(player1->idle.GetCurrentFrame()));
	//App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, player1->spriteSection);
	return true;
}

bool j1Player::CleanUp()
{
	return true;
}
