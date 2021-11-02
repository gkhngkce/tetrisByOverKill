#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <list>

using namespace sf;
using namespace std;

//Game field variables
const int fieldRow = 20;
const int fieldColumn = 10;

//Defining the game field
int gameField[fieldRow][fieldColumn] = { 0 };
list <int> tetrominoBatch;

// ??? point of tetrominos ???
struct Point
{
	int x, y;
} a[4], b[4];

//Defining tetrominos
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

Text gameOverText;
RectangleShape textBackgroundRect;
FloatRect gameOverTextRect;
Font font;

int loadResources()
{
	// Metinler icin kullanacagimiz font
	if (!font.loadFromFile("sources/sansation.ttf"))
	{
		cout << "Font yuklenmesinde hata!" << endl;
		return -1;
	}
	return 0;
}

int initialize()
{
	if (0 != loadResources())
	{
		return -1;
	}


	// message for game over
	gameOverText.setPosition({ 320 / 2, 480 / 2 });
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(40);
	gameOverText.setFillColor(Color::Green);
	gameOverText.setString("  Game Over ");


	// text center
	gameOverTextRect = gameOverText.getLocalBounds();
	gameOverText.setOrigin(gameOverTextRect.left + gameOverTextRect.width / 2.0f,gameOverTextRect.top + gameOverTextRect.height / 2.0f);

	// background for text
	textBackgroundRect = sf::RectangleShape{ sf::Vector2f{ gameOverTextRect.width + 10, gameOverTextRect.height + 30} };
	textBackgroundRect.setFillColor(sf::Color(0, 0, 0, 200));
	textBackgroundRect.setPosition({ 320 / 2, 480 / 2 });
	textBackgroundRect.setOrigin(gameOverTextRect.left + gameOverTextRect.width / 2.0f,
		gameOverTextRect.top + gameOverTextRect.height / 2.0f);
}
//Collusion detection function
bool collisionDetection()
{
	for (int i = 0; i < 4; i++)
	{
		if (a[i].x < 0 || a[i].x >= fieldColumn || a[i].y >= fieldRow)
		{
			return 0;
		}
		else if (gameField[a[i].y][a[i].x])
		{
			return 0;
		}
	}
	return 1;
};

int rotate(int currTetromino)
{
	if (currTetromino != 7)
	{
		Point centerOfRotation = a[1]; //center of rotation
		for (int i = 0; i < 4; i++)
		{
			int x = a[i].y - centerOfRotation.y;
			int y = a[i].x - centerOfRotation.x;
			a[i].x = centerOfRotation.x - x;
			a[i].y = centerOfRotation.y + y;
		}
		if (!collisionDetection())
		{
			for (int i = 0; i < 4; i++)
			{
				a[i] = b[i];
			}
		}
	}
	return 0;
};

void createTetrominoBatch()
{
	tetrominoBatch.clear();
	do
	{
		int randomPiece = rand() % 7;
		if (std::find(tetrominoBatch.begin(), tetrominoBatch.end(), randomPiece) == tetrominoBatch.end())
		{
			tetrominoBatch.push_back(randomPiece);
		}
	} while (tetrominoBatch.size() <= 6);
}

int newTetromino()
{
	if (tetrominoBatch.empty())
	{
		createTetrominoBatch();
	}
	int currentTetromino = tetrominoBatch.back();
	tetrominoBatch.pop_back();
	int colorNum = currentTetromino + 1;
	for (int i = 0; i < 4; i++)
	{
		a[i].x = (tetrominos[currentTetromino][i] % 2) + (fieldColumn / 2) - 1;//defining x coordinate of tetromino & centeralizing the tetromino
		a[i].y = tetrominos[currentTetromino][i] / 2; //defining y coordinate of tetromino
	}
	return colorNum;
}


int gameWindow()
{
	srand(time(0));
	RenderWindow window(VideoMode(320, 480), "The Game!");
	Texture tiles, bground, outFrame;
	tiles.loadFromFile("sources/tiles.png");
	bground.loadFromFile("sources/background.png");
	outFrame.loadFromFile("sources/frame.png");
	Sprite sprite(tiles), background(bground), frame(outFrame);
	Clock clock;
	initialize();

	int dx = 0;
	bool isRotated = 0;
	bool isGameOver = false;
	int colorNum = newTetromino();
	float timer = 0, delay = 0.3;

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Up)
				{
					cout << "Current Tetromino: " << colorNum;
					rotate(colorNum);
				}
				if (event.key.code == Keyboard::Left)
				{
					dx = -1;
				}
				if (event.key.code == Keyboard::Right)
				{
					dx = 1;
				}
			}
		}
		if (!isGameOver)
		{
			if (Keyboard::isKeyPressed(Keyboard::Down))
			{
				delay = 0.05;
			}

			// <- Move ->
			for (int i = 0; i < 4; i++)
			{
				b[i] = a[i];
				a[i].x += dx;
			}
			if (!collisionDetection())
			{
				for (int i = 0; i < 4; i++)
				{
					a[i] = b[i];
				}
			}

			//Vertical motion
			if (timer > delay)
			{
				for (int i = 0; i < 4; i++)
				{
					b[i] = a[i];
					a[i].y += 1;
				}
				if (!collisionDetection())
				{
					for (int i = 0; i < 4; i++)
					{
						gameField[b[i].y][b[i].x] = colorNum;
					}
					//random tetromino generation and coloring
					colorNum = newTetromino();
				}
				timer = 0;
			}

			//Line clear check
			int k = fieldRow - 1;
			for (int i = fieldRow - 1; i > 0; i--)
			{
				int count = 0;
				for (int j = 0; j < fieldColumn; j++)
				{
					if (gameField[i][j])
					{
						count++;
					}
					gameField[k][j] = gameField[i][j];
				}
				if (count < fieldColumn)
				{
					k--;
				}
			}
			for (int i = 0; i < fieldColumn; i++)
			{
				if (gameField[0][i] || gameField[1][i])
				{
					isGameOver = true;
					break;
				}
			}
		}

		dx = 0;
		isRotated = 0;
		delay = 0.3;

		//Drawing to the screen
		window.clear(Color::White);
		window.draw(background);

		for (int i = 0; i < fieldRow; i++)
		{
			for (int j = 0; j < fieldColumn; j++)
			{
				if (gameField[i][j] == 0)
				{
					continue;
				}
				sprite.setTextureRect(IntRect(gameField[i][j] * 18, 0, 18, 18));
				sprite.setPosition(j * 18, i * 18);
				sprite.move(28, 31); //offset
				window.draw(sprite);
			}
		}
		for (int i = 0; i < 4; i++)
		{
			sprite.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
			sprite.setPosition(a[i].x * 18, a[i].y * 18);
			sprite.move(28, 31); //offset
			window.draw(sprite);
		}
		if ( isGameOver)
		{
			window.draw(textBackgroundRect);
			window.draw(gameOverText);
		}
		else
		{
				window.draw(frame);
				
		}
		window.display();
	}
	return 0;
}

int main()
{
	int choice;
	cout << "\n\n\t\t\t\tWelcome to the Tetris by the team OverKill\n\n\n\n\n\n";
	cout << "\t\t\t\t\t1- Start Game\n\n";
	cout << "\t\t\t\t\t2- Exit\n\n";
	cout << "\t\t\t\t\t3- High Scores(Feature)\n\n";

	cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\nYour choice? :";
	cin >> choice;

	if (choice == 1)
	{
		gameWindow();
	}

	return 0;
}
