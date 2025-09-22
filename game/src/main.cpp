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
float x = 500;
float y = 500;
float frequency = 1;
float amplitude = 100;

float speed = 100;
float angle = 30;

void Update()
{
	dt = 1.0f / TARGET_FPS;
    time += dt;
	x = x + (-sin(time * frequency)) * frequency * amplitude * dt;
	y = y + (cos(time * frequency)) * frequency * amplitude * dt;
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

    //Drawing the Line
    Vector2 startPos = { 200, GetScreenHeight() - 200 };
	Vector2 velocity = { cos(angle * DEG2RAD) * speed, -sin(angle * DEG2RAD) * speed};

	DrawLineEx(startPos, startPos + velocity, 3, BLUE);

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