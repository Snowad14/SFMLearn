#include <SFML/Graphics.hpp>
#include <iostream>

const int ROWS = 10;
const int COLUMNS = 10;
const int CELL_SIZE = 25;
const int WIDTH = 1080;
const int HEIGHT = 1080;

class BattleShipPiece
{
public:
	sf::RectangleShape shape;

	BattleShipPiece(int cell_size, sf::Vector2f& position)
	{
		shape.setSize(sf::Vector2f(cell_size, cell_size));
		shape.setPosition(position);
		shape.setFillColor(sf::Color::Blue);
	}
};

class BattleShipBoard
{
	std::vector<std::vector<BattleShipPiece>> grid;

public:
	
	BattleShipBoard(int row, int col, int cell_size, sf::Vector2f position)
	{
		grid.resize(row);
		for (int i = 0; i < row; i++) {
			//grid[i].resize(col);
			for (int j = 0; j < col; j++) {
				sf::Vector2f newPos(position.x + i * cell_size, position.y + j * cell_size);
				grid[i].push_back(BattleShipPiece(cell_size, newPos));
			}
		}
	}

	void draw(sf::RenderWindow& window)
	{
		for (auto& row : grid)
		{
			for (auto& gridPiece : row)
			{
				window.draw(gridPiece.shape);
			}
		}
	}
};

int main() {
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Online Battleship", sf::Style::Close);
	auto board = BattleShipBoard(ROWS, COLUMNS, CELL_SIZE, sf::Vector2f(0, 0));
	auto board2 = BattleShipBoard(ROWS, COLUMNS, CELL_SIZE, sf::Vector2f(500, 500));
	while (window.isOpen()) {

		sf::Event event;

		while (window.pollEvent(event))
		{
			switch (event.type) {

			case sf::Event::Closed:
				window.close();
				break;

			}
		}

		window.clear();
		board.draw(window);
		board2.draw(window);
		window.display();
	}
}
