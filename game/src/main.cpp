#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
// https://www.raylib.com/cheatsheet/cheatsheet.html


/* THIS IS WEEK 1 CODE

const unsigned int TARGET_FPS = 50;
float time = 0;
float dt;

float x = 500;
float y = 500;
float frequency = 5.0f;
float amplitude = 1;

//Change world state
void update() {

	dt = 1.0 / TARGET_FPS;
	time += dt;
    y = y + (cos(time * frequency)) * frequency * amplitude * dt;
    x = x + (-sin(time * frequency)) * frequency * amplitude * dt;
}

//Display world State
void draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Rhieyanne Fajardo: 101554981", 10, 80, 20, BLUE);
    GuiSliderBar(Rectangle{ 60, 5, 1000, 10 }, "Time", TextFormat("%.2f", time), &time, 0, 240);
	DrawText(TextFormat("FPS: %i, TIME: %.2f", TARGET_FPS, time), GetScreenWidth() - 400, 40, 15, BLUE);
	DrawCircle(x, y, 50, RED);
    DrawCircle(GetScreenWidth()/2 + cos(time * frequency) * amplitude, GetScreenHeight()/2 * sin(time * frequency) * amplitude, 60, GREEN);
    EndDrawing();
}

int main()
{
    InitWindow(InitialWidth, InitialHeight, "Rhieyanne-Fajardo-101554981");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
        update();
		draw();
    }

    CloseWindow();
    return 0;
} */


// THIS IS WEEK 2 CODE
const unsigned int TARGET_FPS = 60;
float dt = 1.0f / TARGET_FPS;
float time;

float speed = 100;
float angle = 30;
float positionX = 200;
float positionY = 200;

Vector2 velocity;
Vector2 position;
Vector2 gravityAcceleration = { 0, 100 };

void Update()
{
	// dt = 1.0f / TARGET_FPS;
    // time += dt;
    /* WORK IN PROGRESSif (IsKeyPressed(KEY_SPACE))
    {
        //Drawing the Line
        position = { positionX, (float)GetScreenHeight() - positionY };
        velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };
    }

    /*position += position + velocity * dt;*/
}
void Draw()
{   
    
	BeginDrawing();
	ClearBackground(BLACK);
    DrawText("Rhieyanne Fajardo: 101554981", 10, GetScreenHeight() - 20 - 10, 20, WHITE);
    DrawText(TextFormat("FPS: %02i", GetFPS()), 10, 10, 20, LIME);

	// GUI slider bars
	GuiSliderBar(Rectangle{ 10, 40, 200, 20 }, "", TextFormat("Speed: %.0f", speed), &speed, -100, 1000);
    GuiSliderBar(Rectangle{ 10, 60, 200, 20 }, "", TextFormat("Angle: %.0f", angle), &angle, -180, 180); 
    GuiSliderBar(Rectangle{ 10, 80, 200, 20 }, "", TextFormat("Position X: %.0f", positionX), &positionX, 0, 300);
    GuiSliderBar(Rectangle{ 10, 100, 200, 20 }, "", TextFormat("Position Y: %.0f", positionY), &positionY, 0, 300);

    //Drawing the Line
    Vector2 startPos = { positionX, GetScreenHeight() - positionY };
    Vector2 velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };

	DrawLineEx(startPos, startPos + velocity, 3, RED);

	EndDrawing();
}


int main()
{
	InitWindow(InitialWidth, InitialHeight, "Rhieyanne-Fajardo-101554981");
    SetTargetFPS(TARGET_FPS);
    while (!WindowShouldClose())
    {
        Update();
        Draw();
    }
	CloseWindow();
	return 0;
}