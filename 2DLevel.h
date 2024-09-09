#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

//struct Cell {	
//	enum class Type : int {
//		WATER,
//		EARTH,
//		GRASS,
//		POINT
//	};
//	constexpr static float cellSize = 16;
//	Type type;
//	sf::Vector2f pos;
//	
//	bool isWalkable;
//
//	Cell();
//	Cell(int _x, int _y, Cell::Type _type);
//	void setType(int newType);
//	int getState();
//};

struct TileMap : public sf::Drawable, public sf::Transformable {
	bool load(const std::string& tileset, sf::Vector2u _tileSize, const std::vector<std::vector<int>> tiles, unsigned int width, unsigned int height);
	void changeTex(const int x, const int y, const int tileNum, unsigned int width);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Vector2u tileSize;
	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};

struct Grid {
	TileMap map;
	//std::vector<std::vector<Cell>>gridVector;
	std::vector<std::vector<int>>level;
	std::map<std::vector<std::vector<int>>, int> prevLevels;
	int32_t height;
	int32_t width;
	bool randomizeNeighbors;
	int density;
	std::vector<std::tuple<sf::Sprite, int>>sprites;
	sf::Vector2i start, end;

	int countNeighborsSame(int x, int y, int areaSize);
	int countNeighborsDiff(int x, int y, int areaSize);
	int getCellNeighbors(int x, int y, int areaSize);
	bool checkPlusShape(int x, int y, int l, std::vector<std::vector<int>>& gridVec);
	bool checkPlusShapeFull(int x, int y, int l, std::vector<std::vector<int>>& gridVec);
	void initGridVector(bool randomStates, int number);
	void generatePoints();
	bool loadPrevLevel();
	bool undo();
	
	

	void setWidth(int width);
	void setHeight(int height);
	bool resetGrid();
	void printLevelArray();

	std::vector<std::vector<int>> gridCopy(const std::vector<std::vector<int>>& gridVec);
	void fillGaps(int areaSize);
	void update(int& density);

	Grid();
	Grid(int gridWidth, int gridHeight, bool random);
};

//TODO: Saving and loading tile map. Loading tileset from files. Undo on sprite placements.
struct LevelApp 
{
	LevelApp();
	~LevelApp();
	sf::RenderWindow* app;
	std::shared_ptr<Grid> grid;
	bool isRunning;
	int density;
	bool generated;
	int generatedX, generatedY;
	int tileSize;
	sf::Vector2i mousePos;
	std::vector<sf::Texture>textures;
	sf::Sprite sprite;
	bool spritePlaceOn;
	bool editOn;
	
	void drawAt(sf::Sprite& mySprite, int x, int y);
	void guiGrid();
	bool init();
	bool open();
	void run();
	void close();
	void exit();
};