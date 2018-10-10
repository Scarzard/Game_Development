#include "p2Log.h"
#include "j1Player.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Collision.h"


j1Player::j1Player()
{
}


j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node &config)
{

	// Create all animations (HARDCODED!!!!! NEED TO CHANGE LOADING TO TMX READING)

	player1.jumpStrength.x = config.child("jumpStrength").attribute("x").as_int();
	player1.jumpStrength.y = config.child("jumpStrength").attribute("y").as_int();

	player1.dashValue.x = config.child("dashValue").attribute("x").as_int();
	player1.dashValue.y = config.child("dashValue").attribute("y").as_int();

	player1.runningSpeed = config.child("runningSpeed").attribute("value").as_int();
	player1.gravity = config.child("gravity").attribute("value").as_int();

	player1.idle.PushBack({ 0,0,38,50 });
	player1.idle.PushBack({ 38,0,38,50 });
	player1.idle.PushBack({ 76,0,38,50 });
	player1.idle.speed = 0.03f;
	player1.idle.loop = true;




	return true;
}

bool j1Player::Start()
{

	player1.playerTexture = App->tex->Load("textures/main_character.png");
	player1.playerHitbox = App->collision->AddCollider({ player1.position.x, player1.position.y, 7, 6 }, COLLIDER_PLAYER, this);

	return true;
}

bool j1Player::PreUpdate()
{
	return true;
}

bool j1Player::Update(float dt)
{
	//player1->currentAnimation = &(player1->idle);
	App->render->Blit(player1.playerTexture, player1.position.x, player1.position.x, &(player1.idle.GetCurrentFrame()));
	//App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, player1->spriteSection);


	//Player controls
	if (player1.alive) {
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			player1.position.x = player1.runningSpeed;
			player1.currentAnimation = &player1.run;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			player1.position.x = 0;
			player1.currentAnimation = &player1.idle;
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			player1.position.x = - player1.runningSpeed;
			player1.currentAnimation = &player1.run;
		}
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			player1.position.x = 0;
			player1.currentAnimation = &player1.idle;
		}
		if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		{
			player1.position.y = player1.jumpStrength.y;
			player1.currentAnimation = &player1.jump;
		}
		else if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		{
			player1.position.y = 0;
			player1.currentAnimation = &player1.idle;
		}
	}




















	return true;
}

bool j1Player::CleanUp()
{
	return true;
}
