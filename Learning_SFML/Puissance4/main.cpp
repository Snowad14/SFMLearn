#include <SFML/Graphics.hpp>
#include <iostream>

const int ROWS = 6;
const int COLUMNS = 7;
const int CELL_SIZE = 150;
const int PADDING = 10;
const int WIDTH = COLUMNS * CELL_SIZE + 2 * PADDING;
const int HEIGHT = ROWS * CELL_SIZE + 2 * PADDING;

class BoardPiece {
    sf::RectangleShape squarePiece;
    sf::CircleShape circleInside;
    
public:
    int row;
    int column;

    BoardPiece() {};
    BoardPiece(int row, int column) {
        this->row = row;
        this->column = column;
        squarePiece.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        squarePiece.setFillColor(sf::Color::Blue);
        squarePiece.setPosition(column * CELL_SIZE + PADDING, row * CELL_SIZE + PADDING);

        circleInside.setRadius(CELL_SIZE / 2 - PADDING);
        circleInside.setFillColor(sf::Color::White);
        circleInside.setPosition(column * CELL_SIZE + 2 * PADDING, row * CELL_SIZE + 2 * PADDING);
        circleInside.setOutlineThickness(4);
        circleInside.setOutlineColor(sf::Color::Black);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(squarePiece);
        window.draw(circleInside);
    }

    void setColor(sf::Color newColor)
    {
        circleInside.setFillColor(newColor);
    }

    sf::Color getColor()
    {
        return circleInside.getFillColor();
    }

    bool isEmpty() {
        return circleInside.getFillColor() == sf::Color::White;
    }

    bool isGravited(std::vector<std::vector<BoardPiece>>& board) {
        for (int i = ROWS - 1; i >= row; i--)
        {
            if (board[i][column].isEmpty() && row != i) {
                return false;
            }
        }
        return true;
    }

};

std::vector<std::vector<BoardPiece>> createBoard() {
    std::vector<std::vector<BoardPiece>> board(ROWS, std::vector<BoardPiece>(COLUMNS));
    for (int row = 0; row < ROWS; ++row) {
        for (int column = 0; column < COLUMNS; ++column) {
            board[row][column] = BoardPiece(row, column);
        }
    }
    return board;
}

void drawBoard(std::vector<std::vector<BoardPiece>>& board, sf::RenderWindow& window) {
    for (int row = 0; row < ROWS; ++row) {
        for (int column = 0; column < COLUMNS; ++column) {
            board[row][column].draw(window);
        }
    }
}

bool checkPower4(std::vector<std::vector<BoardPiece>>& board) {
    sf::Color colors[] = { sf::Color::Red, sf::Color::Yellow };

    for (sf::Color color : colors) {
        for (int row = 0; row < ROWS; ++row) {
            for (int column = 0; column < COLUMNS; ++column) {
                if (board[row][column].getColor() == color) {
                    // Check horizontally
                    if (column + 3 < COLUMNS &&
                        board[row][column + 1].getColor() == color &&
                        board[row][column + 2].getColor() == color &&
                        board[row][column + 3].getColor() == color) {
                        return true;
                    }

                    // Check vertically
                    if (row + 3 < ROWS &&
                        board[row + 1][column].getColor() == color &&
                        board[row + 2][column].getColor() == color &&
                        board[row + 3][column].getColor() == color) {
                        return true;
                    }

                    // Check diagonal
                    if (row + 3 < ROWS && column + 3 < COLUMNS &&
                        board[row + 1][column + 1].getColor() == color &&
                        board[row + 2][column + 2].getColor() == color &&
                        board[row + 3][column + 3].getColor() == color) {
                        return true;
                    }

                    // Check diagonal
                    if (row - 3 >= 0 && column + 3 < COLUMNS &&
                        board[row - 1][column + 1].getColor() == color &&
                        board[row - 2][column + 2].getColor() == color &&
                        board[row - 3][column + 3].getColor() == color) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Puissance 4 - SFML", sf::Style::Close);
    std::vector<std::vector<BoardPiece>> board = createBoard();
    sf::Color player_1_color = sf::Color::Red;
    sf::Color player_2_color = sf::Color::Yellow;
    bool player_1_turn = true;

    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) 
            {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                int colNum = (event.mouseButton.x - PADDING) / CELL_SIZE;
                int rowNum = (event.mouseButton.y - PADDING) / CELL_SIZE;

                if (rowNum >= 0 && colNum >= 0 && rowNum < ROWS && colNum < COLUMNS)
                {
                    if (board[rowNum][colNum].isEmpty() && board[rowNum][colNum].isGravited(board)) {
                        sf::Color newColor = player_1_turn ? player_1_color : player_2_color;
                        board[rowNum][colNum].setColor(newColor);
                        if (checkPower4(board)) {
                            std::cout << "Player " << (player_1_turn ? "1" : "2") << " wins!" << std::endl;
                            board = createBoard();
                            player_1_turn = true;
                        }
                        else {
                            player_1_turn = !player_1_turn;
                        }
                    }
                }
            }

        }

        window.clear();
        drawBoard(board, window);
        window.display();
    }

    return 0;
}