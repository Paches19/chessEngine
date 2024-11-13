// board.cpp
#include "board.hpp"

Board::Board(bool enable_history) 
    : board(8, std::vector<int>(8, EMPTY)), move_count(0),
		turn(1), enable_history(enable_history) {}

void Board::initialize()
{
    // Initial position of the board
    board[0] = {ROOK_WHITE, KNIGHT_WHITE, BISHOP_WHITE, QUEEN_WHITE, KING_WHITE, BISHOP_WHITE, KNIGHT_WHITE, ROOK_WHITE};
    board[1] = {PAWN_WHITE, PAWN_WHITE, PAWN_WHITE, PAWN_WHITE, PAWN_WHITE, PAWN_WHITE, PAWN_WHITE, PAWN_WHITE};

    board[7] = {ROOK_BLACK, KNIGHT_BLACK, BISHOP_BLACK, QUEEN_BLACK, KING_BLACK, BISHOP_BLACK, KNIGHT_BLACK, ROOK_BLACK};
    board[6] = {PAWN_BLACK, PAWN_BLACK, PAWN_BLACK, PAWN_BLACK, PAWN_BLACK, PAWN_BLACK, PAWN_BLACK, PAWN_BLACK};

	move_count = 0;
    turn = 1;
    fifty_move_counter = 0;
    position_history.clear();
    history.clear();
}

void Board::display() const
{
    // Unicode symbols for chess pieces
    const std::unordered_map<int, std::string> piece_symbols =
    {
        {KING_WHITE, "♔"}, {QUEEN_WHITE, "♕"}, {ROOK_WHITE, "♖"},
        {BISHOP_WHITE, "♗"}, {KNIGHT_WHITE, "♘"}, {PAWN_WHITE, "♙"},
        {KING_BLACK, "♚"}, {QUEEN_BLACK, "♛"}, {ROOK_BLACK, "♜"},
        {BISHOP_BLACK, "♝"}, {KNIGHT_BLACK, "♞"}, {PAWN_BLACK, "♟"}
    };

    // ANSI color codes for bold white and bold black pieces
    const std::string WHITE_PIECE = "\033[1;37m"; // Bold white for white pieces
    const std::string BLACK_PIECE = "\033[1;30m"; // Bold black for black pieces
    const std::string RESET = "\033[0m";           // Reset color

    // Print column labels at the top
    std::cout << "    A   B   C   D   E   F   G   H\n";
    std::cout << "  +---+---+---+---+---+---+---+---+\n";

    // Print each row of the board
    for (int i = 0; i < 8; ++i)
    {
        std::cout << 8 - i << " "; // Row label (8 to 1)
        for (int j = 0; j < 8; ++j)
        {
            int cell = board[i][j];

            // Determine the symbol and color for the piece or empty square
            std::string piece = (cell == EMPTY) ? " " : piece_symbols.at(cell);
            std::string piece_color = (cell > 0) ? WHITE_PIECE : BLACK_PIECE;

            // Print each square with piece color and centered alignment
            std::cout << "| " << piece_color << piece << RESET << " ";
        }
        std::cout << "| " << 8 - i << "\n"; // Row label on the right side

        std::cout << "  +---+---+---+---+---+---+---+---+\n";
    }

    // Print column labels at the bottom
    std::cout << "    A   B   C   D   E   F   G   H\n";
    std::cout << "Turn: " << (turn == 1 ? "White" : "Black") << ", Move count: " << move_count << "\n";
}

void Board::set_history_enabled(bool enable)
{
    enable_history = enable;
    if (!enable_history)
        history.clear(); // Clear history to free memory if history is disabled
}

bool Board::is_valid_move(int x1, int y1, int x2, int y2, int player) const
{
    int piece = board[x1][y1];
    if ((player == 1 && piece < 0) || (player == -1 && piece > 0))
	{
        std::cout << "You can only move your own pieces.\n";
        return false;
    }
    return true;
}

bool Board::move_piece(const std::string& from, const std::string& to)
{
    auto [x1, y1] = chess_to_index(from);
    auto [x2, y2] = chess_to_index(to);

    int moving_piece = board[x1][y1];
    int target_piece = board[x2][y2];

    if (moving_piece == EMPTY)
    {
        std::cout << "No piece at the origin position.\n";
        return false;
    }

    if (!is_valid_move(x1, y1, x2, y2, turn)) return false;

    auto moves = get_moves(x1, y1, board);

    if (std::find(moves.begin(), moves.end(), std::make_pair(x2, y2))
        != moves.end())
    {
        // **50-Move Rule Handling**: Reset the counter if a pawn moves or a capture is made
        if (moving_piece == PAWN_WHITE || moving_piece == PAWN_BLACK ||
            target_piece != EMPTY)
            fifty_move_counter = 0;
        else
            ++fifty_move_counter;

        // Execute the move
        board[x2][y2] = moving_piece;
        board[x1][y1] = EMPTY;

        // Record the new board state in position history for threefold repetition
        std::string state = board_to_string();
        ++position_history[state];

        // Only record the move if history is enabled
        if (enable_history)
        {
            MoveRecord record;
            record.move_number = ++move_count;
            record.from = from;
            record.to = to;
            record.player = turn;
            record.timestamp = std::chrono::steady_clock::now();
            history.push_back(record);
        }
        else
            ++move_count;

        turn = -turn;
        return true;
    }
    else
    {
        std::cout << "Invalid move for this piece.\n";
        return false;
    }
}

// Displays the history of moves, including move number, player, and move time
void Board::show_history() const
{
    if (!enable_history)
	{
        std::cout << "History tracking is disabled.\n";
        return;
    }

    std::cout << "\nMove History:\n";
    for (const auto& record : history)
	{
        std::cout << "Move " << record.move_number << " by " 
                  << (record.player == 1 ? "White" : "Black") << ": " 
                  << record.from << " to " << record.to << "\n";
    }
}

// Returns the piece at the given board coordinates
int Board::get_piece(int x, int y) const
{
    return board[x][y];
}

// Returns a const reference to the board matrix
const std::vector<std::vector<int>>& Board::get_board() const
{
    return board;
}

std::string Board::board_to_string() const
{
    std::string state;
    for (const auto& row : board)
    {
        for (const auto& piece : row)
            state += std::to_string(piece) + ",";
    }
    return state + " turn: " + std::to_string(turn);
}

bool Board::is_threefold_repetition() const
{
    auto state = board_to_string();
    return position_history[state] >= 3;
}