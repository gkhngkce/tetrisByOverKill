#include <SFML/Graphics.hpp> //main library for UI
#include <SFML/Audio.hpp> // for the music
#include <time.h> //for timing and randomizing
#include <iostream> //standart library
#include <list> //list operations
#include <fstream> //file operations
#include <conio.h> // for getch() function(waiting an input before exit)


int mainMenuWindow();

using namespace sf;
using namespace std;

//Score and related variables
int score = 0;
int totalLinesCleared = 0;
string name="Name = ";
bool isGameOver = false; 
bool isScoreSaved = false;

//Game field variables
const int fieldRow = 24;
const int fieldColumn = 10;
const int playWidth = 320;
const int playHeight = 480;
int gameField[fieldRow][fieldColumn] = { 0 };

//Textures of gamefield
Texture tiles, bground, outFrame;
Sprite sprite, background, nextTetrominoSprite;
string bgPath = "sources/menuScreen.png";
string tilePath = "sources/tilesDeniz.png";
//Gamoever screen variables
Text gameOverText, scoreText, totalLinesClearText,highScoresText,restartText,nameText;
RectangleShape textBackgroundRect;
FloatRect gameOverTextRect;
Font gameOverFont;

//Defining the batch of tetrominos
list <int> tetrominoBatch;
int nextTetromino = 0;

string fromKtoS(Keyboard::Key k) {
	string ret;
	switch (k) {

	case sf::Keyboard::A:
		ret = "A";
		break;
	case sf::Keyboard::B:
		ret = "B";
		break;
	case sf::Keyboard::C:
		ret = "C";
		break;
	case sf::Keyboard::D:
		ret = "D";
		break;
	case sf::Keyboard::E:
		ret = "E";
		break;
	case sf::Keyboard::F:
		ret = "F";
		break;
	case sf::Keyboard::G:
		ret = "G";
		break;
	case sf::Keyboard::H:
		ret = "H";
		break;
	case sf::Keyboard::I:
		ret = "I";
		break;
	case sf::Keyboard::J:
		ret = "J";
		break;
	case sf::Keyboard::K:
		ret = "K";
		break;
	case sf::Keyboard::L:
		ret = "L";
		break;
	case sf::Keyboard::M:
		ret = "M";
		break;
	case sf::Keyboard::N:
		ret = "N";
		break;
	case sf::Keyboard::O:
		ret = "O";
		break;
	case sf::Keyboard::P:
		ret = "P";
		break;
	case sf::Keyboard::Q:
		ret = "Q";
		break;
	case sf::Keyboard::R:
		ret = "R";
		break;
	case sf::Keyboard::S:
		ret = "S";
		break;
	case sf::Keyboard::T:
		ret = "T";
		break;
	case sf::Keyboard::U:
		ret = "U";
		break;
	case sf::Keyboard::V:
		ret = "V";
		break;
	case sf::Keyboard::W:
		ret = "W";
		break;
	case sf::Keyboard::X:
		ret = "X";
		break;
	case sf::Keyboard::Y:
		ret = "Y";
		break;
	case sf::Keyboard::Z:
		ret = "Z";
		break;
	case sf::Keyboard::BackSpace:
		ret = "BACK";
		break;
	default:
		ret = "UNK";
		break;
	}
	return ret;
}

//points of tetrominos on the screen a=>shown ones b=>hidden ones c=>next tetromino
struct Point
{
	int x, y;
} currentTetrominosPosition[4], tetrominosFieldPosition[4], nextTetrominosPosition[4];

