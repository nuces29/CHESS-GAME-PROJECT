#include <SFML/Graphics.hpp>
#include <iostream>

const int EMPTY = 0;
const int PAWN = 1;
const int ROOK = 2;
const int KNIGHT = 3;
const int BISHOP = 4;
const int QUEEN = 5;
const int KING = 6;

const int WHITE = 0;
const int BLACK = 1;

const int PLAYING = 0;
const int CHECK = 1;
const int CHECKMATE = 2;

int board[8][8];
int boardColor[8][8];
int currentTurn = WHITE;
int selectedRow = -1;
int selectedCol = -1;
int pieceSelected = 0;
int validMoves[8][8];
sf::Texture textures[2][7];
sf::Sprite sprites[8][8];

int capturedWhite[16];
int capturedBlack[16];
int capturedWhiteCount = 0;
int capturedBlackCount = 0;

int gameState = PLAYING;
int checkedKingRow = -1;
int checkedKingCol = -1;
int winnerColor = -1;

float squareSize = 0;
float boardStartX = 0;
float boardStartY = 0;

void loadtextures() {
    textures[WHITE][PAWN].loadFromFile("IMAGES/w_pawn.png");
    textures[WHITE][ROOK].loadFromFile("IMAGES/w_rook.png");
    textures[WHITE][KNIGHT].loadFromFile("IMAGES/w_knight.png");
    textures[WHITE][BISHOP].loadFromFile("IMAGES/w_bishop.png");
    textures[WHITE][QUEEN].loadFromFile("IMAGES/w_queen.png");
    textures[WHITE][KING].loadFromFile("IMAGES/w_king.png");

    textures[BLACK][PAWN].loadFromFile("IMAGES/b_pawn.png");
    textures[BLACK][ROOK].loadFromFile("IMAGES/b_rook.png");
    textures[BLACK][KNIGHT].loadFromFile("IMAGES/b_knight.png");
    textures[BLACK][BISHOP].loadFromFile("IMAGES/b_bishop.png");
    textures[BLACK][QUEEN].loadFromFile("IMAGES/b_queen.png");
    textures[BLACK][KING].loadFromFile("IMAGES/b_king.png");
}

void initializeboard() {
    int row, col;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            board[row][col] = EMPTY;
            boardColor[row][col] = WHITE;
        }
    }

    board[7][0] = ROOK;
    boardColor[7][0] = WHITE;
    board[7][1] = KNIGHT;
    boardColor[7][1] = WHITE;
    board[7][2] = BISHOP;
    boardColor[7][2] = WHITE;
    board[7][3] = QUEEN;
    boardColor[7][3] = WHITE;
    board[7][4] = KING;
    boardColor[7][4] = WHITE;
    board[7][5] = BISHOP;
    boardColor[7][5] = WHITE;
    board[7][6] = KNIGHT;
    boardColor[7][6] = WHITE;
    board[7][7] = ROOK;
    boardColor[7][7] = WHITE;

    for (col = 0; col < 8; col++) {
        board[6][col] = PAWN;
        boardColor[6][col] = WHITE;
    }

    board[0][0] = ROOK;
    boardColor[0][0] = BLACK;
    board[0][1] = KNIGHT;
    boardColor[0][1] = BLACK;
    board[0][2] = BISHOP;
    boardColor[0][2] = BLACK;
    board[0][3] = QUEEN;
    boardColor[0][3] = BLACK;
    board[0][4] = KING;
    boardColor[0][4] = BLACK;
    board[0][5] = BISHOP;
    boardColor[0][5] = BLACK;
    board[0][6] = KNIGHT;
    boardColor[0][6] = BLACK;
    board[0][7] = ROOK;
    boardColor[0][7] = BLACK;

    for (col = 0; col < 8; col++) {
        board[1][col] = PAWN;
        boardColor[1][col] = BLACK;
    }

    capturedWhiteCount = 0;
    capturedBlackCount = 0;
    gameState = PLAYING;
    checkedKingRow = -1;
    checkedKingCol = -1;
    winnerColor = -1;
}

int findking(int kingcolor) {
    int row, col;
    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (board[row][col] == KING && boardColor[row][col] == kingcolor) {
                return row * 8 + col;
            }
        }
    }
    return -1;
}

int ispathclear(int fromrow, int fromcol, int torow, int tocol) {
    int rowdirection = 0, coldirection = 0;
    int row, col;

    if (torow > fromrow) rowdirection = 1;
    else if (torow < fromrow) rowdirection = -1;

    if (tocol > fromcol) coldirection = 1;
    else if (tocol < fromcol) coldirection = -1;

    row = fromrow + rowdirection;
    col = fromcol + coldirection;

    while (row != torow || col != tocol) {
        if (board[row][col] != EMPTY) return 0;
        row += rowdirection;
        col += coldirection;
    }
    return 1;
}

