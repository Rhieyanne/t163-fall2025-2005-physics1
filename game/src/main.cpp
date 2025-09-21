#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
// https://www.raylib.com/cheatsheet/cheatsheet.html


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
}
