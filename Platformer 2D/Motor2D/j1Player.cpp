#include "p2Log.h"
#include "j1Player.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "p2Log.h"
#include <string>
#include "j1Input.h"
#include "j1Collision.h"
#include "j1Scene.h"
#include <math.h>


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
	player1->godmodeTexture = App->tex->Load("textures/main_character_godmode.png");

	pugi::xml_parse_result result = storedAnims.load_file("textures/animations.tmx");
	if (result == NULL)
		LOG("Couldn't load player animations! Error:", result.description());

	else
	{
		LoadAnimations();
	}
	
	player1->playerCollider = App->collision->FindPlayer();
	player1->playerNextFrameCol = App->collision->AddCollider({ player1->playerCollider->rect.x, player1->playerCollider->rect.y, 
								player1->playerCollider->rect.w, player1->playerCollider->rect.h }, COLLIDER_PLAYERFUTURE, this);

	player1->currentAnimation = &player1->idle;
	player1->jumpsLeft = 2;

	return true;
}

bool j1Player::PreUpdate()
{
	if (player1->alive == true) {
		SetSpeed();

		player1->playerNextFrameCol->SetPos(player1->playerCollider->rect.x + player1->speed.x, player1->playerCollider->rect.y + player1->speed.y);
		if (player1->jumping == false)
			player1->jumpsLeft = 2;
	}
	return true;
}


bool j1Player::Update(float dt)
{
	//Player controls
	if (player1->alive == true) 
	{
		if (player1->godmode)
			player1->jumpsLeft = 2;

		CenterCameraOnPlayer();

		//Check Horizontal Movement
		//Right
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
			player1->facingLeft = false;

		//Left
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
			player1->facingLeft = true;
		//---------------------------

		//Prevent animations from glitching
		if (player1->speed.x > 0)
			player1->facingLeft = false;
		if (player1->speed.x < 0)
			player1->facingLeft = true;
		//---------------------------------

		//Check Jump ----------------------
		if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN && player1->jumpsLeft != 0)
		{
			player1->jumpsLeft--;
			player1->jumping = true;
		}
		if (player1->jumpsLeft == 2 && player1->speed.y > 0)
			player1->jumpsLeft--;
		//---------------------------------

		//Check godmode debug -------------
		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
			player1->godmode = !player1->godmode;

		// Set the correct animation for current movement
		SetAnimations();

		// Move player as of its current speed
		Move();

		// Update present collider
		player1->playerCollider->SetPos(player1->position.x + player1->colliderOffset.x, player1->position.y + player1->colliderOffset.y);

		// Blit player
		if (player1->facingLeft)
		{
			if (!player1->godmode)
				App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, &player1->currentAnimation->GetCurrentFrame(), SDL_FLIP_NONE);
			else if (player1->godmode)
				App->render->Blit(player1->godmodeTexture, player1->position.x, player1->position.y, &player1->currentAnimation->GetCurrentFrame(), SDL_FLIP_NONE);
		}

		else if (!player1->facingLeft)
		{
			if (!player1->godmode)
				App->render->Blit(player1->playerTexture, player1->position.x, player1->position.y, &player1->currentAnimation->GetCurrentFrame(), SDL_FLIP_HORIZONTAL);
			else if (player1->godmode)
				App->render->Blit(player1->godmodeTexture, player1->position.x, player1->position.y, &player1->currentAnimation->GetCurrentFrame(), SDL_FLIP_HORIZONTAL);
		}
			
	}

	else if (!player1->alive)
	{
		Respawn();
	}

	return true;
}


bool j1Player::PostUpdate()
{
	p2List_item<ImageLayer*>* img = nullptr;

	for (img = App->map->data.image.start; img; img = img->next)
	{
		if (img->data->speed > 0)
		{
			if (player1->parallaxToLeft = true)
			{
				img->data->position.x += player1->speed.x / img->data->speed;
			}
			else if (player1->parallaxToRight = true)
			{
				img->data->position.x -= player1->speed.x / img->data->speed;
			}

		}
	}
	return true;
}

