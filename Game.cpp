#include <iostream>
#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"

const float Game::PlayerSpeed = 100.f;
//const sf::Time Game::TimePerFrame = sf::seconds(20.f);
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(840, 600), "Donkey Kong 1981", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mPrincess()
	, princessTexture()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
	, mIsJump(false)
	, mRestart(false)
	, score(0)
	, bestScore(0)
	, status(GameStatus::inGame)
{
	mWindow.setFramerateLimit(160);

	// Draw blocks

	_TextureBlock.loadFromFile("Media/Textures/Block.png");
	_sizeBlock = _TextureBlock.getSize();

	_TextureCoin.loadFromFile("Media/Textures/Piece.png");

	for (int j = 0; j < BLOCK_COUNT_Y; j++)
	{
		for (int i = 0; i < BLOCK_COUNT_X; i++)
		{
			_Block[i][j].setTexture(_TextureBlock);
			_Block[i][j].setPosition(100.f + 70.f * (i + 1), 0.f + BLOCK_SPACE * (j + 1));

			std::shared_ptr<Entity> se = std::make_shared<Entity>();
			se->m_sprite = _Block[i][j];
			se->m_type = EntityType::block;
			se->m_size = _TextureBlock.getSize();
			se->m_position = _Block[i][j].getPosition();
			EntityManager::m_Entities.push_back(se);
		}

		for (int i = 0; i < 3; i++) {

			_Coin[i][j].setTexture(_TextureCoin);
			_Coin[i][j].setPosition(100.f + 70.f * ((i * 2) + 2), 0.f + BLOCK_SPACE * (j + 1) - _TextureCoin.getSize().y);

			std::shared_ptr<Entity> se = std::make_shared<Entity>();
			se->m_sprite = _Coin[i][j];
			se->m_type = EntityType::coin;
			se->m_size = _TextureCoin.getSize();
			se->m_position = _Coin[i][j].getPosition();
			EntityManager::m_Entities.push_back(se);
		}
	}

	// Draw Echelles

	_TextureEchelle.loadFromFile("Media/Textures/Echelle.png");

	std::cout << _sizeBlock.y << std::endl;
	std::cout << _TextureEchelle.getSize().x << std::endl;
	std::cout << _TextureEchelle.getSize().y << std::endl;

	for (int i = 0; i < ECHELLE_COUNT; i++)
	{
		_Echelle[i].setTexture(_TextureEchelle);
		_Echelle[i].setPosition(100.f + 70.f * (i + 1), 0.f + BLOCK_SPACE * (i + 1) + _sizeBlock.y);

		std::shared_ptr<Entity> se = std::make_shared<Entity>();
		se->m_sprite = _Echelle[i];
		se->m_type = EntityType::echelle;
		se->m_size = _TextureEchelle.getSize();
		se->m_position = _Echelle[i].getPosition();
		EntityManager::m_Entities.push_back(se);
	}

	// Draw Mario

	mTexture.loadFromFile("Media/Textures/Mario_small_transparent.png"); // Mario_small.png");
	_sizeMario = mTexture.getSize();
	mPlayer.setTexture(mTexture);
	sf::Vector2f posMario;
	posMario.x = 100.f + 70.f;
	posMario.y = BLOCK_SPACE * 5 - _sizeMario.y;

	mPlayer.setPosition(posMario);

	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = mTexture.getSize();
	player->m_position = mPlayer.getPosition();
	EntityManager::m_Entities.push_back(player);


	// Draw Princess

	princessTexture.loadFromFile("Media/Textures/princess.png");
	_sizePrincess = princessTexture.getSize();
	mPrincess.setTexture(princessTexture);
	sf::Vector2f posPrincess;
	posPrincess.x = 100.f + 70.f * (BLOCK_COUNT_X + 1) - _sizePrincess.x;
	posPrincess.y = BLOCK_SPACE - _sizePrincess.y;

	mPrincess.setPosition(posPrincess);

	std::shared_ptr<Entity> princess = std::make_shared<Entity>();
	princess->m_sprite = mPrincess;
	princess->m_type = EntityType::princess;
	princess->m_size = mTexture.getSize();
	princess->m_position = mPrincess.getPosition();
	EntityManager::m_Entities.push_back(princess);

	// Draw Statistic Font 

	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setString("Welcome to Donkey Kong 1981");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update(sf::Time elapsedTime)
{
	sf::Vector2f finalMove(0.f, 0.f);

	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp)
		movement.y -= PlayerSpeed;
	if (mIsMovingDown)
		movement.y += PlayerSpeed;
	if (mIsMovingLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight)
		movement.x += PlayerSpeed;

	std::shared_ptr<Entity> player = EntityManager::GetPlayer();
	sf::FloatRect playerBound = player->m_sprite.getGlobalBounds();
	sf::Vector2f wantedMove = movement * elapsedTime.asSeconds();
	finalMove.x = wantedMove.x;

	if (mRestart && status != GameStatus::inGame) {

		restart(player);
	}

	// Si le joueur à perdu
	if (playerBound.top > 600) {

		status = GameStatus::loss;

		return;
	}

	bool inBlock = false;
	bool inEchelle = false;
	sf::FloatRect inBlockElement;
	sf::FloatRect inEchelleElement;

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		sf::FloatRect entityBound = entity->m_sprite.getGlobalBounds();

		if (entity->m_type == EntityType::player || entity->m_enabled == false)

			continue;

		if (playerBound.intersects(entityBound)) {

			if (entity->m_type == EntityType::echelle) {

				inEchelle = true;
				inEchelleElement = entityBound;
			}

			if (entity->m_type == EntityType::block) {

				if (playerBound.top + playerBound.height <= entityBound.top + entityBound.height) {

					inBlock = true;
					inBlockElement = entityBound;
				}
			}

			if (entity->m_type == EntityType::princess) {

				status = GameStatus::win;

				std::cout << "Victory" << std::endl;

				return;
			}

			if (entity->m_type == EntityType::coin) {

				entity->m_enabled = false;
				score += 10;
			}
		}
	}

	if (inEchelle && inBlock) {

		std::cout << "in echelle and in block" << std::endl;

		if (inEchelleElement.top != inBlockElement.top && wantedMove.y < 0) {

			finalMove.y = wantedMove.y;
		}
	}
	else if (inEchelle) {

		finalMove.y = wantedMove.y;
	}
	else if (inBlock) {

		if (playerBound.top + playerBound.height > inBlockElement.top && wantedMove.y < 0) {

			finalMove.y = wantedMove.y;
		}

		if (mIsJump)
			finalMove.y = -40;

	}else {
		
		finalMove.y = 3;
	}

	updatePlayerTexture(player, finalMove);
	

	player->m_position += finalMove;
	player->m_sprite.move(finalMove);
}

