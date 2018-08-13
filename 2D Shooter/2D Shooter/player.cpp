#include "player.h"


void Player::move(const Vector2f& offset)
{
	shotgunRange.move(offset.x, offset.y);
	position += offset;
	texture.move(offset.x, offset.y);
	playerVelocity = offset;
}


void Player::setPosition(const Vector2f& position_)
{
	const Vector2f offset = position_ - position;
	move(offset);
}


void Player::obstacleCollisionMove(const RectangleShape &square)
{
	const float nearestX = std::max(square.getPosition().x - CREST_SIZE / 2, std::min(getPosition().x, square.getPosition().x + CREST_SIZE / 2));
	const float nearestY = std::max(square.getPosition().y - CREST_SIZE / 2, std::min(getPosition().y, square.getPosition().y + CREST_SIZE / 2));
	const float length = distance(Vector2f(nearestX, nearestY), getPosition());
	
	if (length < PLAYER_RADIUS)
	{
		Vector2f distError;
		distError.x = ((getPosition().x - nearestX)*(length - PLAYER_RADIUS)) / length;
		distError.y = ((getPosition().y - nearestY)*(length - PLAYER_RADIUS)) / length;
		move(-distError);
	}
}


void Player::enemyCollisionMove(const Vector2f& enemyPosition)
{
	const float length = distance(getPosition(), enemyPosition);
	if (length < 2 * PLAYER_RADIUS)
	{
		Vector2f distError;
		distError.x = ((getPosition().x - enemyPosition.x) * (length - 2 * PLAYER_RADIUS)) / length;
		distError.y = ((getPosition().y - enemyPosition.y) * (length - 2 * PLAYER_RADIUS)) / length;
		move(-distError);
	}
}


void Player::outOfBoundsMove(const RectangleShape &floor)
{

	Vector2f dist(0.f, 0.f);
	//if (player1.getPosition().x - PLAYER_RADIUS < 0)
	if (getPosition().x - PLAYER_RADIUS - floor.getPosition().x < 0)
		dist.x = getPosition().x - PLAYER_RADIUS - floor.getPosition().x;
	else if (getPosition().x + PLAYER_RADIUS - floor.getPosition().x > FLOOR_WIDTH)
		dist.x = getPosition().x + PLAYER_RADIUS - floor.getPosition().x - FLOOR_WIDTH;
	if (getPosition().y - PLAYER_RADIUS - floor.getPosition().y < 0)
		dist.y = getPosition().y - PLAYER_RADIUS - floor.getPosition().y;
	else if (getPosition().y + PLAYER_RADIUS - floor.getPosition().y > FLOOR_HEIGHT)
		dist.y = getPosition().y + PLAYER_RADIUS - floor.getPosition().y - FLOOR_HEIGHT;

	if (dist.x != 0 || dist.y != 0)
		move(-dist);
}


void Player::updateRotation(const Vector2f &crosshairPosition)
{ 
	aimDir = crosshairPosition - weaponPosition;
	aimDir = aimDir / distance(Vector2f(0.f, 0.f), aimDir);
	const float PI = 3.14159265f;
	const float angle = atan2(aimDir.y, aimDir.x) * 180 / PI;
	texture.setRotation(angle);
	weaponPosition = Vector2f(getPosition().x - 8 * aimDir.x - 13 * aimDir.y, getPosition().y - 8 * aimDir.y + 13 * aimDir.x);
	if (currentWeapon == 1)
	{
		rifle.setRotation(angle);
		rifle.setPosition(weaponPosition);
	}
	else if (currentWeapon == 2)
	{
		shotgun.setRotation(angle);
		shotgun.setPosition(weaponPosition);
	}
		
}


