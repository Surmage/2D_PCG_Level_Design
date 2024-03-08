#include "2DLevel.h"
#include "imgui.h"
#include "imgui-SFML.h"

const int width = 1080;
const int height = 720;
const sf::Vector2i gridSize(300, 300);
const int minSize = 30;
const float camD = 0.84; //camera depth
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
    this->randomizeNeighbors = random;
}

void Grid::initGridVector(bool randomStates, int number) { //TODO: change to vertex array for optimization
    gridVector.clear();
    //gridVector.empty();
    if (!randomStates) {
        for (int i = 0; i < this->width; i++) {
            std::vector<Cell> cellVec;
            for (int j = 0; j < this->height; j++) {
                cellVec.push_back(*new Cell(i, j, Cell::Type::WATER));
            }
            gridVector.push_back(cellVec);
        }
    }
    else {
        srand(time(nullptr));
        for (int i = 0; i < this->width; i++) {
            std::vector<Cell> cellVec;
            for (int j = 0; j < this->height; j++) {
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
    for (int x = 0; x < gridSize.x; x++) {
        for (int y = 0; y < gridSize.y; y++) {
            gridVector[x][y].cell.setPosition(float(x) * gridVector[x][y].cell.getSize().x, float(y) * gridVector[x][y].cell.getSize().y);
            //gridVector[x][y].cell.setSize(sf::Vector2f(10, 10));
            //gridVector[x][y].cell.setOutlineThickness(1);
            //gridVector[x][y].cell.setOutlineColor(GREY);
            if (gridVector[x][y].type == Cell::Type::WATER) { gridVector[x][y].cell.setFillColor(WATER); }
            else if(gridVector[x][y].type == Cell::Type::EARTH){ gridVector[x][y].cell.setFillColor(DIRT); }
            else { gridVector[x][y].cell.setFillColor(GRASS); }
        }
    }
}

int Grid::countNeighborsSame(int x, int y, int areaSize) {
    int xMin = x - areaSize < 0 ? 0 : x - areaSize;
    int xMax = x + areaSize > this->width - 1 ? this->width - 1 : x + areaSize;
    int yMin = y - areaSize < 0 ? 0 : y - areaSize;
    int yMax = y + areaSize > this->height - 1 ? this->height - 1 : y + areaSize;
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
    int xMax = x + areaSize > this->width - 1 ? this->width - 1 : x + areaSize;
    int yMin = y - areaSize < 0 ? 0 : y - areaSize;
    int yMax = y + areaSize > this->height - 1 ? this->height - 1 : y + areaSize;
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
    int xMax = x + areaSize > this->width - 1 ? this->width - 1 : x + areaSize;
    int yMin = y - areaSize < 0 ? 0 : y - areaSize;
    int yMax = y + areaSize > this->height - 1 ? this->height - 1 : y + areaSize;
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
    int xMax = x + 1 > this->width - 1 ? this->width - 1 : x + 1;
    int yMin = y - 1 < 0 ? 0 : y - 1;
    int yMax = y + 1 > this->height - 1 ? this->height - 1 : y + 1;

    if ((gridVec[x][y].getState() == gridVec[xMin][y].getState() && xMin != 0)
        || (gridVec[x][y].getState() == gridVec[xMax][y].getState() && xMax != this->width-1)
        || (gridVec[x][y].getState() == gridVec[x][yMin].getState() && yMin != 0)
        || (gridVec[x][y].getState() == gridVec[x][yMax].getState() && yMax != this->height-1))
        return true;
    
    return false;
}

void Grid::update(int& density) {
    //std::vector<std::vector<Cell>> newGrid = gridCopy(gridVector);
    for (int i = 0; i < gridSize.x; i++) {
        for (int j = 0; j < gridSize.y; j++) {
            if ((gridVector[i][j].type == Cell::Type::WATER))
            {
                if (countNeighborsDiff(i, j, 1) >= 4) { //water turn to land
                    gridVector[i][j].setType(getCellNeighbors(i, j, 1));
                    continue;
                }

            }

            else if (countNeighborsDiff(i, j, (int)(height * 0.018)) >= density) { //land spread
                gridVector[i][j].setType(getCellNeighbors(i, j, (int)(height * 0.018))); //increased density means this gets triggered less
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
    initGridVector(true, 200);
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
    //typePlace = 0;
    density = 90;
}
LevelApp::~LevelApp() {

}

bool LevelApp::open()
{
    bool randomize = true;
    grid = new Grid(gridSize.x, gridSize.y, randomize);
    //gameOfLife.run();
    //int extra = width <= minSize ? 15 : 0;
    this->app = new sf::RenderWindow(sf::VideoMode(int(width), int(height)), "Cellular Automata", sf::Style::Default);
    app->setFramerateLimit(60);
    ImGui::SFML::Init(*app);
    grid->initGridVector(randomize, 200);
    return 1;
}

void LevelApp::run() {   
    sf::Clock deltaClock;
    sf::Vector2f oldPos;
    bool moving = false;
    float zoom = 1;
    bool cameraMoveOn = false;
    bool editOn = false;
    sf::Image image;
    if (!image.loadFromFile("../images/red.png"))
    {
        // Error...
    }
    sf::Texture texture;
    texture.loadFromImage(image);  //Load Texture from image
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprites.push_back(sprite);
    

    sf::View view = app->getDefaultView();
    //Run the program as long as the window is open
    while (app->isOpen()) {
        //Check all the window's events that were triggered since the last iteration of the loop
        ImGui::SFML::Update(*app, deltaClock.restart());
        ImGui::Begin("Imgui");
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
                if (cameraMoveOn && !ImGui::GetIO().WantCaptureMouse)
                    moving = true;
                oldPos = app->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                sprites[0].setPosition(oldPos);
                
                if (!isRunning && editOn && !ImGui::GetIO().WantCaptureMouse)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        //std::cout << event.mouseButton.x / Cell::cellSize << std::endl;
                        if (oldPos.x <= gridSize.x * Cell::cellSize &&
                            oldPos.x >= 0 &&
                            oldPos.y <= gridSize.y * Cell::cellSize &&
                            oldPos.y >= 0)
                            grid->gridVector[int(oldPos.x) / int(Cell::cellSize)][int(oldPos.y) / int(Cell::cellSize)].setType(1);
                        //                                std::cout << "Number of alive neighbors: " << countNeighbors(int(event.mouseButton.x)/int(Cell::cellSize),int(event.mouseButton.y)/int(Cell::cellSize)) << std::endl;
                        //                                std::cout<< "Current cell state: " << gridVector[int(event.mouseButton.x)/int(Cell::cellSize)][int(event.mouseButton.y)/int(Cell::cellSize)].getState() << std::endl;
                    }
                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        //std::cout << event.mouseButton.x / Cell::cellSize  << std::endl;
                        if (oldPos.x <= gridSize.x * Cell::cellSize &&
                            oldPos.x >= 0 &&
                            oldPos.y <= gridSize.y * Cell::cellSize &&
                            oldPos.y >= 0)
                            grid->gridVector[int(oldPos.x) / int(Cell::cellSize)][int(oldPos.y) / int(Cell::cellSize)].setType(0);
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
                mousePos = (sf::Vector2i)app->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
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
                if (!cameraMoveOn || moving)
                    break;
                // Ignore the mouse wheel unless we're not moving

                // Determine the scroll direction and adjust the zoom level
                // Again, you can swap these to invert the direction
                if (event.mouseWheelScroll.delta <= -1)
                    zoom = std::min(5.f, zoom + .1f);
                else if (event.mouseWheelScroll.delta >= 1)
                    zoom = std::max(.1f, zoom - .1f);

                // Update our view
                view.setSize(app->getDefaultView().getSize()); // Reset the size
                view.zoom(zoom); // Apply the zoom level (this transforms the view)
                app->setView(view);
            }
            break;


            }
        }           
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
        
        app->clear(sf::Color::White);
        ImGui::Checkbox("Movement", &cameraMoveOn);
        ImGui::Checkbox("Edit", &editOn);
        ImGui::SliderInt("Density", &density, 80, 120);
        ImGui::End();

        for (const auto& i : grid->gridVector) {
            for (const auto& j : i) {
                if(true) //WIP. if on screen
                    app->draw(j.cell);
            }
        }
        
        app->draw(sprites[0]);

        if (isRunning) {
            grid->update(density);
        }
        ImGui::SFML::Render(*app);
        app->display();
        //sf::sleep(sf::milliseconds(window_delay));
        
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