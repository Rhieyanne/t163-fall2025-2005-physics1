#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
#include <vector>
using namespace std;
// https://www.raylib.com/cheatsheet/cheatsheet.html

// Default Global Values
const unsigned int TARGET_FPS = 60;
float speed = 100;
float angle = 30;
float positionX = 200;
float positionY = 200;

// PhysicsBody Struct
struct PhysicsBody { 
    Vector2 position;
    Vector2 velocity;
    float drag;
    float mass;

    PhysicsBody(Vector2 position = { 0, 0 }, Vector2 velocity = { 0, 0 }, float drag = 0.1f, float mass = 1.0f) : position(position), velocity(velocity), drag(drag), mass(mass) {}
};

// PhysicsSim Class

class PhysicsSim {
public:
    float dt = 1.0f / TARGET_FPS;
    float time;
    Vector2 gravityAcceleration;
    vector<PhysicsBody> balls;


    PhysicsSim(float dt, float time, Vector2 gravityAcceleration = { 0, 0 }) : dt(dt), time(time), gravityAcceleration(gravityAcceleration) {}
    // FUNCTIONS

    //ADD BALL
    void addBall(const PhysicsBody& ball) { balls.push_back(ball); }
    //CLEAR BALL
    void clearBalls() { balls.clear(); }
    //UPDATE BALL
    void UpdateBall() {
        for (auto& ball : balls) {
            //Acceleration changes velocity over time.
            // accel = delta velocity / time
            // delta velocity = accel * time
            // OG CODE: velocity += gravityAcceleration * dt;

            ball.velocity.x += gravityAcceleration.x * dt;
            ball.velocity.y += gravityAcceleration.y * dt;

            //Velocity changes positiions over time. 
            // Velocity = dispalcement / time
            // Displacement = velocity * time 
            // OG CODE: position += velocity * dt;

            ball.position.x += ball.velocity.x * dt;
            ball.position.y += ball.velocity.y * dt;

          }
        }
    };

// PhysicsSim instance
PhysicsSim sim(1.0f / TARGET_FPS, 0, { 0, 100.0f });

// Spawn Ball Function
void SpawnBall() {
    if (IsKeyPressed(KEY_SPACE)) {
        sim.addBall(PhysicsBody({ positionX, GetScreenHeight() - positionY }, { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed }, 0.1f, 1.0f));
    }
    
	// Clears all balls when C is pressed
    if (IsKeyPressed(KEY_C)) {
        sim.clearBalls();
	}

	// Holding down the space bar continuously spawns balls
    if (IsKeyDown(KEY_SPACE)) {
        sim.addBall(PhysicsBody({ positionX, GetScreenHeight() - positionY }, { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed }, 0.1f, 1.0f));
	}

    // Now to do the angles and different parameters presets
    
	// ANGLE 0 DEGREES
    if (IsKeyPressed(KEY_ONE)) {
        sim.addBall(PhysicsBody({ positionX, GetScreenHeight() - positionY }, { (float)cos(0 * DEG2RAD) * speed, (float)-sin(0 * DEG2RAD) * speed }, 0.1f, 1.0f));
    }

    // ANGLE 45 DEGREES
    if (IsKeyPressed(KEY_TWO)) {
        sim.addBall(PhysicsBody({ positionX, GetScreenHeight() - positionY }, { (float)cos(45 * DEG2RAD) * speed, (float)-sin(45 * DEG2RAD) * speed }, 0.1f, 1.0f));
    }

    // ANGLE 60 DEGREES
    if (IsKeyPressed(KEY_THREE)) {
        sim.addBall(PhysicsBody({ positionX, GetScreenHeight() - positionY }, { (float)cos(60 * DEG2RAD) * speed, (float)-sin(60 * DEG2RAD) * speed }, 0.1f, 1.0f));
    }

	// ANGLE 90 DEGREES
    if (IsKeyPressed(KEY_FOUR)) {
        sim.addBall(PhysicsBody({ positionX, GetScreenHeight() - positionY }, { (float)cos(90 * DEG2RAD) * speed, (float)-sin(90 * DEG2RAD) * speed }, 0.1f, 1.0f));
    }
}

// Draw Function
void Draw()
{
    // STEP1: Begin drawing and initialize the screen
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Rhieyanne Fajardo: 101554981", 10, GetScreenHeight() - 20 - 10, 20, WHITE);
    DrawText(TextFormat("FPS: %02i", GetFPS()), 10, 10, 20, LIME);

    // STEP2: ADJUST AND CONFIGURE


    // GUI slider bars
    GuiSliderBar(Rectangle{ 10, 40, 200, 20 }, "", TextFormat("Speed: %.0f", speed), &speed, -100, 1000);
    GuiSliderBar(Rectangle{ 10, 60, 200, 20 }, "", TextFormat("Angle: %.0f", angle), &angle, -180, 180);
    GuiSliderBar(Rectangle{ 10, 80, 200, 20 }, "", TextFormat("Position X: %.0f", positionX), &positionX, 0, 300);
    GuiSliderBar(Rectangle{ 10, 100, 200, 20 }, "", TextFormat("Position Y: %.0f", positionY), &positionY, 0, 300);
    GuiSliderBar(Rectangle{ 10, 140, 200, 20 }, "", TextFormat("Gravity Acceleration: %.0f", sim.gravityAcceleration.y), &sim.gravityAcceleration.y, -300, 300);

    // STEP3: SIMULATION AND DRAWING BALL AND LINE
    //Drawing the Line
    Vector2 startPos = { positionX, GetScreenHeight() - positionY };
    Vector2 velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };

    //Drawing the Circle
    //void DrawCircleV(Vector2 center, float radius, Color color); // Draw a color-filled circle (Vector version)
    for (auto& ball : sim.balls) {
        DrawCircleV(ball.position, 10, BLUE);
    }

    DrawLineEx(startPos, startPos + velocity, 3, RED);

    //STEP4: END DRAWING
    EndDrawing();
}

// Main Function
int main() {
    InitWindow(InitialWidth, InitialHeight, "Rhieyanne-Fajardo-101554981");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        SpawnBall();
        sim.UpdateBall();
        Draw();
    }

    CloseWindow();
    return 0;
}