bool Player::canSee(const Player &enemy, const std::vector <RectangleShape>& obstacles, RenderWindow &window, const Color& color)
{
	bool flag = false;
	const float PI = 3.14159265f;
	const float enemyDistance_ = distance(getPosition(), enemy.getPosition());

	Vector2f enemyDir_ = enemy.getPosition() - weaponPosition;
	enemyDir_ = enemyDir_ / distance(Vector2f(0.f, 0.f), enemyDir_);
	float maxAngle = atan2(PLAYER_RADIUS, enemyDistance_);
	Vector2f enemyLeftSideDir;
	enemyLeftSideDir.x = enemyDir_.x * cos(-maxAngle) - enemyDir_.y * sin(-maxAngle);
	enemyLeftSideDir.y = enemyDir_.x * sin(-maxAngle) + enemyDir_.y * cos(-maxAngle);

	Vector2f enemyRightSideDir;
	enemyRightSideDir.x = enemyDir_.x * cos(maxAngle) - enemyDir_.y * sin(maxAngle);
	enemyRightSideDir.y = enemyDir_.x * sin(maxAngle) + enemyDir_.y * cos(maxAngle);

	Vector2f leftSightAim;
	leftSightAim.x = aimDir.x * cos(-SIGHT_ANGLE * PI / 180) - aimDir.y * sin(-SIGHT_ANGLE * PI / 180);
	leftSightAim.y = aimDir.x * sin(-SIGHT_ANGLE * PI / 180) + aimDir.y * cos(-SIGHT_ANGLE * PI / 180);

	Vector2f rightSightAim;
	rightSightAim.x = aimDir.x * cos(SIGHT_ANGLE * PI / 180) - aimDir.y * sin(SIGHT_ANGLE * PI / 180);
	rightSightAim.y = aimDir.x * sin(SIGHT_ANGLE * PI / 180) + aimDir.y * cos(SIGHT_ANGLE * PI / 180);


	enemyDistance = enemyDistance_;
	enemyDir = enemyDir_;


	float ang = angleDirections(leftSightAim, enemyLeftSideDir);
	if (ang < -maxAngle * 2 || ang > SIGHT_ANGLE * 2 * PI / 180)
	{
		isEnemyVisible = false;
		lastTimeEnemyVisible++;
		leftEnemySideVisible = 0;
		rightEnemySideVisible = 0;
		return false;
	}
		

	Vector2f points[48];
	const float eps = 0.0001f;

	// Get all vertices of crates
	int i = 0;
	for (std::vector<RectangleShape>::const_iterator it = obstacles.begin(); it != obstacles.end(); it++)
	{
		points[i * 8] = (*it).getPosition() + Vector2f(-CREST_SIZE / 2, -CREST_SIZE / 2) + Vector2f(eps, eps);
		points[1 + i * 8] = (*it).getPosition() + Vector2f(-CREST_SIZE / 2, -CREST_SIZE / 2) + Vector2f(-eps, -eps);

		points[2 + i * 8] = (*it).getPosition() + Vector2f(CREST_SIZE / 2, -CREST_SIZE / 2) + Vector2f(-eps, eps);
		points[3 + i * 8] = (*it).getPosition() + Vector2f(CREST_SIZE / 2, -CREST_SIZE / 2) + Vector2f(eps, -eps);

		points[4 + i * 8] = (*it).getPosition() + Vector2f(CREST_SIZE / 2, CREST_SIZE / 2) + Vector2f(-eps, -eps);
		points[5 + i * 8] = (*it).getPosition() + Vector2f(CREST_SIZE / 2, CREST_SIZE / 2) + Vector2f(eps, eps);


		points[6 + i * 8] = (*it).getPosition() + Vector2f(-CREST_SIZE / 2, CREST_SIZE / 2) + Vector2f(-eps, eps);
		points[7 + i * 8] = (*it).getPosition() + Vector2f(-CREST_SIZE / 2, CREST_SIZE / 2) + Vector2f(eps, -eps);
		i++;
	}


	Vector2f leftBoundaryDir;
	if (ang < 0.f)
		leftBoundaryDir = leftSightAim;
	else
		leftBoundaryDir = enemyLeftSideDir;
	const Vector2f leftBoundaryDirPoint = weaponPosition + leftBoundaryDir;


	Vector2f rightBoundaryDir;
	if (ang > SIGHT_ANGLE * 2 * PI / 180 - 2 * maxAngle)
		rightBoundaryDir = rightSightAim;
	else
		rightBoundaryDir = enemyRightSideDir;
	const Vector2f rightBoundaryDirPoint = weaponPosition + rightBoundaryDir;

	float leftBoundaryAngle = angleDirections(enemyLeftSideDir, leftBoundaryDir);
	float rightBoundaryAngle = angleDirections(enemyLeftSideDir, rightBoundaryDir);

	Vector2f intersectionPoint = lineObstaclesIntersectingLine(weaponPosition, pointOnWall(weaponPosition, leftBoundaryDirPoint), obstacles);
	if (enemyDistance_ > distance(getPosition(), intersectionPoint))
		leftBoundaryDir = Vector2f(0.f, 0.f);
	intersectionPoint = lineObstaclesIntersectingLine(weaponPosition, pointOnWall(weaponPosition, rightBoundaryDirPoint), obstacles);
	if (enemyDistance_ > distance(getPosition(), intersectionPoint))
		rightBoundaryDir = Vector2f(0.f, 0.f);

	for (int i = 0; i < 48; i++)
	{
		if (distance(points[i], getPosition()) < enemyDistance_)
		{
			Vector2f pointDir = points[i] - weaponPosition;
			ang = angleDirections(enemyLeftSideDir, pointDir);
			if (ang >= leftBoundaryAngle && ang <= rightBoundaryAngle)
			{
				Vector2f pointDirPoint = weaponPosition + pointDir;
				intersectionPoint = lineObstaclesIntersectingLine(weaponPosition, pointOnWall(weaponPosition, pointDirPoint), obstacles);
				if (enemyDistance_ < distance(getPosition(), intersectionPoint))
				{
					if (leftBoundaryDir == Vector2f(0.f, 0.f))
						leftBoundaryDir = pointDir;
					else
						rightBoundaryDir = pointDir;
				}
			}
		}
	}

	if (leftBoundaryDir != Vector2f(0.f, 0.f) && rightBoundaryDir != Vector2f(0.f, 0.f))
	{
		float angleFromLeft = angleDirections(enemyLeftSideDir, leftBoundaryDir);
		float angleFromRight = angleDirections(rightBoundaryDir, enemyRightSideDir);

		leftEnemySideVisible = (maxAngle - angleFromLeft) / maxAngle;
		rightEnemySideVisible = (maxAngle - angleFromRight) / maxAngle;

		if (angleFromLeft > maxAngle)
		{
			rightEnemySideVisible = angleDirections(leftBoundaryDir, rightBoundaryDir) / maxAngle;
			leftEnemySideVisible = 0;
		}
		if (angleFromRight > maxAngle)
		{
			leftEnemySideVisible = angleDirections(leftBoundaryDir, rightBoundaryDir) / maxAngle;
			rightEnemySideVisible = 0;
		}

		isEnemyVisible = true;
		enemyPosition = enemy.getPosition();
		enemyDistance = enemyDistance_;
		enemyAimDir = enemy.getAimDir();
		enemyHp = enemy.getHp();
		enemyVelocity = enemy.getVelocity();
		lastTimeEnemyVisible = 0;
		enemyCurrentWeapon = enemy.getCurrentWeapon();
		return true;
	}
	else
	{
		isEnemyVisible = false;
		lastTimeEnemyVisible++;
		leftEnemySideVisible = rightEnemySideVisible = 0;
		enemyPosition = enemy.getPosition();
		enemyDistance = enemyDistance_;
		enemyAimDir = enemy.getAimDir();
		enemyHp = enemy.getHp();
		enemyVelocity = enemy.getVelocity();
		enemyCurrentWeapon = enemy.getCurrentWeapon();

		/*
		isEnemyVisible = false;
		lastTimeEnemyVisible++;
		leftEnemySideVisible = 0;
		rightEnemySideVisible = 0;
		*/
		return false;
	}
		
}


