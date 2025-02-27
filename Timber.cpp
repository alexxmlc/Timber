#include <cmath>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#define SFML_STATIC
using namespace sf;

void updateBranches(int seed);

//Branch vector
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

//Cloud vector
const int NUM_CLOUDS = 5;
Sprite spriteCloud[NUM_CLOUDS];

//Tree bector
const int  NUM_TREE = 5;
Sprite tree[NUM_TREE];

//Where is the player/branch, lef or right
enum class side {LEFT, RIGHT, NONE};
side branchPosition[NUM_BRANCHES];


int main()
{
	//creating game window
	VideoMode vm(1920, 1080);
	RenderWindow window(vm, "Timber", Style::Fullscreen);
	
	//Creating the backgreound
	Texture textureBackground;									//create a texture to hold graphics on GPU
	textureBackground.loadFromFile("graphics/background.png");
	Sprite spriteBackgreound;									//create a sprite (design object)
	spriteBackgreound.setTexture(textureBackground);			//attach the texture to the sprite
	spriteBackgreound.setPosition(0, 0);						//to cover the screen

	//Make the main tree
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);
	
	//Make the background trees
	Texture textureTreeBack;
	textureTreeBack.loadFromFile("graphics/tree.png");
	for (int i = 0; i < NUM_TREE; i++)
	{
		tree[i].setTexture(textureTreeBack);
	}
	tree[0].setPosition(100, 0);
	tree[1].setPosition(500, -500);
	tree[2].setPosition(300, -350);
	tree[3].setPosition(1300, -100);
	tree[4].setPosition(1600, -100);
	tree[0].setScale(0.5f, 4.0f);
	tree[1].setScale(0.5f, 1.5f);
	tree[2].setScale(0.25f, 1.25f);
	tree[3].setScale(0.5f, 1.0f);
	tree[4].setScale(1.0f, 1.25f);

	//Make a bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	bool  beeActive = false;

	//Make 3 clouds with the same texture
	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");
	bool cloudActive[NUM_CLOUDS];
	for (int i = 0; i < 4; i++)
	{
		spriteCloud[i].setTexture(textureCloud);
		spriteCloud[i].setPosition(2000, 2000);
		cloudActive[i] = false;
	}
	
	//Controlling the movement of bee
	float beeSpeed = 0.0f;
	Clock clock;				//variable to control time
	float cloudSpeed[NUM_CLOUDS];
	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		cloudSpeed[i] = 0.0f;
	}

	//Preparing the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	side playerSide = side::LEFT;	

	//Preparing the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	//Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);
	const float AXE_POSITION_LEFT = 700;				//Line the axe with the tree
	const float AXE_POSITION_RIGHT = 1075;

	//Prepare the fying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	bool logActive = true;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	//Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timebarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timebarHeight));		//Vector2f class that holds 2 floats
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;	//number of pixels that the time bar shrinks every second

	//Variable to pause game
	bool paused = true;

	//Making the HUD
	int score = 0;

	Text messageText;
	Text scoreText;

	Font font;											//choose a font 
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	messageText.setFont(font);
	scoreText.setFont(font);							//set font to HUD text

	messageText.setString("Press Enter to start!");		//assing the message
	scoreText.setString("Score = 0");	
		
	messageText.setCharacterSize(75);					//dimension of text
	scoreText.setCharacterSize(100);			

	messageText.setFillColor(Color::White);				//set color
	scoreText.setFillColor(Color::White);

	//Positioning the text
	FloatRect textRect = messageText.getLocalBounds();	//rectangle with floating point coord
														//getLocalBounds - initializes textRect with the coordinates that wrap messageText

	messageText.setOrigin(textRect.left +		//setting the origin in the center of the textRect
		textRect.width / 2.0f,				
		textRect.top +
		textRect.height / 2.0f);

	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);	
	scoreText.setPosition(20, 20);					    //top left

	//Make the text easy to read
	int outlineThickness = 2;
	Color outlineColor = Color::Black;
	messageText.setOutlineThickness(outlineThickness);
	messageText.setOutlineColor(outlineColor);
	scoreText.setOutlineThickness(outlineThickness);
	scoreText.setOutlineColor(outlineColor);

	//Prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	//Set texture for each branch
	for (int i = 0; i < NUM_BRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 20);				//setting the origin to the center of the sprite
	}

	//Control the player input
	bool acceptInput = false;

	//Sounds
	//Chopping sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	//Dead
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	//Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	while (window.isOpen())
	{

		/*
		****************************************
		Handle the players input
		****************************************
		*/

		Event event;
																			//While cus there may be many events stored in queue, load them on at a time in "event"
		while (window.pollEvent(event))										//Puts data into the "event" object that describes an operating system event 
		{																	// ex: key press, key release, mouse movement, mouse click, game controller action ...
			if (event.type == Event::KeyReleased && !paused)
			{
				acceptInput = true;
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;

			//Restart game
			score = 0;
			timeRemaining = 6;

			//Make all the branches dissappear
		//starting in the second position
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				branchPosition[i] = side::NONE;
			}
			spriteRIP.setPosition(675, 2000);			//hide the gravestone
			spritePlayer.setPosition(580, 720);			//move the player into position

			acceptInput = true;
		}

		//Wrap the player controls
		if (acceptInput)
		{
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				playerSide = side::RIGHT;
				
				score++;

				timeRemaining += (2 / score) + .15;		//Add more time
				
				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				
				spritePlayer.setPosition(1200, 720);
				
				updateBranches(score);					//Update branches

				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				chop.play();

			}

			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				playerSide = side::LEFT;
				score++;
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);
				updateBranches(score);
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;
				chop.play();
			}
		}

		/*
	    ****************************************
	    Update the scene
	    ****************************************
	    */

		if (!paused) {
			Time dt = clock.restart();			//Measure time (restart the clock every frame so that we can time how long each and every frame takes)
												//returns the time elapsed since the last restart
			
			
			timeRemaining -= dt.asSeconds();													//lower the time remaining
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timebarHeight));		//resize the time bar

			if (timeRemaining <= 0.0f)
			{
				//pause the game
				paused = true;					

				//display out of time when the time runs out
				messageText.setString("Out of time!!");					

				//reposition the text based on it s new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
				outOfTime.play();
			}

			//Setup the bee
			if (!beeActive)
			{
				//Speed
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;			//   200  < beeSpeed <  399

				//How high is the bee
				srand((int)time(0) * 10);					//Seed the random number generator again
				float height = (rand() % 500) + 500;		// 500  < height <  999  
				spriteBee.setPosition(2000, height);		// position on the Ox = 2000, Oy = height

				beeActive = true;
			}
			else
				//Move the bee
			{
				float amplitude = 100.0f;  // Adjust this for the desired height of the wave
				float frequency = 1.0f;    // Adjust this for the frequency of the wave
		
				// Calculate the vertical position based on the sine wave
				float newY = spriteBee.getPosition().y;
				//float newY = amplitude * sin(frequency * (spriteBee.getPosition().x / 100)) - spriteBee.getPosition().y ;

				// Update the horizontal position of the bee
				float newX = spriteBee.getPosition().x - (beeSpeed * dt.asSeconds());

				// Combine the horizontal and vertical positions
				spriteBee.setPosition(newX, newY);						

				//Has the bee reach left side edge
				if (spriteBee.getPosition().x < -100)	
					beeActive = false;					//Setup a bee to be ready next frame
			}

			//Manage the clouds
			int speedFactor = 100;
			int heightFactor = 100;
			int randFactor = 1;
			for (int i = 0; i < NUM_CLOUDS; i++)
			{
				if (!cloudActive[i])
				{
					srand((int)time(0) * randFactor * 10);
					cloudSpeed[i] = (rand() % 100);
					srand((int)time(0) * 10);
					float height = (rand() % heightFactor);
					spriteCloud[i].setPosition(-200, height);
					
					cloudActive[i] = true;
					
					speedFactor += 50;
					heightFactor += 150;
					randFactor++;
				}
				else
				{
					spriteCloud[i].setPosition(
						spriteCloud[i].getPosition().x +
						(cloudSpeed[i] * dt.asSeconds()),
						spriteCloud[i].getPosition().y);

					if (spriteCloud[i].getPosition().x > 1920)
						cloudActive[i] = false;
				}
			}
			//Update score text
			std::stringstream ss;
			ss << "Score = " << score;					//concat
			scoreText.setString(ss.str());				//sets the string contained in ss to scoreText

			//Update the branch sprites every frame
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;

				if (branchPosition[i] == side::LEFT)
				{
					branches[i].setPosition(610, height);		//Move the sprite to the left side
					branches[i].setRotation(180);				//Flip the branch other side
				}
				else if (branchPosition[i] == side::RIGHT)
				{
					branches[i].setPosition(1330, height);		//Move sprite to right
					branches[i].setRotation(0);					//set sprite rotation to normal
				}
				else
				{
					branches[i].setPosition(3000, height);		//hide the branch
				}
			}

			//Setup the flying log
			if (logActive)
			{
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y +
					(logSpeedY * dt.asSeconds()));

				//Has the log reach right hand edge
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000)
				{



					//Set it up to be a new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			//Has the player been squished by a branch
			if (branchPosition[5] == playerSide)
			{
				//death
				paused = true;
				acceptInput = false;

				//Draw the gravestone
				spriteRIP.setPosition(525, 760);

				//Hide the player
				spritePlayer.setPosition(2000, 660);

				//Change the text message
				messageText.setString("SQUISHED!!");

				//Ceneter it on the screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
				death.play();
			}
		}

		/*
		**********************************************
		Draw the scene
		**********************************************
		*/
		window.clear();						//Clear prev frame of animation
		
		window.draw(spriteBackgreound);		//Draw the backgroound scene
		for (int i = 0; i < NUM_CLOUDS; i++)
		{
			window.draw(spriteCloud[i]);	//Draw the clouds
		}

		for (int i = 0; i < NUM_TREE; i++)
		{
			window.draw(tree[i]);
		}

		window.draw(spriteTree);			//Draw tree
		window.draw(scoreText);				//Draw score
		window.draw(spritePlayer);			//Draw player
		window.draw(spriteAxe);				//Draw axe
		window.draw(spriteLog);				//Draw log
		window.draw(spriteRIP);				//Draw gravestone
		window.draw(spriteBee);				//Draw bee
	
		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			window.draw(branches[i]);		//Draw branches
		}

		

		window.draw(timeBar);				//Draw time
		if (paused)
		{	
			window.draw(messageText);		//Draw message if the mage is paused 
		}

		window.display();					//Display what we drew

	}
	return 0;
}

void updateBranches(int seed)
{
	//Move all branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--)
	{
		branchPosition[j] = branchPosition[j - 1];

		//Spawn a new branch at position 0
		//LEFT, RIGHT or NONE
		srand((int)time(0) + seed);
		int r = (rand() % 5);				// 0 < r < 4

		switch (r)
		{
			case 0: 
				branchPosition[0] = side::LEFT;
				break;
			case 1: 
				branchPosition[0] = side::RIGHT;
				break;
			default: 
				branchPosition[0] = side::NONE;
				break;
		}
	}
}