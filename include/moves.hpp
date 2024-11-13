// moves.hpp
#ifndef MOVES_HPP
#define MOVES_HPP

#include <vector>
#include <utility>
#include <cmath>
#include <string>
#include "piece.hpp"

// Getting pieces moves functions
std::vector<std::pair<int, int>> get_pawn_moves(int x, int y,
	const std::vector<std::vector<int>>& board, bool is_white);
std::vector<std::pair<int, int>> get_knight_moves(int x, int y,
	const std::vector<std::vector<int>>& board);
std::vector<std::pair<int, int>> get_bishop_moves(int x, int y,
	const std::vector<std::vector<int>>& board);
std::vector<std::pair<int, int>> get_rook_moves(int x, int y,
	const std::vector<std::vector<int>>& board);
std::vector<std::pair<int, int>> get_queen_moves(int x, int y,
	const std::vector<std::vector<int>>& board);
std::vector<std::pair<int, int>> get_king_moves(int x, int y,
	const std::vector<std::vector<int>>& board);

//Utils
std::string index_to_chess(int row, int col);
std::pair<int, int> chess_to_index(const std::string& position);

// General piece moves function
std::vector<std::pair<int, int>> get_moves(int x, int y, const std::vector<std::vector<int>>& board);

#endif // MOVES_HPP