void Player::drawView(const std::vector <RectangleShape>& obstacles, RenderWindow &window, const Color& color)
{
	bool flag = false;
	const float PI = 3.14159265f;

	Vector2f points[48];
	const float eps = 0.0001f;

	// Get all vertices of crates
	points[0] = Vector2f(0.f, 0.f) + Vector2f(0, eps);
	points[1] = Vector2f(0.f, 0.f) + Vector2f(eps, 0);
	points[2] = Vector2f(FLOOR_WIDTH, 0.f) + Vector2f(-eps, 0);
	points[3] = Vector2f(FLOOR_WIDTH, 0.f) + Vector2f(0, eps);
	points[4] = Vector2f(FLOOR_WIDTH, FLOOR_HEIGHT) + Vector2f(0, -eps);
	points[5] = Vector2f(FLOOR_WIDTH, FLOOR_HEIGHT) + Vector2f(-eps, 0);
	points[6] = Vector2f(0.f, FLOOR_HEIGHT) + Vector2f(eps, 0);
	points[7] = Vector2f(0.f, FLOOR_HEIGHT) + Vector2f(0, -eps);

	int i = 0;
	for (std::vector<RectangleShape>::const_iterator it = obstacles.begin(); it != obstacles.end(); it++)
	{
		points[8 + i * 8] = (*it).getPosition() + Vector2f(-CREST_SIZE / 2, -CREST_SIZE / 2) + Vector2f(eps, eps);
		points[9 + i * 8] = (*it).getPosition() + Vector2f(-CREST_SIZE / 2, -CREST_SIZE / 2) + Vector2f(-eps, -eps);

		points[10 + i * 8] = (*it).getPosition() + Vector2f(CREST_SIZE / 2, -CREST_SIZE / 2) + Vector2f(-eps, eps);
		points[11 + i * 8] = (*it).getPosition() + Vector2f(CREST_SIZE / 2, -CREST_SIZE / 2) + Vector2f(eps, -eps);

		points[12 + i * 8] = (*it).getPosition() + Vector2f(CREST_SIZE / 2, CREST_SIZE / 2) + Vector2f(-eps, -eps);
		points[13 + i * 8] = (*it).getPosition() + Vector2f(CREST_SIZE / 2, CREST_SIZE / 2) + Vector2f(eps, eps);


		points[14 + i * 8] = (*it).getPosition() + Vector2f(-CREST_SIZE / 2, CREST_SIZE / 2) + Vector2f(-eps, eps);
		points[15 + i * 8] = (*it).getPosition() + Vector2f(-CREST_SIZE / 2, CREST_SIZE / 2) + Vector2f(eps, -eps);
		i++;
	}


	Vector2f leftSightAim;
	leftSightAim.x = aimDir.x * cos(-SIGHT_ANGLE * PI / 180) - aimDir.y * sin(-SIGHT_ANGLE * PI / 180);
	leftSightAim.y = aimDir.x * sin(-SIGHT_ANGLE * PI / 180) + aimDir.y * cos(-SIGHT_ANGLE * PI / 180);
	const Vector2f leftSightPoint = weaponPosition + leftSightAim;

	Vector2f rightSightAim;
	rightSightAim.x = aimDir.x * cos(SIGHT_ANGLE * PI / 180) - aimDir.y * sin(SIGHT_ANGLE * PI / 180);
	rightSightAim.y = aimDir.x * sin(SIGHT_ANGLE * PI / 180) + aimDir.y * cos(SIGHT_ANGLE * PI / 180);
	const Vector2f rightSightPoint = weaponPosition + rightSightAim;


	Vertex line[2];
	line[0] = weaponPosition;
	line[1] = pointOnWall(weaponPosition, rightSightPoint);
	window.draw(line, 2, Lines);

	line[0] = weaponPosition;
	line[1] = pointOnWall(weaponPosition, leftSightPoint);
	window.draw(line, 2, Lines);


	std::vector <VisiblePoint> visiblePoints;
	float ang;
	for (int i = 0; i < 48; i++)
	{
		Vector2f pointDir = points[i] - weaponPosition;
		ang = angleDirections(leftSightAim, pointDir);
		if (ang >= 0 && ang <= SIGHT_ANGLE * 2 * PI / 180)
			visiblePoints.push_back(VisiblePoint(points[i], ang));
	}

	// sort points in ascending order
	std::sort(visiblePoints.begin(), visiblePoints.end());
	Vector2f pointDir = rightSightPoint - weaponPosition;
	visiblePoints.push_back(VisiblePoint(rightSightPoint, angleDirections(leftSightAim, pointDir)));


	Vector2f intersectionPoint;
	Vector2f prevIntersectionPoint;

	prevIntersectionPoint = lineObstaclesIntersectingLine(weaponPosition, pointOnWall(weaponPosition, leftSightPoint), obstacles);

	VertexArray triangle(Triangles, 3);
	triangle[0].color = color;
	triangle[1].color = color;
	triangle[2].color = color;


	const float distanceEps = 0.1f;
	for (std::vector<VisiblePoint>::iterator it = visiblePoints.begin(); it != visiblePoints.end(); it++)
	{
		intersectionPoint = lineObstaclesIntersectingLine(weaponPosition, pointOnWall(weaponPosition, (*it).point), obstacles);
		if (distance(weaponPosition, intersectionPoint) + distanceEps >= distance(weaponPosition, (*it).point))
		{
			triangle[0].position = prevIntersectionPoint;
			triangle[1].position = intersectionPoint;
			triangle[2].position = weaponPosition;
			window.draw(triangle);
			prevIntersectionPoint = intersectionPoint;
		}
	}
}