int isvalidpawnmove(int fromrow, int fromcol, int torow, int tocol) {
    int color = boardColor[fromrow][fromcol];

    if (color == WHITE) {
        if (tocol != fromcol) {
            if (torow == fromrow - 1 && (tocol == fromcol - 1 || tocol == fromcol + 1)) {
                if (board[torow][tocol] != EMPTY && boardColor[torow][tocol] != color) {
                    return 1;
                }
            }
            return 0;
        }
        if (torow == fromrow - 1 && board[torow][tocol] == EMPTY) {
            return 1;
        }
    }
    else {
        if (tocol != fromcol) {
            if (torow == fromrow + 1 && (tocol == fromcol - 1 || tocol == fromcol + 1)) {
                if (board[torow][tocol] != EMPTY && boardColor[torow][tocol] != color) {
                    return 1;
                }
            }
            return 0;
        }
        if (torow == fromrow + 1 && board[torow][tocol] == EMPTY) {
            return 1;
        }
    }
    return 0;
}

int isvalidrookmove(int fromrow, int fromcol, int torow, int tocol) {
    if (fromrow != torow && fromcol != tocol) return 0;
    if (fromrow == torow && fromcol == tocol) return 0;
    return ispathclear(fromrow, fromcol, torow, tocol);
}

int isvalidbishopmove(int fromrow, int fromcol, int torow, int tocol) {
    int rowdiff = torow - fromrow;
    int coldiff = tocol - fromcol;

    if (rowdiff < 0) rowdiff = -rowdiff;
    if (coldiff < 0) coldiff = -coldiff;

    if (rowdiff != coldiff) return 0;
    if (fromrow == torow && fromcol == tocol) return 0;
    return ispathclear(fromrow, fromcol, torow, tocol);
}

int isvalidknightmove(int fromrow, int fromcol, int torow, int tocol) {
    int rowdiff = torow - fromrow;
    int coldiff = tocol - fromcol;

    if (rowdiff < 0) rowdiff = -rowdiff;
    if (coldiff < 0) coldiff = -coldiff;

    return (rowdiff == 2 && coldiff == 1) || (rowdiff == 1 && coldiff == 2);
}

int isvalidqueenmove(int fromrow, int fromcol, int torow, int tocol) {
    return isvalidrookmove(fromrow, fromcol, torow, tocol) ||
        isvalidbishopmove(fromrow, fromcol, torow, tocol);
}

int isvalidkingmove(int fromrow, int fromcol, int torow, int tocol) {
    int rowdiff = torow - fromrow;
    int coldiff = tocol - fromcol;

    if (rowdiff < 0) rowdiff = -rowdiff;
    if (coldiff < 0) coldiff = -coldiff;

    return rowdiff <= 1 && coldiff <= 1 && !(fromrow == torow && fromcol == tocol);
}

int isvalidmove(int fromrow, int fromcol, int torow, int tocol) {
    int piecetype;
    int movingcolor;
    int targettype;
    int targetcolor;

    if (torow < 0 || torow >= 8 || tocol < 0 || tocol >= 8) return 0;
    if (fromrow == torow && fromcol == tocol) return 0;

    movingcolor = boardColor[fromrow][fromcol];
    targettype = board[torow][tocol];
    targetcolor = boardColor[torow][tocol];

    if (targettype != EMPTY && targetcolor == movingcolor) {
        return 0;
    }

    piecetype = board[fromrow][fromcol];

    if (piecetype == PAWN) return isvalidpawnmove(fromrow, fromcol, torow, tocol);
    if (piecetype == ROOK) return isvalidrookmove(fromrow, fromcol, torow, tocol);
    if (piecetype == BISHOP) return isvalidbishopmove(fromrow, fromcol, torow, tocol);
    if (piecetype == KNIGHT) return isvalidknightmove(fromrow, fromcol, torow, tocol);
    if (piecetype == QUEEN) return isvalidqueenmove(fromrow, fromcol, torow, tocol);
    if (piecetype == KING) return isvalidkingmove(fromrow, fromcol, torow, tocol);

    return 0;
}

int canpieceattack(int row, int col, int targetrow, int targetcol) {
    int piecetype = board[row][col];
    int color = boardColor[row][col];

    if (piecetype == EMPTY) return 0;
    if (boardColor[targetrow][targetcol] == color) return 0;

    if (piecetype == PAWN) {
        if (color == WHITE) {
            if (targetrow == row - 1 && (targetcol == col - 1 || targetcol == col + 1)) {
                return 1;
            }
        }
        else {
            if (targetrow == row + 1 && (targetcol == col - 1 || targetcol == col + 1)) {
                return 1;
            }
        }
    }
    else if (piecetype == ROOK) {
        return isvalidrookmove(row, col, targetrow, targetcol);
    }
    else if (piecetype == BISHOP) {
        return isvalidbishopmove(row, col, targetrow, targetcol);
    }
    else if (piecetype == KNIGHT) {
        return isvalidknightmove(row, col, targetrow, targetcol);
    }
    else if (piecetype == QUEEN) {
        return isvalidqueenmove(row, col, targetrow, targetcol);
    }
    else if (piecetype == KING) {
        return isvalidkingmove(row, col, targetrow, targetcol);
    }

    return 0;
}

