#include <SFML/Graphics.hpp> //main library for UI
#include <time.h> //for timing and randomizing
#include <iostream> //standart library
#include <list> //list operations
#include <fstream> //file operations
#include <conio.h> // for getch() function(waiting an input before exit)

using namespace sf;
using namespace std;

//Score and related variables
int score = 0;
int totalLinesCleared = 0;
string name;
bool isGameOver = false;

//Game field variables
const int fieldRow = 24;
const int fieldColumn = 10;
int gameField[fieldRow][fieldColumn] = { 0 };

//Textures of gamefield
Texture tiles, bground, outFrame;
Sprite sprite, background, nextTetrominoSprite;
//Gamoever screen variables
Text gameOverText, scoreText, totalLinesClearText;
RectangleShape textBackgroundRect;
FloatRect gameOverTextRect;
Font gameOverFont;

//Defining the batch of tetrominos
list <int> tetrominoBatch;
int nextTetromino = 0;

//points of tetrominos on the screen a=>shown ones b=>hidden ones c=>next tetromino
struct Point
{
	int x, y;
} a[4], b[4], c[4];

//Defining tetrominos
int tetrominos[7][4] =
{
	3,5,4,7, // T - 0
	2,4,5,7, // S - 1
	3,5,4,6, // Z - 2
	2,3,4,5, // O - 3
	1,3,5,7, // I - 4
	2,3,5,7, // L - 5
	3,5,7,6, // J - 6
};

bool wannaContinue(string section)
{
	char decision;
	cout << endl << section << "section has problem with loading files GAME MAY NOT WORK wanna continue anyway(y/n)? :";
	cin >> decision;
	if (decision == 'y' || decision == 'Y')
	{
		return true;
	}
	else
	{
		return false;
	}
}

int loadTextures(Texture& texture, string path)
{
	if (!texture.loadFromFile(path))
	{
		cout << "Failed to load texture from:" << path << endl;
		if (!(wannaContinue("Texture Loading")))
		{
			return -1;
		}
	}
	return 0;
}

void Scoring(int linesCleared)
{
	switch (linesCleared)
	{
	case 1:
		score += 40;
		cout << score << endl;
		break;
	case 2:
		score += 100;
		cout << score << endl;
		break;
	case 3:
		score += 300;
		cout << score << endl;
		break;
	case 4:
		score += 1200;
		cout << score << endl;
		break;
	}
	scoreText.setString(to_string(score));
	totalLinesClearText.setString(to_string(totalLinesCleared));
}

void highscoreWrite(string nickname)
{
	ofstream highscores("highscores.txt", ios::app);
	string savedtext;
	string strScore = to_string(score);
	string strLinesCleared = to_string(totalLinesCleared);
	savedtext = name + ": Highscore = " + strScore + ", Total lines cleared = " + strLinesCleared + "\n";
	highscores << savedtext;
	highscores.close();
}

void showhighScores()
{
	ifstream readHighscores("highscores.txt", ios::in);
	string l;
	while (getline(readHighscores, l))
	{
		cout << l << endl;
	}
	readHighscores.close();
	cout << '\n' << "Press a key to continue...";
	getch();
}

int loadFonts(Font& font, string path)
{
	if (!font.loadFromFile(path))
	{
		cout << "Failed to load font from:" << path << endl;
		if (!(wannaContinue("Font Loading")))
		{
			return -1;
		}
	}
	return 0;
}

int initialize()
{
	if (loadFonts(gameOverFont, "sources/sansation.ttf") != 0 || loadTextures(tiles, "sources/tiles.png") != 0 || loadTextures(bground, "sources/background.png") != 0)
	{
		return -1;
	}
	sprite.setTexture(tiles);
	background.setTexture(bground);
	nextTetrominoSprite.setTexture(tiles);

	//Message for game over
	gameOverText.setPosition({ 320 / 2, 480 / 2 });
	gameOverText.setFont(gameOverFont);
	gameOverText.setCharacterSize(40);
	gameOverText.setFillColor(Color::Green);
	gameOverText.setString(" Game Over ");

	//Score text on the right sight
	scoreText.setPosition(225, 287);
	scoreText.setFont(gameOverFont);
	scoreText.setCharacterSize(15);
	scoreText.setFillColor(Color::Black);
	scoreText.setString(to_string(score));

	//Total lines cleared text on the right sight
	totalLinesClearText.setPosition(225, 315);
	totalLinesClearText.setFont(gameOverFont);
	totalLinesClearText.setCharacterSize(15);
	totalLinesClearText.setFillColor(Color::Black);
	totalLinesClearText.setString(to_string(totalLinesCleared));

	//Centeralizing the text
	gameOverTextRect = gameOverText.getLocalBounds();
	gameOverText.setOrigin(gameOverTextRect.left + gameOverTextRect.width / 2.0f, gameOverTextRect.top + gameOverTextRect.height / 2.0f);

	//Text background
	textBackgroundRect = RectangleShape{ Vector2f{ gameOverTextRect.width + 10, gameOverTextRect.height + 30 } };
	textBackgroundRect.setFillColor(sf::Color(0, 0, 0, 200));
	textBackgroundRect.setPosition({ 320 / 2, 480 / 2 });
	textBackgroundRect.setOrigin(gameOverTextRect.left + gameOverTextRect.width / 2.0f,
		gameOverTextRect.top + gameOverTextRect.height / 2.0f);
	return 0;
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
	if (currTetromino != 4)
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
}

