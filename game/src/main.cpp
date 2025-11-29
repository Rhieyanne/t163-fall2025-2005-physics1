#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
#include <vector>
#include <string>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    ________          _____             .__   __   
//    \______ \   _____/ ____\____   __ __|  |_/  |_ 
//     |    |  \_/ __ \   __\\__  \ |  |  \  |\   __\
//     |    `   \  ___/|  |   / __ \|  |  /  |_|  |  
//    /_______  /\___  >__|  (____  /____/|____/__|  
//            \/     \/           \/                 

// Frame rate and time variables
const unsigned int TARGET_FPS = 60;
float time;
float dt;

// User-controlled parameters
float speed = 0;
float angle = 30;
float positionX = 200;
float positionY = 200;
float coefficientofFriction = 0.5f;
float spawnMass = 1.0f;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    __________.__                 .__         ________ ___.        __               __          
//    \______   \  |__ ___.__. _____|__| ____   \_____  \\_ |__     |__| ____   _____/  |_  ______
//     |     ___/  |  <   |  |/  ___/  |/ ___\   /   |   \| __ \    |  |/ __ \_/ ___\   __\/  ___/
//     |    |   |   Y  \___  |\___ \|  \  \___  /    |    \ \_\ \   |  \  ___/\  \___|  |  \___ \ 
//     |____|   |___|  / ____/____  >__|\___  > \_______  /___  /\__|  |\___  >\___  >__| /____  >
//                   \/\/         \/        \/          \/    \/\______|    \/     \/          \/ 

// Note: Polymorphism
/* We can use void draw() override, or we can use virtual void draw() = 0; pure virtual function
   Overriding keyword makes sure we are actually overriding a base class function
   if you are not, the compiler will throw an error
   [Virtual on Main class], override on derived class */

   // Declare what type of object
enum ObjectType
{
	CIRCLE,
	HALFSPACE
};

// Parent class for physics objects
struct physicObject
{
	// Physic Object Variables
	Vector2 position; // In pixels
	Vector2 velocity; // In pixels per second
	Vector2 netForce = { 0,0 }; // In Newtons (kg*m/s^2)
	float mass = 1.0f;
	float drag = 0.1f;
	float grip = 0.5f; // Coefficient of friction for object
	string name;
	Color color;
	bool isStatic = false; // If true, object will not move or be affected by forces

	// Functions
	virtual void draw()  // Virtual Draw function |  Virtual keyword is required to allow this function to be overridden
	{
		DrawCircleV(position, 10, color);
		DrawText(name.c_str(), position.x, position.y, 20, LIGHTGRAY);
		DrawLineEx(position, position + velocity, 1, color);
	}

	virtual ObjectType Shape() = 0; // Pure virtual function to make PhysicObject an abstract class
	//It is a declaration that has no definition, and it forces derived classes to provide an implementation for this function.
};

// Circle class derived from PhysicObject
class physicsCircle : public physicObject
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

//Halfspace class derived from PhysicObject

class physicsHalfspace : public physicObject
{
private:
	float rotation = 0;
	Vector2 normal = { 0,-1 }; //normal vector represents the direction perpendicular to the surface, pointing away from the halfspace
	// We always keep normal vectors at a magnitude of 1, so they denote orientation, but no magnitude
public:

	// Functions | Setters and Getters
	void setRotation(float degrees) { rotation = degrees; normal = Vector2Rotate({ 0,-1 }, rotation * DEG2RAD); } // Set rotation and update normal vector based on rotation
	float getRotation() { return rotation; }
	Vector2 getNormal() { return normal; }

	void draw() override
	{
		DrawCircle(position.x, position.y, 8, RED); // Draw arbitrary line based on position and rotation
		DrawLineEx(position, position + normal * 30, 1, RED); // Draw normal vector
		Vector2 parrellelToSurface = Vector2Rotate(normal, PI * 0.5f);// Rotate function, takes radians. 360 degrees = 2PI radians
		DrawLineEx(position - parrellelToSurface * 4000, position + parrellelToSurface * 4000, 1, RED); // Draw surface line
	}

