// validation.hpp
#ifndef VALIDATION_HPP
#define VALIDATION_HPP

#include "board.hpp"

// Checks if a move puts the king in check
bool is_check(const Board& board, int player);

// Validates castling conditions
bool can_castle(const Board& board, int player, bool is_kingside);

// Handles pawn promotion
bool is_promotion(const Board& board, int x, int y);

// Checks if the current player is in checkmate
bool is_checkmate(const Board& board, int player);

// Checks if the current player is in stalemate
bool is_stalemate(const Board& board, int player);

// Checks if the current player is in stalemate
bool is_insufficient_material(const Board& board);

#endif // VALIDATION_HPP
