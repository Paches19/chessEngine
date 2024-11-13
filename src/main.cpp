// main.cpp
#include <iostream>
#include <thread>
#include <cstdlib>     // for std::rand and std::srand
#include <ctime>       // for std::time
#include "board.hpp"
#include "moves.hpp"
#include "ai.hpp"
#include "validation.hpp"

// Function to get all possible moves for a player
std::vector<std::pair<std::string, std::string>> get_all_moves(Board& board, int player)
{
    std::vector<std::pair<std::string, std::string>> moves;
    for (int x = 0; x < 8; ++x)
    {
        for (int y = 0; y < 8; ++y)
        {
            if ((player == 1 && board.get_piece(x, y) > 0) ||
                (player == -1 && board.get_piece(x, y) < 0))
            {
                auto possible_moves = get_moves(x, y, board.get_board());
                for (const auto& move : possible_moves)
                {
                    std::string from = index_to_chess(x, y);
                    std::string to = index_to_chess(move.first, move.second);
                    moves.emplace_back(from, to);
                }
            }
        }
    }
    return moves;
}

void play_auto_game(Board& board)
{
    int turn = 1; // 1 for White, -1 for Black
    int move_count = 0;

    while (true) {

        board.display();
        // Check for end-game conditions
        if (is_checkmate(board, turn)) {
            std::cout << (turn == 1 ? "Black" : "White") << " wins by checkmate!\n";
            break;
        } else if (is_stalemate(board, turn)) {
            std::cout << "The game is a draw by stalemate.\n";
            break;
        } else if (board.get_fifty_move_counter() >= 50) {
            std::cout << "The game is a draw by the 50-move rule.\n";
            break;
        } else if (board.is_threefold_repetition()) {
            std::cout << "The game is a draw by threefold repetition.\n";
            break;
        } else if (is_insufficient_material(board)) {
            std::cout << "The game is a draw due to insufficient material.\n";
            break;
        }

        // Get all possible moves for the current player
        auto moves = get_all_moves(board, turn);

        // If no moves are available and not in checkmate or stalemate, end the game
        if (moves.empty()) {
            std::cout << "The game is a draw (no moves available).\n";
            break;
        }

        // Select a random move from the available moves
        std::pair<std::string, std::string> selected_move = moves[std::rand() % moves.size()];

        // Apply the selected move
        if (board.move_piece(selected_move.first, selected_move.second)) {
            std::cout << "Move " << ++move_count << ": " 
                      << (turn == 1 ? "White" : "Black") << " plays " 
                      << selected_move.first << " to " << selected_move.second << "\n";
        } else {
            std::cout << "Failed move attempt. Invalid move detected.\n";
            break;
        }
std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // Toggle the turn to the other player
        turn = -turn;
    }
}

int main()
{

    std::srand(static_cast<unsigned>(std::time(nullptr))); // Seed for random move selection

    // Initialize board without history to speed up simulation
    Board board(false);
    board.initialize();

    play_auto_game(board);

/*
    char history_choice;
    std::cout << "Enable move history tracking? (y/n): ";
    std::cin >> history_choice;
    bool enable_history = (history_choice == 'y' || history_choice == 'Y');

    Board board(enable_history);
    board.initialize();
    board.display();

    std::string from, to;
    while (true) {
        std::cout << "Enter your move (e.g., E2 E4), type 'history' to view move history, or 'exit' to quit: ";
        std::cin >> from;
        
        if (from == "exit") {
            std::cout << "Game ended. Goodbye!\n";
            break;
        } else if (from == "history") {
            board.show_history();
        } else {
            std::cin >> to;
            if (board.move_piece(from, to))
                board.display();
            else
                std::cout << "Invalid move. Please try again.\n";
        }
    }
    */
    return 0;
}