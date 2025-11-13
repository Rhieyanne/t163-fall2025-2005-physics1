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

enum ObjectType
{
    BOX,
    CIRCLE,
    HALFSPACE
};
// Notes on Polymorphism
//We can use void draw() override, or we can use virtual void draw() = 0; pure virtual function
// Overriding keyword makes sure we are actually overriding a base class function
// if you are not, the compiler will throw an error
// [Virtual on Main class], override on derived class
struct pMain // Parent Class
{
	bool isStatic = false; // If true, object will not move or be affected by forces
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
       
	   virtual ObjectType Shape() = 0; // pure virtual function to make pMain an abstract class
	   //It is a declaration that has no definition, and it forces derived classes to provide an implementation for this function.
};


class pBox : public pMain
{
public:
	Vector2 Size; // x width, y height

    ObjectType Shape() override
    {
        return BOX;
    }
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

    ObjectType Shape() override
    {
        return CIRCLE;
	}
};

class pHalfspace : public pMain
{
private:
    float rotation = 0;
    Vector2 normal = { 0,-1 }; //normal vector represents the direction perpendicular to the surface, pointing away from the halfspace
    // We always keep normal vectors at a magnitude of 1, so they donte orrientation, but no magnitude
public:
    void setRotation(float degrees)
    {
        rotation = degrees;
        normal = Vector2Rotate({ 0,-1 }, rotation * DEG2RAD); // Update normal vector based on rotation
    }

    float getRotation()
    {
        return rotation;
    }

    Vector2 getNormal()
    {
        return normal;
    }

    void draw() override
    {
        // Draw arbitrary line based on position and rotation
        DrawCircle(position.x, position.y, 8, RED);
        // Draw normal vector
        DrawLineEx(position, position + normal * 30, 1, RED);

        // Draw surface line
        //Rotate function, takes radians. 360 degrees = 2PI radians
        Vector2 parrellelToSurface = Vector2Rotate(normal, PI * 0.5f);
        DrawLineEx(position - parrellelToSurface * 4000, position + parrellelToSurface * 4000, 1, RED);
    }

