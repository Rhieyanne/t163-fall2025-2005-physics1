#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
#include <vector>
#include <string>
using namespace std;

// https://www.raylib.com/cheatsheet/cheatsheet.html

// Default Global Values
const unsigned int TARGET_FPS = 60;
float speed = 100;
float angle = 30;
float positionX = 200;
float positionY = 200;
float time;
float dt;

// Notes on Polymorphism
//We can use void draw() override, or we can use virtual void draw() = 0; pure virtual function
// Overriding keyword makes sure we are actually overriding a base class function
// if you are not, the compiler will throw an error
// [Virtual on Main class], override on derived class
struct pMain // Parent Class
{
    // Vector2 
       Vector2 position;
       Vector2 velocity;
    // Float 
       float mass;
       float drag;
	// String 
       string name;
    // Color
	   Color color;

	// Constructor
       pMain(
           // Vector2 
		   Vector2 position = { 0, 0 },
		   Vector2 velocity = { 0, 0 },
		   // Float
		   float mass = 1.0f,
		   float drag = 0.1f,
		   // String
		   string name = "Object",
		   // Color
           Color color = {
               /*[R]*/static_cast<unsigned char>(rand() % 256),
               /*[G]*/static_cast<unsigned char>(rand() % 256),
               /*[B]*/static_cast<unsigned char>(rand() % 256),
               /*[A]*/255
               // https://stackoverflow.com/questions/60580647/narrowing-conversion-from-int-to-unsigned-char
		   }) :
           position(position),
           velocity(velocity),
           mass(mass),
           drag(drag),
           name(name),
		   color(color){}
	   // Virtual Draw function |  Virtual keyword is required to allow this function to be overridden
       virtual void draw()
       {
           DrawCircleV(position, 10, color);
           DrawText(name.c_str(), position.x, position.y, 20, LIGHTGRAY);
		   DrawLineEx(position, position + velocity, 1, color);
       }
       

};


class pBox : public pMain
{
public:
	Vector2 Size; // x width, y height
};

class pCircle : public pMain
{
public:
    float radius; // radius of circle in pixels

    void draw() override // Override the parent draw function
    {
        DrawCircleV(position, radius, color);
        DrawText(name.c_str(), (int)position.x, (int)position.y, (int)(radius * 2), LIGHTGRAY);
        DrawLineEx(position, position + velocity, 1, color);
	}
};
bool CircleOverlap(pCircle* circleA, pCircle* circleB)
{
	Vector2 displacementFromAtoB = Vector2Subtract(circleB->position, circleA->position); // Same thing as circleB.position - circleA.position
    float distance = Vector2Length(displacementFromAtoB); // Use pythagorean thoreom to get magnitude of displacement vector betwen circles
	float sumOfRadii = circleA->radius + circleB->radius;
    if (sumOfRadii > distance)
    {
        return true; // Overlapping
    }
    else
        return false;
}
class pWorld {
private:
    unsigned int objCount = 0;
public:
    // Vector2
    Vector2 gravityAcceleration;
    vector<pMain*> objects; // All objects in physics world
    vector<pBox> boxes;
    vector<pCircle> circles;

    // Constructor
    pWorld(Vector2 gravityAcceleration = { 0, 0 }) : gravityAcceleration(gravityAcceleration) {};

    // Functions
    void addObject(pMain* obj) {
        obj->name = to_string(objCount);
        objects.push_back(obj);
        objCount++;
    }

