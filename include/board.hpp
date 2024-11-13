// board.h
#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono> // for timestamps
#include "moves.hpp"
#include "piece.hpp"

// Define a struct to keep track of each move's details
struct MoveRecord {
    int move_number;
    std::string from;
    std::string to;
    int player; // 1 for white, -1 for black
    std::chrono::steady_clock::time_point timestamp;
};

class Board
{
    public:
        Board(bool enable_history = true);
        void initialize();
        void display() const;
        bool move_piece(const std::string& from, const std::string& to);
		void show_history() const;

		void set_history_enabled(bool enable);

        int get_piece(int x, int y) const;                      // Get piece at (x, y)
        const std::vector<std::vector<int>>& get_board() const; // Get entire board
        int get_fifty_move_counter() const { return fifty_move_counter; }
        bool is_threefold_repetition() const;
        std::string board_to_string() const;

    private:
        std::vector<std::vector<int>> board;
		std::vector<MoveRecord> history; // Stores the history of moves
        int move_count; // Count of the total moves made
        int turn; // 1 for white's turn, -1 for black's turn
        bool enable_history;
        int fifty_move_counter = 0; // Counter for 50-move rule
        mutable std::unordered_map<std::string, int> position_history; 

        bool is_valid_move(int x1, int y1, int x2, int y2, int player) const;
};

#endif // BOARD_HPP
