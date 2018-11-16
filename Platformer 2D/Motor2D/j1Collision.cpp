
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "Brofiler/Brofiler.h"
#include "p2Log.h"

j1Collision::j1Collision()
{
	name.create("collision");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_LEVELEND][COLLIDER_LEVELEND] = false;
	matrix[COLLIDER_LEVELEND][COLLIDER_DEATH] = false;
	matrix[COLLIDER_LEVELEND][COLLIDER_SOLID_FLOOR] = false;
	matrix[COLLIDER_LEVELEND][COLLIDER_PHASABLE_FLOOR] = false;
	matrix[COLLIDER_LEVELEND][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_LEVELEND][COLLIDER_PLAYERFUTURE] = false;

	matrix[COLLIDER_DEATH][COLLIDER_LEVELEND] = false;
	matrix[COLLIDER_DEATH][COLLIDER_DEATH] = false;
	matrix[COLLIDER_DEATH][COLLIDER_SOLID_FLOOR] = false;
	matrix[COLLIDER_DEATH][COLLIDER_PHASABLE_FLOOR] = false;
	matrix[COLLIDER_DEATH][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_DEATH][COLLIDER_PLAYERFUTURE] = true;

	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_LEVELEND] = false;
	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_DEATH] = false;
	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_SOLID_FLOOR] = false;
	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_PHASABLE_FLOOR] = false;
	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_SOLID_FLOOR][COLLIDER_PLAYERFUTURE] = true;

	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_LEVELEND] = false;
	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_DEATH] = false;
	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_SOLID_FLOOR] = false;
	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_PHASABLE_FLOOR] = false;
	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PHASABLE_FLOOR][COLLIDER_PLAYERFUTURE] = true;

	matrix[COLLIDER_PLAYER][COLLIDER_LEVELEND] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_SOLID_FLOOR] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_PHASABLE_FLOOR] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYERFUTURE] = false;

	matrix[COLLIDER_PLAYERFUTURE][COLLIDER_LEVELEND] = true;
	matrix[COLLIDER_PLAYERFUTURE][COLLIDER_DEATH] = true;
	matrix[COLLIDER_PLAYERFUTURE][COLLIDER_SOLID_FLOOR] = true;
	matrix[COLLIDER_PLAYERFUTURE][COLLIDER_PHASABLE_FLOOR] = true;
	matrix[COLLIDER_PLAYERFUTURE][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYERFUTURE][COLLIDER_PLAYERFUTURE] = false;




}

j1Collision::~j1Collision()
{}

bool j1Collision::PreUpdate()
{
	BROFILER_CATEGORY("Collision_PreUpdate", Profiler::Color::Aquamarine)
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i]->callback;
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

bool j1Collision::Update(float dt)
{
	BROFILER_CATEGORY("Collision_Update", Profiler::Color::Orchid)

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
	BROFILER_CATEGORY("Collision_PostUpdate", Profiler::Color::Crimson)

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
			break;
		case COLLIDER_DEATH: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_LEVELEND: // orange
			App->render->DrawQuad(colliders[i]->rect, 255, 100, 100, alpha);
			break;
		case COLLIDER_SOLID_FLOOR: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_PLAYERFUTURE: // dark green
			App->render->DrawQuad(colliders[i]->rect, 0, 185, 0, alpha);
			break;
		case COLLIDER_PHASABLE_FLOOR: //light blue
			App->render->DrawQuad(colliders[i]->rect, 125, 255, 255, alpha);
			break;
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