int iskingincheck(int kingcolor) {
    int kingpos = findking(kingcolor);
    if (kingpos == -1) return 0;

    int kingrow = kingpos / 8;
    int kingcol = kingpos % 8;

    int row, col;
    int enemycolor = (kingcolor == WHITE) ? BLACK : WHITE;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (boardColor[row][col] == enemycolor) {
                if (canpieceattack(row, col, kingrow, kingcol)) {
                    checkedKingRow = kingrow;
                    checkedKingCol = kingcol;
                    return 1;
                }
            }
        }
    }

    checkedKingRow = -1;
    checkedKingCol = -1;
    return 0;
}

int wouldkingbeincheck(int fromrow, int fromcol, int torow, int tocol, int color) {
    int oldpiece = board[torow][tocol];
    int oldcolor = boardColor[torow][tocol];

    board[torow][tocol] = board[fromrow][fromcol];
    boardColor[torow][tocol] = boardColor[fromrow][fromcol];
    board[fromrow][fromcol] = EMPTY;
    boardColor[fromrow][fromcol] = WHITE;

    int kingpos = findking(color);
    int kingrow = kingpos / 8;
    int kingcol = kingpos % 8;

    int row, col;
    int enemycolor = (color == WHITE) ? BLACK : WHITE;
    int incheck = 0;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (boardColor[row][col] == enemycolor) {
                if (canpieceattack(row, col, kingrow, kingcol)) {
                    incheck = 1;
                    break;
                }
            }
        }
        if (incheck) break;
    }

    board[fromrow][fromcol] = board[torow][tocol];
    boardColor[fromrow][fromcol] = boardColor[torow][tocol];
    board[torow][tocol] = oldpiece;
    boardColor[torow][tocol] = oldcolor;

    return incheck;
}

