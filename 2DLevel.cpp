#include "2DLevel.h"
#include "imgui.h"
#include "imgui-SFML.h"

int width = 200;
int height = 200;
int minSize = 30;
float camD = 0.84; //camera depth
constexpr int window_delay = 50;
const sf::Color GREY(169, 169, 169);
const sf::Color DIRT(153, 76, 0);
const sf::Color WATER(51, 255, 255);
const sf::Color GRASS(0, 204, 0);

Cell::Cell(){}

Cell::Cell(int _x, int _y, Cell::Type _type) {
    this->x = _x;
    this->y = _y;
    this->type = _type;
    cell.setSize(sf::Vector2f(cellSize, cellSize));
    isWalkable = false;
}

void Cell::setPosition(int _x, int _y) {
    x = _x;
    y = _y;
}
void Cell::setType(int newType) {
    switch (newType)
    {
    case 0:
        type = Cell::Type::WATER;
        cell.setFillColor(WATER);
        isWalkable = false;
        break;
    case 1:
        type = Cell::Type::EARTH;
        cell.setFillColor(DIRT);
        isWalkable = true;
        break;
    case 2:
        type = Cell::Type::GRASS;
        cell.setFillColor(GRASS);
        isWalkable = true;
        break;
    }
}
int Cell::getState() {
    if (type == Cell::Type::EARTH)
        return 1;
    else if (type == Cell::Type::GRASS)
        return 2;
    else
        return 0;
}
Grid::Grid() {
    this->height = 800;
    this->width = 600;
    //this->isRunning = false;
    this->randomizeNeighbors = true;
}
Grid::Grid(int gridWidth, int gridHeight, bool random) {
    this->height = gridHeight;
    this->width = gridWidth;
    //this->isRunning = false;
    this->randomizeNeighbors = random;
}

void Grid::initGridVector(bool randomStates) {
    gridVector.clear();
    gridVector.empty();
    if (!randomStates) {
        for (int i = 0; i < width; i++) {
            std::vector<Cell> cellVec;
            for (int j = 0; j < height; j++) {
                cellVec.push_back(*new Cell(i, j, Cell::Type::WATER));
            }
            gridVector.push_back(cellVec);
        }
    }
    else {
        srand(time(nullptr));
        for (int i = 0; i < width; i++) {
            std::vector<Cell> cellVec;
            for (int j = 0; j < height; j++) {
                int n = rand() % 3;
                if(n == 0)
                    cellVec.push_back(*new Cell(i, j, Cell::Type::WATER));
                else if(n == 1)
                    cellVec.push_back(*new Cell(i, j, Cell::Type::EARTH));
                else
                    cellVec.push_back(*new Cell(i, j, Cell::Type::GRASS));
                
            }
            gridVector.push_back(cellVec);
        }
    }

    //Not the most efficient but I just want to demonstrate the use of the Cell class, however this can be done in a single nested loop
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            gridVector[x][y].cell.setPosition(float(x) * gridVector[x][y].cell.getSize().x, float(y) * gridVector[x][y].cell.getSize().y);
            gridVector[x][y].cell.setSize(sf::Vector2f(30, 30));
            //gridVector[x][y].cell.setOutlineThickness(1);
            //gridVector[x][y].cell.setOutlineColor(GREY);
            if (gridVector[x][y].type == Cell::Type::WATER) { gridVector[x][y].cell.setFillColor(WATER); }
            else if(gridVector[x][y].type == Cell::Type::EARTH){ gridVector[x][y].cell.setFillColor(DIRT); }
            else { gridVector[x][y].cell.setFillColor(GRASS); }
        }
    }
}

void Grid::display(int gWidth, int gHeight) {
   
}

