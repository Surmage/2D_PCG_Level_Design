#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
using namespace sf;

int width = 1280;
int height = 720;
int roadW = 2000;
int segL = 200; //segment length
float camD = 0.84; //camera depth

void drawQuad(RenderWindow &w, Color c, int x1,int y1,int x2,int y2)
{
    ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, Vector2f(x1, y1));
    shape.setPoint(1, Vector2f(x2, y1));
    shape.setPoint(2, Vector2f(x2, y2));
    shape.setPoint(3, Vector2f(x1, y2));
    w.draw(shape);
}

void drawGrid(RenderWindow& w) {
    int x = 0, y = 0;
    int rows = 100, distance = 25;
    Color color(200, 200, 200);
    for (int i = 0; i < rows; i++) {    
        drawQuad(w, color, x, y, x+2, y+height);
        x += distance;
    }
    x = 0, y = 0;
    for (int i = 0; i < rows; i++) {
        drawQuad(w, color, x, y, x+width, y+2);
        y += distance;
    }
    
}

struct Line
{
    float x,y,z; //3d center of line
    float X,Y,W; //screen coord
    float curve,spriteX,clip,scale;
    Sprite sprite;

    Line()
    {spriteX=curve=x=y=z=0;}

    void project(int camX,int camY,int camZ)
    {
        scale = camD/(z-camZ);
        X = (1 + scale*(x - camX)) * width/2;
        Y = (1 - scale*(y - camY)) * height/2;
        W = scale * roadW  * width/2;
    }

    void drawSprite(RenderWindow &app)
    {
        Sprite s = sprite;
        int w = s.getTextureRect().width;
        int h = s.getTextureRect().height;

        float destX = X + scale * spriteX * width/2;
        float destY = Y + 4;
        float destW  = w * W / 266;
        float destH  = h * W / 266;

        destX += destW * spriteX; //offsetX
        destY += destH * (-1);    //offsetY

        float clipH = destY+destH-clip;
        if (clipH<0) clipH=0;

        if (clipH>=destH) return;
        s.setTextureRect(IntRect(0,0,w,h-h*clipH/destH));
        s.setScale(destW/w,destH/h);
        s.setPosition(destX, destY);
        app.draw(s);
    }
};



int main()
{
    RenderWindow app(VideoMode(width, height), "2D Level");
    app.setFramerateLimit(60);

    Texture bg;
    bg.loadFromFile("../images/bg.png");
    bg.setRepeated(true);
    Sprite sBackground(bg);
    sBackground.setTextureRect(IntRect(0,0,5000,2000));
    sBackground.setPosition(-2000,0);

    float playerX = 0;

    ImGui::SFML::Init(app);
    sf::Clock deltaClock;

    while (app.isOpen())
    {
        Event e;
        
        while (app.pollEvent(e))
        {
            ImGui::SFML::ProcessEvent(e);
            if (e.type == Event::Closed)
                app.close();
        }
        ImGui::SFML::Update(app, deltaClock.restart());

        ImGui::Begin("A");
        ImGui::Text("A");
        ImGui::End();

        int speed=0;
        int H = 0;

        if (Keyboard::isKeyPressed(Keyboard::Right)) playerX+=0.1;
        if (Keyboard::isKeyPressed(Keyboard::Left)) playerX-=0.1;
        if (Keyboard::isKeyPressed(Keyboard::Up)) speed=200;
        if (Keyboard::isKeyPressed(Keyboard::Down)) speed=-200;
        if (Keyboard::isKeyPressed(Keyboard::LShift)) speed*=3;
        if (Keyboard::isKeyPressed(Keyboard::W)) H+=100;
        if (Keyboard::isKeyPressed(Keyboard::S)) H-=100;

        


       
        
        app.clear(Color(105, 205, 4));            
        app.draw(sBackground);
        Color color(0, 154, 200);
        drawQuad(app, color, 100, 100, 200, 200); //x, y
        drawGrid(app);
        ImGui::SFML::Render(app);
        app.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}