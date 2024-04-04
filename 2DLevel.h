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
	int32_t x, y;
	Type type;
	sf::Sprite cellWater;
	sf::Sprite cellDirt;
	sf::Sprite cellGrass;
	sf::Sprite point;
	bool isWalkable;

	Cell();
	Cell(int _x, int _y, Cell::Type _type);
	void setPosition(int _x, int _y);
	void setType(int newType);
	int getState();
};

struct Grid {
	std::vector<std::vector<Cell>>gridVector;
	int32_t height;
	int32_t width;
	bool randomizeNeighbors;
	int density;
	std::vector<sf::Sprite>sprites;
	sf::Texture dirt;
	sf::Texture grass;
	sf::Texture water;
	sf::Texture point;
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
	
	void guiGrid();
	bool init();
	bool open();
	void run();
	void close();
	void exit();
};