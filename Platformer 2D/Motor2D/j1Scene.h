#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

struct levelToLoad
{
	const char * lvl1_name = "GameLevelXML.tmx";

	const char * lvl2_name = "GameLevel2XML.tmx";

};

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void LoadLevel(uint lvl = 0);

private:
	int currentLevel = 1;
	levelToLoad levelToLoad;

};

#endif // __j1SCENE_H__