int hasanyvalidmove(int color) {
    int row, col, targetrow, targetcol;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (boardColor[row][col] == color) {
                for (targetrow = 0; targetrow < 8; targetrow++) {
                    for (targetcol = 0; targetcol < 8; targetcol++) {
                        if (isvalidmove(row, col, targetrow, targetcol)) {
                            if (!wouldkingbeincheck(row, col, targetrow, targetcol, color)) {
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void checkgamestate() {
    int kingcolor = currentTurn;

    if (iskingincheck(kingcolor)) {
        if (!hasanyvalidmove(kingcolor)) {
            gameState = CHECKMATE;
            winnerColor = (kingcolor == WHITE) ? BLACK : WHITE;
        }
        else {
            gameState = CHECK;
        }
    }
    else {
        gameState = PLAYING;
    }
}

void promotepawn(int row, int col) {
    if (board[row][col] == 1) {
        int color = boardColor[row][col];

        if (color == WHITE && row == 0) {
            board[row][col] = QUEEN;
        }
        else if (color == BLACK && row == 7) {
            board[row][col] = QUEEN;
        }
    }
}

void calculatevalidmoves() {
    int row, col;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            validMoves[row][col] = 0;
        }
    }

    if (pieceSelected && selectedRow >= 0 && selectedCol >= 0) {
        for (row = 0; row < 8; row++) {
            for (col = 0; col < 8; col++) {
                if (isvalidmove(selectedRow, selectedCol, row, col)) {
                    if (!wouldkingbeincheck(selectedRow, selectedCol, row, col, boardColor[selectedRow][selectedCol])) {
                        validMoves[row][col] = 1;
                    }
                }
            }
        }
    }
}

void movepiece(int fromrow, int fromcol, int torow, int tocol) {
    int capturedpiece = board[torow][tocol];
    int capturedcolor = boardColor[torow][tocol];

    if (capturedpiece != EMPTY && capturedpiece != KING) {
        if (capturedcolor == WHITE) {
            capturedWhite[capturedWhiteCount] = capturedpiece;
            capturedWhiteCount++;
        }
        else {
            capturedBlack[capturedBlackCount] = capturedpiece;
            capturedBlackCount++;
        }
    }

    board[torow][tocol] = board[fromrow][fromcol];
    boardColor[torow][tocol] = boardColor[fromrow][fromcol];
    board[fromrow][fromcol] = EMPTY;
    boardColor[fromrow][fromcol] = WHITE;

    promotepawn(torow, tocol);

    if (currentTurn == WHITE) {
        currentTurn = BLACK;
    }
    else {
        currentTurn = WHITE;
    }

    checkgamestate();
}

void handleclick(int row, int col) {
    if (gameState == CHECKMATE) return;
    if (row < 0 || row >= 8 || col < 0 || col >= 8) return;

    if (pieceSelected == 0) {
        if (board[row][col] != EMPTY && boardColor[row][col] == currentTurn) {
            selectedRow = row;
            selectedCol = col;
            pieceSelected = 1;
            calculatevalidmoves();
        }
    }
    else {
        if (isvalidmove(selectedRow, selectedCol, row, col)) {
            movepiece(selectedRow, selectedCol, row, col);
            pieceSelected = 0;
            selectedRow = -1;
            selectedCol = -1;
            calculatevalidmoves();
        }
        else {
            pieceSelected = 0;
            selectedRow = -1;
            selectedCol = -1;
            calculatevalidmoves();
        }
    }
}

void updatesprites() {
    int row, col;
    float spritesize = squareSize * 0.7f;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (board[row][col] != EMPTY) {
                sprites[row][col].setTexture(textures[boardColor[row][col]][board[row][col]]);
                sprites[row][col].setPosition(
                    boardStartX + col * squareSize + (squareSize - spritesize) / 2,
                    boardStartY + row * squareSize + (squareSize - spritesize) / 2
                );
                sprites[row][col].setScale(spritesize / 100.0f, spritesize / 100.0f);
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Chess Game", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    float windowwidth = window.getSize().x;
    float windowheight = window.getSize().y;
    float boardwidth = windowheight * 0.9f;

    squareSize = boardwidth / 8.0f;
    boardStartX = (windowwidth - boardwidth) / 2.0f;
    boardStartY = (windowheight - boardwidth) / 2.0f;

    initializeboard();
    loadtextures();

    int row, col;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                initializeboard();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                float mousex = event.mouseButton.x;
                float mousey = event.mouseButton.y;

                int col2 = (mousex - boardStartX) / squareSize;
                int row2 = (mousey - boardStartY) / squareSize;
                handleclick(row2, col2);
            }
        }

        window.clear(sf::Color::Black);

        for (row = 0; row < 8; row++) {
            for (col = 0; col < 8; col++) {
                sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
                square.setPosition(boardStartX + col * squareSize, boardStartY + row * squareSize);

                if ((row + col) % 2 == 0) {
                    square.setFillColor(sf::Color(173, 216, 230));
                }
                else {
                    square.setFillColor(sf::Color(0, 102, 204));
                }

                if (gameState == CHECK && checkedKingRow == row && checkedKingCol == col) {
                    square.setFillColor(sf::Color::Red);
                }

                if (pieceSelected && selectedRow == row && selectedCol == col) {
                    square.setFillColor(sf::Color::Yellow);
                }

                if (validMoves[row][col] == 1) {
                    square.setFillColor(sf::Color::Green);
                }

                window.draw(square);
            }
        }

        updatesprites();
        for (row = 0; row < 8; row++) {
            for (col = 0; col < 8; col++) {
                if (board[row][col] != EMPTY) {
                    window.draw(sprites[row][col]);
                }
            }
        }

        float capturedx = 20;
        float capturedy = 20;
        for (int i = 0; i < capturedBlackCount; i++) {
            sf::Sprite capturedsprite;
            capturedsprite.setTexture(textures[BLACK][capturedBlack[i]]);
            capturedsprite.setPosition(capturedx, capturedy);
            capturedsprite.setScale(0.3f, 0.3f);
            window.draw(capturedsprite);
            capturedx += 35;
            if (capturedx > 200) {
                capturedx = 20;
                capturedy += 35;
            }
        }

        capturedx = windowwidth - 200;
        capturedy = 20;
        for (int i = 0; i < capturedWhiteCount; i++) {
            sf::Sprite capturedsprite;
            capturedsprite.setTexture(textures[WHITE][capturedWhite[i]]);
            capturedsprite.setPosition(capturedx, capturedy);
            capturedsprite.setScale(0.3f, 0.3f);
            window.draw(capturedsprite);
            capturedx += 35;
            if (capturedx > windowwidth - 20) {
                capturedx = windowwidth - 200;
                capturedy += 35;
            }
        }

        if (gameState == CHECK) {
            std::cout << "CHECK! " << (currentTurn == WHITE ? "White" : "Black") << " King is in check!" << std::endl;
        }
        else if (gameState == CHECKMATE) {
            std::cout << "CHECKMATE! " << (winnerColor == WHITE ? "White" : "Black") << " wins!" << std::endl;
        }

        window.display();
    }

    return 0;
}