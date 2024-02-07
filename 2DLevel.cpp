#include "2DLevel.h"
#include "imgui.h"
#include "imgui-SFML.h"

int width = 75;
int height = 75;
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
    this->isRunning = false;
    this->randomizeNeighbors = true;
}
Grid::Grid(int gridWidth, int gridHeight, bool random) {
    this->height = gridHeight;
    this->width = gridWidth;
    this->isRunning = false;
    this->randomizeNeighbors = random;
}

void Grid::initGridVector(bool randomStates) {
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
    sf::RenderWindow window(sf::VideoMode(gWidth, gHeight), "Cellular Automata", sf::Style::Default);
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;
    //Run the program as long as the window is open
    while (window.isOpen()) {
        //Check all the window's events that were triggered since the last iteration of the loop
        sf::Event event{};
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            //"close requested" event: closes the window
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
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
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        placeCell(int(event.mouseButton.x) / int(Cell::cellSize), int(event.mouseButton.y) / int(Cell::cellSize), gridVector);
                        //                                std::cout << "Number of alive neighbors: " << countNeighbors(int(event.mouseButton.x)/int(Cell::cellSize),int(event.mouseButton.y)/int(Cell::cellSize)) << std::endl;
                        //                                std::cout<< "Current cell state: " << gridVector[int(event.mouseButton.x)/int(Cell::cellSize)][int(event.mouseButton.y)/int(Cell::cellSize)].getState() << std::endl;
                    }
                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        deleteCell(int(event.mouseButton.x) / int(Cell::cellSize), int(event.mouseButton.y) / int(Cell::cellSize), gridVector);
                        //                                std::cout << "Number of alive neighbors: " << countNeighbors(int(event.mouseButton.x)/int(Cell::cellSize),int(event.mouseButton.y)/int(Cell::cellSize)) << std::endl;
                        //                                std::cout<< "Current cell state: " << gridVector[int(event.mouseButton.x)/int(Cell::cellSize)][int(event.mouseButton.y)/int(Cell::cellSize)].getState() << std::endl;

                    }
                }
                break;
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Imgui");
        //ImGui::Text("A");
        if (ImGui::Button("Start")) {
            isRunning = !isRunning;
        }
        if (ImGui::Button("Reset")) {
            //isRunning = !isRunning;
        }
        ImGui::End();
        window.clear(sf::Color::White);

        for (const auto& i : gridVector) {
            for (const auto& j : i) {
                window.draw(j.cell);
            }
        }

        if (isRunning) {
            update();
        }
        ImGui::SFML::Render(window);
        window.display();
        sf::sleep(sf::milliseconds(window_delay));
    }
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

void Grid::setWidth(int gWidth) {
    this->width = gWidth;
}

void Grid::setHeight(int gHeight) {
    this->height = gHeight;
}

LevelApp::LevelApp() {

}
LevelApp::~LevelApp() {

}

bool LevelApp::open()
{
    bool randomize = true;
    Grid gameOfLife(width, height, randomize);
    gameOfLife.run();
    this->app = new sf::RenderWindow(sf::VideoMode(width, height), "2D Level");
    app->setFramerateLimit(60);

    
    return 1;
}

void LevelApp::run() {
    sf::Texture bg;
    bg.loadFromFile("../images/bg.png");
    bg.setRepeated(true);
    sf::Sprite sBackground(bg);
    sBackground.setTextureRect(sf::IntRect(0, 0, 5000, 2000));
    sBackground.setPosition(-2000, 0);

    float playerX = 0;

    ImGui::SFML::Init(*app);
    sf::Clock deltaClock;

    

    while (app->isOpen())
    {
        sf::Event e;

        while (app->pollEvent(e))
        {
            ImGui::SFML::ProcessEvent(e);
            if (e.type == sf::Event::Closed)
                app->close();
        }
        ImGui::SFML::Update(*app, deltaClock.restart());

        ImGui::Begin("A");
        ImGui::Text("A");
        ImGui::End();

        int speed = 0;
        int H = 0;

        /*  if (Keyboard::isKeyPressed(Keyboard::Right)) playerX+=0.1;
          if (Keyboard::isKeyPressed(Keyboard::Left)) playerX-=0.1;
          if (Keyboard::isKeyPressed(Keyboard::Up)) speed=200;
          if (Keyboard::isKeyPressed(Keyboard::Down)) speed=-200;
          if (Keyboard::isKeyPressed(Keyboard::LShift)) speed*=3;
          if (Keyboard::isKeyPressed(Keyboard::W)) H+=100;
          if (Keyboard::isKeyPressed(Keyboard::S)) H-=100;*/






        app->clear(sf::Color(105, 205, 4));
        app->draw(sBackground);
        sf::Color color(0, 154, 200);
        //drawQuad(*app, color, 100, 100, 200, 200); //x, y
        //drawGrid(*app);
        ImGui::SFML::Render(*app);
        app->display();
    }

    ImGui::SFML::Shutdown();

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