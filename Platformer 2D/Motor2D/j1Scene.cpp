#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Collision.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	App->map->Load("GameLevelXML.tmx");
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();

	if(App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame();

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y -= 2;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y += 2;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x -= 2;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x += 2;

	//Start lvl 1
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->player->player1->changingLevel = true;

		if (currentLevel != 1)
			LoadLevel(1), currentLevel = 1;
	}

	//Start from current level
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->player->player1->alive = false;

	//Start lvl 2
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->player->player1->changingLevel = true;

		if (currentLevel != 2)
			LoadLevel(2), currentLevel = 2;
	}

	//App->render->Blit(img, 0, 0);
	App->map->Draw();

	//Window title like "Map:%dx%d Tiles:%dx%d Tilesets:%d"

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count());

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void j1Scene::LoadLevel(uint lvl)
{
	App->collision->CleanUp();
	App->map->CleanUp();

	if (lvl == 1)
		App->map->Load(levelToLoad.lvl1_name);

	else
		App->map->Load(levelToLoad.lvl2_name);

}
