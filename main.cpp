#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
using namespace sf;
using namespace std;

const int fieldRow = 20;
const int fieldColumn = 10;

int gameField[fieldRow][fieldColumn] = { 0 };

struct Point
{
	int x, y;
} a[4], b[4];

int tetrominos[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};


bool collisionDetection()
{
	for (int i = 0; i < 4; i++)
		if (a[i].x < 0 || a[i].x >= fieldColumn || a[i].y >= fieldRow) return 0;
		else if (gameField[a[i].y][a[i].x]) return 0;

	return 1;
};

//////Rotat//////
int rotate()
{
	Point centerOfRotation = a[1]; //center of rotation
	for (int i = 0; i < 4; i++)
	{
		int x = a[i].y - centerOfRotation.y;
		int y = a[i].x - centerOfRotation.x;
		a[i].x = centerOfRotation.x - x;
		a[i].y = centerOfRotation.y + y;
	}
	if (!collisionDetection()) for (int i = 0; i < 4; i++) a[i] = b[i];

	return 0;
};

int menuScreen()
{
	RenderWindow menuWindow(VideoMode(320, 480), "The Menu!");

	while (menuWindow.isOpen())
	{
		Event event;
		while (menuWindow.pollEvent(event))
		{
			if (event.type == Event::Closed)
				menuWindow.close();

			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Up) ;
				if (event.key.code == Keyboard::Down) ;
				if (event.key.code == Keyboard::Enter) theGame();
			}
		}

		menuWindow.clear(Color::White);
		// End the current frame and display its contents on screen
		menuWindow.display();
	}

	return 0;
}

int theGame()
{
	srand(time(0));
	RenderWindow gameWindow(VideoMode(320, 480), "The Game!");
	Texture tiles, bground, outFrame;
	tiles.loadFromFile("sources/tiles.png");
	bground.loadFromFile("sources/background.png");
	outFrame.loadFromFile("sources/frame.png");

	Sprite sprite(tiles), background(bground), frame(outFrame);

	int dx = 0, colorNum = 1;
	float timer = 0, delay = 0.3;

	Clock clock;

	while (gameWindow.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		Event event;
		while (gameWindow.pollEvent(event))
		{
			if (event.type == Event::Closed)
				gameWindow.close();

			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Up) rotate();
				if (event.key.code == Keyboard::Left) dx = -1;
				if (event.key.code == Keyboard::Right) dx = 1;
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

		//// <- Move -> ///
		for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].x += dx; }
		if (!collisionDetection()) for (int i = 0; i < 4; i++) a[i] = b[i];

		///////Tick//////
		if (timer > delay)
		{
			for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].y += 1; }

			if (!collisionDetection())
			{
				for (int i = 0; i < 4; i++) gameField[b[i].y][b[i].x] = colorNum;

				// Feature: assign a fixed color to each shape
				int n = rand() % 7;
				colorNum = n + 1;
				for (int i = 0; i < 4; i++)
				{
					a[i].x = tetrominos[n][i] % 2;
					a[i].y = tetrominos[n][i] / 2;
				}
			}

			timer = 0;
		}

		///////collisionDetection lines//////////
		int k = fieldRow - 1;
		for (int i = fieldRow - 1; i > 0; i--)
		{
			int count = 0;
			for (int j = 0; j < fieldColumn; j++)
			{
				if (gameField[i][j]) count++;
				gameField[k][j] = gameField[i][j];
			}
			if (count < fieldColumn) k--;
		}

		dx = 0; isRotated = 0; delay = 0.3;

		/////////draw//////////
		gameWindow.clear(Color::White);
		gameWindow.draw(background);

		for (int i = 0; i < fieldRow; i++)
			for (int j = 0; j < fieldColumn; j++)
			{
				if (gameField[i][j] == 0) continue;
				sprite.setTextureRect(IntRect(gameField[i][j] * 18, 0, 18, 18));
				sprite.setPosition(j * 18, i * 18);
				sprite.move(28, 31); //offset
				gameWindow.draw(sprite);
			}

		for (int i = 0; i < 4; i++)
		{
			sprite.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
			sprite.setPosition(a[i].x * 18, a[i].y * 18);
			sprite.move(28, 31); //offset
			gameWindow.draw(sprite);
		}

		gameWindow.draw(frame);
		gameWindow.display();
	}
	return 0;
}


int main()
{
	menuScreen();
	return 0;
}
