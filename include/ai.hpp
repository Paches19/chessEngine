// ai.hpp
#ifndef AI_HPP
#define AI_HPP

#include <utility>
#include "moves.hpp"
#include "board.hpp"

// Evaluates the board and chooses the best move for the AI
std::pair<int, int> select_best_move(Board& board, int player);

#endif // AI_HPP
