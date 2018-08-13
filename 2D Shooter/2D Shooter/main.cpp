#include <iostream>
#include "player.h"
#include "SFML\Graphics.hpp"
#include "SFML\Window.hpp"
#include "SFML\System.hpp"
#include "SFML\Audio.hpp"
#include <math.h>
#include <vector>
#include <cstdlib>

#define GAME_MODE 1 // 0 - game 2 players, 1 - saveToFile, 2 - imitation learning bot

/*
std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
std::cout << 1000 * time_span.count();
std::cout << std::endl;
*/

int main()
{
	srand(time(NULL));


	RenderWindow window(VideoMode(1000, 850), "2D shooter", Style::Default);
	window.setFramerateLimit(60);


	//==============================================================================================================
	//=============================================== VARIABLES ====================================================
	//==============================================================================================================


	RectangleShape whiteRect;
	whiteRect.setSize(Vector2f(1000.f, 150.f));
	whiteRect.setFillColor(Color::White);
	whiteRect.setPosition(0.f, FLOOR_HEIGHT);

	// Vectors
	Vector2f mousePosition;

	Vector2f joyPosition;
	Vector2f joyAim;
	Vector2f joyMov;
	
	bool prevMouseButtonState = false;
	bool prevJoyTriggerState = false;
	bool gameRunning = true;

	// Joystick
	float joyAimSpeed = 0.05;

	if (Joystick::isConnected(0))
		std::cout << "Joystick is connected" << std::endl;
	else
		;// std::cout << "Joystick is not connected" << std::endl;


	//==============================================================================================================
	//=============================================== HUD ==========================================================
	//==============================================================================================================


	window.setMouseCursorVisible(false);
	float crosshairSize = 60.f;

	Texture healthCrossTex;
	healthCrossTex.loadFromFile("Textures/healthCross.png");

	Texture crosshairTex;
	crosshairTex.loadFromFile("Textures/crosshair.png");
	Texture crosshairTex2;
	crosshairTex2.loadFromFile("Textures/crosshair2.png");

	Font font;
	if (!font.loadFromFile("Fonts/28 Days Later.ttf"))
		throw("Could not load font!");


	RectangleShape verticalLine(Vector2f(5.f, 150.f));
	verticalLine.setPosition(497.5f, FLOOR_HEIGHT);
	verticalLine.setFillColor(Color::Black);

	RectangleShape horizontalLine(Vector2f(FLOOR_WIDTH + 100.f, 5.f));
	horizontalLine.setPosition(0.f, FLOOR_HEIGHT);
	horizontalLine.setFillColor(Color::Black);


	Text gameOver;
	gameOver.setFont(font);
	gameOver.setFillColor(Color::Red);
	gameOver.setCharacterSize(150);
	gameOver.setPosition(30.f, 270.f);


	// Player 1
	Text player1Text;
	player1Text.setFont(font);
	player1Text.setString("Player 1");
	player1Text.setFillColor(Color::Black);
	player1Text.setCharacterSize(50);
	player1Text.setPosition(20.f, FLOOR_HEIGHT + 20.f);

	RectangleShape healthCross1(Vector2f(50.f, 50.f));
	healthCross1.setTexture(&healthCrossTex);
	healthCross1.setPosition(20.f, FLOOR_HEIGHT + 80.f);

	Text healthText1;
	healthText1.setFont(font);
	healthText1.setString("100");
	healthText1.setFillColor(Color::Black);
	healthText1.setCharacterSize(50);
	healthText1.setPosition(90.f, FLOOR_HEIGHT + 80.f);

	RectangleShape crosshair1(Vector2f(crosshairSize, crosshairSize));
	crosshair1.setOrigin(crosshairSize / 2, crosshairSize / 2);
	
	// Player 2

	Text player2Text;
	player2Text.setFont(font);
	player2Text.setString("Player 2");
	player2Text.setFillColor(Color::Black);
	player2Text.setCharacterSize(50);
	player2Text.setPosition(520.f, FLOOR_HEIGHT + 20.f);

	RectangleShape healthCross2(Vector2f(50.f, 50.f));
	healthCross2.setTexture(&healthCrossTex);
	healthCross2.setPosition(520.f, FLOOR_HEIGHT + 80.f);

	Text healthText2;
	healthText2.setFont(font);
	healthText2.setString("100");
	healthText2.setFillColor(Color::Black);
	healthText2.setCharacterSize(50);
	healthText2.setPosition(590.f, FLOOR_HEIGHT + 80.f);
	
	RectangleShape crosshair2(Vector2f(crosshairSize, crosshairSize));
	crosshair2.setOrigin(crosshairSize / 2, crosshairSize / 2);

	// sound
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("Sounds/M4A1.wav"))
		throw("Could not load sound!");
	Sound m4a1;
	m4a1.setBuffer(buffer);
	

	//==============================================================================================================
	//=============================================== OBJECTS ======================================================
	//==============================================================================================================


	float verticalMargin = 200.f;
	float horizontalMargin = 200.f;

	Texture floorTex;
	floorTex.loadFromFile("Textures/floor3.jpg");

	Texture crestTex;
	crestTex.loadFromFile("Textures/crest.jpg");

	RectangleShape floor(Vector2f(FLOOR_WIDTH, FLOOR_HEIGHT));
	floor.setTexture(&floorTex);

	std::vector<RectangleShape> obstacles;
	RectangleShape obstacle(Vector2f(CREST_SIZE, CREST_SIZE));
	obstacle.setOrigin(Vector2f(CREST_SIZE / 2, CREST_SIZE / 2));
	obstacle.setTexture(&crestTex);

	obstacle.setPosition(horizontalMargin, verticalMargin); // upper left
	obstacles.push_back(obstacle);
	obstacle.setPosition(FLOOR_WIDTH - horizontalMargin, verticalMargin); // upper right
	obstacles.push_back(obstacle);
	obstacle.setPosition(horizontalMargin, FLOOR_HEIGHT - verticalMargin); // lower left
	obstacles.push_back(obstacle);
	obstacle.setPosition(FLOOR_WIDTH - horizontalMargin, FLOOR_HEIGHT - verticalMargin); // lower right
	obstacles.push_back(obstacle);
	obstacle.setPosition(FLOOR_WIDTH / 2, FLOOR_HEIGHT / 2); // center
	obstacles.push_back(obstacle);


	//==============================================================================================================
	//=============================================== PLAYERS ======================================================
	//==============================================================================================================


	Texture playerTex;
	if (!playerTex.loadFromFile("Textures/player.png"))
		throw("Could not load playerBody texture!");
	Texture player1Tex;
	player1Tex.loadFromFile("Textures/player1.png");


	Vector2f player1StartingPosition;
	Vector2f player2StartingPosition;

	Vector2f spawnPoints[10] = { Vector2f(105.f, 180.f), Vector2f(195.f, 110.f), Vector2f(105.f, 520.f), Vector2f(190.f, 590.f), Vector2f(410.f, 350.f),
								 Vector2f(810.f, 110.f), Vector2f(890.f, 180.f), Vector2f(890.f, 490.f), Vector2f(800.f, 590.f), Vector2f(590.f, 350.f) };
	int random;


	if (GAME_MODE == 0 || GAME_MODE == 2)
	{
		player1StartingPosition = Vector2f(obstacles[2].getPosition().x, obstacles[2].getPosition().y + CREST_SIZE / 2 + PLAYER_RADIUS + 20);
		//player1StartingPosition = Vector2f(obstacles[4].getPosition().x - 140, obstacles[4].getPosition().y - CREST_SIZE / 2 + PLAYER_RADIUS - 80);
		player2StartingPosition = Vector2f(obstacles[1].getPosition().x , obstacles[1].getPosition().y - CREST_SIZE / 2 - PLAYER_RADIUS - 20);
		//player2StartingPosition = Vector2f(obstacles[1].getPosition().x - 30, obstacles[1].getPosition().y - CREST_SIZE / 2 - PLAYER_RADIUS - 20);
	}
	else if (GAME_MODE == 1)
	{
		//player1StartingPosition = Vector2f(obstacles[1].getPosition().x, obstacles[1].getPosition().y - CREST_SIZE / 2 - PLAYER_RADIUS - 20);
		//player2StartingPosition = Vector2f(obstacles[2].getPosition().x, obstacles[2].getPosition().y + CREST_SIZE / 2 + PLAYER_RADIUS + 20);
		int side = rand() % 2;
		random = rand() % 5;
		player1StartingPosition = spawnPoints[side * 5 + random];
		player2StartingPosition = spawnPoints[((side + 1) % 2) * 5 + (4 - random)];

	}
	
	//Vector2f player2StartingPosition = 
	Player player1(&playerTex, player1StartingPosition, player2StartingPosition);
	Player player2(&playerTex, player2StartingPosition, player1StartingPosition);

	bool startRecording = false;
	joyPosition = player1.getPosition();
	bool player2CanSee = false;
	bool player1CanSee = false;

	int botState = 0;
	int prevBotState = -1;
	int botFreezeTime = 0;
	int botRunningDelay = 0;
	std::vector <Vector2f> botPositionHistory;
	Vector2f botPointDestination = Vector2f(0.f, 0.f);

	int crosshairRandom = 0;
	bool mouseButtonPressed = false;
	int bot1FreezeTime = 0;



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN +
	///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	

	bool firstFrame = true;
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		
		if (gameRunning == true)
		{

			//==============================================================================================================
			//=============================================== KEYBOARD =====================================================
			//==============================================================================================================


			mousePosition = Vector2f(Mouse::getPosition(window));
			if (mousePosition.x < 0)
				mousePosition.x = 0;
			else if (mousePosition.x > FLOOR_WIDTH)
				mousePosition.x = FLOOR_WIDTH;
			if (mousePosition.y < 0)
				mousePosition.y = 0;
			else if (mousePosition.y > FLOOR_HEIGHT)
				mousePosition.y = FLOOR_HEIGHT;

			Vector2f movDirection = Vector2f(0, 0);

			if (Keyboard::isKeyPressed(Keyboard::A))
				movDirection += Vector2f(-1, 0);
			if (Keyboard::isKeyPressed(Keyboard::D))
				movDirection += Vector2f(1, 0);
			if (Keyboard::isKeyPressed(Keyboard::W))
				movDirection += Vector2f(0, -1);
			if (Keyboard::isKeyPressed(Keyboard::S))
				movDirection += Vector2f(0, 1);

			if (distance(Vector2f(0, 0), movDirection) != 0)
				movDirection = movDirection / distance(Vector2f(0, 0), movDirection);

			player1.move(PLAYER_SPEED * movDirection);

			if (Keyboard::isKeyPressed(Keyboard::E) && player1.isWeaponChanged())
				player1.changeWeapon();

			if (movDirection != Vector2f(0.f, 0.f))
				startRecording = true;


			//==============================================================================================================
			//=============================================== JOYSTICK =====================================================
			//==============================================================================================================

			
			movDirection = Vector2f(0, 0);
			bool presJoyTriggerState = false;
			
			if (firstFrame == false && GAME_MODE == 2)  // ======================== bot imitation learning
			{
				float random;
				player2.printState(player1.getLeftEnemySideVisible(), player1.getRightEnemySideVisible());
				// python prediction here
				float inputControl[5];
				for (int i = 0; i < 5; i++)
					std::cin >> inputControl[i];
				std::cin.clear();

				movDirection = Vector2f(inputControl[0], inputControl[1]);
				if (distance(Vector2f(0, 0), movDirection) != 0)
					movDirection = movDirection / distance(Vector2f(0, 0), movDirection);

				Vector2f aimDir = Vector2f(inputControl[2], inputControl[3]);
				float enemyDistance = distance(player1.getPosition(), player2.getWeapPosition());
				joyPosition = player2.getWeapPosition() + aimDir * enemyDistance;

				player2.move(PLAYER_SPEED * movDirection);


				random = (rand() % 1000) / 1000.f;
				if (inputControl[4] > random)
					presJoyTriggerState = true;
				else
					presJoyTriggerState = false;
			}
			

			//==============================================================================================================
			//=============================================== RANDOM BOT ===================================================
			//==============================================================================================================

			
			if (GAME_MODE == 1) // ======================== player vs bot
			{
				Vector2f enemyDir =  player2.getPosition() - player1.getWeapPosition();
				enemyDir = enemyDir / distance(Vector2f(0.f, 0.f), enemyDir);
				Vector2f enemyDirRight = Vector2f(-enemyDir.y, enemyDir.x);
				Vector2f enemyDirLeft = Vector2f(enemyDir.y, -enemyDir.x);

				crosshairRandom = 0;
				for (int i = 0; i < 9; i++)
					crosshairRandom += rand() % 5 - 2;
				crosshairRandom = crosshairRandom / 9;

				float leftEnemySideVisible = player1.getLeftEnemySideVisible();
				float rightEnemySideVisible = player1.getRightEnemySideVisible();
			
			}
			

			if (firstFrame == false && GAME_MODE == 1) // ======================== player vs bot
			{
				movDirection = Vector2f(Keyboard::isKeyPressed(Keyboard::D) - Keyboard::isKeyPressed(Keyboard::A), Keyboard::isKeyPressed(Keyboard::S) - Keyboard::isKeyPressed(Keyboard::W));
				if (distance(Vector2f(0, 0), movDirection) != 0)
					movDirection = movDirection / distance(Vector2f(0, 0), movDirection);
				mouseButtonPressed = Mouse::isButtonPressed(sf::Mouse::Left);
				Vector2f aimDirection = mousePosition - player1.getWeapPosition();
				if (distance(Vector2f(0, 0), aimDirection) != 0)
					aimDirection = aimDirection / distance(Vector2f(0, 0), aimDirection);
				player1.saveToFile(player2.getLeftEnemySideVisible(), player2.getRightEnemySideVisible(), movDirection, aimDirection, mouseButtonPressed);
			}	
			firstFrame = false;
			
			
			if (GAME_MODE == 1)  // ======================== player vs bot
			{
				Vector2f enemyDir =  player1.getPosition() - player2.getWeapPosition();
				enemyDir = enemyDir / distance(Vector2f(0.f, 0.f), enemyDir);
				Vector2f enemyDirRight = Vector2f(-enemyDir.y, enemyDir.x);
				Vector2f enemyDirLeft = Vector2f(enemyDir.y, -enemyDir.x);

				crosshairRandom = 0;
				for (int i = 0; i < 9; i++)
					crosshairRandom += rand() % 5 - 2;
				crosshairRandom = crosshairRandom / 9;

				float leftEnemySideVisible = player2.getLeftEnemySideVisible();
				float rightEnemySideVisible = player2.getRightEnemySideVisible();
				if (leftEnemySideVisible != 0 && rightEnemySideVisible == 0)
					joyPosition = player1.getPosition() + enemyDirLeft * 20.f + Vector2f(crosshairRandom, crosshairRandom);
				else if (leftEnemySideVisible == 0 && rightEnemySideVisible != 0)
					joyPosition = player1.getPosition() + enemyDirRight * 20.f + Vector2f(crosshairRandom, crosshairRandom);
				else if (leftEnemySideVisible != 0 && rightEnemySideVisible != 0)
					joyPosition = player1.getPosition() + enemyDirLeft * leftEnemySideVisible * 20.f + enemyDirRight * rightEnemySideVisible * 20.f + Vector2f(crosshairRandom, crosshairRandom);
				else
					joyPosition = player1.getPosition() + Vector2f(crosshairRandom, crosshairRandom);

				if (botState == -1) // running to point
				{
					Vector2f moveDir = botPointDestination - player2.getPosition();
					moveDir = moveDir / distance(Vector2f(0.f, 0.f), moveDir);
					player2.move(PLAYER_SPEED * moveDir);
					botPositionHistory.push_back(player2.getPosition());

					if (player2CanSee && player2.canShoot())
					{
						player2.shoot();
						player1.enemyShot(player2);
					}

					if (distance(player2.getPosition(), botPointDestination) < 3.f)
						botState = 0;
				}
				if (botState == 0) // running towards enemy
				{
					Vector2f enemyDir = player1.getPosition() - player2.getPosition();
					enemyDir = enemyDir / distance(Vector2f(0.f, 0.f), enemyDir);
					player2.move(PLAYER_SPEED * enemyDir);
					botPositionHistory.push_back(player2.getPosition());
					if (player2CanSee == true)
					{
						if (botRunningDelay <= 0)
							botRunningDelay = rand() % 5 + 5;
						else
						{
							botRunningDelay--;
							if (botRunningDelay <= 0)
							{
								botState = 3;
								prevBotState = 0;
							}
						}
					}
					else
						botRunningDelay = 0;
				}
				else if (botState == 1) // enemy spotted and trying to shot
				{
					if (player2.canShoot() && player2CanSee == true)
					{
						player2.shoot();
						player1.enemyShot(player2);
						botState = 3;
						prevBotState = 1;
					}
				}
				else if (botState == 2) // already shot and trying to hide
				{
					player2.setPosition(botPositionHistory.back());
					botPositionHistory.pop_back();
					if (player1CanSee == false)
					{
						botState = 3;
						prevBotState = 2;
					}
					else if (player2CanSee && player2.canShoot())
					{
						player2.shoot();
						player1.enemyShot(player2);
					}
				}
				else if (botState == 3) // change next state
				{
					if (botFreezeTime <= 0)
						botFreezeTime = rand() % 5 + 3;
					else
					{
						botFreezeTime--;
						if (botFreezeTime <= 0)
						{
							if (prevBotState == 1)
							{
								random = rand() % 5;
								if (random == 0)
									botState = 1;
								else
									botState = 2;
							}
							else if (prevBotState == 2)
							{
								random = rand() % 10;
								if (random <= 8)
								{
									Vector2f botPointDestination = spawnPoints[random];
									botState == -1;
								}
								else
									botState = 0;
							}
							else 
								botState = (prevBotState + 1) % 3;
						}
					}
				}
			}
			
			

			if (GAME_MODE == 0) // ======================== player vs player
			{
				// aim 
				joyAim.x = Joystick::getAxisPosition(0, sf::Joystick::U);
				if (joyAim.x < 25 && joyAim.x > -25)
					joyAim.x = 0;
				joyAim.y = Joystick::getAxisPosition(0, sf::Joystick::R);
				if (joyAim.y < 25 && joyAim.y > -25)
					joyAim.y = 0;

				joyPosition += joyAimSpeed * joyAim;

				if (joyPosition.x <= 0)
					joyPosition.x = 0;
				else if (joyPosition.x >= FLOOR_WIDTH)
					joyPosition.x = FLOOR_WIDTH;
				if (joyPosition.y <= 0)
					joyPosition.y = 0;
				else if (joyPosition.y >= FLOOR_HEIGHT)
					joyPosition.y = FLOOR_HEIGHT;

				// movement 
				joyMov.x = Joystick::getAxisPosition(0, sf::Joystick::X);
				if (joyMov.x < 25 && joyMov.x > -25)
					joyMov.x = 0;
				joyMov.y = Joystick::getAxisPosition(0, sf::Joystick::Y);
				if (joyMov.y < 25 && joyMov.y > -25)
					joyMov.y = 0;

				player2.move(PLAYER_SPEED * joyMov / 100.f);

				if (Joystick::isButtonPressed(0, 5) && player2.isWeaponChanged())
					player2.changeWeapon();
			}

			
			//==============================================================================================================
			//=============================================== COLLISIONS ===================================================
			//==============================================================================================================


			// player 2
			for (int i = 0; i < obstacles.size(); i++)
				player2.obstacleCollisionMove(obstacles[i]);
			player2.enemyCollisionMove(player1.getPosition());
			player2.outOfBoundsMove(floor);

			// player 1
			for (int i = 0; i < obstacles.size(); i++)
				player1.obstacleCollisionMove(obstacles[i]);

			player1.enemyCollisionMove(player2.getPosition());
			player1.outOfBoundsMove(floor);


			//==============================================================================================================
			//=============================================== AIM UPDATE ===================================================
			//==============================================================================================================

			
			// player 1
			player1.updateRotation(mousePosition);
			crosshair1.setPosition(mousePosition);

			// player 2
			player2.updateRotation(joyPosition);
			crosshair2.setPosition(joyPosition);


			//==============================================================================================================
			//=============================================== SHOOTING =====================================================
			//==============================================================================================================

		
			// player 1
			if (GAME_MODE == 0 || GAME_MODE == 2)
				mouseButtonPressed = Mouse::isButtonPressed(sf::Mouse::Left);
			
			player1.updateCounters(mousePosition);
			if (player1.isWeaponChanged())
			{
				if (prevMouseButtonState == false && player1.canShoot() && mouseButtonPressed )
				{
					player1.shoot();
					player2.enemyShot(player1);
					//m4a1.play();
				}
			}
			prevMouseButtonState = mouseButtonPressed;
			player1.updateBullet(floor, obstacles, player2);

			// player 2
			player2.updateCounters(joyPosition);
			
			if (GAME_MODE == 0)
			{
				if (Joystick::getAxisPosition(0, sf::Joystick::Z) < -90.f)
					presJoyTriggerState = true;
				else
					presJoyTriggerState = false;
			}

			if (player2.isWeaponChanged())
			{
				if (prevJoyTriggerState == false && player2.canShoot() && presJoyTriggerState == true)
				{
					player2.shoot();
					player1.enemyShot(player2);
					m4a1.play();
				}
			}
			prevJoyTriggerState = presJoyTriggerState;
			player2.updateBullet(floor, obstacles, player1);


			// player 1
			if (player1.getHp() > 0)
				healthText1.setString(std::to_string(player1.getHp()));
			else
			{
				healthText1.setString("DEAD");
				gameOver.setString("PLAYER 2 WINS");
				gameRunning = false;
			}


			// player 2
			if (player2.getHp() > 0)
				healthText2.setString(std::to_string(player2.getHp()));
			else
			{
				healthText2.setString("DEAD");
				gameOver.setString("PLAYER 1 WINS");
				gameRunning = false;
			}


			//==============================================================================================================
			//=============================================== DRAW =========================================================
			//==============================================================================================================

			window.clear(Color::White);
			// obstacles
			window.draw(floor);
			for (int i = 0; i < obstacles.size(); i++)
				window.draw(obstacles[i]);

			// players
			window.draw(player1.getTex());
			window.draw(player2.getTex());

			
			// bullets
			if (player1.getBullet() != NULL)
				window.draw(player1.getBullet()->getShape());
			if (player2.getBullet() != NULL)
				window.draw(player2.getBullet()->getShape());


			// shotgun range
			if (player1.getCurrentWeapon() == 2)
				window.draw(player1.getShotgunRangeShape());
			if (player2.getCurrentWeapon() == 2)
				window.draw(player2.getShotgunRangeShape());

			
			// HUD draw
			window.draw(whiteRect);
			window.draw(horizontalLine);
			window.draw(verticalLine);

			window.draw(player1Text);
			window.draw(healthCross1);
			window.draw(healthText1);

			window.draw(player2Text);
			window.draw(healthCross2);
			window.draw(healthText2);


			
			player1.drawView(obstacles, window, Color(0, 255, 0, 100));
			player2.drawView(obstacles, window, Color(0, 255, 255, 100));

			


			//==============================================================================================================
			//=============================================== VISION =======================================================
			//==============================================================================================================


			player1CanSee = player1.canSee(player2, obstacles, window, Color(0, 255, 255, 100));
			if (player1CanSee)
				player2.setFillColor(Color(255, 255, 255, 255));
			else
				player2.setFillColor(Color(255, 255, 255, 100));



			player2CanSee = player2.canSee(player1, obstacles, window, Color(0, 255, 255, 100));
			if (player2CanSee)
				player1.setFillColor(Color(255, 255, 255, 255));
			else
				player1.setFillColor(Color(255, 255, 255, 100));


			// crosshairs
			if (player1.isWeaponChanged())
				crosshair1.setTexture(&crosshairTex);
			else
				crosshair1.setTexture(&crosshairTex2);

			if (player2.isWeaponChanged())
				crosshair2.setTexture(&crosshairTex);
			else
				crosshair2.setTexture(&crosshairTex2);

			window.draw(crosshair1);
			window.draw(crosshair2);

			window.draw(gameOver);
			
			window.display();

		}
	}
	return 0;
}
