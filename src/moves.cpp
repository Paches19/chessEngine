// moves.cpp
#include "moves.hpp"

std::pair<int, int> chess_to_index(const std::string& position)
{
    int row = 8 - (position[1] - '0');           // Convierte '1'-'8' a 7-0
    int col = std::toupper(position[0]) - 'A';   // Convierte 'A'-'H' a 0-7
    return {row, col};
}

std::string index_to_chess(int row, int col)
{
    char col_char = 'A' + col;                   // Convierte 0-7 a 'A'-'H'
    char row_char = '8' - row;                   // Convierte 0-7 a '8'-'1'
    return std::string{col_char, row_char};
}

// Pawn moves and captures
std::vector<std::pair<int, int>> get_pawn_moves(int x, int y,
    const std::vector<std::vector<int>>& board, bool is_white)
{
    std::vector<std::pair<int, int>> moves;
    int direction = is_white ? 1 : -1;

    // Ensure x is within board bounds before checking moves
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return moves; // Return an empty moves list if out of bounds

    // Forward move by one square, with boundary check
    if (x + direction >= 0 && x + direction < 8 && board[x + direction][y] == EMPTY)
    {
        moves.emplace_back(x + direction, y);

        // Initial double move for pawns in their starting rank
        if ((is_white && x == 1) || (!is_white && x == 6))
        {
            if (board[x + 2 * direction][y] == EMPTY)
                moves.emplace_back(x + 2 * direction, y);
        }
    }

    // Diagonal captures with boundary checks
    if (x + direction >= 0 && x + direction < 8)
    {
        // Left diagonal capture
        if (y - 1 >= 0 && board[x + direction][y - 1] * (is_white ? -1 : 1) < 0)
            moves.emplace_back(x + direction, y - 1);
        // Right diagonal capture
        if (y + 1 < 8 && board[x + direction][y + 1] * (is_white ? -1 : 1) < 0)
            moves.emplace_back(x + direction, y + 1);
    }

    return moves;
}

// Knight moves
std::vector<std::pair<int, int>> get_knight_moves(int x, int y,
    const std::vector<std::vector<int>>& board)
    {
    std::vector<std::pair<int, int>> moves;
    std::vector<std::pair<int, int>> knight_offsets =
    { {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2} };

    for (const auto& offset : knight_offsets)
    {
        int nx = x + offset.first;
        int ny = y + offset.second;
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 &&
            board[nx][ny] * board[x][y] <= 0)
            moves.emplace_back(nx, ny);
    }
    return moves;
}

// Bishop movements (diagonal)
std::vector<std::pair<int, int>> get_bishop_moves(int x, int y,
	const std::vector<std::vector<int>>& board)
{
    std::vector<std::pair<int, int>> moves;
    std::vector<std::pair<int, int>> directions =
        { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

    for (const auto& dir : directions)
	{
        int nx = x + dir.first;
        int ny = y + dir.second;
        while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
		{
            if (board[nx][ny] == EMPTY)
                moves.emplace_back(nx, ny);
            else
			{
                if (board[nx][ny] * board[x][y] < 0)
                    moves.emplace_back(nx, ny);  // Capture
                break;
            }
            nx += dir.first;
            ny += dir.second;
        }
    }
    return moves;
}

// Rook movements (horizontal & vertical)
std::vector<std::pair<int, int>> get_rook_moves(int x, int y,
	const std::vector<std::vector<int>>& board)
	{
    std::vector<std::pair<int, int>> moves;
    std::vector<std::pair<int, int>> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    for (const auto& dir : directions)
	{
        int nx = x + dir.first;
        int ny = y + dir.second;
        while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            if (board[nx][ny] == EMPTY)
                moves.emplace_back(nx, ny);
			else
			{
                if (board[nx][ny] * board[x][y] < 0)
                    moves.emplace_back(nx, ny);  // Capture
                break;
            }
            nx += dir.first;
            ny += dir.second;
        }
    }
    return moves;
}

// Queen moves (combination of rook and bishop moves)
std::vector<std::pair<int, int>> get_queen_moves(int x, int y,
	const std::vector<std::vector<int>>& board)
{
    auto rook_moves = get_rook_moves(x, y, board);
    auto bishop_moves = get_bishop_moves(x, y, board);
    rook_moves.insert(rook_moves.end(), bishop_moves.begin(), bishop_moves.end());
    return rook_moves;
}

// King moves
std::vector<std::pair<int, int>> get_king_moves(int x, int y,
	const std::vector<std::vector<int>>& board)
	{
    std::vector<std::pair<int, int>> moves;
    std::vector<std::pair<int, int>> directions =
	{ {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };

    for (const auto& dir : directions)
	{
        int nx = x + dir.first;
        int ny = y + dir.second;
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && board[nx][ny] * board[x][y] <= 0)
            moves.emplace_back(nx, ny);
    }
    return moves;
}

std::vector<std::pair<int, int>> get_moves(int x, int y, const std::vector<std::vector<int>>& board)
{
    int piece = board[x][y];
    if (piece == PAWN_WHITE) {
        return get_pawn_moves(x, y, board, true);
    } else if (piece == PAWN_BLACK) {
        return get_pawn_moves(x, y, board, false);
    } else if (piece == KNIGHT_WHITE || piece == KNIGHT_BLACK) {
        return get_knight_moves(x, y, board);
    } else if (piece == BISHOP_WHITE || piece == BISHOP_BLACK) {
        return get_bishop_moves(x, y, board);
    } else if (piece == ROOK_WHITE || piece == ROOK_BLACK) {
        return get_rook_moves(x, y, board);
    } else if (piece == QUEEN_WHITE || piece == QUEEN_BLACK) {
        return get_queen_moves(x, y, board);
    } else if (piece == KING_WHITE || piece == KING_BLACK) {
        return get_king_moves(x, y, board);
    }
    return {};
}