	ObjectType Shape() override
	{
		return HALFSPACE;
	}
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//     __      __            .__       .___
//    /  \    /  \___________|  |    __| _/
//    \   \/\/   /  _ \_  __ \  |   / __ | 
//     \        (  <_> )  | \/  |__/ /_/ | 
//      \__/\  / \____/|__|  |____/\____ | 
//           \/                         \/ 

// Linker functions for collision responses, will be defined later on, just have the declarations here as a placeholder
bool CircleCircleCollisionResponse(physicsCircle* circleA, physicsCircle* circleB);
bool CircleHalfspaceCollisionResponse(physicsCircle* circle, physicsHalfspace* halfspace);

// Physics World class
class physicsWorld {
private:
	unsigned int objCount = 0;
public:

	// Variables for physics world
	Vector2 gravityAcceleration; // Gravity acceleration vector
	vector<physicObject*> objects; // All objects in physics world
	vector<physicsCircle> circles; // Store circles separately for easy access

	// Functions

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//        _      _    _       _     _        _   
	//       /_\  __| |__| |  ___| |__ (_)___ __| |_ 
	//      / _ \/ _` / _` | / _ \ '_ \| / -_) _|  _|
	//     /_/ \_\__,_\__,_| \___/_.__// \___\__|\__|
	//                               |__/            
	// Add object to physics world
	void addObject(physicObject* obj) {
		obj->name = to_string(objCount);
		objects.push_back(obj);
		objCount++;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                      _   _  _     _    __                   
	//      _ _ ___ ___ ___| |_| \| |___| |_ / _|___ _ _ __ ___ ___
	//     | '_/ -_|_-</ -_)  _| .` / -_)  _|  _/ _ \ '_/ _/ -_|_-<
	//     |_| \___/__/\___|\__|_|\_\___|\__|_| \___/_| \__\___/__/
	//                                                             
	// Reset net forces on all objects
	void resetNetForces() {
		for (auto& obj : objects) {
			obj->netForce = { 0,0 };
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//              _    _  ___              _ _        ___               
	//      __ _ __| |__| |/ __|_ _ __ ___ _(_) |_ _  _| __|__ _ _ __ ___ 
	//     / _` / _` / _` | (_ | '_/ _` \ V / |  _| || | _/ _ \ '_/ _/ -_)
	//     \__,_\__,_\__,_|\___|_| \__,_|\_/|_|\__|\_, |_|\___/_| \__\___|
	//                                             |__/     
	// Add gravity force to all objects             
	void addGravityForce()
	{
		for (auto* obj : objects) {
			if (obj->isStatic) continue; // Avoid modifying static objects, if static, skip to next object
			Vector2 gravityForce = gravityAcceleration * obj->mass; // F = m * a
			obj->netForce += gravityForce; // Add gravity force to net force
			DrawLineEx(obj->position, obj->position + gravityForce, 1, PURPLE); // Draw gravity force vector
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                     _      _  ___                     _   _       
	//      __ _ _ __ _ __| |_  _| |/ (_)_ _  ___ _ __  __ _| |_(_)__ ___
	//     / _` | '_ \ '_ \ | || | ' <| | ' \/ -_) '  \/ _` |  _| / _(_-<
	//     \__,_| .__/ .__/_|\_, |_|\_\_|_||_\___|_|_|_\__,_|\__|_\__/__/
	//          |_|  |_|     |__/                                        
	// Apply kinematics to all objects
	void applyKinematics()
	{
		for (auto* obj : objects) {
			if (obj->isStatic) continue; // Avoid modifying static objects, if static, skip to next object
			obj->position = obj->position + obj->velocity * dt; // Velocity = change in position over time p/t, therefore change in position = velocity * time
			Vector2 acceleration = obj->netForce / obj->mass; // F = ma, so a = F/m where F is net force on an object
			obj->velocity += acceleration * dt; // gravityAcceleration = deltaV / time, therefore deltaV = gravityAcceleration * time
			DrawLineEx(obj->position, obj->position + obj->netForce, 1, GRAY); // Draw net force vector
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//         _           _    ___     _ _ _              
	//      __| |_  ___ __| |__/ __|___| | (_)___ ___ _ _  
	//     / _| ' \/ -_) _| / / (__/ _ \ | | (_-</ _ \ ' \ 
	//     \__|_||_\___\__|_\_\\___\___/_|_|_/__/\___/_||_|
	//                                                     
	// Check for collisions between objects
	void checkCollision()
	{
		vector<bool> collided(objects.size(), false); // Track which objects have collided
		for (int i = 0; i < objects.size(); i++) {
			for (int j = i + 1; j < objects.size(); j++) { // Start checking from the next object, no need to check previous objects again

				// Pointers to objects
				physicObject* objpointerA = objects[i];
				physicObject* objpointerB = objects[j];
				bool didCollide = false;

				// Ask Objects what shape they are
				ObjectType shapeofA = objpointerA->Shape();
				ObjectType shapeofB = objpointerB->Shape();

				// Call appropriate collision response function based on object shapes
				if (shapeofA == CIRCLE && shapeofB == CIRCLE)
				{
					didCollide = CircleCircleCollisionResponse((physicsCircle*)objpointerA, (physicsCircle*)objpointerB);
				}
				else if (shapeofA == CIRCLE && shapeofB == HALFSPACE)
				{
					didCollide = CircleHalfspaceCollisionResponse((physicsCircle*)objpointerA, (physicsHalfspace*)objpointerB);
				}
				else if (shapeofA == HALFSPACE && shapeofB == CIRCLE)
				{
					didCollide = CircleHalfspaceCollisionResponse((physicsCircle*)objpointerB, (physicsHalfspace*)objpointerA);
				}

				// Mark objects as collided if a collision occurred
				if (didCollide)
				{
					collided[i] = true;
					collided[j] = true;
				}
			}
		}

		// Update object colors based on collision status
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

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                    _      _        ___  _     _        _   
	//      _  _ _ __  __| |__ _| |_ ___ / _ \| |__ (_)___ __| |_ 
	//     | || | '_ \/ _` / _` |  _/ -_) (_) | '_ \| / -_) _|  _|
	//      \_,_| .__/\__,_\__,_|\__\___|\___/|_.__// \___\__|\__|
	//          |_|                               |__/ 
	// Updatephysics world for one time step, order of operations matters          
	void updateObject() {
		resetNetForces(); // Set net force variable to 0, [physicObject.netForce] tracks all forces applying to it in one frame
		addGravityForce(); // Add Gravity Force
		checkCollision(); // Apply collision detection and response, add Normal force if applicable
		applyKinematics(); // Accerates and moves objects according to a = F/m and kinematics equations
	}
};

// Define global physics world
physicsWorld world;

// Define global halfspace
physicsHalfspace halfspace;
physicsHalfspace halfspace_2;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    _________        .__  .__  .__                   __________                                                    ___________                   __  .__                      
//    \_   ___ \  ____ |  | |  | |__| __________   ____\______   \ ____   ____________   ____   ____   ______ ____   \_   _____/_ __  ____   _____/  |_|__| ____   ____   ______
//    /    \  \/ /  _ \|  | |  | |  |/  ___/  _ \ /    \|       _// __ \ /  ___/\____ \ /  _ \ /    \ /  ___// __ \   |    __)|  |  \/    \_/ ___\   __\  |/  _ \ /    \ /  ___/
//    \     \___(  <_> )  |_|  |_|  |\___ (  <_> )   |  \    |   \  ___/ \___ \ |  |_> >  <_> )   |  \\___ \\  ___/   |     \ |  |  /   |  \  \___|  | |  (  <_> )   |  \\___ \ 
//     \______  /\____/|____/____/__/____  >____/|___|  /____|_  /\___  >____  >|   __/ \____/|___|  /____  >\___  >  \___  / |____/|___|  /\___  >__| |__|\____/|___|  /____  >
//            \/                         \/           \/       \/     \/     \/ |__|               \/     \/     \/       \/             \/     \/                    \/     \/     




	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//       ___ _        _      ___ _        _      ___     _ _ _    _          ___                               
	//      / __(_)_ _ __| |___ / __(_)_ _ __| |___ / __|___| | (_)__(_)___ _ _ | _ \___ ____ __  ___ _ _  ___ ___ 
	//     | (__| | '_/ _| / -_) (__| | '_/ _| / -_) (__/ _ \ | | (_-< / _ \ ' \|   / -_|_-< '_ \/ _ \ ' \(_-</ -_)
	//      \___|_|_| \__|_\___|\___|_|_| \__|_\___|\___\___/_|_|_/__/_\___/_||_|_|_\___/__/ .__/\___/_||_/__/\___|
	//                                                                                     |_| 
	//                                         Circle-Circle Collision Response                   
bool CircleCircleCollisionResponse(physicsCircle* circleA, physicsCircle* circleB)
{
	Vector2 displacementFromAtoB = Vector2Subtract(circleB->position, circleA->position); // Same thing as circleB.position - circleA.position
	float distance = Vector2Length(displacementFromAtoB); // Use pythagorean thoreom to get magnitude of displacement vector betwen circles
	float sumOfRadii = circleA->radius + circleB->radius;
	float overlap = sumOfRadii - distance; // Sum of radii = 5, distance = 10, overlap = -5 (no overlap)


	if (overlap > 0) // if overlap is positive, we have collision
	{
		Vector2 normal = displacementFromAtoB / distance; // Normalize displacement vector to get collision normal
		Vector2 mtv = normal * overlap; // minimum translation vector (to move objects out of collision)
		circleA->position -= mtv * 0.5f;
		circleB->position += mtv * 0.5f;
		return true; // Overlapping
	}
	else
		return false; // Not overlapping
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//       ___ _        _     _  _      _  __                       ___     _ _ _    _          ___                               
//      / __(_)_ _ __| |___| || |__ _| |/ _|____ __  __ _ __ ___ / __|___| | (_)__(_)___ _ _ | _ \___ ____ __  ___ _ _  ___ ___ 
//     | (__| | '_/ _| / -_) __ / _` | |  _(_-< '_ \/ _` / _/ -_) (__/ _ \ | | (_-< / _ \ ' \|   / -_|_-< '_ \/ _ \ ' \(_-</ -_)
//      \___|_|_| \__|_\___|_||_\__,_|_|_| /__/ .__/\__,_\__\___|\___\___/_|_|_/__/_\___/_||_|_|_\___/__/ .__/\___/_||_/__/\___|
//                                            |_|                                                       |_|                     
//										Circle-Halfspace Collision Response
bool CircleHalfspaceCollisionResponse(physicsCircle* circle, physicsHalfspace* halfspace)
{
	Vector2 displacementFromHalfspaceToCircle = Vector2Subtract(circle->position, halfspace->position); // Same thing as circleB.position - circleA.position
	float dot = Vector2DotProduct(displacementFromHalfspaceToCircle, halfspace->getNormal());
	Vector2 projectionDisplacementOntoNormal = halfspace->getNormal() * dot; // If normal is already normalized, we dont need to do the whole equation for vector projection

	//DRAW LINE FROM CIRCLE TO HALFSPACE
	//DrawLineEx(circle->position, circle->position - projectionDisplacementOntoNormal, 1, GRAY);
	Vector2 midpoint = circle->position - projectionDisplacementOntoNormal * 0.5f;
	//DRAW DISTANCE TEXT
	//DrawText(TextFormat("Dist: %6.0f", dot), midpoint.x, midpoint.y, 30, GRAY); 

	float overlap = circle->radius - dot;// Sum of radii = 5, distance = 10, overlap = -5 (no overlap)
	if (overlap > 0) // if overlap is positive, we have collision
	{
		Vector2 mtv = halfspace->getNormal() * overlap; // minimum translation vector (to move objects out of collision)
		circle->position += mtv;

		//Get Gravity Force
		Vector2 Fgravity = world.gravityAcceleration * circle->mass;

		//Apply Normal Force
		Vector2 FgPerp = halfspace->getNormal() * Vector2DotProduct(Fgravity, halfspace->getNormal());
		Vector2 Fnormal = FgPerp * -1;
		circle->netForce += Fnormal;

		DrawLineEx(circle->position, circle->position + Fnormal, 1, GREEN);

		//Friction
		//F = uN where is coefficient of friction between two surfaces;
		//F is the max magnitude of force of friction
		//N is magnitude of normal force.
		float u = circle->grip * halfspace->grip;
		float frictionMagnitude = Vector2Length(Fnormal) * u;

		//the direction of friction = opposite other applied forces in the surface plane
		Vector2 FgPara = Fgravity - FgPerp;
		Vector2 FrictionDirection = Vector2Normalize(FgPara) * -1;
		//Vector2 Ffriciton = FrictionDirection * frictionMagnitude;
		// We have to clamp friction to not exceed the force trying to move the object
		float maxFriction = Vector2Length(FgPara);
		if (frictionMagnitude > maxFriction) frictionMagnitude = maxFriction;
		Vector2 Ffriciton = FrictionDirection * frictionMagnitude;

		circle->netForce += Ffriciton;
		DrawLineEx(circle->position, circle->position + Ffriciton, 2, ORANGE);
		return true; // Overlapping
	}
	else
		return false; // Not overlapping
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    ________ __________    _____  __      __       /\   ____ _____________________      ___________________________      /\  _________ .____     ___________   _____    _______   ____ _____________ 
//    \______ \\______   \  /  _  \/  \    /  \     / /  |    |   \______   \______ \    /  _  \__    ___/\_   _____/     / /  \_   ___ \|    |    \_   _____/  /  _  \   \      \ |    |   \______   \
//     |    |  \|       _/ /  /_\  \   \/\/   /    / /   |    |   /|     ___/|    |  \  /  /_\  \|    |    |    __)_     / /   /    \  \/|    |     |    __)_  /  /_\  \  /   |   \|    |   /|     ___/
//     |    `   \    |   \/    |    \        /    / /    |    |  / |    |    |    `   \/    |    \    |    |        \   / /    \     \___|    |___  |        \/    |    \/    |    \    |  / |    |    
//    /_______  /____|_  /\____|__  /\__/\  /    / /     |______/  |____|   /_______  /\____|__  /____|   /_______  /  / /      \______  /_______ \/_______  /\____|__  /\____|__  /______/  |____|    
//            \/       \/         \/      \/     \/                                 \/         \/                 \/   \/              \/        \/        \/         \/         \/                    


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//         _                      __      __       _    _ 
	//      __| |___ __ _ _ _ _  _ _ _\ \    / /__ _ _| |__| |
	//     / _| / -_) _` | ' \ || | '_ \ \/\/ / _ \ '_| / _` |
	//     \__|_\___\__,_|_||_\_,_| .__/\_/\_/\___/_| |_\__,_|
	//                            |_|                         
	// Cleanup world by removing objects that are out of bounds
void cleanupWorld() {
	for (int i = 0; i < world.objects.size(); i++) {
		physicObject* obj = world.objects[i];
		if (obj->position.y > GetScreenHeight()
			|| obj->position.y < 0
			|| obj->position.x > GetScreenWidth()
			|| obj->position.x < 0) {
			auto iterator = world.objects.begin() + i;
			physicObject* pointerTopMain = *iterator;
			delete pointerTopMain; // Free memory
			world.objects.erase(iterator); // Remove from vector
			i--; // Adjust index after erasing
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//                    _      _       
//      _  _ _ __  __| |__ _| |_ ___ 
//     | || | '_ \/ _` / _` |  _/ -_)
//      \_,_| .__/\__,_\__,_|\__\___|
//          |_|       
// Update physics world 
void update()
{
	dt = 1.0f / TARGET_FPS;
	time += dt;

	cleanupWorld();
	world.updateObject();
	//if (IsKeyPressed(KEY_SPACE))
	//{
	//	physicsCircle* newCircle = new physicsCircle(); // New keyword allocates memory on the heap (as opposed to the stack, where the data will be lost on exisiting scope)
	//	newCircle->position = { positionX, GetScreenHeight() - positionY };
	//	newCircle->velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };
	//	newCircle->radius = (float)(rand() % 20 + 10);
	//	//newCircle->color = { static_cast<unsigned char>(rand() % 256),static_cast<unsigned char>(rand() % 256),static_cast<unsigned char>(rand() % 256),255 };
	//	world.addObject(newCircle);
	//}

	if (IsKeyPressed(KEY_SPACE))
	{
		physicsCircle* newCircle = new physicsCircle();

		// POSITION & VELOCITY
		newCircle->position = { positionX, GetScreenHeight() - positionY };
		newCircle->velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };

		// RADIUS
		newCircle->radius = 20;
		// MASS
		newCircle->mass = spawnMass;
		world.addObject(newCircle);
	}


	if (IsKeyDown(KEY_C))
	{
		physicsCircle* newCircle = new physicsCircle(); // New keyword allocates memory on the heap (as opposed to the stack, where the data will be lost on exisiting scope)
		newCircle->position = { positionX, GetScreenHeight() - positionY };
		newCircle->velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };
		newCircle->radius = (float)(rand() % 20 + 10);
		//newCircle->color = { static_cast<unsigned char>(rand() % 256),static_cast<unsigned char>(rand() % 256),static_cast<unsigned char>(rand() % 256),255 };
		world.addObject(newCircle);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//      ___                  
//     |   \ _ _ __ ___ __ __
//     | |) | '_/ _` \ V  V /
//     |___/|_| \__,_|\_/\_/ 
//
//  Draw physics world objects 
void Draw()
{
	// [STEP 1: BEGIN DRAWING AND INITIALIZE THE SCREEN]
	BeginDrawing();
	ClearBackground(BLACK);
	DrawText("Rhieyanne Fajardo: 101554981", 10, GetScreenHeight() - 20 - 10, 20, WHITE);
	DrawText(TextFormat("FPS: %02i", GetFPS()), 10, 10, 20, LIME);

	// [STEP 2: ADJUST AND CONFIGURE]

	// GUI slider bars

	// Controls for ball launch
	GuiSliderBar(Rectangle{ 10, 40, 200, 20 }, "", TextFormat("Speed: %.0f", speed), &speed, -100, 1000);
	GuiSliderBar(Rectangle{ 10, 60, 200, 20 }, "", TextFormat("Angle: %.0f", angle), &angle, -180, 180);
	GuiSliderBar(Rectangle{ 10, 80, 200, 20 }, "", TextFormat("Position X: %.0f", positionX), &positionX, 0, 300);
	GuiSliderBar(Rectangle{ 10, 100, 200, 20 }, "", TextFormat("Position Y: %.0f", positionY), &positionY, 0, 300);
	GuiSliderBar(Rectangle{ 10, 140, 200, 20 }, "", TextFormat("Gravity Acceleration: %.0f", world.gravityAcceleration.y), &world.gravityAcceleration.y, -300, 300);

	// Controls for halfspace
	GuiSliderBar(Rectangle{ 80, 160, 240, 20 }, "HalfspaceX", TextFormat("%.0f", halfspace.position.x), &halfspace.position.x, 0, GetScreenWidth());
	GuiSliderBar(Rectangle{ 380, 160, 240, 20 }, "HalfspaceY", TextFormat("%.0f", halfspace.position.y), &halfspace.position.y, 0, GetScreenHeight());
	float halfspaceRotation = halfspace.getRotation();
	GuiSliderBar(Rectangle{ 780, 160, 100, 20 }, "Halfspace Rotation", TextFormat("%.0f", halfspace.getRotation()), &halfspaceRotation, -180, 180);
	halfspace.setRotation(halfspaceRotation);

	// Control for Friction
	//GuiSliderBar(Rectangle{ 1100, 160, 200, 20 }, "Coefficient of Friction", TextFormat("%.2f", coefficientofFriction), &coefficientofFriction, 0.0f, 1.0f);
	GuiSliderBar(Rectangle{ 1100, 190, 200, 20 }, "Mass", TextFormat("%.2f", spawnMass), &spawnMass, 0.1f, 10.0f);
	GuiSliderBar(Rectangle{ 1100, 220, 200, 20 }, "Grip | slippery-grippy", TextFormat("%.2f", halfspace.grip), &halfspace.grip, 0.0f, 1.0f);

	

	// [STEP 3: SIMULATION AND DRAWING BALL AND LINE]
	// Drawing the Line
	Vector2 startPos = { positionX, GetScreenHeight() - positionY };
	Vector2 velocity = { (float)cos(angle * DEG2RAD) * speed, (float)-sin(angle * DEG2RAD) * speed };
	DrawLineEx(startPos, startPos + velocity, 3, RED);

	// Drawing all objects in the world with their own draw function
	/* Through the magic of polymorphism, we can place multiple types of objects in world.objects. [Circle, Box, Halfspace, etc...]
	   Then we call the parent function draw(), we should get the derived class behavior specific to what the object actually is.
	   Example, physicsCircle.draw() should call the Circle draw function, physicsHalfspace.draw() should call the halfspace draw function
	*/
	for (int i = 0; i < world.objects.size(); i++)
	{
		physicObject* obj = world.objects[i];
		obj->draw();
	}

	halfspace.draw();

	// Old Drawing Functions
	/* void DrawCircleV(Vector2 center, float radius, Color color); // Draw a color-filled circle (Vector version)
		for (auto& ball : sim.balls) {
		DrawCircleV(ball.position, ball.radius, ball.randomColor);
		DrawText(ball.name.c_str(), ball.position.x, ball.position.y , ball.radius * 2, LIGHTGRAY);
		DrawLineEx(ball.position, ball.position + ball.velocity, 1, ball.randomColor);
	*/

	// DRAW FBD
	/*Vector2 location = {300,800};
	DrawCircleLines(location.x, location.y, 50, WHITE);
	float mass = 0;
	//Draw Gravity
	Vector2 Fgravity = sim.gravityAcceleration * mass;
	DrawLine(location.x, location.y, location.x + Fgravity.x, location.y + Fgravity.y, PURPLE);
	//Draw Normal Force
	Vector2 FgPerp = halfspace.getNormal() * Vector2DotProduct(Fgravity, halfspace.getNormal());
	Vector2 Fnormal = FgPerp * -1;
	DrawLine(location.x, location.y, location.x + Fnormal.x, location.y + Fnormal.y, YELLOW);
	//Draw Friction Force
	Vector2 FgPara = Fgravity - FgPerp;
	Vector2 Ffriction = FgPara * -1;
	DrawLine(location.x, location.y, location.x + Ffriction.x, location.y + Ffriction.y, ORANGE);
	*/

	//STEP4: END DRAWING
	EndDrawing();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//       _____         .__         ___________                   __  .__               
//      /     \ _____  |__| ____   \_   _____/_ __  ____   _____/  |_|__| ____   ____  
//     /  \ /  \\__  \ |  |/    \   |    __)|  |  \/    \_/ ___\   __\  |/  _ \ /    \ 
//    /    Y    \/ __ \|  |   |  \  |     \ |  |  /   |  \  \___|  | |  (  <_> )   |  \
//    \____|__  (____  /__|___|  /  \___  / |____/|___|  /\___  >__| |__|\____/|___|  /
//            \/     \/        \/       \/             \/     \/                    \/ 
	// Main Function
int main() {
	InitWindow(InitialWidth, InitialHeight, "Rhieyanne-Fajardo-101554981");
	SetTargetFPS(TARGET_FPS);
	halfspace.position = { 500, 900 };
	halfspace.isStatic = true;
	world.addObject(&halfspace); // Add halfspace to simulation for drawing only

	//halfspace2.isStatic = true;
	//halfspace2.position = { 600, 900 };
	//halfspace2.setRotation(-20);
	//sim.addObject(&halfspace2);

	while (!WindowShouldClose()) {
		update();
		Draw();
	}
	CloseWindow();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////