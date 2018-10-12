#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collision.h"
#include <math.h>
#include <string>
j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	// TODO 5: Prepare the loop to draw all tilesets + Blit

	p2List_item<ImageLayer*>* image_list;
	for (image_list = data.image.start; image_list; image_list = image_list->next)
	{
		SDL_Texture* tex = image_list->data->texture;
		SDL_Rect rect = { 0,0,image_list->data->width, image_list->data->height };
		if (image_list->data->position.x < -image_list->data->width)
		{
			image_list->data->position.x = image_list->data->width;
		}
		App->render->Blit(tex, image_list->data->position.x, image_list->data->position.y, &rect);
	}

	MapLayer* layer = data.mapData.start->data;
	TileSet* tileset = data.tilesets.start->data;

	for (uint y = 0; y <= layer->mapHeight; y++)
	{
		
		for (uint x = 0; x <= layer->mapWidth; x++)
		{

			uint tile_id = layer->Get(x, y);
			uint gid = layer->data[tile_id];

			
			if (gid != 0) {
				SDL_Rect rect = tileset->GetTileRect(gid);
				
				App->render->Blit(tileset->texture,
					x * tileset->tile_width,
					y * tileset->tile_height,
					&rect);
			}
		}
	}

}


iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	ret.x = x * data.tile_width;
	ret.y = y * data.tile_height;

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers

	p2List_item<MapLayer*>* layer_info;
	layer_info = data.mapData.start;


	while (layer_info != NULL)
	{
		RELEASE(layer_info->data);
		layer_info = layer_info->next;
	}
	data.mapData.clear();

	// Remove all images

	p2List_item<ImageLayer*>* image_info;
	image_info = data.image.start;


	while (image_info != NULL)
	{
		RELEASE(image_info->data);
		image_info = image_info->next;
	}
	data.image.clear();

	// Remove all colliders

	p2List_item<Collider*>* collider_info;
	collider_info = data.colliderList.start;


	while (collider_info != NULL)
	{
		RELEASE(collider_info->data);
		collider_info = collider_info->next;
	}
	data.colliderList.clear();


	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	//Iterate all layers and load each of them
	// Load layer info ----------------------------------------------

	ret = true;

	pugi::xml_node layer_node;

	for (layer_node = map_file.child("map").child("layer"); layer_node && ret; layer_node = layer_node.next_sibling("layer"))
	{
		MapLayer* layer = new MapLayer;

		if(ret == true)
		{
			ret = LoadLayer(layer_node, layer);
		}
		
		data.mapData.add(layer);

	}

	pugi::xml_node image_node;
	for (image_node = map_file.child("map").child("imagelayer"); image_node && ret; image_node = image_node.next_sibling("imagelayer"))
	{
		ImageLayer* image = new ImageLayer();

		if (ret == true)
		{
			ret = LoadImageLayer(image_node, image);
		}
		data.image.add(image);
	}

	pugi::xml_node col;

	for (col = map_file.child("map").child("objectgroup"); col && ret; col = col.next_sibling("objectgroup"))
	{
		std::string tmp = col.child("properties").child("property").attribute("value").as_string();
		if (tmp == "Collision")
			LoadMapCollisions(col);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}
		
		p2List_item<MapLayer*>* item_layer = data.mapData.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->map_name.GetString());
			LOG("tile width: %d tile height: %d", l->mapWidth, l->mapHeight);
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}


bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{

	//needs to log every single member
	bool ret = true;

	layer->map_name = node.attribute("name").as_string();
	layer->mapHeight = node.attribute("height").as_uint();
	layer->mapWidth = node.attribute("width").as_uint();

	layer->data = new uint[layer->mapWidth*layer->mapHeight];
	memset(layer->data, 0, sizeof(uint)*(layer->mapHeight)*(layer->mapWidth));

	uint i = 0;

	for (pugi::xml_node node = map_file.child("map").child("layer").child("data").child("tile"); node; node = node.next_sibling("tile"))
	{
		layer->data[i++] = node.attribute("gid").as_uint(0);
	}

	return ret;
}

bool j1Map::LoadImageLayer(pugi::xml_node& node, ImageLayer* img)
{
	bool ret = true;

	img->name = node.attribute("name").as_string();
	img->offset_x = node.attribute("offsetx").as_int();
	img->offset_y = node.attribute("offsety").as_int();

	img->position.x = img->offset_x;
	img->position.y = img->offset_y;

	pugi::xml_node image = node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'imagelayer/image' tag.");
		ret = false;
		RELEASE(img);
	}
	else
	{
		img->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		img->width = image.attribute("width").as_int();
		img->height = image.attribute("height").as_int();
	}

	return ret;
}

bool j1Map::LoadMapCollisions(pugi::xml_node &node)
{
	bool ret = true;

	pugi::xml_node colliderObj = node.child("object");

	if (colliderObj == NULL)
	{
		LOG("Error while parsing object node");
		ret = false;
	}


	for (colliderObj = node.child("object"); colliderObj; colliderObj = colliderObj.next_sibling("object"))
	{

		SDL_Rect boundingbox = { colliderObj.attribute("x").as_int(), colliderObj.attribute("y").as_int(),
						  colliderObj.attribute("width").as_int(), colliderObj.attribute("height").as_int()

								};

		std::string comparer = colliderObj.child("properties").child("property").attribute("value").as_string();

		// Load walls
		if (comparer == "wall")
		{
			data.colliderList.add(App->collision->AddCollider(boundingbox, COLLIDER_SOLID_FLOOR));
			LOG("Created wall collider with position %d %d and dimensions of %d %d", boundingbox.x, boundingbox.y, boundingbox.w, boundingbox.h);
		}

		if (comparer == "death")
		{

			data.colliderList.add(App->collision->AddCollider(boundingbox, COLLIDER_DEATH));
			LOG("Created death collider with position %d %d and dimensions of %d %d", boundingbox.x, boundingbox.y, boundingbox.w, boundingbox.h);
		}

		if (comparer == "levelEnd")
		{
			data.colliderList.add(App->collision->AddCollider(boundingbox, COLLIDER_LEVELEND));
			LOG("Created level end collider with position %d %d and dimensions of %d %d", boundingbox.x, boundingbox.y, boundingbox.w, boundingbox.h);
		}

		if (comparer == "player")
		{
			data.colliderList.add(App->collision->AddCollider(boundingbox, COLLIDER_PLAYER));
			LOG("Created player collider with position %d %d and dimensions of %d %d", boundingbox.x, boundingbox.y, boundingbox.w, boundingbox.h);
		}
	}

	return ret;
}
