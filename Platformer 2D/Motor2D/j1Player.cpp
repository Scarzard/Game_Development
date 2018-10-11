#include "p2Log.h"
#include "j1Player.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Log.h"
#include <string>
#include "j1Input.h"
#include "j1Collision.h"


j1Player::j1Player()
{
	name.create("player");
}


j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node &config)
{
	player1 = new Player;

	
	//DOESNT SET THE VALUES FOR THE VARIABLES. NOW ITS HARDCODED. NEED A FIX
	player1->alive = config.child("alive").attribute("value").as_bool();

	player1->position.x = config.child("position").attribute("x").as_int();
	player1->position.y = config.child("position").attribute("y").as_int();

	player1->playerSpeed = config.child("playerSpeed").attribute("value").as_int();
	player1->jumpStrength = config.child("jumpStrength").attribute("value").as_int();

	player1->gravity = config.child("gravity").attribute("value").as_int();



	return true;
}

bool j1Player::Start()
{
	
	player1->playerTexture = App->tex->Load("textures/main_character.png");

	pugi::xml_parse_result result = storedAnims.load_file("textures/animations.tmx");
	if (result == NULL)
		LOG("Couldn't load player animations! Error:", result.description());

	else
	{
		LoadAnimations();
	}
	
	player1->playerHitbox = App->collision->AddCollider({ player1->position.x, player1->position.y, 7, 6 }, COLLIDER_PLAYER, this);
	player1->currentAnimation = &player1->idle;

	return true;
}

bool j1Player::PreUpdate()
{
	return true;
}


bool j1Player::Update(float dt)
{
	//player1->currentAnimation = &(player1->idle);
	//App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, player1->spriteSection);


	//Player controls
	if (player1->alive == true) {
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			player1->speed.x = player1->playerSpeed;
			if (player1->currentAnimation == &player1->idle)
			{
				player1->currentAnimation = &player1->run;
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
		{
			player1->speed.x = 0;
			if (player1->currentAnimation == &player1->run)
			{
				player1->currentAnimation = &player1->idle;
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			player1->speed.x = -player1->playerSpeed;
			if (player1->currentAnimation == &player1->idle)
			{
				player1->currentAnimation = &player1->run;
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
		{
			player1->speed.x = 0;
			if (player1->currentAnimation == &player1->run)
			{
				player1->currentAnimation = &player1->idle;
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		{
			player1->speed.y = -player1->jumpStrength;
			if (player1->currentAnimation == &player1->idle || player1->currentAnimation == &player1->run)
			{
				player1->currentAnimation = &player1->jump;
			}

		}
		else if (App->input->GetKey(SDL_SCANCODE_J) == KEY_UP)
		{
			player1->speed.y = player1->gravity;
			if (player1->currentAnimation == &player1->jump)
			{
				player1->currentAnimation = &player1->idle;
			}
		}
	}
	player1->playerHitbox->SetPos(player1->position.x, player1->position.y);


	App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, &player1->currentAnimation->GetCurrentFrame());
	player1->position.x += player1->speed.x;
	player1->position.y += player1->speed.y;

	if (player1->facingLeft)
		App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, &player1->currentAnimation->GetCurrentFrame());

	else if (!player1->facingLeft)
		App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, &player1->currentAnimation->GetCurrentFrame(), 1.0F, true);

	return true;

}


//bool j1Player::PostUpdate(float dt)
//{
//	player1->position.x += player1->playerSpeed;
//
//	return true;
//}



bool j1Player::CleanUp()
{
	return true;
}

void j1Player::LoadAnimations()
{
	std::string tmp;

	// Loop all different animations (each one is an object layer in the tmx)
	for (animFinder = storedAnims.child("map").child("objectgroup"); animFinder; animFinder = animFinder.next_sibling())
	{
		tmp = animFinder.child("properties").first_child().attribute("value").as_string();

		// Load idle
		if (tmp == "idle")
		{
			// Loop all frames and push them into animation
			for (pugi::xml_node frameIterator = animFinder.child("object"); frameIterator; frameIterator = frameIterator.next_sibling())
			{
				SDL_Rect frameToPush = { frameIterator.attribute("x").as_int(), frameIterator.attribute("y").as_int(), frameIterator.attribute("width").as_int(), frameIterator.attribute("height").as_int() };
				player1->idle.PushBack(frameToPush);
			}

			for (pugi::xml_node propertyIterator = animFinder.child("properties").first_child(); propertyIterator; propertyIterator = propertyIterator.next_sibling())
			{
				std::string checkName = propertyIterator.attribute("name").as_string();

				if (checkName == "loop")
					player1->idle.loop = propertyIterator.attribute("value").as_bool();
				else if (checkName == "speed")
					player1->idle.speed = propertyIterator.attribute("value").as_float();
			}

		}

		// Load run
		if (tmp == "run")
		{
			// Loop all frames and push them into animation
			for (pugi::xml_node frameIterator = animFinder.child("object"); frameIterator; frameIterator = frameIterator.next_sibling())
			{
				SDL_Rect frameToPush = { frameIterator.attribute("x").as_int(), frameIterator.attribute("y").as_int(), frameIterator.attribute("width").as_int(), frameIterator.attribute("height").as_int() };
				player1->run.PushBack(frameToPush);
			}

			for (pugi::xml_node propertyIterator = animFinder.child("properties").first_child(); propertyIterator; propertyIterator = propertyIterator.next_sibling())
			{
				std::string checkName = propertyIterator.attribute("name").as_string();

				if (checkName == "loop")
					player1->run.loop = propertyIterator.attribute("value").as_bool();
				else if (checkName == "speed")
					player1->run.speed = propertyIterator.attribute("value").as_float();
			}

		}

		// Load jump
		if (tmp == "jump")
		{
			// Loop all frames and push them into animation
			for (pugi::xml_node frameIterator = animFinder.child("object"); frameIterator; frameIterator = frameIterator.next_sibling())
			{
				SDL_Rect frameToPush = { frameIterator.attribute("x").as_int(), frameIterator.attribute("y").as_int(), frameIterator.attribute("width").as_int(), frameIterator.attribute("height").as_int() };
				player1->jump.PushBack(frameToPush);
			}

			for (pugi::xml_node propertyIterator = animFinder.child("properties").first_child(); propertyIterator; propertyIterator = propertyIterator.next_sibling())
			{
				std::string checkName = propertyIterator.attribute("name").as_string();

				if (checkName == "loop")
					player1->jump.loop = propertyIterator.attribute("value").as_bool();
				else if (checkName == "speed")
					player1->jump.speed = propertyIterator.attribute("value").as_float();
			}

		}

	}
}

void j1Player::CheckCollision() 
{

}

void j1Player::OnCollision(Collider* collider1, Collider* collider2)
{
	if (collider1->type == COLLIDER_PLAYER && collider2->type == COLLIDER_SOLID_FLOOR)
	{

	}

	if (collider1->type == COLLIDER_PLAYER && collider2->type == COLLIDER_PHASABLE_FLOOR)
	{

	}
}