bool Player::canShoot() const
{
	if (currentWeapon == 1)
		return rifle.canShoot();
	else if (currentWeapon == 2)
		return shotgun.canShoot();
	else if (currentWeapon == 0)
		return false;
}


void Player::shoot()
{
	if (currentWeapon == 1)
		rifle.shoot(aimDir);
	else if (currentWeapon == 2)
		shotgun.shoot(aimDir);
	shootingCooldown = RIFLE_SHOOTING_TIME;
}


void Player::enemyShot(const Player& enemy)
{
	lastTimeEnemyShot = 0;
	isEnemyVisible = true;
	enemyPosition = enemy.getPosition();
	enemyDistance = distance(position, enemy.getPosition());
	enemyAimDir = enemy.getAimDir();
	enemyDir = enemy.getPosition() - weaponPosition;
	enemyDir = enemyDir / distance(Vector2f(0.f, 0.f), enemyDir);
	enemyVelocity = enemy.getVelocity();
	lastTimeEnemyVisible = 0;
	enemyCurrentWeapon = enemy.getCurrentWeapon();
}


void Player::updateCounters(Vector2f mousePosition_)
{ 
	static int nextWeapon = 1;
	rifle.updateCounter(); 
	shotgun.updateCounter(); 
	lastTimeEnemyShot++;
	crosshairPosition = mousePosition_;
	if (shootingCooldown > 0)
		shootingCooldown--;
	
	if (changingCounter == WEAPON_CHANGE)
	{
		if (currentWeapon == 1)
			nextWeapon = 2;
		else if (currentWeapon == 2)
			nextWeapon = 1;
		currentWeapon = 0;
	}
	if (changingCounter == 0)
		currentWeapon = nextWeapon;
	changingCounter--;

}