int Grid::countNeighborsSame(int x, int y, int areaSize) {
    int xMin = x - areaSize < 0 ? 0 : x - areaSize;
    int xMax = x + areaSize > width - 1 ? width - 1 : x + areaSize;
    int yMin = y - areaSize < 0 ? 0 : y - areaSize;
    int yMax = y + areaSize > height - 1 ? height - 1 : y + areaSize;
    /*if (x - areaSize < 0 || x + areaSize > width - 1 || y - areaSize < 0 || y + areaSize > height - 1) {
        return 0;
    }*/
    {
        int state = gridVector[x][y].getState();
        int64_t count = 0;
        for (int i = xMin; i <= xMax; i++) {
            for (int j = yMin; j <= yMax; j++) {
                if (!(i == x && j == y)) {
                    if (gridVector[i][j].getState() == state) {
                        count++;
                    }
                }
            }
        }
        return count;
    }
}

int Grid::countNeighborsDiff(int x, int y, int areaSize) {
    int xMin = x - areaSize < 0 ? 0 : x - areaSize;
    int xMax = x + areaSize > width - 1 ? width - 1 : x + areaSize;
    int yMin = y - areaSize < 0 ? 0 : y - areaSize;
    int yMax = y + areaSize > height - 1 ? height - 1 : y + areaSize;
    /*if (x - areaSize < 0 || x + areaSize > width - 1 || y - areaSize < 0 || y + areaSize > height - 1) {
        return 0;
    }*/
    {
        int state = gridVector[x][y].getState();
        int64_t count = 0;
        for (int i = xMin; i <= xMax; i++) {
            for (int j = yMin; j <= yMax; j++) {
                if (!(i == x && j == y)) {
                    if (gridVector[i][j].getState() != state) {
                        count++;
                    }
                }
            }
        }
        return count;
    }
}

int Grid::getCellNeighbors(int x, int y, int areaSize) {
    int xMin = x - areaSize < 0 ? 0 : x - areaSize;
    int xMax = x + areaSize > width - 1 ? width - 1 : x + areaSize;
    int yMin = y - areaSize < 0 ? 0 : y - areaSize;
    int yMax = y + areaSize > height - 1 ? height - 1 : y + areaSize;
    /*if (x - areaSize < 0 || x + areaSize > width - 1 || y - areaSize < 0 || y + areaSize > height - 1) {
        return 0;
    }*/
    int water = 0, earth = 0, grass = 0;
    int state = 0;
    int sum = areaSize * 3 + areaSize * areaSize; //WIP
    for (int i = xMin; i <= xMax; i++) {
        for (int j = yMin; j <= yMax; j++) {
            if (!(i == x && j == y)) //skip themselves
            {
                state = gridVector[i][j].getState();
                switch (state)
                {             
                case 1:
                    earth++;
                    if (earth > sum)
                        return 1;
                    break;
                case 2:
                    grass++;
                    if (grass > sum)
                        return 2;
                    break;
                case 0:
                    water++;
                    if (water > sum)
                        return 0;
                    break;
                }

            }
        }
    }
    return 0;
}

bool Grid::checkPlusShape(int x, int y, std::vector<std::vector<Cell>>& gridVec) {
    int xMin = x - 1 < 0 ? 0 : x - 1;
    int xMax = x + 1 > width - 1 ? width - 1 : x + 1;
    int yMin = y - 1 < 0 ? 0 : y - 1;
    int yMax = y + 1 > height - 1 ? height - 1 : y + 1;

    if ((gridVec[x][y].getState() == gridVec[xMin][y].getState() && xMin != 0)
        || (gridVec[x][y].getState() == gridVec[xMax][y].getState() && xMax != width-1)
        || (gridVec[x][y].getState() == gridVec[x][yMin].getState() && yMin != 0)
        || (gridVec[x][y].getState() == gridVec[x][yMax].getState() && yMax != height-1))
        return true;
    
    return false;
}

