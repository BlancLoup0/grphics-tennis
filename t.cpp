#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <string>

#ifdef SFML_SYSTEM_IOS
#include <SFML/Main.hpp>
#endif

std::string resourcesDir()
{
#ifdef SFML_SYSTEM_IOS
    return "";
#else
    return "resources/";
#endif
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    const float pi = 3.14159f;
    const float gameWidth = 800;
    const float gameHeight = 600;
    const sf::Vector2f paddleSize(25, 100);
    const float ballRadius = 10.f;
    int leftPlayerScore = 0;
    int rightPlayerScore = 0;

    sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(gameWidth), static_cast<unsigned int>(gameHeight), 32), "SFML Tennis",
        sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    sf::SoundBuffer ballSoundBuffer;
    if (!ballSoundBuffer.loadFromFile(resourcesDir() + "ball.wav"))
        return EXIT_FAILURE;
    sf::Sound ballSound(ballSoundBuffer);

    sf::Texture sfmlLogoTexture;
    if (!sfmlLogoTexture.loadFromFile(resourcesDir() + "sfml_logo.png"))
        return EXIT_FAILURE;
    sf::Sprite sfmlLogo;
    sfmlLogo.setTexture(sfmlLogoTexture);
    sfmlLogo.setPosition(170, 50);

    sf::RectangleShape leftPaddle;
    leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    leftPaddle.setOutlineThickness(3);
    leftPaddle.setOutlineColor(sf::Color::Black);
    leftPaddle.setFillColor(sf::Color(100, 100, 200));
    leftPaddle.setOrigin(paddleSize / 2.f);

    sf::RectangleShape rightPaddle;
    rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    rightPaddle.setOutlineThickness(3);
    rightPaddle.setOutlineColor(sf::Color::Black);
    rightPaddle.setFillColor(sf::Color(200, 100, 100));
    rightPaddle.setOrigin(paddleSize / 2.f);

    std::vector<sf::CircleShape> balls;
    std::vector<float> ballAngles;

    auto addBall = [&balls, &ballAngles, ballRadius, gameWidth, gameHeight, pi]()
    {
        sf::CircleShape ball;
        ball.setRadius(ballRadius - 3);
        ball.setOutlineThickness(2);
        ball.setOutlineColor(sf::Color::Black);
        ball.setFillColor(sf::Color::White);
        ball.setOrigin(ballRadius / 2, ballRadius / 2);
        ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);

        float ballAngle;
        do
        {
            ballAngle = static_cast<float>(std::rand() % 360) * 2.f * pi / 360.f;
        } while (std::abs(std::cos(ballAngle)) < 0.7f);

        balls.push_back(ball);
        ballAngles.push_back(ballAngle);
    };

    addBall();

    sf::Font font;
    if (!font.loadFromFile(resourcesDir() + "tuffy.ttf"))
        return EXIT_FAILURE;

    sf::Text leftScoreText;
    sf::Text rightScoreText;
    leftScoreText.setFont(font);
    rightScoreText.setFont(font);
    leftScoreText.setCharacterSize(24);
    rightScoreText.setCharacterSize(24);
    leftScoreText.setPosition(50.f, 50.f);
    rightScoreText.setPosition(gameWidth - 150.f, 50.f);
    leftScoreText.setFillColor(sf::Color::White);
    rightScoreText.setFillColor(sf::Color::White);

    sf::Text pauseMessage;
    pauseMessage.setFont(font);
    pauseMessage.setCharacterSize(40);
    pauseMessage.setPosition(170.f, 200.f);
    pauseMessage.setFillColor(sf::Color::White);

#ifdef SFML_SYSTEM_IOS
    pauseMessage.setString("Welcome to SFML Tennis!\nTouch the screen to start the game.");
#else
    pauseMessage.setString("Welcome to SFML Tennis!\n\nPress space to start the game.");
