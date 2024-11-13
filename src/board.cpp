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

// Helper function to find the player's king position on the board
std::pair<int, int> Board::find_king_position(int player) const
{
    for (int x = 0; x < 8; ++x)
	{
        for (int y = 0; y < 8; ++y)
		{
            if (get_piece(x, y) == (player == 1 ? KING_WHITE : KING_BLACK))
			{
                return {x, y};
            }
        }
    }
    return {-1, -1}; // King not found (shouldn't happen in a valid game)
}

// Checks if the player's king is in check
bool Board::is_in_check(int player) const
{
    auto [king_x, king_y] = find_king_position(player);
    if (king_x == -1 || king_y == -1) return false;

    int opponent = -player;
    for (int x = 0; x < 8; ++x)
	{
        for (int y = 0; y < 8; ++y)
		{
            if ((opponent == 1 && get_piece(x, y) > 0) || 
                (opponent == -1 && get_piece(x, y) < 0))
			{
                auto moves = get_moves(x, y, get_board());
                for (const auto& move : moves)
				{
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

        // Check if it's the player's turn and if the move is valid
        if (!is_valid_move(x1, y1, x2, y2, turn)) return false;

        auto moves = get_moves(x1, y1, board);

        // Ensure the target square is a valid move
        if (std::find(moves.begin(), moves.end(), std::make_pair(x2, y2)) == moves.end())
        {
            std::cout << "Invalid move for this piece.\n";
            return false;
        }

        // Simulate the move on a temporary board to check for self-check
        Board temp_board = *this;
        temp_board.board[x2][y2] = moving_piece;
        temp_board.board[x1][y1] = EMPTY;

        if (is_in_check(turn))
        {
            std::cout << "Move would leave the king in check.\n";
            return false;
        }

        // If the simulated move does not leave the player in check, execute it
        board[x2][y2] = moving_piece;
        board[x1][y1] = EMPTY;

        // Handle 50-move rule and threefold repetition tracking
        if (moving_piece == PAWN_WHITE || moving_piece == PAWN_BLACK || target_piece != EMPTY)
            fifty_move_counter = 0; // Reset the 50-move counter
        else
            ++fifty_move_counter; // Increment if no pawn move or capture

        // Track the board state for threefold repetition
        std::string state = board_to_string();
        ++position_history[state];

        // Record history if enabled
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

        // Toggle the turn
        turn = -turn;

        return true;
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