bool j1Player::CleanUp()
{
	player1->currentAnimation = nullptr;

	delete player1;

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

		// Load fall
		if (tmp == "fall")
		{
			// Loop all frames and push them into animation
			for (pugi::xml_node frameIterator = animFinder.child("object"); frameIterator; frameIterator = frameIterator.next_sibling())
			{
				SDL_Rect frameToPush = { frameIterator.attribute("x").as_int(), frameIterator.attribute("y").as_int(), frameIterator.attribute("width").as_int(), frameIterator.attribute("height").as_int() };
				player1->fall.PushBack(frameToPush);
			}

			for (pugi::xml_node propertyIterator = animFinder.child("properties").first_child(); propertyIterator; propertyIterator = propertyIterator.next_sibling())
			{
				std::string checkName = propertyIterator.attribute("name").as_string();

				if (checkName == "loop")
					player1->fall.loop = propertyIterator.attribute("value").as_bool();
				else if (checkName == "speed")
					player1->fall.speed = propertyIterator.attribute("value").as_float();
			}

		}

	}
}

void j1Player::ApplyGravity()
{
	if (player1->speed.y < 9)
		player1->speed.y += player1->gravity;
	else if (player1->speed.y > 9)
		player1->speed.y = player1->gravity;
}

void j1Player::Respawn()
{
	player1->speed.SetToZero();
	player1->position.x = App->map->data.startingPointX;
	player1->position.y = App->map->data.startingPointY;

	player1->playerCollider = nullptr;
	player1->playerNextFrameCol = nullptr;

	player1->playerCollider = App->collision->FindPlayer();
	player1->playerNextFrameCol = App->collision->AddCollider({ player1->playerCollider->rect.x, player1->playerCollider->rect.y,
								player1->playerCollider->rect.w, player1->playerCollider->rect.h }, COLLIDER_PLAYERFUTURE, this);

	player1->playerCollider->SetPos(player1->position.x + player1->colliderOffset.x, player1->position.y + player1->colliderOffset.y);
	player1->playerNextFrameCol->SetPos(player1->playerCollider->rect.x + player1->speed.x, player1->playerCollider->rect.y + player1->speed.y);
	App->render->cameraRestart = true;
	ResetParallax();


	player1->alive = true;
}

void j1Player::SetAnimations()
{
	// Reset to idle if no input is received
	player1->currentAnimation = &player1->idle;

	// Run if moving on the x axis
	if (player1->speed.x != 0)
		player1->currentAnimation = &player1->run;

	// Idle if A and D are pressed simultaneously
	if (player1->speed.x == 0)
		player1->currentAnimation = &player1->idle;

	// Set jumping animations
	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		player1->jump.Reset();

	if (player1->jumping)
	{
		if (player1->speed.y > 0)
			player1->currentAnimation = &player1->fall;
		else if (player1->speed.y < 0)
			player1->currentAnimation = &player1->jump;
	}
}

void j1Player::SetSpeed()
{
	// Apply gravity in each frame
	ApplyGravity();

	// Check for horizontal movement
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
		player1->speed.x = player1->playerSpeed;

	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
		player1->speed.x = -player1->playerSpeed;

	else
		player1->speed.x = 0;

	// Check for jumps
	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN && player1->jumpsLeft > 0)
		player1->speed.y = -player1->jumpStrength;
}

void j1Player::Move()
{
	player1->position.x += player1->speed.x;
	player1->position.y += player1->speed.y;
}

void j1Player::ResetParallax()
{
	p2List_item<ImageLayer*>* img = nullptr;

	if (!player1->alive)
	{
		for (img = App->map->data.image.start; img; img = img->next)
		{
			img->data->position.x = 0;
		}
	}
}

