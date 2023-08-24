#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include "chip8.h"

const int screenWidth = 640;
const int screenHeight = 320;
const int pixelSize = 10;

int main()
{

	std::cout << "Launched Emulator" << std::endl;

    Chip8 emulator;
    emulator.initialize();
    emulator.loadGame("C:\\Development\\Cpp\\Learning_SFML\\Learning_SFML\\CHIP-8\\roms\\SpaceInvaders.ch8");


    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "CHIP-8 Emulator");
    sf::Clock clock;
    sf::Time timePerFrame = sf::seconds(1.0f / 60.0f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
            {
                bool newBool = (event.type == sf::Event::KeyPressed) ? true : false;
                if (event.key.code == sf::Keyboard::Num1) emulator.key[0x0] = newBool;
                if (event.key.code == sf::Keyboard::Num2) emulator.key[0x1] = newBool;
                if (event.key.code == sf::Keyboard::Num3) emulator.key[0x2] = newBool;
                if (event.key.code == sf::Keyboard::Num4) emulator.key[0x3] = newBool;
                if (event.key.code == sf::Keyboard::Q) emulator.key[0x4] = newBool;
                if (event.key.code == sf::Keyboard::W) emulator.key[0x5] = newBool;
                if (event.key.code == sf::Keyboard::E) emulator.key[0x6] = newBool;
                if (event.key.code == sf::Keyboard::R) emulator.key[0x7] = newBool;
                if (event.key.code == sf::Keyboard::A) emulator.key[0x8] = newBool;
                if (event.key.code == sf::Keyboard::S) emulator.key[0x9] = newBool;
                if (event.key.code == sf::Keyboard::D) emulator.key[0xA] = newBool;
                if (event.key.code == sf::Keyboard::F) emulator.key[0xB] = newBool;
                if (event.key.code == sf::Keyboard::Z) emulator.key[0xC] = newBool;
                if (event.key.code == sf::Keyboard::X) emulator.key[0xD] = newBool;
                if (event.key.code == sf::Keyboard::C) emulator.key[0xE] = newBool;
                if (event.key.code == sf::Keyboard::V) emulator.key[0xF] = newBool;
                // I hate doing those many if statements, i should you use a dictionary
            }

        }

        emulator.emulateCycle(window);
        
        for (int i = 0; i < 2048; i++)
        {
            if (emulator.screen[i] == 1)
            {
                int x = (i % 64) * pixelSize;
                int y = (i / 64) * pixelSize;
                sf::RectangleShape pixel(sf::Vector2f(pixelSize, pixelSize));
                pixel.setPosition(x, y);
                pixel.setFillColor(sf::Color::White);
                window.draw(pixel);
            }
        }
        
        window.display();

        // Ensure 60 Hz
        sf::Time elapsedTime = clock.getElapsedTime();
        sf::Time sleepTime = timePerFrame - elapsedTime;
        if (sleepTime > sf::Time::Zero)
            sf::sleep(sleepTime);
        clock.restart();

    }
    

    return 0;

}