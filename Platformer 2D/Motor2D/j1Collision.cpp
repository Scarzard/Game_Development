#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "p2Log.h"

j1Collision::j1Collision()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_SOLID_FLOOR] = false;
	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_PHASABLE_FLOOR] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_SOLID_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PHASABLE_FLOOR] = true;
	
	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_PHASABLE_FLOOR] = false;
	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_SOLID_FLOOR] = false;
	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_PLAYER] = true;


}

j1Collision::~j1Collision()
{}

bool j1Collision::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

bool j1Collision::Update(float dt)
{
	
	// Test all collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];
		c1->Update();

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];
			c2->Update();

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	

	return true;
}

//A PostUpdate is needed because the maps blits during the update. So the DebugDraw() appeared behind the map

bool j1Collision::PostUpdate()
{

	DebugDraw();

	return true;
}

void j1Collision::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		LOG("clicking f9");
		debug = !debug;
		LOG("debug swap");
	}
		

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
		case COLLIDER_DEATH: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
		case COLLIDER_LEVELEND: // orange
			App->render->DrawQuad(colliders[i]->rect, 255, 100, 100, alpha);
		case COLLIDER_SOLID_FLOOR: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
		case COLLIDER_PLAYERFUTURE: // dark green
			App->render->DrawQuad(colliders[i]->rect, 0, 185, 0, alpha);
		case COLLIDER_PHASABLE_FLOOR: //light blue
			App->render->DrawQuad(colliders[i]->rect, 125, 255, 255, alpha);
		}

	}
}

Collider * j1Collision::FindPlayer()
{
	for (uint i = 0; colliders[i] != nullptr; ++i)
	{
		if (colliders[i]->type == COLLIDER_PLAYER)
			return colliders[i];
	}
	return nullptr;
}

bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

bool j1Collision::EraseCollider(Collider* collider)
{
	if (collider != nullptr)
	{
		
		for (uint i = 0; i < MAX_COLLIDERS; ++i)
		{
			if (colliders[i] == collider)
			{
				collider->to_delete = true;
				break;
			}
		}
	}


	return false;
}


bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}

bool Collider::Update()
{
	return true;
}