void j1Player::OnCollision(Collider* collider1, Collider* collider2)
{

	// COLLISIONS ------------------------------------------------------------------------------------ //
	if (collider1->type == COLLIDER_PLAYERFUTURE && collider2->type == COLLIDER_SOLID_FLOOR)
	{
		SDL_Rect intersectCol;
		if (SDL_IntersectRect(&collider1->rect, &collider2->rect, &intersectCol));
		//future player collider and a certain wall collider have collided
		{
			if (player1->speed.y > 0) // If player is falling
			{
				if (collider1->rect.y < collider2->rect.y) // Checking if player is colliding from above
				{
					if (player1->speed.x == 0)
						player1->speed.y -= intersectCol.h, player1->jumping = false;

					else if (player1->speed.x < 0)
						if (intersectCol.h >= intersectCol.w)
						{
							if (collider1->rect.x <= collider2->rect.x + collider2->rect.w)
								player1->speed.y -= intersectCol.h, player1->jumping = false;
							else
								player1->speed.x += intersectCol.w;
						}
						else
							player1->speed.y -= intersectCol.h, player1->jumping = false;

					else if (player1->speed.x > 0)
						if (intersectCol.h >= intersectCol.w)
						{
							if (collider1->rect.x + collider1->rect.w <= collider2->rect.x)
								player1->speed.y -= intersectCol.h, player1->jumping = false;
							else
								player1->speed.x -= intersectCol.w;
						}
						else
							player1->speed.y -= intersectCol.h, player1->jumping = false;

				}
				else
				{
					if (player1->speed.x < 0)
						player1->speed.x += intersectCol.w;
					else if (player1->speed.x > 0)
						player1->speed.x -= intersectCol.w;
				}
			}

			else if (player1->speed.y == 0) // If player is not moving vertically
			{
				if (player1->speed.x > 0)
					player1->speed.x -= intersectCol.w;
			
				if (player1->speed.x < 0)
					player1->speed.x += intersectCol.w;
			}

			else if (player1->speed.y < 0) // If player is moving up
			{
				if (collider1->rect.y + collider1->rect.h > collider2->rect.y + collider2->rect.h) // Checking if player is colliding from below
				{
					if (player1->speed.x == 0)
						player1->speed.y += intersectCol.h;

					else if (player1->speed.x < 0)
					{
						if (intersectCol.h >= intersectCol.w)
						{
							if (collider1->rect.x <= collider2->rect.x + collider2->rect.w)
								player1->speed.y += intersectCol.h;
							else
								player1->speed.x += intersectCol.w;
						}
						else
							player1->speed.y += intersectCol.h;
					}
					else if (player1->speed.x > 0)
					{
						if (intersectCol.h >= intersectCol.w)
						{
							if (collider1->rect.x + collider1->rect.w >= collider2->rect.x)
								player1->speed.y += intersectCol.h;
							else
								player1->speed.x -= intersectCol.w;
						}
						else
							player1->speed.y += intersectCol.h;
					}

				}
				else
				{
					if (player1->speed.x < 0)
						player1->speed.x += intersectCol.w;
					else if (player1->speed.x > 0)
						player1->speed.x -= intersectCol.w;
				}
			}
		}
	}

	// COLLISIONS ------------------------------------------------------------------------------------ //

	else if (collider1->type == COLLIDER_PLAYERFUTURE && collider2->type == COLLIDER_DEATH)
	{
		if (!player1->godmode)
		player1->alive = false;

		else if (player1->godmode)
		{
			SDL_Rect intersectCol;
			if (SDL_IntersectRect(&collider1->rect, &collider2->rect, &intersectCol));
			//future player collider and a certain wall collider have collided
			{
				if (player1->speed.y > 0) // If player is falling
				{
					if (collider1->rect.y < collider2->rect.y) // Checking if player is colliding from above
					{
						if (player1->speed.x == 0)
							player1->speed.y -= intersectCol.h, player1->jumping = false;

						else if (player1->speed.x < 0)
							if (intersectCol.h >= intersectCol.w)
							{
								if (collider1->rect.x <= collider2->rect.x + collider2->rect.w)
									player1->speed.y -= intersectCol.h, player1->jumping = false;
								else
									player1->speed.x += intersectCol.w;
							}
							else
								player1->speed.y -= intersectCol.h, player1->jumping = false;

						else if (player1->speed.x > 0)
							if (intersectCol.h >= intersectCol.w)
							{
								if (collider1->rect.x + collider1->rect.w <= collider2->rect.x)
									player1->speed.y -= intersectCol.h, player1->jumping = false;
								else
									player1->speed.x -= intersectCol.w;
							}
							else
								player1->speed.y -= intersectCol.h, player1->jumping = false;

					}
					else
					{
						if (player1->speed.x < 0)
							player1->speed.x += intersectCol.w;
						else if (player1->speed.x > 0)
							player1->speed.x -= intersectCol.w;
					}
				}

				else if (player1->speed.y == 0) // If player is not moving vertically
				{
					if (player1->speed.x > 0)
						player1->speed.x -= intersectCol.w;

					if (player1->speed.x < 0)
						player1->speed.x += intersectCol.w;
				}

				else if (player1->speed.y < 0) // If player is moving up
				{
					if (collider1->rect.y + collider1->rect.h > collider2->rect.y + collider2->rect.h) // Checking if player is colliding from below
					{
						if (player1->speed.x == 0)
							player1->speed.y += intersectCol.h;

						else if (player1->speed.x < 0)
						{
							if (intersectCol.h >= intersectCol.w)
							{
								if (collider1->rect.x <= collider2->rect.x + collider2->rect.w)
									player1->speed.y += intersectCol.h;
								else
									player1->speed.x += intersectCol.w;
							}
							else
								player1->speed.y += intersectCol.h;
						}
						else if (player1->speed.x > 0)
						{
							if (intersectCol.h >= intersectCol.w)
							{
								if (collider1->rect.x + collider1->rect.w >= collider2->rect.x)
									player1->speed.y += intersectCol.h;
								else
									player1->speed.x -= intersectCol.w;
							}
							else
								player1->speed.y += intersectCol.h;
						}

					}
					else
					{
						if (player1->speed.x < 0)
							player1->speed.x += intersectCol.w;
						else if (player1->speed.x > 0)
							player1->speed.x -= intersectCol.w;
					}
				}
			}
		}
	}

	else if (collider1->type == COLLIDER_PLAYERFUTURE && collider2->type == COLLIDER_LEVELEND)
	{
		App->player->player1->alive = false;

		if (App->scene->currentLevel == 1)
			App->scene->LoadLevel(2), App->scene->currentLevel = 2;

		else if (App->scene->currentLevel == 2)
			App->scene->LoadLevel(1), App->scene->currentLevel = 1;
	}
}

bool j1Player::CenterCameraOnPlayer()
{
		uint w, h;
		App->win->GetWindowSize(w, h); 

		if (player1->position.x > App->render->camera.x + w) 
		{ 
			App->render->camera.x -= player1->speed.x * 2;
			player1->parallaxToRight = true;
		
		}
		else if (player1->position.x < App->render->camera.x + w) 
		{ 
			App->render->camera.x += player1->speed.x * 2; 

			player1->parallaxToLeft = true;
		}

		if (player1->position.y > App->render->camera.y + h) { App->render->camera.y -= player1->speed.y * 2; }
		else if (player1->position.y < App->render->camera.y + h) { App->render->camera.y += player1->speed.y * 2; }

	return true;

			
}


bool j1Player::Load(pugi::xml_node &node)
{
	player1->position.x = node.child("position").attribute("x").as_int();
	player1->position.y = node.child("position").attribute("y").as_int();

	return true;
}

bool j1Player::Save(pugi::xml_node &node) const
{
	pugi::xml_node position = node.append_child("position");

	position.append_attribute("x") = player1->position.x;
	position.append_attribute("y") = player1->position.y;

	return true;
}