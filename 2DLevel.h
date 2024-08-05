#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

struct Cell {	
	enum class Type : int {
		WATER,
		EARTH,
		GRASS,
		POINT
	};
	constexpr static float cellSize = 16;
	Type type;
	sf::Vector2f pos;
	
	bool isWalkable;

	Cell();
	Cell(int _x, int _y, Cell::Type _type);
	void setType(int newType);
	int getState();
};

struct TileMap : public sf::Drawable, public sf::Transformable {
	bool load(const std::string& tileset, sf::Vector2u tileSize, const std::vector<std::vector<int>> tiles, unsigned int width, unsigned int height);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};

struct Grid {
	TileMap map;
	std::vector<std::vector<Cell>>gridVector;
	std::vector<std::vector<int>>level;
	int32_t height;
	int32_t width;
	bool randomizeNeighbors;
	int density;
	std::vector<sf::Sprite>sprites;
	sf::Texture dirt;
	sf::Texture grass;
	sf::Texture water;
	sf::Texture point;

	sf::Sprite cellWater;
	sf::Sprite cellDirt;
	sf::Sprite cellGrass;
	sf::Sprite cellPoint;
	sf::Vector2i start, end;

	int countNeighborsSame(int x, int y, int areaSize);
	int countNeighborsDiff(int x, int y, int areaSize);
	int getCellNeighbors(int x, int y, int areaSize);
	bool checkPlusShape(int x, int y, int l, std::vector<std::vector<Cell>>& gridVec);
	bool checkPlusShapeFull(int x, int y, int l, std::vector<std::vector<Cell>>& gridVec);
	void initGridVector(bool randomStates, int number);
	void generatePoints();
	
	

	void setWidth(int width);
	void setHeight(int height);
	bool resetGrid();
	void printLevelArray();

	std::vector<std::vector<Cell>> gridCopy(const std::vector<std::vector<Cell>>& gridVec);
	void fillGaps(int areaSize);
	void update(int& density);

	Grid();
	Grid(int gridWidth, int gridHeight, bool random);
};

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