//Defining tetrominos
int tetrominos[7][4] =
{
	3,5,4,7, // T - 0
	3,5,4,6, // Z - 1
	2,4,5,7, // S - 2
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
	case 0:
		break;
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
	default:
		cout << "Invalid amount of lines cleared. No score has been added.";
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

void showHighScores()
{
	ifstream readHighscores("highscores.txt", ios::in);
	string l, scores = "";
	while (getline(readHighscores, l))
	{
		l.replace(l.find("Highscore ="), 12, "");
		l.replace(l.find("Total"), 19, "TLC");
		scores.append(l);
		scores.append("\n");
		highScoresText.setString(scores);
	}
	readHighscores.close();
	scores.append("\nESC for main menu");
	highScoresText.setString(scores);
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
	if (loadFonts(gameOverFont, "sources/sansation.ttf") != 0 || loadTextures(tiles, tilePath) != 0 || loadTextures(bground, bgPath) != 0)
	{
		return -1;
	}
	sprite.setTexture(tiles);
	background.setTexture(bground);
	nextTetrominoSprite.setTexture(tiles);

	//Message for game over
	gameOverText.setPosition({ playWidth / 2, playHeight / 2 });
	gameOverText.setFont(gameOverFont);
	gameOverText.setCharacterSize(40);
	gameOverText.setFillColor(Color::Green);
	gameOverText.setString(" Game Over ");

	//Message for restart game
	restartText.setPosition({ playWidth / 10, (playHeight / 2)+15 });
	restartText.setFont(gameOverFont);
	restartText.setCharacterSize(10);
	restartText.setFillColor(Color::Green);
	restartText.setString("Hit ENTER for restart, ESC for main menu");

	//Message for getting name from user
	nameText.setPosition({ playWidth / 10, (playHeight / 2) + 30 });
	nameText.setFont(gameOverFont);
	nameText.setCharacterSize(10);
	nameText.setFillColor(Color::Green);
	nameText.setString(name);

	//Message for showing high scores
	highScoresText.setPosition({ playWidth / 10, playHeight / 10 });
	highScoresText.setFont(gameOverFont);
	highScoresText.setCharacterSize(15);
	highScoresText.setFillColor(Color::Black);
	highScoresText.setString("");

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
	textBackgroundRect = RectangleShape{ Vector2f{ gameOverTextRect.width + 10, gameOverTextRect.height + 60 } };
	textBackgroundRect.setFillColor(sf::Color(0, 0, 0, 200));
	textBackgroundRect.setPosition({ playWidth / 2, playHeight / 2 });
	textBackgroundRect.setOrigin(gameOverTextRect.left + gameOverTextRect.width / 2.0f,
		gameOverTextRect.top + gameOverTextRect.height / 2.0f);
	return 0;
}

//Collusion detection function
bool collisionDetection()
{
	for (int i = 0; i < 4; i++)
	{
		if (currentTetrominosPosition[i].x < 0 || currentTetrominosPosition[i].x >= fieldColumn || currentTetrominosPosition[i].y >= fieldRow)
		{
			return 0;
		}
		else if (gameField[currentTetrominosPosition[i].y][currentTetrominosPosition[i].x])
		{
			return 0;
		}
	}
	return 1;
};

int rotate(int currTetromino, Point* pointOfCurrent)
{
	if (currTetromino != 4)
	{
		Point centerOfRotation = pointOfCurrent[1]; //center of rotation
		for (int i = 0; i < 4; i++)
		{
			int x = pointOfCurrent[i].y - centerOfRotation.y;
			int y = pointOfCurrent[i].x - centerOfRotation.x;
			pointOfCurrent[i].x = centerOfRotation.x - x;
			pointOfCurrent[i].y = centerOfRotation.y + y;
		}
		if (!collisionDetection())
		{
			for (int i = 0; i < 4; i++)
			{
				currentTetrominosPosition[i] = tetrominosFieldPosition[i];
			}
		}
	}
	return 0;
}

int counterRotate(int currTetromino, Point* pointOfCurrent)
{
	rotate(currTetromino, pointOfCurrent);
	rotate(currTetromino, pointOfCurrent);
	rotate(currTetromino, pointOfCurrent);
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
		currentTetrominosPosition[i].x = (tetrominos[createdTetromino][i] % 2) + (fieldColumn / 2) - 1;//defining x coordinate of tetromino & centeralizing the tetromino
		currentTetrominosPosition[i].y = tetrominos[createdTetromino][i] / 2; //defining y coordinate of tetromino
	}
	if (currentTetromino != 4)
	{
		rotate(currentTetromino, currentTetrominosPosition);
	}
	if (tetrominoBatch.empty())
	{
		createTetrominoBatch();
	}
	nextTetromino = tetrominoBatch.back();
	cout << "Current Tetromino :" << currentTetromino - 1 << "\tNext Tetromino :" << nextTetromino << endl;
	return currentTetromino;
}

