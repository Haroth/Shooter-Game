#include <iostream>
#include "SFML\Graphics.hpp"
#include "SFML\Window.hpp"
#include "SFML\System.hpp"
#include <math.h>
#include <vector>
#include <cstdlib>
#include <algorithm> 
#include <limits>
#include <vector>
#include <fstream>

#include <ctime> // clock
#include <ratio>
#include <chrono>


#define CREST_SIZE 120.f
#define FLOOR_HEIGHT 700.f
#define FLOOR_WIDTH 1000.f

#define PLAYER_RADIUS 25.f
#define PLAYER_SPEED 3.f
#define SIGHT_ANGLE 20.f


#define BULLET_SPEED 70.f
#define BULLET_RADIUS 2.f
#define WEAPON_CHANGE 200

#define RIFLE_DAMAGE 20 //35.f
#define RIFLE_SHOOTING_TIME 30

#define SHOTGUN_DAMAGE 100
#define SHOTGUN_SHOOTING_TIME 30
#define SHOTGUN_RANGE 200


using namespace sf;

class Player;
class Rifle;
class Shotgun;
class Bullet;

class Bullet
{
private:
	CircleShape shape;
	Vector2f velocity;
public:
	inline const CircleShape getShape() const { return shape; }
	inline const Vector2f getPosition() const { return shape.getPosition(); }
	inline const Vector2f getVelocity() const { return velocity; }
	inline void setPosition(Vector2f position) { shape.setPosition(position); }
	Bullet(const Vector2f& position, const Vector2f& aimDir, const Color& color);
	~Bullet() {};
};


class Rifle
{
private:
	RectangleShape shape; // texture
	Bullet *bullet; 
	int shootingCounter;
public:
	bool canShoot() const; // checking if shottingCounter is less than or equal to 0 
	void shoot(const Vector2f& aimDir); // shoot bullet if canShoot returns true
	void updateBullet(const RectangleShape &floor, const std::vector <RectangleShape>& obstacles, Player &enemy); // update bullet position according to its velocity vector
	inline void updateCounter() { shootingCounter--; }
	inline const Vector2f getPosition() const { return shape.getPosition(); }
	inline Bullet* const getBullet() const { return bullet; }
	inline void setPosition(const Vector2f& position) { shape.setPosition(position); }
	inline void setRotation(const float &angle) { shape.setRotation(angle); }
	Rifle(const Vector2f& position);
	~Rifle() {};
};


class Shotgun
{
	RectangleShape shape;
	Bullet *bullet;
	int shootingCounter;
public:
	bool canShoot() const; // checking if shottingCounter is less than or equal to 0 
	void shoot(const Vector2f& aimDir); // shoot bullet if canShoot returns true
	void updateBullet(const RectangleShape &floor, const std::vector <RectangleShape>& obstacles, const Player& player, Player &enemy); // update bullet position according to its velocity vector
	inline void updateCounter() { shootingCounter--; }
	inline const Vector2f getPosition() const { return shape.getPosition(); }
	inline Bullet* const getBullet() const { return bullet; }
	inline void setPosition(const Vector2f& position) { shape.setPosition(position); }
	inline void setRotation(const float &angle) { shape.setRotation(angle); }
	Shotgun(const Vector2f &position);
	~Shotgun() {};
};


class Player
{
	// the attributes are numbered according to state of the agent
private:
	int hp; // [13]
	int hpMax;
	Vector2f position; // [1]
	Vector2f playerVelocity;
	Vector2f crosshairPosition; // [2]
	Vector2f aimDir; // [3]
	Vector2f weaponPosition;
	RectangleShape texture;
	Rifle rifle;
	Shotgun shotgun;
	CircleShape shotgunRange;
	int currentWeapon; // [15] 0 - changing weapon, 1 - rifle, 2 - shotgun
	int changingCounter;
	
	
	Vector2f enemyPosition; // [4]
	float enemyDistance; // [5]
	Vector2f enemyDir; // [6]
	bool isEnemyVisible; // [7]
	int lastTimeEnemyVisible; // [8]
	float leftEnemySideVisible; // [9] 1 - full left side visible, 0 - no left side visible
	float rightEnemySideVisible; // [10] 1 - full left side visible, 0 - no left side visible
	Vector2f enemyVelocity; // [11]
	Vector2f enemyAimDir; // [12]
	int enemyHp; // [14]
	
	int enemyCurrentWeapon; // [16]
	