    ObjectType Shape() override
    {
        return HALFSPACE;
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

bool CircleCircleOverlapCollision(pCircle* circleA, pCircle* circleB)
{
    Vector2 displacementFromAtoB = Vector2Subtract(circleB->position, circleA->position); // Same thing as circleB.position - circleA.position
    float distance = Vector2Length(displacementFromAtoB); // Use pythagorean thoreom to get magnitude of displacement vector betwen circles
    float sumOfRadii = circleA->radius + circleB->radius;
	float overlap = sumOfRadii - distance;
   

    if (sumOfRadii > distance)
    {
        Vector2 normalAtoB = (displacementFromAtoB / distance);
        Vector2 mtv = normalAtoB * overlap; // Minimum translation vector to separate circles
		circleA->position -= mtv * 0.5f; // Move circle A back by half the overlap
		circleB->position += mtv * 0.5f; // Move circle B forward by half the overlap
        return true; // Overlapping
    }
    else
        return false;
}

// Returns true if the circle overlaps the halfspace, false otherwise
bool CircleHalfspaceOverlap(pCircle* circle, pHalfspace* halfspace)
{
	//Get a displacement vector FROM the arbitrary point on the halfspace TO the center of the circle
    Vector2 displacementFromHalfspaceToCircle = Vector2Subtract(circle->position, halfspace->position);
	// Let D be the DOT PRODUCT of the displacement vector and the halfspace normal vector
	// IF D < 0, the circle is behind the halfspace (no overlap), IF D >= 0, the circle is in front of the halfspace (potential overlap)
    // In other words... return (D < radius)
	// reutrn (Dot(displacement, normal) < radius) 
	float dot = Vector2DotProduct(displacementFromHalfspaceToCircle, halfspace->getNormal());

    //Vector projection Proj a onto b (both are vectors)      
    // a dot b * (b/||b||)
	// Since normal is always normalized, ||b|| = 1
	// If b is already normalized, we dont have to divide by its magnitude
	//                  
	Vector2 projectionDisplacementOntoNormal = halfspace->getNormal() * dot; // If normal is already normalized, we dont need to do the whole equation for vector projection
    //float distance = Vector2Length(displacementFromHalfspaceToCircle);
	DrawLineEx(circle->position, circle->position - projectionDisplacementOntoNormal, 1, GRAY);
    // DRAW LINE FROM CIRCLE TO HALFSPACE
    //DrawLineEx(circle->position, halfspace->position, 1, GRAY);

    Vector2 midpoint = circle->position - projectionDisplacementOntoNormal * 0.5f;
    DrawText(TextFormat("Dist: %6.0f", dot), midpoint.x, midpoint.y, 30, GRAY);
    if (dot < -circle->radius)
        return false;
	return true;
	
}
bool CircleHalfspaceOverlapCollison(pCircle* circle, pHalfspace* halfspace)
{
    Vector2 displacementFromHalfspaceToCircle = Vector2Subtract(circle->position, halfspace->position);
	float dot = Vector2DotProduct(displacementFromHalfspaceToCircle, halfspace->getNormal());
	Vector2 projectionDisplacementOntoNormal = halfspace->getNormal() * dot; // If normal is already normalized, we dont need to do the whole equation for vector projection
	float overlap = circle->radius - dot;
    if (dot < circle->radius)
    {
		Vector2 mtv = halfspace->getNormal() * overlap; // Minimum translation vector to separate circle from halfspace
		circle->position += mtv; // Move circle out of halfspace
        return true;
    }
    else
		return false;
	/*/DrawLineEx(circle->position, circle->position - projectionDisplacementOntoNormal, 1, GRAY);
    // DRAW LINE FROM CIRCLE TO HALFSPACE
    //DrawLineEx(circle->position, halfspace->position, 1, GRAY);

    Vector2 midpoint = circle->position - projectionDisplacementOntoNormal * 0.5f;
    DrawText(TextFormat("Dist: %6.0f", dot), midpoint.x, midpoint.y, 30, GRAY);
    if (dot < -circle->radius)
        return false;
	return true;*/
	
}

//template<int N = 0>struct _ { auto operator()(auto* t, auto* u)const { return Vector2DotProduct(t->position - u->position, u->getNormal()); }template<auto V>auto operator[](V&& v)const { return v; } }; template<class T, T V = T{} > struct __ { static constexpr T value = V; };
//bool C(pCircle* c, pHalfspace* h) { return[c, h](auto d) {return d < c->radius ? (__<bool, 1>::value && (c->position += h->getNormal() * (c->radius - d), 1)) : __<bool>::value; }(_<>{} [_<>{}(c, h)] ); }


// FgPerp + Fgnormal = 0s
//Fnormal = -Fgperp
// FgPara = Fg = FgPerp
// Ffriction = =fgPara 
// Function


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

			if (obj->isStatic) // Avoid modifying static objects
                continue; // Skip static objects
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
        vector<bool> collided(objects.size(), false);
        for (int i = 0; i < objects.size(); i++) {
            for (int j = i + 1; j < objects.size(); j++) { // Start checking from the next object, no need to check previous objects again
                // Pointers to objects
                pMain* objpointerA = objects[i];
                pMain* objpointerB = objects[j];
                bool didCollide = false;
                // Ask Objects what shape they are
                ObjectType shapeofA = objpointerA->Shape();
                ObjectType shapeofB = objpointerB->Shape();
                if (shapeofA == CIRCLE && shapeofB == CIRCLE)
                {
					didCollide = CircleCircleOverlapCollision((pCircle*)objpointerA, (pCircle*)objpointerB);
                }
                else if (shapeofA == CIRCLE && shapeofB == HALFSPACE)
                {
					didCollide = CircleHalfspaceOverlapCollison((pCircle*)objpointerA, (pHalfspace*)objpointerB);
                }
                else if (shapeofA == HALFSPACE && shapeofB == CIRCLE)
                {
                    didCollide = CircleHalfspaceOverlapCollison((pCircle*)objpointerB, (pHalfspace*)objpointerA);
                }

                if (didCollide)
                {
                    collided[i] = true;
                    collided[j] = true;
                }
            }
            // Ask Objects what shape they are
            /*vector<int> collidedObjects(objects.size(), 0);
            for (int i = 0; i < objects.size(); i++) {
                pMain* objpointerA = objects[i];
                ObjectType shapeofA = objpointerA->Shape();
                if (shapeofA != CIRCLE)
                    continue; // Skip non-circle objects for now
                pCircle* circlePointerA = (pCircle*)objpointerA;


                for (int j = i + 1; j < objects.size(); j++) { // Start checking from the next object, no need to check previous objects again

                    pMain* objpointerB = objects[j];
                    ObjectType shapeofB = objpointerB->Shape();
                    if (shapeofB == CIRCLE)
                    {
                        pCircle* circlePointerB = (pCircle*)objpointerB;
                        if (CircleOverlap(circlePointerA, circlePointerB))
                        {
                            collidedObjects[i]++;
                            collidedObjects[j]++;
                        }
                    }
                    else if (shapeofB == HALFSPACE)
                    {
                        pHalfspace* halfspacePointerB = (pHalfspace*)objpointerB;
                        if (CircleHalfspaceOverlap(circlePointerA, halfspacePointerB))
                        {
                            collidedObjects[i]++;
                        }
                    }
                }
                for (int i = 0; i < collidedObjects.size(); i++)
                {
                    pMain* objpointerC = objects[i];
                    ObjectType shapeofC = objpointerC->Shape();
                    if (shapeofC != CIRCLE) continue;
                    pCircle* circlePointerC = (pCircle*)objpointerC;

                    if (collidedObjects[i] > 0)
                        circlePointerC->color = PURPLE;
                    else
                        circlePointerC->color = GREEN;
                }

                // Lets try to implement halfcircle collision

            }*/
        }
        for (int i = 0; i < collided.size(); i++)
        {
            if (collided[i])
            {
                objects[i]->color = RED;
            }
            else
            {
                objects[i]->color = GREEN;
            }
        }
	}
};
pWorld sim;
pHalfspace halfspace;
pHalfspace halfspace2;


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

    if (IsKeyDown(KEY_SPACE))
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

    //Controls for halfspace
    GuiSliderBar(Rectangle{ 80, 160, 240, 20 }, "HalfspaceX", TextFormat("%.0f", halfspace.position.x), &halfspace.position.x, 0, GetScreenWidth());
    GuiSliderBar(Rectangle{ 380, 160, 240, 20 }, "HalfspaceY", TextFormat("%.0f", halfspace.position.y), &halfspace.position.y, 0, GetScreenHeight());

	float halfspaceRotation = halfspace.getRotation();
    GuiSliderBar(Rectangle{ 780, 160, 100, 20 }, "Halfspace Rotation", TextFormat("%.0f", halfspace.getRotation()), &halfspaceRotation, -360,360);
	halfspace.setRotation(halfspaceRotation);


    for (int i = 0; i < sim.objects.size(); i++)
    {
		pMain* obj = sim.objects[i];
        obj->draw();
    }

	halfspace.draw();

	Vector2 CirclePos = { 800, 800 };
	DrawCircleLines(CirclePos.x, CirclePos.y, 50, GREEN);
    float mass = 0;
    //Gravity
    Vector2 ForceGravity = { 100, sim.gravityAcceleration.y * mass };
	DrawLine(CirclePos.x, CirclePos.y, CirclePos.x + ForceGravity.x, CirclePos.y + ForceGravity.y, RED);
	// Normal Force
    // FgPerp
    Vector2 FgPerp = halfspace.getNormal() * Vector2DotProduct(ForceGravity, halfspace.getNormal());
	Vector2 FNormal = FgPerp * -1;
    DrawLine(CirclePos.x, CirclePos.y, CirclePos.x + FNormal.x, CirclePos.y + FNormal.y, GREEN);

	// Friction Force
	Vector2 FgPara = ForceGravity - FgPerp;
	Vector2 Ffrticion = FgPara * -1;
	DrawLine(CirclePos.x, CirclePos.y, CirclePos.x + FgPara.x, CirclePos.y + FgPara.y, ORANGE);
    //STEP4: END DRAWING
    EndDrawing();
}

// Main Function
int main() {
    InitWindow(InitialWidth, InitialHeight, "Rhieyanne-Fajardo-101554981");
    SetTargetFPS(TARGET_FPS);
    halfspace.position = { 500, 900 };
	halfspace.isStatic = true;
    sim.addObject(&halfspace); // Add halfspace to simulation for drawing only
	//halfspace2.position = { 1000, 600 };
	//halfspace2.isStatic = true;
 //   halfspace2.setRotation(-40);
	//sim.addObject(&halfspace2); // Add halfspace to simulation for drawing only

    while (!WindowShouldClose()) {
        update();
        Draw();
    }
    CloseWindow();
    return 0;
}
