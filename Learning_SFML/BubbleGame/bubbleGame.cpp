#include <SFML/Graphics.hpp>
#include <iostream>

int WIDTH = 1920;
int HEIGHT = 1080;

class Ball 
{
public:
	sf::CircleShape shape;
	sf::Vector2f speed = sf::Vector2f((rand() % 2000 - 1000) / 10000.0, (rand() % 2000 - 1000) / 10000.0);
	sf::Color color = sf::Color(rand() % 255, rand() % 255, rand() % 255);
	int radius = rand() % 35 + 15;

	Ball(sf::Vector2f initialPos) {
		shape.setRadius(radius);
		shape.setFillColor(color);
		shape.setPosition(initialPos.x, initialPos.y);
		shape.setOutlineThickness(4);
		shape.setOutlineColor(sf::Color::Black);
	}

	bool checkCollision(const sf::CircleShape& currentBubble, const sf::CircleShape& otherBubble) {
		float dx = currentBubble.getPosition().x + currentBubble.getRadius() - (otherBubble.getPosition().x + otherBubble.getRadius());
		float dy = currentBubble.getPosition().y + currentBubble.getRadius() - (otherBubble.getPosition().y + otherBubble.getRadius());
		float distance = std::sqrt(dx * dx + dy * dy);
		float radiusSum = currentBubble.getRadius() + otherBubble.getRadius();

		return distance <= radiusSum;
	}

	void update(std::vector<Ball>& balls) {
		sf::Vector2f currentPosition = shape.getPosition();
		if (currentPosition.x < 0 || currentPosition.x > WIDTH - radius * 2) {
			speed.x = -speed.x;
		}
		if (currentPosition.y < 0 || currentPosition.y > HEIGHT -  radius * 2) {
			speed.y = -speed.y;
		}

		for (auto& otherBall : balls) {
			if (this != &otherBall && checkCollision(shape, otherBall.shape) /* && colladed == false && otherBall.colladed == false */) {
				speed.x = -speed.x;
				speed.y = -speed.y;

			}
		}

		shape.setPosition(currentPosition.x + speed.x, currentPosition.y + speed.y);
	}

};

int main()
{

	sf::RenderWindow window (sf::VideoMode(WIDTH, HEIGHT), "Hello SFML" /*, sf::Style::Close */ );
	std::vector<Ball> balls;

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			switch (event.type) {

				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::MouseButtonPressed:
					balls.push_back(Ball(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)));
					break;
			}
				
		}

		window.clear(sf::Color::Blue);

		for (auto& ball : balls) {
			ball.update(balls);
			window.draw(ball.shape);
		}

		window.display();

	}

}