	int lastTimeEnemyShot; // [18]
	int shootingCooldown; // [17]
public:
	void move(const Vector2f& offset); // moving the agent according to the offset
	void setPosition(const Vector2f& position_); // moving the agent to indicated position
	void obstacleCollisionMove(const RectangleShape &square); // checking the collision with crates and moving the agent back
	void enemyCollisionMove(const Vector2f& enemyPosition); // checking the collision with enemy and moving the agent back
	void outOfBoundsMove(const RectangleShape &floor); // checking if the agent is within the floor
	void updateRotation(const Vector2f &crosshairPosition); // updating rotation of the agent

	bool canSee(const Player &enemy, const std::vector <RectangleShape>& obstacles, RenderWindow &window, const Color& color); // returns logic value if the player sees the agemy
	void drawView(const std::vector <RectangleShape>& obstacles, RenderWindow &window, const Color& color); // draws the cone of view
	bool canShoot() const; // checks if currently kept weapon can shoot
	void shoot(); // shoots if canShoot retuns true
	void enemyShot(const Player& enem); // updates attributes about enemy if he shoots
	void updateCounters(Vector2f mousePosition_); // updates all cooldowns
	bool isDead() const; // checks if player has hp <= 0
	Bullet* const getBullet() const;
	
	inline bool isWeaponChanged() { return changingCounter < 0; } // checks if counting is done and changing weapon
	inline void changeWeapon() { changingCounter = WEAPON_CHANGE; shootingCooldown = WEAPON_CHANGE; } // starts changing weapon by reseting the counter
	inline void takeDamage(const int& damage) { hp -= damage; }
	inline void updateBullet(const RectangleShape &floor, const std::vector <RectangleShape>& obstacles, Player &enemy) { rifle.updateBullet(floor, obstacles, enemy); shotgun.updateBullet(floor, obstacles, *this, enemy);}
	inline const float getLeftEnemySideVisible() const { return leftEnemySideVisible; }
	inline const float getRightEnemySideVisible() const { return rightEnemySideVisible; }
	inline const int getHp() const { return hp; }
	inline const int getCurrentWeapon() const { return currentWeapon; }
	inline const Vector2f getWeapPosition() const { return weaponPosition; }
	inline const Vector2f getPosition() const { return position; }
	inline const Vector2f getAimDir() const { return aimDir;  }
	inline const Vector2f getVelocity() const { return playerVelocity; }
	inline const CircleShape getShotgunRangeShape() const { return shotgunRange; }
	inline const RectangleShape getTex() { return texture; }
	inline void setFillColor(const Color& color) { texture.setFillColor(color); }

	void saveToFile(float leftSideVisible, float rightSideVisible, Vector2f moveDirection, Vector2f newAimDir, bool mouseLeftButtonPressed); // saving state and control to file
	void printState(float leftSideVisible, float rightSideVisible);
	Player(Texture* const playerTex, const Vector2f &position, const Vector2f& enemyPosition_);
	~Player() {};

};

struct VisiblePoint
{
	Vector2f point;
	float angle;
	VisiblePoint(const Vector2f& point_, const float& angle_) : point(point_), angle(angle_) {}
	bool operator < (const VisiblePoint& vis) const { return (angle < vis.angle); }
};



Vector2f pointOnWall(const Vector2f& begin, const Vector2f& end);
bool lineLine(const Vector2f& begin1, const Vector2f& end1, const Vector2f& begin2, const Vector2f& end2); // returns the logial value of the collision of two segments
bool lineSquare(const Vector2f& begin, const Vector2f& end, const Vector2f& square); // returns the logial value of the segment collision with a square
bool lineCircle(const Vector2f& begin, const Vector2f& end, const Vector2f& circle); // returns the logial value of the segment collision with a circle
bool CircleTriangle(const Vector2f& point, const Vector2f& t1, const Vector2f& t2, const Vector2f& t3); // returns the logial value of the circle collision with a triangle
Vector2f lineLineIntersection(const Vector2f& begin1, const Vector2f& end1, const Vector2f& begin2, const Vector2f& end2); // returns the intersection point of two segments
Vector2f lineObstaclesIntersectingLine(const Vector2f& begin, const Vector2f& end, const std::vector <RectangleShape>& obstacles); // returns the closest intersection point of the segment with obstacles
float closestDistFromSquare(const CircleShape& circle, const RectangleShape& square); // returns closest distance from point to square


Vector2f matrixMultVec(const float& m11, const float& m12, const float& m21, const float& m22, const Vector2f& vec); // returns product of the 2x2 matrix with the 2x1 vector
float distance(const Vector2f& point1, const Vector2f& point2); // returns the distance from point to point
float angleDirections(const Vector2f& direction1, const Vector2f& direction2); // returns angle between two directions
inline std::ostream & operator<< (std::ostream &out, const Vector2f& vec) { return out << vec.x << " " << vec.y; } // overload the << operator