void Game::render()
{
	mWindow.clear();

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		mWindow.draw(entity->m_sprite);
	}

	if (status != GameStatus::inGame) {

		if (status == GameStatus::win) {

			mWindow.draw(createSfText("You win", sf::Color::Yellow, 400, 260, 24));
		}

		if (status == GameStatus::loss) {

			mWindow.draw(createSfText("You loss", sf::Color::Yellow, 400, 260, 24));
		}

		mWindow.draw(createSfText("To restart press R", sf::Color::Red, 400, 290, 24));
		
	}

	mWindow.draw(createSfText("Score: " + std::to_string(score), sf::Color::Green, 20, 40, 16));

	mWindow.draw(createSfText("Best score: " + std::to_string(bestScore), sf::Color::Green, 20, 60, 16));

	mWindow.draw(mStatisticsText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}

	//
	// Handle collision
	//

	if (mStatisticsUpdateTime >= sf::seconds(0.050f))
	{
		// Handle collision weapon enemies
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::Up)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::Down)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::Left)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::Right)
		mIsMovingRight = isPressed;

	if (key == sf::Keyboard::Space)
	{
		mIsJump = isPressed;
	}

	if (key == sf::Keyboard::R)
		mRestart = isPressed;
}

sf::Text Game::createSfText(std::string text, sf::Color color, float x, float y, int fontSize) {

	sf::Text sfText;
	sfText.setString(text);
	sfText.setCharacterSize(18);
	sfText.setFillColor(color);
	sfText.setStyle(sf::Text::Bold);
	sfText.setPosition(x, y);
	sfText.setFont(mFont);

	return sfText;
}

void Game::restart(std::shared_ptr<Entity> player){

	status = GameStatus::inGame;

	sf::Vector2f startPos;
	startPos.x = 100.f + 70.f;
	startPos.y = BLOCK_SPACE * 5 - _sizeMario.y;

	if (score > bestScore)
		bestScore = score;

	score = 0;
	player->m_sprite.setPosition(startPos);
}

void Game::updatePlayerTexture(std::shared_ptr<Entity> player, sf::Vector2f move) {

	if (move.y != 0) {

		mTexture.loadFromFile("Media/Textures/Mario_small_transparent.png");
		player->m_sprite.setTexture(mTexture);

	}
	else if (move.x != 0) {
		mTexture.loadFromFile("Media/Textures/Mario1.png");
		player->m_sprite.setTexture(mTexture);
	}
	else {

		mTexture.loadFromFile("Media/Textures/Mario2.png");
		player->m_sprite.setTexture(mTexture);
	}
}