#endif

    sf::Clock AITimer;
    const sf::Time AITime = sf::seconds(0.1f);
    const float paddleSpeed = 400.f;
    float rightPaddleSpeed = 0.f;
    const float ballSpeed = 400.f;

    sf::Clock clock;
    bool isPlaying = false;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if ((event.type == sf::Event::Closed) ||
                ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }

            if (((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space)) ||
                (event.type == sf::Event::TouchBegan))
            {
                if (!isPlaying)
                {
                    isPlaying = true;
                    clock.restart();

                    leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
                    rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
                    leftPaddle.setOrigin(leftPaddle.getSize() / 2.f);
                    rightPaddle.setOrigin(rightPaddle.getSize() / 2.f);
                    leftPaddle.setPosition(10.f + paddleSize.x / 2.f, gameHeight / 2.f);
                    rightPaddle.setPosition(gameWidth - 10.f - paddleSize.x / 2.f, gameHeight / 2.f);

                    leftPlayerScore = 0;
                    rightPlayerScore = 0;

                    balls.clear();
                    ballAngles.clear();
                    addBall();
                }
            }

            if (event.type == sf::Event::Resized)
            {
                sf::View view;
                view.setSize(gameWidth, gameHeight);
                view.setCenter(gameWidth / 2.f, gameHeight / 2.f);
                window.setView(view);
            }
        }

        if (isPlaying)
        {
            float deltaTime = clock.restart().asSeconds();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
                (leftPaddle.getPosition().y - leftPaddle.getSize().y / 2 > 5.f))
            {
                leftPaddle.move(0.f, -paddleSpeed * deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
                (leftPaddle.getPosition().y + leftPaddle.getSize().y / 2 < gameHeight - 5.f))
            {
                leftPaddle.move(0.f, paddleSpeed * deltaTime);
            }

            if (sf::Touch::isDown(0))
            {
                sf::Vector2i pos = sf::Touch::getPosition(0);
                sf::Vector2f mappedPos = window.mapPixelToCoords(pos);
                leftPaddle.setPosition(leftPaddle.getPosition().x, mappedPos.y);
            }

            if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - rightPaddle.getSize().y / 2 > 5.f)) ||
                ((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y + rightPaddle.getSize().y / 2 < gameHeight - 5.f)))
            {
                rightPaddle.move(0.f, rightPaddleSpeed * deltaTime);
            }

            if (AITimer.getElapsedTime() > AITime)
            {
                AITimer.restart();
                if (!balls.empty())
                {
                    if (balls[0].getPosition().y + ballRadius > rightPaddle.getPosition().y + rightPaddle.getSize().y / 2)
                        rightPaddleSpeed = paddleSpeed;
                    else if (balls[0].getPosition().y - ballRadius < rightPaddle.getPosition().y - rightPaddle.getSize().y / 2)
                        rightPaddleSpeed = -paddleSpeed;
                    else
                        rightPaddleSpeed = 0.f;
                }
            }

            for (size_t i = 0; i < balls.size(); ++i)
            {
                if (i >= ballAngles.size())
                    continue; // 예외 방지

                float factor = ballSpeed * deltaTime;
                balls[i].move(std::cos(ballAngles[i]) * factor, std::sin(ballAngles[i]) * factor);

                if (balls[i].getPosition().x - ballRadius < 0.f)
                {
                    rightPlayerScore++;
                    rightPaddle.setSize(rightPaddle.getSize() - sf::Vector2f(0, 10));
                    rightPaddle.setOrigin(rightPaddle.getSize() / 2.f);

                    leftPaddle.setPosition(10.f + paddleSize.x / 2.f, gameHeight / 2.f);
                    rightPaddle.setPosition(gameWidth - 10.f - paddleSize.x / 2.f, gameHeight / 2.f);

                    balls.clear();
                    ballAngles.clear();
                    addBall();

                    if (rightPlayerScore % 5 == 0)
                    {
                        addBall();
                    }

                    if (rightPlayerScore >= 10)
                    {
                        isPlaying = false;
                        pauseMessage.setString("Right Player Wins!\n\nPress space to restart");
                    }
                }

                if (balls[i].getPosition().x + ballRadius > gameWidth)
                {
                    leftPlayerScore++;
                    leftPaddle.setSize(leftPaddle.getSize() - sf::Vector2f(0, 10));
                    leftPaddle.setOrigin(leftPaddle.getSize() / 2.f);

                    leftPaddle.setPosition(10.f + paddleSize.x / 2.f, gameHeight / 2.f);
                    rightPaddle.setPosition(gameWidth - 10.f - paddleSize.x / 2.f, gameHeight / 2.f);

                    balls.clear();
                    ballAngles.clear();
                    addBall();

                    if (leftPlayerScore % 5 == 0)
                    {
                        addBall();
                    }

                    if (leftPlayerScore >= 10)
                    {
                        isPlaying = false;
                        pauseMessage.setString("Left Player Wins!\n\nPress space to restart");
                    }
                }

                if (balls[i].getPosition().y - ballRadius < 0.f)
                {
                    ballSound.play();
                    ballAngles[i] = -ballAngles[i];
                    balls[i].setPosition(balls[i].getPosition().x, ballRadius + 0.1f);
                }

                if (balls[i].getPosition().y + ballRadius > gameHeight)
                {
                    ballSound.play();
                    ballAngles[i] = -ballAngles[i];
                    balls[i].setPosition(balls[i].getPosition().x, gameHeight - ballRadius - 0.1f);
                }

                if (balls[i].getPosition().x - ballRadius < leftPaddle.getPosition().x + leftPaddle.getSize().x / 2 &&
                    balls[i].getPosition().x - ballRadius > leftPaddle.getPosition().x &&
                    balls[i].getPosition().y + ballRadius >= leftPaddle.getPosition().y - leftPaddle.getSize().y / 2 &&
                    balls[i].getPosition().y - ballRadius <= leftPaddle.getPosition().y + leftPaddle.getSize().y / 2)
                {
                    if (balls[i].getPosition().y > leftPaddle.getPosition().y)
                        ballAngles[i] = pi - ballAngles[i] + static_cast<float>(std::rand() % 20) * pi / 180;
                    else
                        ballAngles[i] = pi - ballAngles[i] - static_cast<float>(std::rand() % 20) * pi / 180;

                    ballSound.play();
                    balls[i].setPosition(leftPaddle.getPosition().x + ballRadius + leftPaddle.getSize().x / 2 + 0.1f, balls[i].getPosition().y);
                }

                if (balls[i].getPosition().x + ballRadius > rightPaddle.getPosition().x - rightPaddle.getSize().x / 2 &&
                    balls[i].getPosition().x + ballRadius < rightPaddle.getPosition().x &&
                    balls[i].getPosition().y + ballRadius >= rightPaddle.getPosition().y - rightPaddle.getSize().y / 2 &&
                    balls[i].getPosition().y - ballRadius <= rightPaddle.getPosition().y + rightPaddle.getSize().y / 2)
                {
                    if (balls[i].getPosition().y > rightPaddle.getPosition().y)
                        ballAngles[i] = pi - ballAngles[i] + static_cast<float>(std::rand() % 20) * pi / 180;
                    else
                        ballAngles[i] = pi - ballAngles[i] - static_cast<float>(std::rand() % 20) * pi / 180;

                    ballSound.play();
                    balls[i].setPosition(rightPaddle.getPosition().x - ballRadius - rightPaddle.getSize().x / 2 - 0.1f, balls[i].getPosition().y);
                }
            }
        }

        window.clear(sf::Color(50, 50, 50));

        leftScoreText.setString("Left Player: " + std::to_string(leftPlayerScore));
        rightScoreText.setString("Right Player: " + std::to_string(rightPlayerScore));

        if (isPlaying)
        {
            window.draw(leftPaddle);
            window.draw(rightPaddle);
            for (const auto& ball : balls)
            {
                window.draw(ball);
            }
            window.draw(leftScoreText);
            window.draw(rightScoreText);
        }
        else
        {
            window.draw(pauseMessage);
            window.draw(sfmlLogo);
        }

        window.display();
    }

    return EXIT_SUCCESS;
}