bool Player::isDead() const
{
	if (hp <= 0)
		return true;
	else
		return false;
}


Bullet* const Player::getBullet() const
{ 
	if (currentWeapon == 1)
		return rifle.getBullet();
	else if (currentWeapon == 2)
		return shotgun.getBullet();
	else if (currentWeapon == 0)
		return NULL;
}


Player::Player(Texture* const playerTex, const Vector2f& position_, const Vector2f& enemyPosition_) : rifle(position_), shotgun(position_)
{
	hp = 100;
	hpMax = 100;
	currentWeapon = 1;
	position = position_;
	enemyPosition = enemyPosition_;
	enemyDistance = distance(enemyPosition_, position);
	isEnemyVisible = false;
	lastTimeEnemyVisible = 0;
	leftEnemySideVisible = 0;
	rightEnemySideVisible = 0;
	enemyDir = enemyPosition - weaponPosition;
	enemyDir = enemyDir / distance(Vector2f(0.f, 0.f), enemyDir);
	enemyHp = 100;
	enemyCurrentWeapon = 1;
	lastTimeEnemyShot = 30;


	texture.setSize(Vector2f(2.f * PLAYER_RADIUS * 1.70667f, 2.f * PLAYER_RADIUS));
	texture.setOrigin(Vector2f(PLAYER_RADIUS, PLAYER_RADIUS));
	texture.setPosition(position_);
	texture.setTexture(playerTex);
	texture.setFillColor(Color(255, 255, 255, 255));

	shotgunRange.setRadius(SHOTGUN_RANGE);
	shotgunRange.setOrigin(SHOTGUN_RANGE, SHOTGUN_RANGE);
	shotgunRange.setPosition(position_);
	shotgunRange.setOutlineColor(Color::Black);
	shotgunRange.setOutlineThickness(1.0f);
	shotgunRange.setFillColor(sf::Color(0, 0, 0, 0));
}


void Player::saveToFile(float leftSideVisible, float rightSideVisible, Vector2f moveDirection, Vector2f newAimDir, bool mouseLeftButtonPressed)
{
	std::fstream file;
	file.open("input.txt", std::ios::out | std::ios::app);
	if (file.good() == true)
	{
		file << position << " " << aimDir << " " << shootingCooldown << " " << leftSideVisible << " " << rightSideVisible << " ";
		file << enemyDistance << " " << enemyDir << " " << (int)isEnemyVisible << " " << leftEnemySideVisible << " " << rightEnemySideVisible << " ";
		file << moveDirection << " " << newAimDir << " " << (int)mouseLeftButtonPressed << std::endl;
	}
	else std::cout << "Unable to open a file!" << std::endl;
}


void Player::printState(float leftSideVisible, float rightSideVisible)
{
	std::cout << position << " " << aimDir << " " << shootingCooldown << " " << leftSideVisible << " " << rightSideVisible << " ";
	std::cout << enemyDistance << " " << enemyDir << " " << (int)isEnemyVisible << " " << leftEnemySideVisible << " " << rightEnemySideVisible << std::endl;
	std::cout.flush();
}


//==============================================================================================================
//=============================================== Rifle =======================================================
//==============================================================================================================


bool Rifle::canShoot() const
{
	if (shootingCounter <= 0)
		return true;
	else
		return false;
}


void Rifle::shoot(const Vector2f& aimDir)
{
	bullet = new Bullet(getPosition(), aimDir, Color::Black);
	shootingCounter = RIFLE_SHOOTING_TIME;
}


