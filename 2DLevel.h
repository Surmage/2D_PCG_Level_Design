#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

struct Cell {	
	enum class Type : int {
		ALIVE,
		DEAD
	};
	constexpr static float cellSize = 10;
	int32_t x, y;
	sf::Color color;
	Type type;
	sf::RectangleShape cell;

	Cell();
	Cell(int _x, int _y, Cell::Type _type);
	void setPosition(int _x, int _y);
	int getState();
};

struct Grid {
	std::vector<std::vector<Cell>>gridVector;
	int32_t height;
	int32_t width;
	bool randomizeNeighbors;

	int countNeighbors(int x, int y);
	void initGridVector(bool randomStates);

	void setWidth(int width);
	void setHeight(int height);

	static void placeCell(int x, int y, std::vector<std::vector<Cell>>& gridVec);
	static void deleteCell(int x, int y, std::vector<std::vector<Cell>>& gridVec);
	bool resetGrid();

	std::vector<std::vector<Cell>> gridCopy(const std::vector<std::vector<Cell>>& gridVec);
	void update();

	void run();

	void display(int gWidth, int gHeight);

	Grid();
	Grid(int gridWidth, int gridHeight, bool random);
};

struct LevelApp 
{
	LevelApp();
	~LevelApp();
	//sf::RenderWindow app;
	sf::RenderWindow* app;
	Grid* grid;
	bool isRunning;
	sf::Vector2i mousePos;

	bool open();
	void run();
	void close();
	void exit();

	//int fillGrid(Grid& grid);
};