// validation.cpp
#include "validation.hpp"

// Determines if the player's king is in check
bool is_check(const Board& board, int player)
{
    // Check if any opposing piece is attacking the king
    auto [king_x, king_y] = board.find_king_position(player);
    if (king_x == -1 || king_y == -1) return false;

    int opponent = -player;
    for (int x = 0; x < 8; ++x)
    {
        for (int y = 0; y < 8; ++y)
        {
            if ((opponent == 1 && board.get_piece(x, y) > 0) || 
                (opponent == -1 && board.get_piece(x, y) < 0))
            {
                auto moves = get_moves(x, y, board.get_board());
                for (const auto& move : moves) {
                    if (move.first == king_x && move.second == king_y)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// Validates castling for kingside or queenside
bool can_castle(const Board& board, int player, bool is_kingside)
{
    int row = (player == 1) ? 7 : 0;
    int king_col = 4;
    int rook_col = is_kingside ? 7 : 0;

    // Check if king and rook are in the correct starting positions
    if (board.get_piece(row, king_col) != (player == 1 ? KING_WHITE : KING_BLACK) ||
        board.get_piece(row, rook_col) != (player == 1 ? ROOK_WHITE : ROOK_BLACK))
        return false;

    // Check if path between king and rook is clear
    int step = is_kingside ? 1 : -1;
    for (int col = king_col + step; col != rook_col; col += step)
    {
        if (board.get_piece(row, col) != EMPTY) {
            return false;
        }
    }

    // Ensure that neither the king nor the rook has moved, and king does not pass through check
    // (Additional tracking needed to enforce this)

    return !is_check(board, player);
}

// Determines if a pawn is in a promotion position
bool is_promotion(const Board& board, int x, int y)
{
    int piece = board.get_piece(x, y);
    if (piece == PAWN_WHITE && x == 0) return true;  // White pawn reaches rank 8
    if (piece == PAWN_BLACK && x == 7) return true;  // Black pawn reaches rank 1
    return false;
}

// Checks if the player is in checkmate
bool is_checkmate(const Board& board, int player)
{
    if (!board.is_in_check(player)) return false;

    for (int x = 0; x < 8; ++x)
	{
        for (int y = 0; y < 8; ++y)
		{
            if ((player == 1 && board.get_piece(x, y) > 0) || 
                (player == -1 && board.get_piece(x, y) < 0))
			{
                auto moves = get_moves(x, y, board.get_board());
                for (const auto& move : moves)
				{
                    Board temp_board = board;
                    temp_board.move_piece(index_to_chess(x, y), index_to_chess(move.first, move.second));
                    if (!temp_board.is_in_check(player))
					{
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

// Checks if the player is in stalemate
bool is_stalemate(const Board& board, int player)
{
    if (board.is_in_check(player)) return false;

    for (int x = 0; x < 8; ++x)
	{
        for (int y = 0; y < 8; ++y)
		{
            if ((player == 1 && board.get_piece(x, y) > 0) || 
                (player == -1 && board.get_piece(x, y) < 0))
			{
                auto moves = get_moves(x, y, board.get_board());
                for (const auto& move : moves)
				{
                    Board temp_board = board;
                    temp_board.move_piece(index_to_chess(x, y), index_to_chess(move.first, move.second));
                    if (!temp_board.is_in_check(player))
					{
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool is_insufficient_material(const Board& board)
{
    bool white_has_minor_piece = false;
    bool black_has_minor_piece = false;
    bool white_has_bishop_on_white = false;
    bool white_has_bishop_on_black = false;
    bool black_has_bishop_on_white = false;
    bool black_has_bishop_on_black = false;

    for (int x = 0; x < 8; ++x)
	{
        for (int y = 0; y < 8; ++y)
		{
            int piece = board.get_piece(x, y);
            
            switch (piece)
			{
                case KING_WHITE:
                case KING_BLACK:
                    // Kings are always present, continue
                    break;
                case KNIGHT_WHITE:
                    white_has_minor_piece = true;
                    break;
                case KNIGHT_BLACK:
                    black_has_minor_piece = true;
                    break;
                case BISHOP_WHITE:
                    white_has_minor_piece = true;
                    // Track the color square of the bishop
                    if ((x + y) % 2 == 0) white_has_bishop_on_white = true;
                    else white_has_bishop_on_black = true;
                    break;
                case BISHOP_BLACK:
                    black_has_minor_piece = true;
                    // Track the color square of the bishop
                    if ((x + y) % 2 == 0) black_has_bishop_on_white = true;
                    else black_has_bishop_on_black = true;
                    break;
                case PAWN_WHITE:
                case PAWN_BLACK:
                case ROOK_WHITE:
                case ROOK_BLACK:
                case QUEEN_WHITE:
                case QUEEN_BLACK:
                    return false; // Sufficient material
            }
        }
    }

    // Check if both sides have only insufficient material
    if (!white_has_minor_piece && !black_has_minor_piece) return true; // King vs. King
    if (white_has_minor_piece && !black_has_minor_piece &&
		!white_has_bishop_on_white && !white_has_bishop_on_black) return true; // King and Knight or King and Bishop vs. King
    if (black_has_minor_piece && !white_has_minor_piece &&
		!black_has_bishop_on_white && !black_has_bishop_on_black) return true; // King vs. King and Knight or King and Bishop

    // Check for King and Bishop vs. King and Bishop, both bishops on same color
    if (white_has_bishop_on_white && black_has_bishop_on_white
		&& !white_has_bishop_on_black && !black_has_bishop_on_black) return true;
    if (white_has_bishop_on_black && black_has_bishop_on_black
		&& !white_has_bishop_on_white && !black_has_bishop_on_white) return true;

    return false; // Sufficient material for other scenarios
}