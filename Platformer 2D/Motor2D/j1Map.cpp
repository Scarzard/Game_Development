#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "Brofiler/Brofiler.h"
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
	BROFILER_CATEGORY("Map_Draw", Profiler::Color::Gold)

	if(map_loaded == false)
		return;

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

	if (map_loaded == false)
		return;

	p2List_item<MapLayer*>* item = data.mapData.start;

	for (; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.Get("Nodraw") != 0)
			continue;

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);

					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = MapToWorld(x, y);

					App->render->Blit(tileset->texture, pos.x, pos.y, &r);
				}
			}
		}
	}
}

int Properties::Get(const char* value, int default_value) const
{
	p2List_item<Property*>* item = list.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return default_value;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = -x / data.tile_width;
		ret.y = -y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2) - 1;
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	p2List_item<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while (item)
	{
		if (id < item->data->firstgid)
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	return set;
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
		App->tex->UnLoad(image_info->data->texture);
		RELEASE(image_info->data);
		image_info = image_info->next;
	}
	data.image.clear();

	// Remove all object layers

	// Remove all colliders

	//p2List_item<Collider*>* collider_info;
	//collider_info = data.colliderList.start;

	//while (collider_info != NULL)
	//{
	//	RELEASE(collider_info->data);
	//	collider_info = collider_info->next;
	//}
	//data.colliderList.clear();

	// Clean up the pugi tree
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

	pugi::xml_node objectToLoad;

	for (objectToLoad = map_file.child("map").child("objectgroup"); objectToLoad && ret; objectToLoad = objectToLoad.next_sibling("objectgroup"))
	{
		std::string tmp = objectToLoad.child("properties").child("property").attribute("value").as_string();
		if (tmp == "collision")
			LoadMapCollisions(objectToLoad);

		else if (tmp == "metadata")
			LoadMetadata(objectToLoad);
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
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
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
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
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
	pugi::xml_node property;

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

		
		for (property = node.child("properties").child("property"); property; property = property.next_sibling("property"))
		{
			p2SString attribute = property.attribute("name").as_string();
			if (attribute == "parallaxSpeed")
			{
				img->speed = property.attribute("value").as_int();
			}
		}
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
			App->collision->AddCollider(boundingbox, COLLIDER_SOLID_FLOOR, App->player);
			LOG("Created wall collider with position %d %d and dimensions of %d %d", boundingbox.x, boundingbox.y, boundingbox.w, boundingbox.h);
		}

		if (comparer == "death")
		{

			App->collision->AddCollider(boundingbox, COLLIDER_DEATH, App->player);
			LOG("Created death collider with position %d %d and dimensions of %d %d", boundingbox.x, boundingbox.y, boundingbox.w, boundingbox.h);
		}

		if (comparer == "levelEnd")
		{
			App->collision->AddCollider(boundingbox, COLLIDER_LEVELEND, App->player);
			LOG("Created level end collider with position %d %d and dimensions of %d %d", boundingbox.x, boundingbox.y, boundingbox.w, boundingbox.h);
		}

		if (comparer == "player")
		{
			App->collision->AddCollider(boundingbox, COLLIDER_PLAYER, App->player);
			LOG("Created player collider with position %d %d and dimensions of %d %d", boundingbox.x, boundingbox.y, boundingbox.w, boundingbox.h);
		}
	}

	return ret;
}

bool j1Map::LoadMetadata(pugi::xml_node & node)
{
	bool ret = true;

	pugi::xml_node dataObj = node.child("object");

	if (dataObj == NULL)
	{
		LOG("Error while parsing object node");
		ret = false;
	}

	for (dataObj = node.child("object"); dataObj; dataObj = dataObj.next_sibling("object"))
	{
		std::string comparer = dataObj.attribute("type").as_string();

		if (comparer == "startingPos")
		{
			data.startingPointX = dataObj.attribute("x").as_int();
			data.startingPointY = dataObj.attribute("y").as_int();
		}

		if (comparer == "cameraStartingPoint")
		{
			data.cameraStartingPoint.create(dataObj.attribute("x").as_int(), dataObj.attribute("y").as_int());
		}
	}
	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.add(p);
		}
	}

	return ret;
}


bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	p2List_item<MapLayer*>* item;
	item = data.mapData.start;

	for (item = data.mapData.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.Get("Navigation", 0) == 1)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}
