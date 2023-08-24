#include <SFML/Graphics.hpp>
#include <iostream>


const int WIDTH = 800;
const int HEIGHT = 1080;
const int PIPE_FREQUENCY = 5000;
const int PIPE_HOLE = 250;
const float PIPE_SPEED = 0.1;
int terrain_height;


class Pipe
{
public:
	sf::Texture pipeTexture;

	sf::Sprite upperPipeSprite;
	int upper_pipe_length = (rand() % 500) + 50;

	sf::Sprite downPipeSprite;
	int down_pipe_length = upper_pipe_length + PIPE_HOLE;

	Pipe()
	{
		std::cout << upper_pipe_length << std::endl;
		std::cout << HEIGHT - terrain_height - upper_pipe_length << std::endl;
		pipeTexture.loadFromFile("Flapy Bird/sprites/pipe-green.png");
		upperPipeSprite.setTexture(pipeTexture);
		upperPipeSprite.setPosition(WIDTH + 500, upper_pipe_length);
		upperPipeSprite.setRotation(180);
		upperPipeSprite.setScale(
			1.75,
			1.75
		);
		downPipeSprite.setTexture(pipeTexture);
		downPipeSprite.setPosition(WIDTH- upperPipeSprite.getGlobalBounds().width + 500,  down_pipe_length);
		downPipeSprite.setScale(
			1.75,
			1.75
		);

		std::cout << "New Pipe" << std::endl;
	}

	void draw(sf::RenderWindow& window)
	{
		window.draw(upperPipeSprite);
		window.draw(downPipeSprite);
	}

};

class Bird
{
public:
	sf::Texture birdTexture;
	sf::Sprite birdSprite;

	Bird()
	{
		birdTexture.loadFromFile("Flapy Bird/sprites/yellowbird-midflap.png");
		birdSprite.setTexture(birdTexture);
		birdSprite.setPosition(150, 150);
		birdSprite.setScale(
			2,
			2
		);
	}

	void updatePosition(int jumpTick)
	{
		sf::Vector2f pos = birdSprite.getPosition();
		int rotateOffset = jumpTick / 16;
		if (jumpTick <= 0)
		{
			birdSprite.setPosition(pos.x, pos.y + 0.1);
			if (rotateOffset > -90)
			{
				birdSprite.setRotation(-rotateOffset);
			}
		}
		else
		{
			birdSprite.setPosition(pos.x, pos.y - 0.1);
			birdSprite.setRotation(-rotateOffset);

		}
	}

	bool isCollide(Pipe* pipe) // SFML pixel perfect not working so simple collide
	{

		sf::FloatRect birdRect = birdSprite.getGlobalBounds();
		sf::FloatRect upPipeRect = pipe->upperPipeSprite.getGlobalBounds();
		sf::FloatRect downPipeRect = pipe->downPipeSprite.getGlobalBounds();

		if (birdSprite.getPosition().y >= HEIGHT - terrain_height)
		{
			return true;
		}

		if (birdRect.intersects(upPipeRect) || birdRect.intersects(downPipeRect))
		{
			return true;
		}

		return false;

	}


	void draw(sf::RenderWindow& window)
	{
		window.draw(birdSprite);
	}

	void reset()
	{
		birdSprite.setPosition(150, 150);
	}

};

void updateMovableGround(sf::Sprite& groundSprite)
{
	sf::Vector2f currentPos = groundSprite.getPosition();
	groundSprite.setPosition(currentPos.x - 0.1, currentPos.y);

	if (currentPos.x < -215) {
		groundSprite.setPosition(0, HEIGHT - groundSprite.getGlobalBounds().height);
	} 
}

int main() {

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Flappy Bird", sf::Style::Close);
	sf::Clock clock;

	// Game Variable
	int new_pipe_time = 1000;
	std::vector<Pipe*> pipes;

	// Register texture & add sprite
	sf::Sprite backgroundSprite;
	sf::Texture backgroundTexture;
	backgroundTexture.loadFromFile("Flapy Bird/sprites/background-day.png");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setPosition(0, 0);
	backgroundSprite.setScale(
		static_cast<float>(WIDTH) / backgroundTexture.getSize().x,
		static_cast<float>(HEIGHT) / backgroundTexture.getSize().y
	);

	sf::Sprite groundSprite;
	sf::Texture groundTexture;
	groundTexture.loadFromFile("Flapy Bird/sprites/ground.png");
	groundSprite.setTexture(groundTexture);
	sf::Vector2f currentScale = groundSprite.getScale();
	sf::Vector2f newScale(currentScale.x * 3, currentScale.y * 2);
	groundSprite.setScale(newScale);
	groundSprite.setPosition(0, HEIGHT - groundSprite.getGlobalBounds().height);
	terrain_height = groundSprite.getGlobalBounds().height;

	Bird* bird = new Bird();
	int jumpTick = 0;


	// Game loop
	while (window.isOpen()) {

		sf::Event event;
		sf::Time deltaTime = clock.restart();
		float dt = deltaTime.asSeconds();

		while (window.pollEvent(event))
		{
			if (event.type ==  sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed)
			{
				jumpTick = 1500;
			}
		}

		new_pipe_time -= 1;
		if (new_pipe_time <= 0) {
			//std::cout << "hello" << std::endl;
			new_pipe_time = PIPE_FREQUENCY;
			pipes.push_back(new Pipe());
			std::cout << pipes.size() << std::endl;
		}

		window.clear();
		window.draw(backgroundSprite);
		for (auto it = pipes.begin(); it != pipes.end();)
		{
			Pipe* pipe = *it;
			sf::Vector2f upperCurrentPos = pipe->upperPipeSprite.getPosition();
			pipe->upperPipeSprite.setPosition(upperCurrentPos.x - PIPE_SPEED, upperCurrentPos.y);
			sf::Vector2f downCurrentPos = pipe->downPipeSprite.getPosition();
			pipe->downPipeSprite.setPosition(downCurrentPos.x - PIPE_SPEED, downCurrentPos.y);

			if (bird->isCollide(pipe)) // reset game
			{
				std::cout << "Game over" << std::endl;
				pipes.clear();
				bird->reset();
				break;
			}

			if (downCurrentPos.x < -150)
			{
				delete pipe;
				it = pipes.erase(it);
			}
			else
			{
				pipe->draw(window);
				++it;
			}

		}
		bird->updatePosition(jumpTick);
		bird->draw(window);
		updateMovableGround(groundSprite);
		window.draw(groundSprite);
		window.display();
		jumpTick--;
	}
}
