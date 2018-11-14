#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

struct Properties
{
	struct Property
	{
		p2SString name;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	p2List<Property*>	list;
};

struct MapLayer
{
	p2SString	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	//Method that receives a tile id and returns it's Rectfind the Rect associated with a specific tile id
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture = nullptr;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

struct ImageLayer
{

	p2SString		name;
	SDL_Texture*	texture = nullptr;
	int				offset_x;
	int				offset_y;
	int				width;
	int				height;
	fPoint			position;
	int				speed = 0;

};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	int					startingPointX;
	int					startingPointY;
	iPoint				cameraStartingPoint;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	// Add a list/array of layers to the map!
	p2List<MapLayer*>	mapData;
	p2List<Collider*>	colliderList;
	p2List<ImageLayer*>	image;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	//Create a method that translates x,y coordinates from map positions to world positions
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadImageLayer(pugi::xml_node& node, ImageLayer* set);
	bool LoadMapCollisions(pugi::xml_node& node);
	bool LoadMetadata(pugi::xml_node& node);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);


	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__