void Rifle::updateBullet(const RectangleShape &floor, const std::vector <RectangleShape>& obstacles, Player &enemy)
{ 
	if (bullet != NULL)
	{ 
		bool enemyHit = false;
		bool obstacleHit = false;
		float closestDist = std::numeric_limits<float>::infinity();

		const Vector2f presPosition = bullet->getPosition();
		const Vector2f newPosition = bullet->getPosition() + bullet->getVelocity();

		// Crest hit
		float distToObstacle;
		for (std::vector<RectangleShape>::const_iterator it = obstacles.begin(); it != obstacles.end(); it++)
		{
			if (lineSquare(presPosition, newPosition, (*it).getPosition()))
			{
				obstacleHit = true;
				distToObstacle = closestDistFromSquare(bullet->getShape(), *it);
				if (distToObstacle < closestDist)
					closestDist = distToObstacle;
			}
		}

		// Player hit
		if (lineCircle(presPosition, newPosition, enemy.getPosition()))
			enemyHit = true;

		if (obstacleHit == true || enemyHit == true) // hit and bullet destroy
		{
			const float distToEnemy = distance(presPosition, enemy.getPosition()) - PLAYER_RADIUS;
			if (distToEnemy < closestDist && enemyHit == true)
				enemy.takeDamage(RIFLE_DAMAGE);
			delete bullet;
			bullet = NULL;
		}
		else // didnt hit and bullet move
		{
			// Bullet move
			bullet->setPosition(newPosition);
			// Out of bounds
			if (presPosition.x + BULLET_RADIUS < 0 || presPosition.x > floor.getSize().x
				|| presPosition.y + BULLET_RADIUS < 0 || presPosition.y > floor.getSize().y)
			{
				delete bullet;
				bullet = NULL;
			}
		}
	}
}


Rifle::Rifle(const Vector2f& position)
{
	shootingCounter = 0;
	bullet = NULL;
	shape.setSize(Vector2f(60.f, 5.f));
	shape.setOrigin(0.f, 2.5f);
	shape.setPosition(position);
	//bulletsLeft = MAGAZINE_SIZE;
}


//==============================================================================================================
//=============================================== SHOTGUN ======================================================
//==============================================================================================================


bool Shotgun::canShoot() const
{
	if (shootingCounter <= 0) //&& bulletsLeft > 0)
		return true;
	else
		return false;
}


void Shotgun::shoot(const Vector2f& aimDir)
{
	bullet = new Bullet(getPosition(), aimDir, Color::Red);
	shootingCounter = SHOTGUN_SHOOTING_TIME;
	//bulletsLeft--;
}


void Shotgun::updateBullet(const RectangleShape &floor, const std::vector <RectangleShape>& obstacles, const Player& player, Player &enemy)
{
	if (bullet != NULL)
	{
		bool playerHit = false;
		bool obstacleHit = false;
		float closestDist = std::numeric_limits<float>::infinity();

		const Vector2f presPosition = bullet->getPosition();
		const Vector2f newPosition = bullet->getPosition() + bullet->getVelocity();

		// Crest hit
		float distToObstacle;
		for (std::vector<RectangleShape>::const_iterator it = obstacles.begin(); it != obstacles.end(); it++)
		{
			if (lineSquare(presPosition, newPosition, (*it).getPosition()))
			{
				obstacleHit = true;
				distToObstacle = closestDistFromSquare(bullet->getShape(), (*it));
				if (distToObstacle < closestDist)
					closestDist = distToObstacle;
			}
		}

		// Player hit
		if (lineCircle(presPosition, newPosition, enemy.getPosition()) && (distance(player.getPosition(), enemy.getPosition()) - PLAYER_RADIUS) <= SHOTGUN_RANGE)
			playerHit = true;

		if (obstacleHit == true || playerHit == true) // hit and bullet destroy
		{
			float distToPlayer = distance(presPosition, enemy.getPosition()) - PLAYER_RADIUS; // tu moze byc jakis bladddddddddddddddddddddddddddddddddddddddddddd
			if (distToPlayer < closestDist && playerHit == true)
				enemy.takeDamage(SHOTGUN_DAMAGE);
			delete bullet;
			bullet = NULL;
		}
		else // didnt hit and bullet move
		{
			// Out of bounds
			if (newPosition.x + BULLET_RADIUS < 0 || newPosition.x > floor.getSize().x
				|| newPosition.y + BULLET_RADIUS < 0 || newPosition.y > floor.getSize().y)
			{
				delete bullet;
				bullet = NULL;
			}
			else if (distance(player.getPosition(), newPosition) > SHOTGUN_RANGE)
			{
				delete bullet;
				bullet = NULL;
			}
			else 
				bullet->setPosition(newPosition);
		}
	}
}


Shotgun::Shotgun(const Vector2f &position)
{
	shootingCounter = 0;
	bullet = NULL;
	shape.setSize(Vector2f(60.f, 5.f));
	shape.setOrigin(0.f, 2.5f);
	shape.setPosition(position);
	//bulletsLeft = MAGAZINE_SIZE;
}


//==============================================================================================================
//=============================================== BULLET =======================================================
//==============================================================================================================


