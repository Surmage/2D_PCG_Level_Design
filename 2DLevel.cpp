#include "2DLevel.h"
#include "imgui.h"
#include "imgui-SFML.h"

int width = 75;
int height = 75;
int minSize = 30;
float camD = 0.84; //camera depth
constexpr int window_delay = 50;
const sf::Color GREY(169, 169, 169);

Cell::Cell(){}

Cell::Cell(int _x, int _y, Cell::Type _type) {
    this->x = _x;
    this->y = _y;
    this->type = _type;
    cell.setSize(sf::Vector2f(cellSize, cellSize));
}

void Cell::setPosition(int _x, int _y) {
    x = _x;
    y = _y;
}
int Cell::getState() {
    if (type == Cell::Type::ALIVE)
        return 1;
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
                cellVec.push_back(*new Cell(i, j, Cell::Type::DEAD));
            }
            gridVector.push_back(cellVec);
        }
    }
    else {
        srand(time(nullptr));
        for (int i = 0; i < width; i++) {
            std::vector<Cell> cellVec;
            for (int j = 0; j < height; j++) {
                int n = rand() % 2;
                if(n == 0)
                    cellVec.push_back(*new Cell(i, j, Cell::Type::DEAD));
                else
                    cellVec.push_back(*new Cell(i, j, Cell::Type::ALIVE));
                
            }
            gridVector.push_back(cellVec);
        }
    }

    //Not the most efficient but I just want to demonstrate the use of the Cell class, however this can be done in a single nested loop
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            gridVector[x][y].cell.setPosition(float(x) * gridVector[x][y].cell.getSize().x, float(y) * gridVector[x][y].cell.getSize().y);
            gridVector[x][y].cell.setSize(sf::Vector2f(30, 30));
            gridVector[x][y].cell.setOutlineThickness(1);
            gridVector[x][y].cell.setOutlineColor(GREY);
            if (gridVector[x][y].type == Cell::Type::ALIVE) { gridVector[x][y].cell.setFillColor(sf::Color::White); }
            else { gridVector[x][y].cell.setFillColor(sf::Color::Black); }
        }
    }
}

void Grid::display(int gWidth, int gHeight) {
   
}

int Grid::countNeighbors(int x, int y) {
    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
        return 0;
    }
    else {
        int16_t count = 0;
        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                if (!(i == x && j == y)) {
                    if (gridVector[i][j].getState() == 1) {
                        count++;
                    }
                }
            }
        }
        return count;
    }
}

void Grid::update() {
    std::vector<std::vector<Cell>> newGrid = gridCopy(gridVector);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if ((countNeighbors(i, j) < 2 || countNeighbors(i, j) > 3) && (gridVector[i][j].getState())) {
                deleteCell(i, j, newGrid);
            }
            else if ((countNeighbors(i, j) == 3) && (!gridVector[i][j].getState())) {
                placeCell(i, j, newGrid);
            }
            else if ((countNeighbors(i, j) == 3 || countNeighbors(i, j) == 2) && (gridVector[i][j].getState())) {
                continue;
            }
        }
    }
    this->gridVector = newGrid;
}

void Grid::deleteCell(int x, int y, std::vector<std::vector<Cell>>& gridVec) {
    gridVec[x][y].type = Cell::Type::DEAD;
    gridVec[x][y].cell.setFillColor(sf::Color::Black);
}

void Grid::placeCell(int x, int y, std::vector<std::vector<Cell>>& gridVec) {
    gridVec[x][y].type = Cell::Type::ALIVE;
    gridVec[x][y].cell.setFillColor(sf::Color::White);
}

void Grid::run() {
    std::cout << "Automata is running, press E to begin the simulation, and P to pause, when paused, you can place your own cells!" << std::endl;
    initGridVector(randomizeNeighbors);
    display(int(Cell::cellSize * float(width)), int(Cell::cellSize * float(height)));

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
            case sf::Event::MouseMoved:
                mousePos = sf::Mouse::getPosition(*app);
                break;
            case sf::Event::MouseButtonPressed:
                if (!isRunning)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        //std::cout << event.mouseButton.x / Cell::cellSize << std::endl;
                        if (event.mouseButton.x / Cell::cellSize <= width)
                            grid->placeCell(int(event.mouseButton.x) / int(Cell::cellSize), int(event.mouseButton.y) / int(Cell::cellSize), grid->gridVector);
                        //                                std::cout << "Number of alive neighbors: " << countNeighbors(int(event.mouseButton.x)/int(Cell::cellSize),int(event.mouseButton.y)/int(Cell::cellSize)) << std::endl;
                        //                                std::cout<< "Current cell state: " << gridVector[int(event.mouseButton.x)/int(Cell::cellSize)][int(event.mouseButton.y)/int(Cell::cellSize)].getState() << std::endl;
                    }
                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        //std::cout << event.mouseButton.x / Cell::cellSize  << std::endl;
                        if (event.mouseButton.x / Cell::cellSize <= width)
                            grid->deleteCell(int(event.mouseButton.x) / int(Cell::cellSize), int(event.mouseButton.y) / int(Cell::cellSize), grid->gridVector);
                        //                                std::cout << "Number of alive neighbors: " << countNeighbors(int(event.mouseButton.x)/int(Cell::cellSize),int(event.mouseButton.y)/int(Cell::cellSize)) << std::endl;
                        //                                std::cout<< "Current cell state: " << gridVector[int(event.mouseButton.x)/int(Cell::cellSize)][int(event.mouseButton.y)/int(Cell::cellSize)].getState() << std::endl;

                    }
                }
                break;
            }
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
            grid->resetGrid();
        }
        ImGui::End();
        app->clear(sf::Color::White);

        for (const auto& i : grid->gridVector) {
            for (const auto& j : i) {
                app->draw(j.cell);
            }
        }

        if (isRunning) {
            grid->update();
        }
        ImGui::SFML::Render(*app);
        app->display();
        sf::sleep(sf::milliseconds(window_delay));
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