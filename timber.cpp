// THIS IS WEHRE OUR GAME STARTS FROM

// include important libraries
#include <SFML/Graphics.hpp>
#include <sstream>
#include <SFML/Audio.hpp>
#include <ctime>
#include <cstdlib>
#include <algorithm>
using namespace sf;
//function declaration
void updateBranches(int seed);

const int NUM_BRANCHES=6;
Sprite branches[NUM_BRANCHES];

//WHere is the player/branch?
//Left or Right
enum class side {LEFT, RIGHT , NONE};
side branchPositions[NUM_BRANCHES];


int main()
{
    // seed random number generator once
    srand((int)time(0));

    // Create a video mode object
    VideoMode vm(1920, 1080);
    // Create and open a window for the game
    RenderWindow window(vm, "Timber!!!", Style::Fullscreen);
    View view(FloatRect(0,0,1920,1080));
    window.setView(view);

    // BACKGROUND//
    // Create a texture to hold a graphic on the GPU
    Texture textureBackground;
    textureBackground.loadFromFile("asset/graphics/background.png"); // load graphics into the texture
    Sprite spriteBackground;                                          // create a sprite
    spriteBackground.setTexture(textureBackground);                   // attach texture to the sprite
    spriteBackground.setPosition(0, 0);                               // set the background to cover the screen

    // TREE//
    Texture textureTree;
    textureTree.loadFromFile("asset/graphics/tree.png");
    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(810, 0);

    // BEE//
    Texture textureBee;
    textureBee.loadFromFile("asset/graphics/bee.png");
    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(0, 800);

    bool beeActive = false; // is there any current movement
    float beeSpeed = 0.0f;  // how fast the bee will fly

    // CLOUD1
    Texture textureCloud;
    textureCloud.loadFromFile("asset/graphics/cloud.png");
    Sprite spriteCloud1;
    spriteCloud1.setTexture(textureCloud);
    spriteCloud1.setPosition(0, 0);
    bool cloud1Active = false;
    float cloud1Speed = 0.0f;

    // CLOUD2
    Sprite spriteCloud2;
    spriteCloud2.setTexture(textureCloud);
    spriteCloud2.setPosition(0, 250);
    bool cloud2Active = false;
    float cloud2Speed = 0.0f;

    // CLOUD3
    Sprite spriteCloud3;
    spriteCloud3.setTexture(textureCloud);
    spriteCloud3.setPosition(0, 500);
    bool cloud3Active = false;
    float cloud3Speed = 0.0f;

    Clock clock;//variable to control time(Class Time)

    //TIME BAR!!
    RectangleShape timeBar;
    float TimeBarStartWidth=400;
    float timeBarHeight=80;
    timeBar.setSize(Vector2f(TimeBarStartWidth,timeBarHeight));
    timeBar.setFillColor(Color::Green);
    timeBar.setPosition((1920/2)-TimeBarStartWidth/2,980);

    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = TimeBarStartWidth/timeRemaining;

    bool paused=true;//track whether the game is running or not
    //TO DRAW TEXT//HUD
    int score=0;

    Text messageText;
    Text scoreText;
    //choose a font
    Font font;
    font.loadFromFile("asset/font/KOMIKAP_.ttf");

    //set the font to the message
    messageText.setFont(font);
    scoreText.setFont(font);

    //Assign the actual message
    messageText.setString("Press Enter to Start!!");
    scoreText.setString("Score = 0");

    //Assign font size to the text
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    //Choose a color
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    //Position the text
    FloatRect textRect=messageText.getLocalBounds();

    messageText.setOrigin(textRect.left+textRect.width/2.0f,textRect.top+textRect.height/2.0f);
    messageText.setPosition(1920/2.0f,1080/2.0f);
    //why do we need FloatRect for message but not for score??
    scoreText.setPosition(20,20);

    //BRANCHES
    //Prepare 6 branches
    Texture textureBranch;
    textureBranch.loadFromFile("asset/graphics/branch.png");

    //set the texture for each branch sprite
    for(int i=0;i<NUM_BRANCHES;i++){
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000,2000);

        //Set the sprites origin to dead center
        //We can then spin it round without changing its position
        branches[i].setOrigin(220,20);
    }

    //PLAYER
    Texture texturePlayer;
    texturePlayer.loadFromFile("asset/graphics/player.png");
    Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(580,720);

    //The player starts on the left
    side playerSide=side::LEFT;

    //GRAVESTONE
    Texture textureRIP;
    textureRIP.loadFromFile("asset/graphics/rip.png");
    Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(600,860);

    //AXE
    Texture textureAxe;
    textureAxe.loadFromFile("asset/graphics/axe.png");
    Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(700,830);
    //line the axe up with the tree
    const float AXE_POSITION_LEFT=700;
    const float AXE_POSITION_RIGHT=1075;

    //FLYING LOG
    Texture textureLog;
    textureLog.loadFromFile("asset/graphics/log.png");
    Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(810,720);

    bool logActive=false;
    float logSpeedX=1000;
    float logSpeedY=-1500;

    //control the player input
    bool acceptInput = false;

    //prepare sound
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("asset/sound/chop.wav");
    Sound chop;
    chop.setBuffer(chopBuffer);

    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("asset/sound/death.wav");
    Sound death;
    death.setBuffer(deathBuffer);

    SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("asset/sound/out_of_time.wav");
    Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);


    // game loop

    while (window.isOpen())
    {
        // Handle the players input

        Event event;
        while(window.pollEvent(event)){
            if(event.type==Event::Closed){
                window.close();
            }
            if(event.type==Event::KeyReleased && !paused){
                //Listen for key press again
                acceptInput=true;

                //hide the axe
                spriteAxe.setPosition(2000,spriteAxe.getPosition().y);
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }
        //start the game
        if(Keyboard::isKeyPressed(Keyboard::Return)){
            paused=false;

            //Reset the time and the score
            score=0;
            timeRemaining=6;
            //Make all the branch disappear - starting in the second position
            for(int i=0;i<NUM_BRANCHES;i++){
                branchPositions[i]=side::NONE;
            }
            //hide the gravestone
            spriteRIP.setPosition(675,2000);
            //Move the player into the positon
            spritePlayer.setPosition(580,720);

            acceptInput= true;
        }

        // update the scene

        // accept the input here
        if(acceptInput){
            // Right cursor key
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                playerSide=side::RIGHT;
                score++;
                timeRemaining+=(2.0f/score)+.15f;
                spriteAxe.setPosition(AXE_POSITION_RIGHT,spriteAxe.getPosition().y);
                spritePlayer.setPosition(1200,720);
                // Update the branches
                updateBranches(score);
                // Set the flying log to the left
                spriteLog.setPosition(810,720);
                logSpeedX=-5000;
                logActive=true;

                acceptInput=false;
                chop.play();
            }
            // Left cursor key
            else if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                playerSide=side::LEFT;
                score++;
                timeRemaining+=(2.0f/score)+.15f;
                spriteAxe.setPosition(AXE_POSITION_LEFT,spriteAxe.getPosition().y);
                spritePlayer.setPosition(580,720);
                // Update the branches
                updateBranches(score);
                // Set the flying log to the left
                spriteLog.setPosition(810,720);
                logSpeedX=5000;
                logActive=true;

                acceptInput=false;
                chop.play();
            }
        }

        if(!paused){

            // Measure the time
            Time dt = clock.restart(); // dt is the time it takes to update and render one frame

            //Subtranct from the amount of time remaining
            timeRemaining-=dt.asSeconds();
            //size up the time bar
            timeBar.setSize(Vector2f(std::max(0.0f, timeBarWidthPerSecond*timeRemaining),timeBarHeight));

            if(timeRemaining<=0.0f){
                //Pause the game
                paused= true;

                //CHange the message shown to the player
                messageText.setString("Out of time!!!");
                //Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();

                messageText.setOrigin(textRect.left+textRect.width/2.0f,textRect.top+textRect.height/2.0f);
                messageText.setPosition(1920/2.0f,1080/2.0f);
                outOfTime.play();
            }

            // BEE
            if (!beeActive)
            {
                //set the speed of the bee(randomized)
                beeSpeed = (rand() % 200) + 200; // bee speed will be between 200 and 399
                //set the height of the bee(randomized)
                float height=(rand()%500)+500;//here scope of the height is limited to this if block only
                spriteBee.setPosition(2000,height);//set the position of the bee offscreen
                beeActive=true;
            }
            else{
                //if bee is moving or active that is game is not paused
                spriteBee.setPosition(spriteBee.getPosition().x-(dt.asSeconds()*beeSpeed),spriteBee.getPosition().y);
                //if bee reached the left hand end of the screen
                if(spriteBee.getPosition().x<-100){
                    beeActive=false;//so that it would reset the setting as in shift its position to the right hand side of the frame (out of the frame) and set a randomized height and speed
                }
            }

            //CLOUD
            if(!cloud1Active){
                cloud1Speed=(rand()%200)+100;
                float height=(rand()%150);
                spriteCloud1.setPosition(-200,height);
                cloud1Active=true;
            }
            else{
                spriteCloud1.setPosition(spriteCloud1.getPosition().x+(dt.asSeconds()*cloud1Speed),spriteCloud1.getPosition().y);
                if(spriteCloud1.getPosition().x>1930){
                    cloud1Active=false;
                }
            }

            if(!cloud2Active){
                cloud2Speed=(rand()%200)+100;
                float height=(rand()%300)-150;
                spriteCloud2.setPosition(-200,height);
                cloud2Active=true;
            }
            else{
                spriteCloud2.setPosition(spriteCloud2.getPosition().x+(dt.asSeconds()*cloud2Speed),spriteCloud2.getPosition().y);
                if(spriteCloud2.getPosition().x>1930){
                    cloud2Active=false;
                }
            }

            if(!cloud3Active){
                cloud3Speed=(rand()%200)+100;
                float height=(rand()%400)-150;
                spriteCloud3.setPosition(-200,height);
                cloud3Active=true;
            }
            else{
                spriteCloud3.setPosition(spriteCloud3.getPosition().x+(dt.asSeconds()*cloud3Speed),spriteCloud3.getPosition().y);
                if(spriteCloud3.getPosition().x>1930){
                    cloud3Active=false;
                }
            }

            //Update the score text
            std::stringstream ss;
            ss<<"Score = "<<score;
            scoreText.setString(ss.str());

            //update the branch sprites
            for(int i=0;i<NUM_BRANCHES;i++){
                float height=i*150;
                if(branchPositions[i]==side::LEFT){
                    //Move the sprite to the left side
                    branches[i].setPosition(610,height);
                    //Flip the splrite arounf the other way
                    branches[i].setRotation(180);
                }
                else if (branchPositions[i]==side::RIGHT){
                    //Move the sprite to the RIGHT side
                    branches[i].setPosition(1330,height);
                    //Set the sprite rotation to normal
                    branches[i].setRotation(0);
                }
                else{
                    //Hide the branches
                    branches[i].setPosition(3000,height);
                }
            }

            //Handle the flying log
            if(logActive){
                spriteLog.setPosition(spriteLog.getPosition().x+(logSpeedX*dt.asSeconds()),spriteLog.getPosition().y+(logSpeedY*dt.asSeconds()));
                //Has log reached the edge ?
                if(spriteLog.getPosition().x<-100||spriteLog.getPosition().x>2000){
                    //new log
                    logActive=false;
                    spriteLog.setPosition(810,720);
                }
            }

            //SQUISHED
            if(branchPositions[5]==playerSide){
                //death
                paused=true;
                acceptInput = false;
                //RIP
                spriteRIP.setPosition(525,760);
                //hide player
                spritePlayer.setPosition(2000,660);
                messageText.setString("SQUISHED!!");
                FloatRect textRect = messageText.getLocalBounds();

                messageText.setOrigin(textRect.left+textRect.width/2.0f,textRect.top+textRect.height/2.0f);
                messageText.setPosition(1920/2.0f,1080/2.0f);
                death.play();
            }

        }//End if(!paused)

        // draw the scene

        window.clear(); // to clear everything from the previous frame

        // draw our game scene here
        window.draw(spriteBackground);
        window.draw(spriteCloud1);
        window.draw(spriteCloud2);
        window.draw(spriteCloud3);
        //Draw the branches
        for (int i=0;i<NUM_BRANCHES;i++){
            window.draw(branches[i]);
        }
        window.draw(spriteTree);
        window.draw(spritePlayer);
        window.draw(spriteAxe);
        window.draw(spriteLog);
        window.draw(spriteRIP);
        window.draw(spriteBee);
        window.draw(scoreText);
        window.draw(timeBar);
        if(paused){
            window.draw(messageText);
        }
        // display everything that we draw
        window.display();
    }

    return 0;
}
//Function definition
void updateBranches(int seed){
    //MOve all the branches down one place
    for(int j=NUM_BRANCHES-1;j>0;j--){
        branchPositions[j]=branchPositions[j-1];
    }
    //SPawn a new branch at position 0
    //LEFT,RIGHT or NONE
    int r=(rand()%5);
    switch(r){
        case 0:
            branchPositions[0]=side::LEFT;
            break;
        case 1:
            branchPositions[0]=side::RIGHT;
            break;
        default:
            branchPositions[0]=side::NONE;
            break;
    }
}