Bullet::Bullet(const Vector2f& position, const Vector2f& aimDir, const Color& color)
{
	shape.setFillColor(color);
	shape.setRadius(BULLET_RADIUS);
	shape.setOrigin(BULLET_RADIUS, BULLET_RADIUS);
	shape.setPosition(position);
	velocity = aimDir * BULLET_SPEED;
}


//==============================================================================================================
//=============================================== REST =========================================================
//==============================================================================================================


Vector2f pointOnWall(const Vector2f& begin, const Vector2f& end)
{
	Vector2f wallIntersection;

	const Vector2f v = end - begin;
	Vector2f dist = -begin;
	if (v.x > 0)
		dist.x = FLOOR_WIDTH - begin.x;
	if (v.y > 0)
		dist.y = FLOOR_HEIGHT - begin.y;

	if (dist.x / v.x < dist.y / v.y)
	{
		wallIntersection.x = begin.x + dist.x;
		wallIntersection.y = begin.y + (dist.x / v.x) * v.y;
	}
	else
	{
		wallIntersection.x = begin.x + (dist.y / v.y) * v.x;
		wallIntersection.y = begin.y + dist.y;
	}
	return wallIntersection;
}


bool lineLine(const Vector2f& begin1, const Vector2f& end1, const Vector2f& begin2, const Vector2f& end2)
{
	const float uA = ((end2.x - begin2.x)*(begin1.y - begin2.y) - (end2.y - begin2.y)*(begin1.x - begin2.x)) / ((end2.y - begin2.y)*(end1.x - begin1.x) - (end2.x - begin2.x)*(end1.y - begin1.y));
	const float uB = ((end1.x - begin1.x)*(begin1.y - begin2.y) - (end1.y - begin1.y)*(begin1.x - begin2.x)) / ((end2.y - begin2.y)*(end1.x - begin1.x) - (end2.x - begin2.x)*(end1.y - begin1.y));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
		return true;
	else
		return false;
}


bool lineSquare(const Vector2f& begin, const Vector2f& end, const Vector2f& square)
{

	if (lineLine(begin, end, Vector2f(square.x - CREST_SIZE / 2, square.y - CREST_SIZE / 2), Vector2f(square.x - CREST_SIZE / 2, square.y + CREST_SIZE / 2)))
		return true;
	if (lineLine(begin, end, Vector2f(square.x + CREST_SIZE / 2, square.y - CREST_SIZE / 2), Vector2f(square.x + CREST_SIZE / 2, square.y + CREST_SIZE / 2)))
		return true;
	if (lineLine(begin, end, Vector2f(square.x - CREST_SIZE / 2, square.y - CREST_SIZE / 2), Vector2f(square.x + CREST_SIZE / 2, square.y - CREST_SIZE / 2)))
		return true;
	if (lineLine(begin, end, Vector2f(square.x - CREST_SIZE / 2, square.y + CREST_SIZE / 2), Vector2f(square.x + CREST_SIZE / 2, square.y + CREST_SIZE / 2)))
		return true;
	return false;
}


bool lineCircle(const Vector2f& begin, const Vector2f& end, const Vector2f& circle)
{
	const Vector2f vec(end.x - begin.x, end.y - begin.y);
	const float m11 = vec.x / distance(begin, end);
	const float m12 = vec.y / distance(begin, end);
	const float m21 = -m12;
	const float m22 = m11;
	const Vector2f newBegin = matrixMultVec(m11, m12, m21, m22, begin);
	const Vector2f newEnd = matrixMultVec(m11, m12, m21, m22, end);
	const Vector2f newCircle = matrixMultVec(m11, m12, m21, m22, circle);

	const float nearestX = std::max(newBegin.x, std::min(newCircle.x, newEnd.x));
	const float distance = sqrt(pow(nearestX - newCircle.x, 2) + pow(newBegin.y - newCircle.y, 2));

	if (distance <= PLAYER_RADIUS)
		return true;
	else
		return false;
}


bool CircleTriangle(const Vector2f& p, const Vector2f& t1, const Vector2f& t2, const Vector2f& t3)
{
	const bool b1 = ((p.x - t2.x) * (t1.y - t2.y) - (t1.x - t2.x) * (p.y - t2.y)) < 0.f;
	const bool b2 = ((p.x - t3.x) * (t2.y - t3.y) - (t2.x - t3.x) * (p.y - t3.y)) < 0.f;
	const bool b3 = ((p.x - t1.x) * (t3.y - t1.y) - (t3.x - t1.x) * (p.y - t1.y)) < 0.f;
	if (b1 == b2 && b2 == b3)
		return true;
	else
	{
		const bool b4 = lineCircle(t1, t2, p);
		const bool b5 = lineCircle(t2, t3, p);
		const bool b6 = lineCircle(t3, t1, p);
		if (b4 || b5 || b6)
			return true;
		else
			return false;
	}
}