    void updateObject() {
        for (auto* obj : objects) {
            obj->velocity.x += gravityAcceleration.x * dt;
            obj->velocity.y += gravityAcceleration.y * dt;
            obj->position.x += obj->velocity.x * dt;
            obj->position.y += obj->velocity.y * dt;

            //OG CODE FOR REFERENCE
            //Acceleration changes velocity over time.
            // accel = delta velocity / time
            // delta velocity = accel * time
            // OG CODE: velocity += gravityAcceleration * dt;
            // 
            //Velocity changes positiions over time. 
            // Velocity = dispalcement / time
            // Displacement = velocity * time 
            // OG CODE: position += velocity * dt;
        }
		checkCollision();
    }
	void checkCollision() // Assuming all objects in objects are circles for now
		// Check each object against every other object
    {
		vector<int> collidedObjects(objects.size(), 0); 
        for (int i = 0; i < objects.size(); i++) {
            pMain* objpointerA = objects[i];
            pCircle* circlePointerA = (pCircle*)objpointerA;


            for (int j = i + 1; j < objects.size(); j++) { // Start checking from the next object, no need to check previous objects again

                pMain* objpointerB = objects[j];
                pCircle* circlePointerB = (pCircle*)objpointerB;

                if (CircleOverlap(circlePointerA, circlePointerB))
                {
                    collidedObjects[i]++;
					collidedObjects[j]++;
                }
			}
            for (int i = 0; i < collidedObjects.size(); i++)
            {
                pMain* objpointerC = objects[i];
                pCircle* circlePointerC = (pCircle*)objpointerC;

				if (collidedObjects[i] > 0)
					circlePointerC->color = PURPLE;
				else
					circlePointerC->color = GREEN;
            }
        }
	}
};
pWorld sim;

void cleanupWorld() {
    for (int i = 0; i < sim.objects.size(); i++) {
        pMain* obj = sim.objects[i];
        if (   obj->position.y > GetScreenHeight()
			|| obj->position.y < 0
			|| obj->position.x > GetScreenWidth()
            || obj->position.x < 0 ) {
			auto iterator = sim.objects.begin() + i;
			pMain* pointerTopMain = *iterator;
			delete pointerTopMain; // Free memory
            sim.objects.erase(iterator); // Remove from vector
			--i; // Adjust index after erasing
		}
	}
}

void update() 
{
    dt = 1.0f / TARGET_FPS;
    time += dt;

    cleanupWorld();
    sim.updateObject();
    if (IsKeyPressed(KEY_SPACE))
    {
		pCircle* newCircle = new pCircle(); // New keyword allocates memory on the heap (as opposed to the stack, where the data will be lost on exisiting scope)
		newCircle->position = { positionX, GetScreenHeight() - positionY };
		newCircle->velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };
		newCircle->radius = (float)(rand() % 20 + 10);
        //newCircle->color = { static_cast<unsigned char>(rand() % 256),static_cast<unsigned char>(rand() % 256),static_cast<unsigned char>(rand() % 256),255 };
        sim.addObject(newCircle); 
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
    /*for (auto& ball : sim.balls) {
        DrawCircleV(ball.position, ball.radius, ball.randomColor);
		DrawText(ball.name.c_str(), ball.position.x, ball.position.y , ball.radius * 2, LIGHTGRAY);
		DrawLineEx(ball.position, ball.position + ball.velocity, 1, ball.randomColor);
    }*/ //We want each child to have their own drawing solution
	//Through the magic of polymorphism, we can place multiple types of objects in world.oject (sim.balls) Circle, Box, Halfspace etc
	// then we call the parent function draw(), we should get the derived class behavior specific to what the object actually is
	// example, Circle.draw() should call the Circle draw function, Box.draw() should call the Box draw function
    DrawLineEx(startPos, startPos + velocity, 3, RED);

    for (int i = 0; i < sim.objects.size(); i++)
    {
		pMain* obj = sim.objects[i];
        obj->draw();
    }
    //STEP4: END DRAWING
    EndDrawing();
}

// Main Function
int main() {
    InitWindow(InitialWidth, InitialHeight, "Rhieyanne-Fajardo-101554981");
    SetTargetFPS(TARGET_FPS);
    while (!WindowShouldClose()) {
        update();
        Draw();
    }
    CloseWindow();
    return 0;
}
