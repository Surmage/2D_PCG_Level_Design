#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

struct Cell {	
	enum class Type : int {
		WATER,
		EARTH,
		GRASS,
		LAVA
	};
	constexpr static float cellSize = 6;
	int32_t x, y;
	sf::Color color;
	Type type;
	sf::RectangleShape cell;
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

	int countNeighborsSame(int x, int y, int areaSize);
	int countNeighborsDiff(int x, int y, int areaSize);
	int getCellNeighbors(int x, int y, int areaSize);
	bool checkPlusShape(int x, int y, std::vector<std::vector<Cell>>& gridVec);
	void initGridVector(bool randomStates, int number);

	void setWidth(int width);
	void setHeight(int height);

	/*static void placeCell(int x, int y, int newType, std::vector<std::vector<Cell>>& gridVec);
	static void deleteCell(int x, int y, std::vector<std::vector<Cell>>& gridVec);*/
	bool resetGrid();

	std::vector<std::vector<Cell>> gridCopy(const std::vector<std::vector<Cell>>& gridVec);
	void update(int& density);

	Grid();
	Grid(int gridWidth, int gridHeight, bool random);
};

struct LevelApp 
{
	LevelApp();
	~LevelApp();
	sf::RenderWindow* app;
	Grid* grid;
	bool isRunning;
	int density;
	sf::Vector2i mousePos;
	std::vector<sf::Sprite>sprites;
	//int typePlace;

	bool open();
	void run();
	void close();
	void exit();

	//int fillGrid(Grid& grid);
};