Vector2f lineLineIntersection(const Vector2f& begin1, const Vector2f& end1, const Vector2f& begin2, const Vector2f& end2)
{
	Vector2f intersection;
	const float denominator = (begin1.x - end1.x) * (begin2.y - end2.y) - (begin1.y - end1.y) * (begin2.x - end2.x);
	intersection.x = ((begin1.x * end1.y - begin1.y * end1.x) * (begin2.x - end2.x) - (begin1.x - end1.x) * (begin2.x * end2.y - begin2.y * end2.x)) / denominator;
	intersection.y = ((begin1.x * end1.y - begin1.y * end1.x) * (begin2.y - end2.y) - (begin1.y - end1.y) * (begin2.x * end2.y - begin2.y * end2.x)) / denominator;
	return intersection;
}


Vector2f lineObstaclesIntersectingLine(const Vector2f& begin, const Vector2f& end, const std::vector <RectangleShape>& obstacles)
{
	Vector2f nearestIntersectingPoint = Vector2f(0.f, 0.f);
	Vector2f intersectionPoint;
	float minDistance = std::numeric_limits<float>::infinity();
	for (std::vector<RectangleShape>::const_iterator it = obstacles.begin(); it != obstacles.end(); it++)
	{
		Vector2f obstacleVertices[4];
		obstacleVertices[0] = Vector2f((*it).getPosition().x - CREST_SIZE / 2, (*it).getPosition().y - CREST_SIZE / 2);
		obstacleVertices[1] = Vector2f((*it).getPosition().x + CREST_SIZE / 2, (*it).getPosition().y - CREST_SIZE / 2);
		obstacleVertices[2] = Vector2f((*it).getPosition().x + CREST_SIZE / 2, (*it).getPosition().y + CREST_SIZE / 2);
		obstacleVertices[3] = Vector2f((*it).getPosition().x - CREST_SIZE / 2, (*it).getPosition().y + CREST_SIZE / 2);

		for (int j = 0; j < 4; j++)
		{
			if (lineLine(begin, end, obstacleVertices[j], obstacleVertices[(j + 1) % 4]))
			{
				intersectionPoint = lineLineIntersection(begin, end, obstacleVertices[j], obstacleVertices[(j + 1) % 4]);
				if (distance(begin, intersectionPoint) < minDistance)
				{
					minDistance = distance(begin, intersectionPoint);
					nearestIntersectingPoint = intersectionPoint;
				}
			}
		}
	}
	Vector2f floorVertices[4];
	floorVertices[0] = Vector2f(0.f, 0.f);
	floorVertices[1] = Vector2f(FLOOR_WIDTH, 0.f);
	floorVertices[2] = Vector2f(FLOOR_WIDTH, FLOOR_HEIGHT);
	floorVertices[3] = Vector2f(0.f, FLOOR_HEIGHT);
	for (int i = 0; i < 4; i++)
	{
		if (lineLine(begin, end, floorVertices[i], floorVertices[(i + 1) % 4]))
		{
			intersectionPoint = lineLineIntersection(begin, end, floorVertices[i], floorVertices[(i + 1) % 4]);
			if (distance(begin, intersectionPoint) < minDistance)
			{
				minDistance = distance(begin, intersectionPoint);
				nearestIntersectingPoint = intersectionPoint;
			}
		}
	}
	return nearestIntersectingPoint;
}


float closestDistFromSquare(const CircleShape& circle, const RectangleShape& square)
{
	const float nearestX = std::max(square.getPosition().x - CREST_SIZE / 2, std::min(circle.getPosition().x, square.getPosition().x + CREST_SIZE / 2));
	const float nearestY = std::max(square.getPosition().y - CREST_SIZE / 2, std::min(circle.getPosition().y, square.getPosition().y + CREST_SIZE / 2));
	return distance(Vector2f(nearestX, nearestY), circle.getPosition());
}




Vector2f matrixMultVec(const float& m11, const float& m12, const float& m21, const float& m22, const Vector2f& vec)
{
	return Vector2f(m11 * vec.x + m12 * vec.y, m21 * vec.x + m22 * vec.y);
}


float distance(const Vector2f& point1, const Vector2f& point2)
{
	return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
}


float angleDirections(const Vector2f& direction1, const Vector2f& direction2)
{
	const float dot = direction1.x * direction2.x + direction1.y * direction2.y;
	const float det = direction1.x * direction2.y - direction1.y * direction2.x;
	return atan2(det, dot);
}

