#include "2DLevel.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <filesystem>

const int width = 1080;
const int height = 720;
sf::Vector2i gridSize(300, 300);
const int cellSize = 32;
const int minSize = 30;
const float camD = 0.84; //camera depth
constexpr int window_delay = 50;


bool TileMap::load(const std::string& tileset, sf::Vector2u _tileSize, const std::vector<std::vector<int>> tiles, unsigned int width, unsigned int height)
{
    // load the tileset texture
    if (!m_tileset.loadFromFile(tileset))
        return false;

    this->tileSize = _tileSize;
    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Triangles);
    m_vertices.resize(width * height * 6);

    // populate the vertex array, with two triangles per tile
    for (unsigned int i = 0; i < width; ++i)
        for (unsigned int j = 0; j < height; ++j)
        {
            // get the current tile number
            //int tileNumber = tiles[i + j * width];
            int tileNumber = tiles[j][i];

            // find its position in the tileset texture
            int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
            int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

            // get a pointer to the triangles' vertices of the current tile
            sf::Vertex* triangles = &m_vertices[(i + j * width) * 6];

            // define the 6 corners of the two triangles
            triangles[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
            triangles[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
            triangles[2].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
            triangles[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
            triangles[4].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
            triangles[5].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);

            // define the 6 matching texture coordinates
            triangles[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
            triangles[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
            triangles[2].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
            triangles[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
            triangles[4].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
            triangles[5].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
        }

    return true;
}

void TileMap::changeTex(const int x, const int y, const int tileNum, unsigned int width) {

    // find its position in the tileset texture
    int tu = tileNum % (m_tileset.getSize().x / tileSize.x);
    int tv = tileNum / (m_tileset.getSize().x / tileSize.x);

    sf::Vertex* triangles = &m_vertices[(x + y * width) * 6];
    int res = (x + y) * 6;
    triangles[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
    triangles[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
    triangles[2].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
    triangles[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
    triangles[4].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
    triangles[5].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);
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

void Grid::printLevelArray() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << level[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void Grid::initGridVector(bool randomStates, int number) { //TODO: change to vertex array for optimization   
    this->level.clear();
    if (!randomStates) {
        for (int j = 0; j < this->height; j++) {
            std::vector<int> intVec;
            for (int i = 0; i < this->width; i++) {
                intVec.push_back(0);

            }
            this->level.push_back(intVec);
        }
    }
    else {
        srand(time(nullptr));
        for (int j = 0; j < this->height; j++) {
            std::vector<int> intVec;
            for (int i = 0; i < this->width; i++) {
                int n = rand() % 3;
                intVec.push_back(n);
                
            }
            this->level.push_back(intVec);
        }
    }

        //Not the most efficient
    
        /*for (int y = 0; y < this->height; y++) {            
            for (int x = 0; x < this->width; x++) {
                gridVector[x][y].pos = sf::Vector2f(float(y) * 16, float(x) * 16);
            }
        }*/
}

int Grid::countNeighborsSame(int x, int y, int areaSize) {
    int xMin = x - areaSize < 0 ? 0 : x - areaSize;
    int xMax = x + areaSize > this->width - 1 ? this->width - 1 : x + areaSize;
    int yMin = y - areaSize < 0 ? 0 : y - areaSize;
    int yMax = y + areaSize > this->height - 1 ? this->height - 1 : y + areaSize;
    {
        int state = level[x][y];
        int64_t count = 0;
        for (int i = xMin; i <= xMax; i++) {
            for (int j = yMin; j <= yMax; j++) {
                if (!(i == x && j == y)) {
                    if (level[i][j] == state) {
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
    {
        int state = level[x][y];
        int64_t count = 0;
        for (int i = xMin; i <= xMax; i++) {
            for (int j = yMin; j <= yMax; j++) {
                if (!(i == x && j == y)) {
                    if (level[i][j] != state) {
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

    int water = 0, earth = 0, grass = 0;
    int state = 0;
    int sum = ((2 * areaSize + 1) * (2 * areaSize + 1) * 0.5) - 1; //Moore neighbourhood formula / 2
    for (int i = xMin; i <= xMax; i++) {
        for (int j = yMin; j <= yMax; j++) {
            if (!(i == x && j == y)) //skip themselves
            {
                state = level[i][j];
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

bool Grid::checkPlusShape(int x, int y, int l, std::vector<std::vector<int>>& gridVec) {
    int xMin = x - l < 0 ? 0 : x - l;
    int xMax = x + l > this->width - 1 ? this->width - 1 : x + l;
    int yMin = y - l < 0 ? 0 : y - l;
    int yMax = y + l > this->height - 1 ? this->height - 1 : y + l;

    if ((gridVec[x][y] == gridVec[xMin][y] && xMin != 0)
        || (gridVec[x][y] == gridVec[xMax][y] && xMax != this->width-1)
        || (gridVec[x][y] == gridVec[x][yMin] && yMin != 0)
        || (gridVec[x][y] == gridVec[x][yMax] && yMax != this->height-1))
        return true;
    
    return false;
}

bool Grid::checkPlusShapeFull(int x, int y, int l, std::vector<std::vector<int>>& gridVec) {
    int xMin = x - 1 < 0 ? 0 : x - 1;
    int xMax = x + 1 > this->width - 1 ? this->width - 1 : x + 1;
    int yMin = y - 1 < 0 ? 0 : y - 1;
    int yMax = y + 1 > this->height - 1 ? this->height - 1 : y + 1;

    if ((gridVec[x][y] == gridVec[xMin][y] && xMin != 0)
        && (gridVec[x][y] == gridVec[xMax][y] && xMax != this->width - 1)
        && (gridVec[x][y] == gridVec[x][yMin] && yMin != 0)
        && (gridVec[x][y] == gridVec[x][yMax] && yMax != this->height - 1))
        return true;

    return false;
}

void Grid::fillGaps(int areaSize) {
    this->prevLevels[this->level] = this->prevLevels.size() + this->sprites.size();;

    int xMin;
    int xMax;
    int yMin;
    int yMax;
    int state;
    int count = 0;
    bool opp = false;
    std::vector<sf::Vector2i>points;
    std::vector<std::vector<int>> newGrid = gridCopy(level);

    for (int x = 0; x < this->width; x++) {
        for (int y = 0; y < this->height; y++) {
            if (level[x][y] == 0)
                continue;
            points.clear();
            opp = false;

            xMin = x - areaSize < 0 ? 0 : x - areaSize;
            xMax = x + areaSize > this->width - 1 ? this->width - 1 : x + areaSize;
            yMin = y - areaSize < 0 ? 0 : y - areaSize;
            yMax = y + areaSize > this->height - 1 ? this->height - 1 : y + areaSize;
            
            state = level[x][y];

            for (int i = xMin; i <= xMax; i++) {
                for (int j = yMin; j <= yMax; j++) {
                    if (!(i == x && j == y)) {
                        if (level[i][j] != state && !opp) {
                            opp = true;
                        }
                        else if (level[i][j] == state && opp) {
                            count++;
                            points.push_back(sf::Vector2i(i, j));
                        }
                    }
                }
            }

            //draw towards it
            for (const sf::Vector2i p : points) { 
                for (int i = x; i-- > p.x;) { //x
                    for (int j = y; j-- > p.y;) { //y
                        newGrid[i][j] = state;
                        //map.changeTex(i, j, state, this->width);
                    }
                    
                }   
            }       
        }
    }
   
    for (int x = 0; x < this->width; x++) {
        for (int y = 0; y < this->height; y++) {
            if (level[x][y] == 0 && countNeighborsDiff(x, y, 2) >= 8) {
                int state = getCellNeighbors(x, y, 2);
                newGrid[x][y] = state;
                //map.changeTex(x, y, state, this->width);
            }
        }
    }
    for (int x = 0; x < this->width; x++) {
        for (int y = 0; y < this->height; y++) {
            if (!checkPlusShape(x, y, 1, newGrid)) //remove single cells with no same neighbors in plus shape
            {
                int state = getCellNeighbors(x, y, 1);
                newGrid[x][y] = state;
                //map.changeTex(x, y, state, this->width);
            }
        }
    }
    //look 3 points to each direction
    this->level = newGrid;
    this->map.load("images/tiles/tileset.png", sf::Vector2u(cellSize, cellSize), this->level, this->width, this->height);
    
}

void Grid::update(int& density) {
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if ((level[i][j] == 0))
            {
                if (countNeighborsDiff(i, j, (int)(this->height * 0.018)) >= density) { //water turn to land
                    int state = getCellNeighbors(i, j, 3);
                    level[i][j] = state;
                    //map.changeTex(i, j, state, this->width);
                    continue;
                }

            }

            else if (countNeighborsDiff(i, j, (int)(this->height * 0.018)) >= density) { //land spread
                int state = getCellNeighbors(i, j, (int)(this->height * 0.018));
                level[i][j] = state; //increased density means this gets triggered less
                //map.changeTex(i, j, state, this->width);
                continue;
            }

            else if (countNeighborsDiff(i, j, 1) >= 4 && !checkPlusShape(i, j, 1, level)) //remove single cells with no same neighbors in plus shape
            {
                int state = getCellNeighbors(i, j, 1);                
                level[i][j] = state;
                //map.changeTex(i, j, state, this->width);
                continue;
            }         
        }
    }
    map.load("images/tiles/tileset.png", sf::Vector2u(cellSize, cellSize), this->level, this->width, this->height);
}

std::vector<std::vector<int>> Grid::gridCopy(const std::vector<std::vector<int>>& gridVec) {
    std::vector<std::vector<int>> copy;
    for (int i = 0; i < this->level.size(); i++) {
        std::vector<int> copyVec;
        for (int j = 0; j < level[i].size(); j++) {
            copyVec.push_back(gridVec[i][j]);
        }
        copy.push_back(copyVec);
    }
    return copy;
}
bool Grid::resetGrid() {
    for (int i = level.size(); i-- > 0;) {
        level[i].clear();
        level[i].shrink_to_fit();
    }
    return true;
}

void Grid::setWidth(int gWidth) {
    this->width = gWidth;
}

void Grid::setHeight(int gHeight) {
    this->height = gHeight;
}

void Grid::generatePoints() {
    srand(time(nullptr));
    int n = (rand() % 5) + 1;
    int n2 = (rand() % 5) + 1;
    int x = gridSize.x / n2;
    int y = gridSize.y / n;
    bool foundS = false, foundE = false;
    
    for (int i = y; i < this->height; i++) {
        for (int j = x; j < this->width; j++) {
            if (level[j][i] == 0) //if is not walkable
                continue;
            if (checkPlusShape(j, i, 1, level) && level[j][i] != 0) {
                start = sf::Vector2i(j, i);
                level[j][i] = 3; //maximum tyles + 1
                map.changeTex(i, j, 3, this->width);
                //gridVector[j][i].point.setScale(sf::Vector2f(200, 200));
                
                foundS = true;
                std::cout << "Start: " << start.x << " " << start.y
                    << std::endl;
                break;
            }
        }
        if (foundS)
            break;
    }

    for (int i = gridSize.y - y; i-- > 0;) {
        for (int j = gridSize.x - x; j-- > 0;) {
            if (checkPlusShape(j, i, 1, level) && level[j][i] != 0) {
                end = sf::Vector2i(j, i);
                level[j][i] = 3;
                map.changeTex(i, j, 3, this->width);
                //gridVector[j][i].point.setScale(sf::Vector2f(200, 200));
                foundE = true;
                std::cout 
                    << "End: " << end.x << " " << end.y << std::endl;
                break;
            }
        }
        if (foundE)
            break;
    }
    if (!foundS || !foundE)
        std::cout << "Failed to generate" << std::endl;
    
    
}

bool Grid::loadPrevLevel(const int i) {
    if (prevLevels.empty()) return false;
    this->level = (std::prev(prevLevels.end())->first);
    this->prevLevels.erase(std::prev(prevLevels.end()));
    this->map.load("images/tiles/tileset.png", sf::Vector2u(cellSize, cellSize), this->level, this->width, this->height);
    return true;
}

LevelApp::LevelApp() {
    isRunning = false;
    //typePlace = 0;
    density = 100;
    generated = false;
    tileSize = 1;
    spritePlaceOn = false;
    editOn = false;
}
LevelApp::~LevelApp() {

}

void LevelApp::drawAt(sf::Sprite& mySprite, int x, int y)
{
    // set sprite position, then draw it
    mySprite.setPosition(sf::Vector2f(x, y));
    app->draw(mySprite);
}

void LevelApp::guiGrid() {
    ImGui::BeginChild("Tileset", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGui::InputInt("Tile Scale", &this->tileSize, 1);
    
    for (const auto& i : this->textures) {
        ImTextureID tilesetTextureId = (ImTextureID)(intptr_t)i.getNativeHandle(); // Cast the texture ID to ImTextureID
        if (ImGui::ImageButton((ImTextureID)tilesetTextureId, ImVec2(50, 50)) && editOn)
        {
            sf::Sprite sprite;
            sprite.setTexture(i);
            spritePlaceOn = true;
            this->sprite = sprite;
        }
    }
    ImGui::EndChild();
}

bool LevelApp::init() {
    bool randomize = true;
    grid = std::make_shared<Grid>(gridSize.x, gridSize.y, randomize);
    grid->initGridVector(randomize, 200);
    grid->map.load("images/tiles/tileset.png", sf::Vector2u(cellSize, cellSize), grid->level, grid->width, grid->height);
    
    generated = true;
    generatedX = gridSize.x;
    generatedY = gridSize.y;
    return true;
}

bool LevelApp::open()
{
    this->app = new sf::RenderWindow(sf::VideoMode(int(width), int(height)), "Cellular Automata", sf::Style::Default);
    app->setFramerateLimit(60);
    ImGui::SFML::Init(*app);

    std::string path = "images/textures/";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        sf::Image image;
        if (!image.loadFromFile(std::string(entry.path().string())))
        {
            // Error...
        }
        sf::Texture texture;
        texture.loadFromImage(image);  //Load Texture from image
        textures.push_back(texture);
    }

    return 1;
}

void LevelApp::run() {   
    sf::Clock deltaClock;
    sf::Vector2f oldPos;
    bool moving = false;
    float zoom = 1;
    bool cameraMoveOn = false;
    float rotateImage = 0;
    
    sf::View view = app->getDefaultView();
    //Run the program as long as the window is open
    while (app->isOpen()) {
        //Check all the window's events that were triggered since the last iteration of the loop
        ImGui::SFML::Update(*app, deltaClock.restart());
        ImGui::Begin("Imgui");
        
        sf::Event event{};
        while (app->pollEvent(event)) { //maybe TODO: make into a function
            ImGui::SFML::ProcessEvent(event);
            //"close requested" event: closes the window
            if (!generated) {
                break;
            }           
            switch (event.type)
            {
            case sf::Event::Closed:
                app->close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                {
                    moving = false;
                    cameraMoveOn = false;
                    isRunning = false;
                }
                if (event.key.code == sf::Keyboard::Space) {
                    isRunning = !isRunning;
                }
                else if (event.key.code == sf::Keyboard::Z && event.key.control) {
                    //Check if latest addition is in levels or sprites
                    std::cout << std::prev(this->grid->prevLevels.end())->second << " " << this->grid->prevLevels.size() + this->grid->sprites.size() - 1 << std::endl;
                    if (!this->grid->prevLevels.empty()) {
                        if (std::prev(this->grid->prevLevels.end())->second
                            == this->grid->prevLevels.size() + this->grid->sprites.size() - 1) {
                            grid->loadPrevLevel(1);
                            break;
                        }
                    }
                    
                    if (!this->grid->sprites.empty()) {
                        if (std::get<1>(this->grid->sprites[this->grid->sprites.size() - 1])
                            == this->grid->prevLevels.size() + this->grid->sprites.size() - 1) {
                            this->grid->sprites.pop_back();
                        }
                    }
                   
                }
                else if (event.key.code == sf::Keyboard::LShift) {
                    cameraMoveOn = true;

                }
                else if (event.key.code == sf::Keyboard::E)
                    editOn = !editOn;

                break;

            case sf::Event::KeyReleased:
                if (event.key.code == sf::Keyboard::LShift) {
                    cameraMoveOn = false;

                }
                break;
                
            case sf::Event::MouseButtonPressed:
                if (cameraMoveOn && !ImGui::GetIO().WantCaptureMouse)
                    moving = true;
                oldPos = app->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                if (!isRunning && editOn && !ImGui::GetIO().WantCaptureMouse)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (!spritePlaceOn) {
                            if (oldPos.x <= gridSize.x * cellSize &&
                                oldPos.x >= 0 &&
                                oldPos.y <= gridSize.y * cellSize &&
                                oldPos.y >= 0)
                                grid->level[int(oldPos.x) / int(cellSize)][int(oldPos.y) / int(cellSize)] = 1;
                        }
                        else {
                            this->grid->sprites.push_back(std::make_tuple
                            (sprite, this->grid->sprites.size() + this->grid->prevLevels.size()));
                        }
                            
                    }
                    if (event.mouseButton.button == sf::Mouse::Right)std::map<sf::Sprite, int>sprites;
                    {
                        if (!spritePlaceOn) {
                            for (int i = grid->sprites.size() - 1; i >= 0; i--) {
                                if (std::get<0>(grid->sprites[i]).getGlobalBounds().contains(oldPos)) {
                                    grid->sprites.erase(grid->sprites.begin() + i);
                                    goto out;
                                }
                            }
                                
                            if (oldPos.x <= gridSize.x * cellSize &&
                                oldPos.x >= 0 &&
                                oldPos.y <= gridSize.y * cellSize &&
                                oldPos.y >= 0) {
                                //this->grid->prevLevels.push_back((this->grid->level = 1));
                                this->grid->prevLevels[this->grid->level] = this->grid->prevLevels.size() + this->grid->sprites.size();
                                grid->level[int(oldPos.y) / int(cellSize)][int(oldPos.x) / int(cellSize)] = 3;
                                grid->map.changeTex(int(oldPos.x) / int(cellSize), (int)oldPos.y / int(cellSize), 3, grid->width);
                            }
                                       
                        }
                        spritePlaceOn = false;
                                                   
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
                if (spritePlaceOn) {
                    if (event.mouseWheelScroll.delta >= 1)
                        sprite.rotate(5);
                        //rotateImage = std::min(15.f, rotateImage + 1.f);
                    else if (event.mouseWheelScroll.delta <= -1)
                        sprite.rotate(-5);
                        //rotateImage = std::max(1.f, rotateImage - 1.f);
                }

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
        out:
        ImGui::Text("Mouse position:(%i, %i)", mousePos.x, mousePos.y);
        ImGui::InputInt("Width", &gridSize.x, 5);
        ImGui::InputInt("Height", &gridSize.y, 5);        
        
        if (ImGui::Button("Generate")) {
            isRunning = false;
            init();
            //grid->printLevelArray();
            grid->map.m_vertices;
            
        }
        if (ImGui::Button("Start")) {
            isRunning = !isRunning;
        }
        if (ImGui::Button("Reset")) {
            //reset
            isRunning = false;
            moving = false;
            cameraMoveOn = false;
            grid->resetGrid();
            generated = false;
        }
        if (ImGui::Button("Fill")) {
            grid->fillGaps(3);
        }
        if (ImGui::Button("Generate points")) {
            grid->generatePoints();
        }
        if (ImGui::Button("Undo")) {
            grid->loadPrevLevel(1);
        }
        
        app->clear(sf::Color::White);

        ImGui::Checkbox("Movement", &cameraMoveOn);
        ImGui::Checkbox("Edit", &editOn);
        ImGui::SliderInt("Density", &density, 80, 120);
        guiGrid();
        
        ImGui::End();
        if (generated) {
            app->draw(grid->map);
            for (const auto& i : grid->sprites)
                app->draw(std::get<0>(i));

            if (isRunning) {
                this->grid->prevLevels[this->grid->level] = this->grid->prevLevels.size() + this->grid->sprites.size();
                grid->update(density);      
            }
        }
        if (spritePlaceOn) {
            sprite.setOrigin(sf::Vector2f(0.5f*tileSize, 0.5f*tileSize));
            sprite.setPosition(sf::Vector2f(mousePos));           
            sprite.setScale(sf::Vector2f(tileSize, tileSize));
            
            app->draw(sprite);
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