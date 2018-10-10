#include "p2Log.h"
#include "j1Player.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Log.h"
#include <string>


j1Player::j1Player()
{
}


j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node &config)
{
	player1 = new Player();

	player1->jumpStrength.x = config.child("jumpStrength").attribute("x").as_int();
	player1->jumpStrength.y = config.child("jumpStrength").attribute("y").as_int();

	player1->dashValue.x = config.child("dashValue").attribute("x").as_int();
	player1->dashValue.y = config.child("dashValue").attribute("y").as_int();

	player1->runningSpeed = config.child("runningSpeed").attribute("value").as_int();

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


	return true;
}

bool j1Player::PreUpdate()
{
	return true;
}

bool j1Player::Update(float dt)
{
	player1->currentAnimation = &player1->run;
	App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, &player1->currentAnimation->GetCurrentFrame());
	return true;
}

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
