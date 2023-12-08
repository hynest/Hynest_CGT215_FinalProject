#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float PL_SPEED = 0.6;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

void playerMove(PhysicsSprite& player, int elapsedMS, int sizex, int sizey) {
    Vector2f newPos(player.getCenter());
    if (Keyboard::isKeyPressed(Keyboard::Right) && ((newPos.x + (sizex / 2)) <= 1110)) {
        newPos.x = newPos.x + (PL_SPEED * elapsedMS);
        player.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left) && ((newPos.x - (sizex / 2)) >= 150)) {
        newPos.x = newPos.x - (PL_SPEED * elapsedMS);
        player.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Up) && ((newPos.y - (sizey / 2)) >= 55)) {
        newPos.y = newPos.y - (PL_SPEED * elapsedMS);
        player.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Down) && ((newPos.y + (sizey / 2)) <= 1145)) {
        newPos.y = newPos.y + (PL_SPEED * elapsedMS);
        player.setCenter(newPos);
    }
}

int main()
{
    RenderWindow window(VideoMode(1600, 1200), "Card Gunner");
    World world(Vector2f(0, 0));
    int score(0);
    int lives(3);
    int gamestate(0);

    Font gamefont;
    if (!gamefont.loadFromFile("arial.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }

    PhysicsSprite& player = *new PhysicsSprite();
    Texture plTex;
    LoadTex(plTex, "images/player.png");
    player.setTexture(plTex);
    Vector2f plSize = player.getSize();
    player.setCenter(Vector2f(630, 800 - (plSize.y / 2)));
    world.AddPhysicsBody(player);
    int shoottimer(0);

    Texture ppTex;
    LoadTex(ppTex, "images/player.png");
    PhysicsShapeList<PhysicsSprite> plprojectiles;
    

    PhysicsSprite enemy1;
    Texture en1Tex;
    LoadTex(en1Tex, "images/player.png");
    enemy1.setTexture(en1Tex);
    Vector2f en1Size = enemy1.getSize();
    enemy1.setCenter(Vector2f(630, 300 - en1Size.y));
    world.AddPhysicsBody(enemy1);
    enemy1.onCollision = [&enemy1, &world, &player, &lives, &gamestate](PhysicsBodyCollisionResult result) {
        if (result.object2 == player) {
            gamestate = 2;
            lives -= 1;
        }
        };

    // Enemies
    PhysicsShapeList<PhysicsCircle> plbullets;

    // Playbounds
    PhysicsRectangle left;
    left.setSize(Vector2f(10,1100));
    left.setCenter(Vector2f(150, 600));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 1100));
    right.setCenter(Vector2f(1110, 600));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    PhysicsRectangle top;
    top.setSize(Vector2f(960, 10));
    top.setCenter(Vector2f(630, 55));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle bottom;
    bottom.setSize(Vector2f(960, 10));
    bottom.setCenter(Vector2f(630, 1145));
    bottom.setStatic(true);
    world.AddPhysicsBody(bottom);

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    while (window.isOpen()) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            if (gamestate == 0) {
                Text startgame;
                window.draw(left);
                window.draw(right);
                window.draw(top);
                window.draw(bottom);
                startgame.setString("Press [Enter] to Start");
                startgame.setFont(gamefont);
                Vector2f sgSize = GetTextSize(startgame);
                startgame.setPosition(Vector2f(630-(sgSize.x / 2), 600-(sgSize.y / 2)));
                window.draw(startgame);
                if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                    gamestate = 1;
                }

            }
            if (gamestate == 2) {
                enemy1.setCenter(Vector2f(630, 300 - en1Size.y));
                player.setCenter(Vector2f(630, 800));
                player.setVelocity(Vector2f(0, 0));
                if (lives <= 0) {
                    gamestate = 3;
                }
                window.draw(left);
                window.draw(right);
                window.draw(top);
                window.draw(bottom);
                Text trygame;
                window.clear();
                trygame.setString("Press [Enter] to Try Again");
                trygame.setFont(gamefont);
                Vector2f sgSize = GetTextSize(trygame);
                trygame.setPosition(Vector2f(630 - (sgSize.x / 2), 600 - (sgSize.y / 2)));
                window.draw(trygame);
                if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                    gamestate = 1;
                }
            }

            if (gamestate == 3) {
                window.clear();
                Text restartgame;
                restartgame.setString("Press [Enter] to Restart");
                restartgame.setFont(gamefont);
                Vector2f rtSize = GetTextSize(restartgame);
                restartgame.setPosition(Vector2f(630 - (rtSize.x / 2), 600 - (rtSize.y / 2)));
                window.draw(restartgame);
                if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                    lives = 3;
                    score = 0;
                    gamestate = 1;
                }
            }
            if (gamestate == 1) {
                if (enemy1.getVelocity().x != 1 && enemy1.getVelocity().y != 0.5) {
                    enemy1.setVelocity(Vector2f(1, 0.5));
                }
                player.setVelocity(Vector2f(0, 0));
                if (shoottimer < 3) {
                    shoottimer++;
                }
                if (Keyboard::isKeyPressed(Keyboard::Space) && shoottimer >= 3) {

                    shoottimer = 0;
                    if (shoottimer > 3) {
                        PhysicsCircle& plbullet = plbullets.Create();
                        plbullet.setSize(Vector2f(5, 5));
                        plbullet.setCenter(player.getCenter());
                        plbullet.setVelocity(Vector2f(0,1));
                        world.AddPhysicsBody(plbullet);
                    }

                }
                
                if (lives == 0) {
                    gamestate = 3;
                }

                window.clear();
                plbullets.DoRemovals();
                for (PhysicsCircle& plbullet : plbullets) {
                    window.draw((PhysicsCircle&)plbullet);
                }
                playerMove(player, deltaMS, plSize.x, plSize.y);
                window.draw(player);
                window.draw(enemy1);
                window.draw(left);
                window.draw(right);
                window.draw(top);
                window.draw(bottom);
                
                Text liveText;
                liveText.setString(to_string(lives));
                liveText.setFont(gamefont);
                liveText.setPosition(Vector2f(1250, 500));
                window.draw(liveText);
                Text live;
                live.setString("Lives");
                live.setFont(gamefont);
                live.setPosition(Vector2f(1250, 450));
                window.draw(live);
                Text scoreText;
                scoreText.setString(to_string(score));
                scoreText.setFont(gamefont);
                scoreText.setPosition(Vector2f(1250, 600));
                window.draw(scoreText);
                Text score;
                score.setString("Score");
                score.setFont(gamefont);
                score.setPosition(Vector2f(1250, 550));
                window.draw(score);
            }

            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }
            window.display();
        }
    }

}