void Grid::update(int& density) {
    //std::vector<std::vector<Cell>> newGrid = gridCopy(gridVector);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if ((gridVector[i][j].type == Cell::Type::WATER))
            {
                if (countNeighborsDiff(i, j, 1) >= 4) { //water turn to land
                    gridVector[i][j].setType(getCellNeighbors(i, j, 1));
                    continue;
                }

            }

            else if (countNeighborsDiff(i, j, 5) >= density) { //land spread
                gridVector[i][j].setType(getCellNeighbors(i, j, 5)); //increased density means this gets triggered less
                continue;
            }


            if (countNeighborsDiff(i, j, 1) >= 4 && !checkPlusShape(i, j, gridVector)) //remove single cells with no same neighbors in plus shape
            {
                gridVector[i][j].setType(getCellNeighbors(i, j, 1));
                continue;
            }
            
        }
    }
    
    //this->gridVector = newGrid;
}

//void Grid::deleteCell(int x, int y, std::vector<std::vector<Cell>>& gridVec) {
//    gridVec[x][y].type = Cell::Type::WATER;
//    gridVec[x][y].cell.setFillColor(WATER);
//}
//
//void Grid::placeCell(int x, int y, int newType, std::vector<std::vector<Cell>>& gridVec) {
//    gridVec[x][y].type = Cell::Type::EARTH;
//    gridVec[x][y].cell.setFillColor(DIRT);
//}

std::vector<std::vector<Cell>> Grid::gridCopy(const std::vector<std::vector<Cell>>& gridVec) {
    std::vector<std::vector<Cell>> copy;
    for (int i = 0; i < gridVector.size(); i++) {
        std::vector<Cell> copyVec;
        for (int j = 0; j < gridVector[i].size(); j++) {
            copyVec.push_back(gridVec[i][j]);
        }
        copy.push_back(copyVec);
    }
    return copy;
}
bool Grid::resetGrid() {
    initGridVector(true);
    return true;
}

void Grid::setWidth(int gWidth) {
    this->width = gWidth;
}

void Grid::setHeight(int gHeight) {
    this->height = gHeight;
}

LevelApp::LevelApp() {
    isRunning = false;
    typePlace = 0;
    density = 75;
}
LevelApp::~LevelApp() {

}

bool LevelApp::open()
{
    bool randomize = true;
    grid = new Grid(width, height, randomize);
    //gameOfLife.run();
    int extra = width <= minSize ? 15 : 0;
    this->app = new sf::RenderWindow(sf::VideoMode(int(Cell::cellSize * float(width+extra)), int(Cell::cellSize * float(height))), "Cellular Automata", sf::Style::Default);
    app->setFramerateLimit(60);
    ImGui::SFML::Init(*app);
    grid->initGridVector(randomize);
    return 1;
}