void clearGameField()
{
	for (int i = 0; i < fieldRow; i++)
	{
		for (int j = 0; j < fieldColumn; j++)
		{
			gameField[i][j] = 0;
		}
	}
	totalLinesCleared = 0;
	score = 0;
	name = "Name =";
	nameText.setString(name);
	isScoreSaved = false;
	isGameOver = false;
	createTetrominoBatch();
	newTetromino();
}

int gameWindow()
{
	//initialize game resources if there is a problem do not run game
	if (initialize() != 0)
	{
		return -1;
	}
	loadTextures(bground, bgPath);
	background.setTexture(bground);
	loadTextures(tiles, tilePath);
	sprite.setTexture(tiles);
	srand(time(0));
	bool drop = false;
	RenderWindow window(VideoMode(playWidth, playHeight), "Tetris By Overkill!");
	auto resolution = VideoMode::getDesktopMode();
	window.setSize({ 480, 720 });
	window.setPosition(Vector2i(resolution.width / 2 - window.getSize().x / 2, resolution.height / 2 - window.getSize().y / 2));
	Clock clock;
	int dx = 0;
	bool isRotated = 0;
	isGameOver = false;
	int currentTetromino = newTetromino();
	float timer = 0, delay = 0.5;
	clearGameField();
	Music music;
	if (!music.openFromFile("sources/themeA.ogg"))
		return -1; // error
	music.setVolume(10.f);
	music.play();
	music.setLoop(true);

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
					if (!isGameOver)
					{
						rotate(currentTetromino, currentTetrominosPosition);
						clock.restart();
					}
				}
				if (event.key.code == Keyboard::Z)
				{
					if (!isGameOver)
					{
						counterRotate(currentTetromino, currentTetrominosPosition);
						clock.restart();
					}
				}
				if (event.key.code == Keyboard::Left)
				{
					dx = -1;
					clock.restart();
				}
				if (event.key.code == Keyboard::Right)
				{
					dx = 1;
					clock.restart();
				}
				if (event.key.code == Keyboard::Space)
				{
					drop = true;
					delay = 0;
					clock.restart();
				}
				if (event.key.code == Keyboard::Enter)
				{
					if (isGameOver)
					{
						if (name.length() > 7 && name.length()<17 && !isScoreSaved)
						{
							highscoreWrite(name.replace(0, 6, ""));
							name += " Score Saved.";
							nameText.setString(name);
							isScoreSaved = true;
						}
						else if (isScoreSaved)
						{
							clearGameField();
						}
					}
				}
				if (event.key.code == Keyboard::Escape)
				{
					if (isGameOver)
					{
						music.stop();
						bgPath = "sources/menuScreen.png";
						window.close();
						mainMenuWindow();
						
					}
				}
				if (event.key.code == Keyboard::RShift || event.key.code == Keyboard::LShift)
				{
					//Restart...
				}
				else
				{
					if (isGameOver)
					{
						string res= fromKtoS(event.key.code);
						if (res.compare("BACK") == 0)
						{
							if (name.length() > 6)
							{
								name = name.replace(name.length() - 1, 1, "");
							}
						}
						else if (res.compare("UNK")!=0 && name.length()<17)
						{
							name += res;
						}
						nameText.setString(name);
					}
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
				tetrominosFieldPosition[i] = currentTetrominosPosition[i];
				currentTetrominosPosition[i].x += dx;
			}
			if (!collisionDetection())
			{
				for (int i = 0; i < 4; i++)
				{
					currentTetrominosPosition[i] = tetrominosFieldPosition[i];
				}
			}

			//Vertical motion
			if (timer > delay)
			{
				if (drop)
				{
					while (drop)
					{
						for (int i = 0; i < 4; i++)
						{
							tetrominosFieldPosition[i] = currentTetrominosPosition[i];
							currentTetrominosPosition[i].y += 1;
						}
						if (!collisionDetection())
						{
							for (int i = 0; i < 4; i++)
							{
								gameField[tetrominosFieldPosition[i].y][tetrominosFieldPosition[i].x] = currentTetromino;
							}
							//Getting the next tetromino from batch
							currentTetromino = newTetromino();
							drop = false;
							delay = 0.3;
						}
					}
				}
				else
				{
					for (int i = 0; i < 4; i++)
					{
						tetrominosFieldPosition[i] = currentTetrominosPosition[i];
						currentTetrominosPosition[i].y += 1;
					}
					if (!collisionDetection())
					{
						for (int i = 0; i < 4; i++)
						{
							gameField[tetrominosFieldPosition[i].y][tetrominosFieldPosition[i].x] = currentTetromino;
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
			nextTetrominosPosition[i].x = tetrominos[nextTetromino][i] % 2;
			nextTetrominosPosition[i].y = tetrominos[nextTetromino][i] / 2;
		}
		rotate((nextTetromino + 1), nextTetrominosPosition);
		for (int i = 0; i < 4; i++)
		{
			nextTetrominoSprite.setPosition((nextTetrominosPosition[i].x * 18) + 245, (nextTetrominosPosition[i].y * 18) + 152);
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
			sprite.setPosition(currentTetrominosPosition[i].x * 18, currentTetrominosPosition[i].y * 18);
			sprite.move(28, 31); //offset
			window.draw(sprite);
		}
		if (isGameOver)
		{
			window.draw(textBackgroundRect);
			window.draw(gameOverText);
			window.draw(restartText);
			window.draw(nameText);
		}
		window.display();
	}
	return 0;
}

int mainMenuWindow()
{
	if (initialize() != 0)
	{
		return -1;
	}
	bool showScores = false;
	bgPath = "sources/themeDeniz.png";
	RenderWindow window(VideoMode(playWidth, playHeight), "Tetris By Overkill!");
	auto resolution = VideoMode::getDesktopMode();
	window.setSize({ 480, 720 });
	window.setPosition(Vector2i(resolution.width / 2 - window.getSize().x / 2, resolution.height / 2 - window.getSize().y / 2));
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Num1)
				{
					//bgPath = "sources/background.png";
					window.close();
					gameWindow();
				}
				if (event.key.code == Keyboard::Num2)
				{
					loadTextures(bground, "sources/settings.png");
					background.setTexture(bground);
				}
				if (event.key.code == Keyboard::Num3)
				{
					showScores = true;
					bground = Texture();
					background.setTexture(bground);
					showHighScores();
				}
				if (event.key.code == Keyboard::Num4)
				{

				}
				if (event.key.code == Keyboard::X)
				{
					bgPath = "sources/themeDeniz.png";
					tilePath = "sources/tilesDeniz.png";
					loadTextures(bground, "sources/menuScreen.png");
					background.setTexture(bground);
				}
				if (event.key.code == Keyboard::Y)
				{
					bgPath = "sources/themeOruchan.png";
					tilePath = "sources/tilesOruchan.png";
					loadTextures(bground, "sources/menuScreen.png");
					background.setTexture(bground);
				}
				if (event.key.code == Keyboard::Escape)
				{
					if (showScores)
					{
						showScores = false;
						loadTextures(bground, "sources/menuScreen.png");
						background.setTexture(bground);
					}
				}
			}
		}
		//Drawing to the screen
		window.clear(Color::White);
		window.draw(background);
		if (showScores)
		{
			window.draw(highScoresText);
		}
		window.display();
	}
}

int main()
{
	mainMenuWindow();
	return 0;
}