int counterRotate(int currTetromino)
{
	rotate(currTetromino);
	rotate(currTetromino);
	rotate(currTetromino);
	return 0;
}

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
	int createdTetromino = tetrominoBatch.back();
	tetrominoBatch.pop_back();
	int currentTetromino = createdTetromino + 1;
	for (int i = 0; i < 4; i++)
	{
		a[i].x = (tetrominos[createdTetromino][i] % 2) + (fieldColumn / 2) - 1;//defining x coordinate of tetromino & centeralizing the tetromino
		a[i].y = tetrominos[createdTetromino][i] / 2; //defining y coordinate of tetromino
	}
	if (currentTetromino != 4 && currentTetromino != 5)
	{
		rotate(currentTetromino);
	}
	if (tetrominoBatch.empty())
	{
		createTetrominoBatch();
	}
	nextTetromino = tetrominoBatch.back();
	cout << "Current Tetromino :" << currentTetromino - 1 << "\tNext Tetromino :" << nextTetromino << endl;
	return currentTetromino;
}


int gameWindow()
{
	//initialize game resources if there is a problem do not run game
	if (initialize() != 0)
	{
		return -1;
	}
	srand(time(0));
	RenderWindow window(VideoMode(320, 480), "Tetris By Overkill!");
	auto resolution = VideoMode::getDesktopMode();
	window.setSize({ 480, 720 });
	window.setPosition(Vector2i(resolution.width / 2 - window.getSize().x / 2, resolution.height / 2 - window.getSize().y / 2 ));
	Clock clock;
	int dx = 0;
	bool isRotated = 0;
	isGameOver = false;
	int currentTetromino = newTetromino();
	float timer = 0, delay = 0.5;

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
					rotate(currentTetromino);
				}
				if (event.key.code == Keyboard::Z)
				{
					counterRotate(currentTetromino);
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
						gameField[b[i].y][b[i].x] = currentTetromino;
					}
					//Getting the next tetromino from batch
					currentTetromino = newTetromino();
				}
				else
				{
					for (int i = 0; i < fieldColumn; i++)
					{
						if (gameField[4][i])
						{
							isGameOver = true;
							break;
						}
					}
				}
				timer = 0;
			}

			//Line clear check
			int k = fieldRow - 1;
			int scoreLinesCleared = 0; // for calculating score
			for (int i = fieldRow - 1; i > 0; i--)
			{
				int count = 0;
				int linesCleared = 0;
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
				else
				{
					linesCleared++;
					totalLinesCleared += linesCleared;
					scoreLinesCleared += linesCleared;
				}

			}
			Scoring(scoreLinesCleared);

			for (int i = 0; i < fieldColumn; i++)
			{
				if (gameField[0][i] || gameField[1][i])
				{
					cout << "Final Score: " << score << endl << "Total Lines Cleared: " << totalLinesCleared << endl;
					isGameOver = true;
					break;
				}
			}
		}

		//Reset dependet variables before draw for next cycle
		dx = 0;
		isRotated = 0;
		delay = 0.3;

		//Drawing to the screen
		window.clear(Color::White);
		window.draw(background);
		window.draw(scoreText);
		window.draw(totalLinesClearText);
		//Draw next tetromino to right side
		for (int i = 0; i < 4; i++)
		{
			c[i].x = tetrominos[nextTetromino][i] % 2;
			c[i].y = tetrominos[nextTetromino][i] / 2;
		}
		for (int i = 0; i < 4; i++)
		{
			nextTetrominoSprite.setPosition((c[i].x * 18) + 245, (c[i].y * 18) + 152);
			nextTetrominoSprite.setTextureRect(IntRect((nextTetromino + 1) * 18, 0, 18, 18));
			window.draw(nextTetrominoSprite);
		}
		//Draw the actual tetromino to the gamefield
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
			sprite.setTextureRect(IntRect(currentTetromino * 18, 0, 18, 18));
			sprite.setPosition(a[i].x * 18, a[i].y * 18);
			sprite.move(28, 31); //offset
			window.draw(sprite);
		}
		if (isGameOver)
		{
			window.draw(textBackgroundRect);
			window.draw(gameOverText);
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
	if (choice == 3)
	{
		showhighScores();
	}
	if (isGameOver)
	{
		cout << "Input name: ";
		cin >> name;
		highscoreWrite(name);
		showhighScores();
	}
	return 0;
}
