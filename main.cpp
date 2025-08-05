#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <limits>

std::string map[4][4];
int ships[4][4];

void resetGame()
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            map[i][j] = "?";
            ships[i][j] = 0;
        }
    }

    int deployShips = 4;
    int deployed = 0;
    while (deployed < deployShips)
    {
        int x = rand() % 4;
        int y = rand() % 4;
        if (ships[x][y] == 0)
        {
            ships[x][y] = 1;
            deployed++;
        }
    }
}

void showMap()
{
    std::cout << "Current Map:\n\n";
    std::cout << u8"\u2554\u2550\u2566\u2550\u2566\u2550\u2566\u2550\u2566\u2550\u2566\u2550\u2566\u2550\u2566\u2550\u2557\n";
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            std::string cell = map[i][j];

            std::cout << (j == 0 ? u8"\u2560 " : u8" \u256c ");

            if (cell == "X") cell = "\033[1;91mX\033[0m";
            else if (cell == "?") cell = "\033[1;93m?\033[0m";
            else if (cell == "-") cell = "\033[1;94m-\033[0m";
            else if (cell == "!") cell = "\033[1;92m!\033[0m";

            std::cout << cell;
        }
        std::cout << u8" \u2563\n";
    }
    std::cout << u8"\u255a\u2550\u2569\u2550\u2569\u2550\u2569\u2550\u2569\u2550\u2569\u2550\u2569\u2550\u2569\u2550\u255d\n\n";
}

void playGame()
{
    int shots = 4;
    int shipsFound = 0;
    int score = 0;

    resetGame();

    do
    {
        std::cout << "\033[2J\033[1;1H";
        if (shipsFound == 4 && shots > 0)
        {
            std::cout << "You Win! You found all the ships\n";
            std::cout.flush();
            break;
        }

        std::cout << "SHOTS: " << shots << "\n";
        std::cout.flush();
        showMap();
        int r, c;
        std::cout << "Select Row (1 - 4): ";
        std::cin >> r;
        std::cout << "Select Column (1 - 4): ";
        std::cin >> c;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        int row = r - 1;
        int col = c - 1;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (map[i][j] == "!") map[i][j] = "?";
            }
        }

        if (shots == 2 && shipsFound < 3)
        {
            std::vector<std::pair<int, int>> shipCell;
            std::vector<std::pair<int, int>> emptyCell;

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    if (map[i][j] == "!") continue;
                    if (ships[i][j] == 1 && map[i][j] == "?")
                        shipCell.push_back({i, j});
                    else if (ships[i][j] == 0 && map[i][j] == "?")
                        emptyCell.push_back({i, j});
                }
            }

            if (!shipCell.empty() && !emptyCell.empty())
            {
                auto shipPick = shipCell[rand() % shipCell.size()];
                auto emptyPick = emptyCell[rand() % emptyCell.size()];
                while (shipPick == emptyPick && emptyCell.size() > 1)
                {
                    emptyPick = emptyCell[rand() % emptyCell.size()];
                }

                map[shipPick.first][shipPick.second] = "!";
                map[emptyPick.first][emptyPick.second] = "!";
            }

            std::cout << u8"📡 Radar pulse! Two targets identified. Choose wisely...\n";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }

        if (row < 0 || row > 3 || col < 0 || col > 3)
        {
            std::cout << "Coordinates must be between 1 and 4! Try again.\n";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        if (ships[row][col] == 0)
        {
            std::cout << u8"❌ Miss! You lost a shot.\n";
            std::cout.flush();
            ships[row][col] = 2;
            map[row][col] = "-";
            shots--;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        if (ships[row][col] == 1)
        {
            std::cout << u8"💥 Hit! Good shot.\n";
            std::cout.flush();
            ships[row][col] = 3;
            map[row][col] = "X";
            shipsFound++;
            score += 10;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        if (ships[row][col] == 2 || ships[row][col] == 3)
        {
            std::cout << "Already fired there. Try somewhere else.\n";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

    } while (shots != 0);

    if (shipsFound < 4)
        std::cout << "You ran out of shots. Better luck next time!\n";
        std::cout.flush();
        
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (ships[i][j] == 1 && map[i][j] == "?")
            {
                map[i][j] = "\033[1;95mS\033[0m";
            }
        }
    }

    showMap();
    std::cout << "FINAL SCORE: " << score << "\n";
    std::cout.flush();
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::srand(static_cast<unsigned>(std::time(0)));

    char playAgain = 'y';
    do
    {
        playGame();
        std::cout << "\nPlay again? (y/n): ";
        std::cout.flush();
        std::cin >> playAgain;

        while (std::cin.fail() || (playAgain != 'y' && playAgain != 'Y' &&
                                   playAgain != 'n' && playAgain != 'N'))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Enter 'y' or 'n': ";
            std::cout.flush();
            std::cin >> playAgain;
        }

    } while (playAgain == 'y' || playAgain == 'Y');

    std::cout << "\nThanks for playing!\n";
    std::cout.flush();

    system("pause");
    return 0;
}