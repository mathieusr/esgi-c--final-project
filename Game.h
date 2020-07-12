#pragma once
#include "Entity.h"

#define ECHELLE_COUNT 4
#define BLOCK_COUNT_X 8
#define BLOCK_COUNT_Y 5
#define BLOCK_SPACE 110.f

enum GameStatus
{
	inGame,
	win,
	loss
};

class Game
{
public:
	Game();
	~Game() { };
	void run();

private:
	void processEvents();
	void update(sf::Time elapsedTime);
	void render();

	void updateStatistics(sf::Time elapsedTime);
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

	sf::Text createSfText(std::string text, sf::Color color, float x, float y, int fontSize);
	void restart(std::shared_ptr<Entity> player);
	void updatePlayerTexture(std::shared_ptr<Entity> player, sf::Vector2f move);
private:
	static const float		PlayerSpeed;
	static const sf::Time	TimePerFrame;

	sf::RenderWindow		mWindow;
	sf::Texture	mTexture;
	sf::Sprite	mPlayer;
	sf::Texture	princessTexture;
	sf::Sprite	mPrincess;
	sf::Font	mFont;
	sf::Text	mStatisticsText;
	sf::Time	mStatisticsUpdateTime;

	std::size_t	mStatisticsNumFrames;
	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingRight;
	bool mIsMovingLeft;
	bool mIsJump;
	bool mRestart;

	int score;
	int bestScore;

	sf::Texture	_TextureEchelle;
	sf::Sprite	_Echelle[ECHELLE_COUNT];
	sf::Texture	_TextureBlock;
	sf::Sprite	_Block[BLOCK_COUNT_X][BLOCK_COUNT_Y];
	sf::Texture	_TextureCoin;
	sf::Sprite _Coin[BLOCK_COUNT_X][3];
	sf::Vector2u _sizeBlock;
	sf::Vector2u _sizeMario;
	sf::Vector2u _sizePrincess;
	GameStatus status;
};