void LevelApp::run() {   
    sf::Clock deltaClock;
    sf::Vector2f oldPos;
    bool moving = false;
    float zoom = 1;
    bool cameraMoveOn = false;

    sf::View view = app->getDefaultView();
    //Run the program as long as the window is open
    while (app->isOpen()) {
        //Check all the window's events that were triggered since the last iteration of the loop
        sf::Event event{};
        while (app->pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            //"close requested" event: closes the window
            switch (event.type)
            {
            case sf::Event::Closed:
                app->close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::E)
                {
                    std::cout << "E is pressed! Begin Automata!" << std::endl;
                    isRunning = true;
                    //                            update(); <-- For testing purposes
                }
                if (event.key.code == sf::Keyboard::P)
                {
                    std::cout << "P is pressed! Automata has stopped/paused!" << std::endl;
                    isRunning = false;
                }

                break;            
            case sf::Event::MouseButtonPressed:
                if (!isRunning)
                {
                    if(cameraMoveOn)
                        moving = true;
                    oldPos = app->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        //std::cout << event.mouseButton.x / Cell::cellSize << std::endl;
                        if (event.mouseButton.x / Cell::cellSize <= width)
                            grid->gridVector[int(event.mouseButton.x) / int(Cell::cellSize)][int(event.mouseButton.y) / int(Cell::cellSize)].setType(1);
                        //                                std::cout << "Number of alive neighbors: " << countNeighbors(int(event.mouseButton.x)/int(Cell::cellSize),int(event.mouseButton.y)/int(Cell::cellSize)) << std::endl;
                        //                                std::cout<< "Current cell state: " << gridVector[int(event.mouseButton.x)/int(Cell::cellSize)][int(event.mouseButton.y)/int(Cell::cellSize)].getState() << std::endl;
                    }
                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        //std::cout << event.mouseButton.x / Cell::cellSize  << std::endl;
                        if (event.mouseButton.x / Cell::cellSize <= width)
                            grid->gridVector[int(event.mouseButton.x) / int(Cell::cellSize)][int(event.mouseButton.y) / int(Cell::cellSize)].setType(0);
                        //                                std::cout << "Number of alive neighbors: " << countNeighbors(int(event.mouseButton.x)/int(Cell::cellSize),int(event.mouseButton.y)/int(Cell::cellSize)) << std::endl;
                        //                                std::cout<< "Current cell state: " << gridVector[int(event.mouseButton.x)/int(Cell::cellSize)][int(event.mouseButton.y)/int(Cell::cellSize)].getState() << std::endl;

                    }
                }
                break;
            case sf::Event::MouseButtonReleased:
                // Mouse button is released, no longer move
                if (event.mouseButton.button == 0) {
                    moving = false;
                }
                break;
            case sf::Event::MouseMoved:
            {
                mousePos = sf::Mouse::getPosition(*app);
                
                // Ignore mouse movement unless a button is pressed (see above)
                if (!moving || !cameraMoveOn)
                    break;
                // Determine the new position in world coordinates
                const sf::Vector2f newPos = app->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                // Determine how the cursor has moved
                // Swap these to invert the movement direction
                const sf::Vector2f deltaPos = oldPos - newPos;

                // Move our view accordingly and update the window
                view.setCenter(view.getCenter() + deltaPos);
                app->setView(view);

                // Save the new position as the old one
                // We're recalculating this, since we've changed the view
                oldPos = app->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            }
                break;

            case sf::Event::MouseWheelScrolled:
            {
                if (!cameraMoveOn)
                    break;
                // Ignore the mouse wheel unless we're not moving
                if (moving)
                    break;

                // Determine the scroll direction and adjust the zoom level
                // Again, you can swap these to invert the direction
                if (event.mouseWheelScroll.delta <= -1)
                    zoom = std::min(2.f, zoom + .1f);
                else if (event.mouseWheelScroll.delta >= 1)
                    zoom = std::max(.5f, zoom - .1f);

                // Update our view
                view.setSize(app->getDefaultView().getSize()); // Reset the size
                view.zoom(zoom); // Apply the zoom level (this transforms the view)
                app->setView(view);
            }
                break;


            }
            ImGui::SFML::Update(*app, deltaClock.restart());

            ImGui::Begin("Imgui");
            ImGui::Text("Mouse position:(%i, %i)", mousePos.x, mousePos.y);
            if (ImGui::Button("Start")) {
                isRunning = !isRunning;
            }
            if (ImGui::Button("Reset")) {
                //reset
                isRunning = false;
                moving = false;
                cameraMoveOn = false;
                grid->resetGrid();
            }
            if (ImGui::Button("Movement")) {
                cameraMoveOn = !cameraMoveOn;
                
            }
            ImGui::SliderInt("Density", &density, 60, 100);
            ImGui::End();
            app->clear(sf::Color::White);

            for (const auto& i : grid->gridVector) {
                for (const auto& j : i) {
                    app->draw(j.cell);
                }
            }

            if (isRunning) {
                grid->update(density);
            }
            ImGui::SFML::Render(*app);
            app->display();
            //sf::sleep(sf::milliseconds(window_delay));
        }
    }
}
void LevelApp::close() {
    delete this->app;
}
void LevelApp::exit() {

}

int main(int argc, const char** argv) {
    LevelApp app;
    if (app.open()) {
        app.run();
        app.close();
    }
    app.exit();
}