#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
// https://www.raylib.com/cheatsheet/cheatsheet.html

struct PhysicsBody { //Declare PhysicsBody
    Vector2 position; // Declare member types
    Vector2 velocity;
    float drag;
    float mass;

    PhysicsBody(Vector2 position = { 0, 0 }, Vector2 velocity = { 0, 0 }, float drag = 0.1f, float mass = 1.0f): position(position), velocity(velocity), drag(drag), mass(mass) {
    }
};

/*class PhysicsSim {
    float dt = 1.0f / TARGET_FPS;
    float time;
    float gravity
};*/

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
	 dt = 1.0f / TARGET_FPS;
    time += dt;
    if (IsKeyPressed(KEY_SPACE))
    {
        //Drawing the Line
        position = { positionX, (float)GetScreenHeight() - positionY };
        velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };
    }

    //Velocity changes positiions over time. 
	// Velocity = dispalcement / time
	// Displacement = velocity * time 
	position += velocity * dt;
    
	//Acceleration changes velocity over time.
	// accel = delta velocity / time
	// delta velocity = accel * time
	velocity += gravityAcceleration * dt;
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
    GuiSliderBar(Rectangle{ 10, 140, 200, 20 }, "", TextFormat("Gravity Acceleration: %.0f", gravityAcceleration.y), &gravityAcceleration.y, -300, 300);

    //Drawing the Line
    Vector2 startPos = { positionX, GetScreenHeight() - positionY };
    Vector2 velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };

	//Drawing the Circle
	DrawCircle(position.x, position.y, 15, RED);

	DrawLineEx(startPos, startPos + velocity, 3, RED);

	EndDrawing();
}


int main()
{
	PhysicsBody body;


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