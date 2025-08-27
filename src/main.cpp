#include <algorithm>
#include <array>
#include <format>
#include <tuple>
#include <vector>

#include "raylib.h"

namespace Constants {
    inline const int SQUARE_LENGTH = 48;
    inline const Color SQUARE_LIGHT = Color{235, 236, 208, 255};
    inline const Color SQUARE_DARK = Color{119, 149, 86, 255};
    inline const Color SQUARE_SELECTED = Color{66, 81, 49, 255};
    inline const Color TRANSPARENT = Color{0, 0, 0, 0};
}  // namespace Constants

typedef struct Position {
    int x;
    int y;
} Position;
bool operator==(const Position &lhs, const Position &rhs) { return (lhs.x == rhs.x) && (lhs.y == rhs.y); }

// namespace Moves {
//     inline const std::array<int, 2> Queen = {2, 2};
// }

enum class Direction { North, East, South, West, NorthEast, SouthEast, SouthWest, NorthWest };

namespace Directions {
    std::vector<Direction> Queen = {Direction::North, Direction::East, Direction::South, Direction::West, Direction::NorthEast, Direction::SouthEast, Direction::SouthWest, Direction::NorthWest};
    std::vector<Direction> Bishop = {Direction::NorthEast, Direction::SouthEast, Direction::SouthWest, Direction::NorthWest};
    std::vector<Direction> Rook = {Direction::North, Direction::East, Direction::South, Direction::West};
}  // namespace Directions

struct Piece {
    static const int None = 0b00000;    // 0
    static const int Pawn = 0b00001;    // 1
    static const int Knight = 0b00011;  // 2
    static const int Bishop = 0b00100;  // 3
    static const int Rook = 0b00101;    // 4
    static const int Queen = 0b00110;   // 5
    static const int King = 0b00111;    // 6

    static const int Black = 0b01000;  // 8
    static const int White = 0b10000;  // 16
};

struct Square {
    // static const int None = 0;
    static const int Selected = 0b00001;  // 1
    static const int Indicator = 0b0010;  // 2
                                          //
    static const int Dark = 0b01000;      // 8
    static const int Light = 0b10000;     // 16
};

class Player {
   public:
    const int color;
    int homeColumn;
    int number_of_moves;
    Player(int clr) : color(clr) {
        if (clr == Piece::Black) {
            homeColumn = 1;
        } else {
            homeColumn = 6;
        }
    };
};

Player player = Player(Piece::Black);

void debug(std::string s) { TraceLog(LOG_INFO, s.c_str()); }

// bool has_flag(int a, int b) { return (a & b) == b; }

int piece_type(int a) { return a & 0b00111; }

// the only reason these are the same is because i used both 8,16 as flags for pieces and squares. the bit masks would have to be different if different numbers
int piece_color(int a) { return a & 0b11000; }
int square_color(int a) { return a & 0b11000; }

bool square_is_selected(int a) { return (a & Square::Selected) == Square::Selected; }
bool square_is_indicator(int a) { return (a & Square::Indicator) == Square::Indicator; }

int opposite_color(int color) {
    switch (color) {
        case Piece::Black:
            return Piece::White;
            break;
        case Piece::White:
            return Piece::Black;
            break;
    }
    return 0;
}

bool within_rectangle(Vector2 mouse_position, Rectangle r) {
    //
    return (mouse_position.x >= (r.x)) && (mouse_position.x <= (r.x + r.width)) && (mouse_position.y >= r.y) && (mouse_position.y <= (r.y + r.width));
}

Rectangle rectangle_from_x_y(int x, int y) {
    // TODO: bounds checking
    return Rectangle{
        (float)x * Constants::SQUARE_LENGTH,
        (float)(7 - y) * Constants::SQUARE_LENGTH,
        Constants::SQUARE_LENGTH,
        Constants::SQUARE_LENGTH,
    };
};

Position position_from_mouse_position(Vector2 mouse_position) {
    // auto x = ((int)trunc(mouse_position.x) - Board::OFFSET) / LENGTH;
    // auto y = ((int)trunc(mouse_position.y) - Board::OFFSET) / LENGTH;
    auto row = ((int)trunc(mouse_position.x)) / Constants::SQUARE_LENGTH;
    auto y = ((int)trunc(mouse_position.y)) / Constants::SQUARE_LENGTH;
    auto col = 7 - y;
    return Position{row, col};
}

// int transpose(std::array<std::array<int, 8>, 8> *pieces, Vector2 initial_position, int drow, int dcol) {
//     int piece = (*pieces)[initial_position.x][initial_position.y];
//     int y;
//     switch (piece_color(piece)) {
//         case Piece::Black:
//             y = dcol;
//             break;
//         case Piece::White:
//             y = -dcol;
//             break;
//     }
//
//     return (*pieces)[initial_position.x + drow][initial_position.y + y];
// }

// TODO: pick one of them
// Vector2 transpose(std::array<std::array<int, 8>, 8> *pieces, Vector2 initial_position, int drow, int dcol) {
//     int piece = (*pieces)[initial_position.x][initial_position.y];
//     Vector2 final;
//
//     switch (piece_color(piece)) {
//         case Piece::Black:
//             final = {initial_position.x + drow, initial_position.y + dcol};
//             break;
//         case Piece::White:
//             final = {initial_position.x + drow, initial_position.y - dcol};
//             break;
//     }
//     return final;
// }

int forward(int color, int col, int dcol) {
    if (color == player.color) {
        return col + dcol;
    } else {
        return col - dcol;
    }
    // switch (color) {
    //     case Piece::Black:
    //         return col + dcol;
    //         break;
    //     case Piece::White:
    //         return col - dcol;
    //         break;
    // }
    return 0;
}

std::tuple<int, Texture2D> load_piece_texture(int piece) {
    std::string filename = "assets/pieces/";

    // Color
    if (piece & Piece::Black) {
        filename.append("b");
    } else if (piece & Piece::White) {
        filename.append("w");
    }

    if (piece_type(piece) == Piece::Queen) {
        filename.append("Q");
    } else if (piece_type(piece) == Piece::Bishop) {
        filename.append("B");
    } else if (piece_type(piece) == Piece::Pawn) {
        filename.append("P");
    } else if (piece_type(piece) == Piece::King) {
        filename.append("K");
    } else if (piece_type(piece) == Piece::Knight) {
        filename.append("N");
    } else if (piece_type(piece) == Piece::Rook) {
        filename.append("R");
    }

    filename.append(".png");

    return std::make_tuple(piece, LoadTexture(filename.c_str()));
}

std::vector<std::tuple<int, Texture2D>> load_textures(std::array<std::array<int, 8>, 8> *pieces) {
    std::vector<std::tuple<int, Texture2D>> all_textures;
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            int piece = (*pieces)[row][column];
            if (piece) {
                all_textures.emplace_back(load_piece_texture(piece));
            }
        }
    }
    return all_textures;
}

void unload_textures(std::vector<std::tuple<int, Texture2D>> *piece_textures) {
    for (const auto &[key, value] : (*piece_textures)) {
        UnloadTexture(value);
    }
}

std::vector<Position> get_positions_in_directions(std::array<std::array<int, 8>, 8> *pieces, Position starting_position, std::vector<Direction> directions) {
    //
    int starting_row = starting_position.x;
    int starting_column = starting_position.y;
    int starting_piece = (*pieces)[starting_row][starting_column];

    std::vector<Position> result;
    for (auto &direction : directions) {
        Position temp_pos = Position{starting_row, starting_column};
        // debug(std::format("Bee  {}", temp_pos.y));
        switch (direction) {
            case (Direction::North):
                while (temp_pos.y < 7) {
                    temp_pos.y++;
                    int p = (*pieces)[starting_row][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({starting_row, temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({starting_row, temp_pos.y});
                }
                break;
            case (Direction::East):
                while (temp_pos.x < 7) {
                    temp_pos.x++;
                    int p = (*pieces)[temp_pos.x][starting_column];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({temp_pos.x, starting_column});
                        }
                        break;
                    }
                    result.push_back({temp_pos.x, starting_column});
                }

                break;
            case (Direction::South):
                while (temp_pos.y > 0) {
                    temp_pos.y--;
                    int p = (*pieces)[starting_row][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({starting_row, temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({starting_row, temp_pos.y});
                    // debug(std::format("{}", temp_pos.y));
                }

                break;
            case (Direction::West):
                while (temp_pos.x > 0) {
                    temp_pos.x--;
                    int p = (*pieces)[temp_pos.x][starting_column];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({temp_pos.x, starting_column});
                        }
                        break;
                    }
                    result.push_back({temp_pos.x, starting_column});
                    // debug(std::format("{}", temp_pos.y));
                }
                break;
            case (Direction::NorthEast):
                while (temp_pos.y < 7 && temp_pos.x < 7) {
                    temp_pos.x++;
                    temp_pos.y++;
                    int p = (*pieces)[temp_pos.x][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({temp_pos.x, temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({temp_pos.x, temp_pos.y});
                    // debug(std::format("{}", temp_pos.y));
                }
                break;
            case (Direction::SouthEast):
                while (temp_pos.y > 0 && temp_pos.x < 7) {
                    temp_pos.x++;
                    temp_pos.y--;
                    int p = (*pieces)[temp_pos.x][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({temp_pos.x, temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({temp_pos.x, temp_pos.y});
                }
                break;
            case (Direction::SouthWest):
                while (temp_pos.y > 0 && temp_pos.x > 0) {
                    temp_pos.x--;
                    temp_pos.y--;
                    int p = (*pieces)[temp_pos.x][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({temp_pos.x, temp_pos.y});
                        }
                        break;
                    }

                    result.push_back({temp_pos.x, temp_pos.y});
                }
                break;
            case (Direction::NorthWest):
                while (temp_pos.y < 7 && temp_pos.x > 0) {
                    temp_pos.x--;
                    temp_pos.y++;
                    int p = (*pieces)[temp_pos.x][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({temp_pos.x, temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({temp_pos.x, temp_pos.y});
                }
                break;
        }
    }

    return result;
}

bool position_is_within_board(Position position) {
    //
    return ((position.x >= 0) && (position.x <= 7) && (position.y >= 0) && (position.y <= 7));
}

/* Generic function for any piece */
bool is_valid_primative_move(std::array<std::array<int, 8>, 8> *pieces, Position start_pos, Position end_pos) {
    int starting_piece = (*pieces)[start_pos.x][start_pos.y];
    int ending_piece = (*pieces)[end_pos.x][end_pos.y];

    // No Pieces can move outside the board
    if (!position_is_within_board(end_pos)) {
        return false;
    }

    // If we are moving nothing, it's not valid
    if (!starting_piece) {
        return false;
    }
    // No 2 Pieces can occupy same position
    if (starting_piece && ending_piece) {
        if (piece_color(starting_piece) == piece_color(ending_piece)) {
            return false;
        }
    }
    return true;
}

std::vector<Position> get_primative_knight_positions(std::array<std::array<int, 8>, 8> *pieces, int x, int y) {
    std::vector<Position> result;

    std::array<Position, 8> candidates = {{
        // top
        {x - 2, y + 1},
        {x - 1, y + 2},
        {x + 1, y + 2},
        {x + 2, y + 1},
        // bottom
        {x - 2, y - 1},
        {x - 1, y - 2},
        {x + 1, y - 2},
        {x + 2, y - 1},
    }};
    for (const auto &[a, b] : candidates) {
        if (is_valid_primative_move(pieces, Position{x, y}, Position{a, b})) {
            result.push_back({a, b});
        };
    }
    return result;
}

std::vector<Position> get_primative_king_positions(std::array<std::array<int, 8>, 8> *pieces, int x, int y) {
    std::vector<Position> result;

    std::array<Position, 8> candidates = {{
        // top
        {x - 1, y + 1},
        {x, y + 1},
        {x + 1, y + 1},
        //  >:3
        {x - 1, y},
        {x + 1, y},
        // bottom (you)
        {x - 1, y - 1},
        {x, y - 1},
        {x + 1, y - 1},
    }};
    for (const auto &[a, b] : candidates) {
        if (is_valid_primative_move(pieces, Position{x, y}, Position{a, b})) {
            result.push_back({a, b});
        };
    }

    // TODO: Casting O-O and O-O-O
    return result;
}

std::vector<Position> get_primative_pawn_positions(std::array<std::array<int, 8>, 8> *pieces, int x, int y) {
    std::vector<Position> result;

    int piece = (*pieces)[x][y];
    int color = piece_color(piece);

    Position inital_position = Position{x, y};
    Position possible_position;

    // Not eating
    possible_position = Position{x, forward(color, y, 1)};
    if (is_valid_primative_move(pieces, inital_position, possible_position)) {
        // If theres nobody 1 squares forward from me
        if ((*pieces)[possible_position.x][possible_position.y] == Piece::None) {
            // i can move up one
            result.push_back(possible_position);

            // additionally, if im on the second column
            if ((y == 1 && color == player.color) || (y == 6 && color == opposite_color(player.color))) {
                possible_position = Position{x, forward(color, y, 2)};
                if (is_valid_primative_move(pieces, inital_position, possible_position)) {
                    // and theres nobody 2 squares forward from me
                    if ((*pieces)[possible_position.x][possible_position.y] == Piece::None) {
                        // i can move up two
                        result.push_back(possible_position);
                    }
                }
            }
        }
    }

    // Eating
    possible_position = Position{x - 1, forward(color, y, 1)};  // top left
    if (is_valid_primative_move(pieces, inital_position, possible_position)) {
        // If theres a piece in top left of me
        auto top_left_piece = (*pieces)[possible_position.x][possible_position.y];
        if (top_left_piece) {
            // If the piece in front has opposite color of me
            if (color != piece_color(top_left_piece)) {
                // i can eat it
                result.push_back(possible_position);
            }
        }
    }

    possible_position = Position{x + 1, forward(color, y, 1)};  // top right
    if (is_valid_primative_move(pieces, inital_position, possible_position)) {
        // If theres a piece in top right of me
        auto top_left_piece = (*pieces)[possible_position.x][possible_position.y];
        if (top_left_piece) {
            // If the piece in front has opposite color of me
            if (color != piece_color(top_left_piece)) {
                // i can eat it
                result.push_back(possible_position);
            }
        }
    }

    // TODO: En passant

    return result;
}

std::vector<Position> get_primative_positions(std::array<std::array<int, 8>, 8> *pieces, int x, int y) {
    std::vector<Position> result;

    int starting_piece = (*pieces)[x][y];

    std::vector<Direction> directions;
    switch (piece_type(starting_piece)) {
        // case Piece::Pawn:
        // case Piece::Knight:
        case (Piece::Bishop):
            directions = Directions::Bishop;
            break;
        case (Piece::Rook):
            directions = Directions::Rook;
            break;
        case (Piece::Queen):
            directions = Directions::Queen;
            break;
            // case Piece::King:
    }
    if (!directions.empty()) {
        auto positions = get_positions_in_directions(pieces, Position{x, y}, directions);
        result.insert(result.end(), positions.begin(), positions.end());
        return result;
    }

    if (piece_type(starting_piece) == Piece::Pawn) {
        auto positions = get_primative_pawn_positions(pieces, x, y);
        result.insert(result.end(), positions.begin(), positions.end());
    } else if (piece_type(starting_piece) == Piece::Knight) {
        auto positions = get_primative_knight_positions(pieces, x, y);
        result.insert(result.end(), positions.begin(), positions.end());
    } else if (piece_type(starting_piece) == Piece::King) {
        auto positions = get_primative_king_positions(pieces, x, y);
        result.insert(result.end(), positions.begin(), positions.end());
    }

    // debug(std::format("finaly result {}", result));
    return result;
}

std::vector<Position> get_attacking_positions(std::array<std::array<int, 8>, 8> *pieces, int x, int y) {
    // TODO: pawn moving forward is primative but not attacking
    return get_primative_positions(pieces, x, y);
}

std::vector<Position> get_all_attacking_positions(int color, std::array<std::array<int, 8>, 8> *pieces) {
    std::vector<Position> result;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            int piece = (*pieces)[x][y];
            if (piece) {
                if (piece_color(piece) == color) {
                    auto attacking_positions = get_attacking_positions(pieces, x, y);
                    result.insert(result.end(), attacking_positions.begin(), attacking_positions.end());
                }
            }
        }
    }

    return result;
}

bool is_under_attack(int color, std::array<std::array<int, 8>, 8> *pieces) {
    std::vector<Position> all_attacking_positions;

    all_attacking_positions = get_all_attacking_positions(opposite_color(color), pieces);

    // debug(std::format("000000"));
    for (auto &[a, b] : all_attacking_positions) {
        // debug(std::format("attacking positions {} {}", x, y));
        if (piece_type((*pieces)[a][b]) == Piece::King && (piece_color((*pieces)[a][b]) == color)) {
            return true;
        }
    }
    return false;
};

void move_piece(std::array<std::array<int, 8>, 8> *pieces, Position initial, Position final) {
    (*pieces)[final.x][final.y] = (*pieces)[initial.x][initial.y];
    if (initial != final) {
        (*pieces)[initial.x][initial.y] = Piece::None;
    }
}

std::vector<Position> get_valid_positions(std::array<std::array<int, 8>, 8> *pieces, int x, int y) {
    //
    std::vector<Position> result;

    auto primative_positions = get_primative_positions(pieces, x, y);
    for (auto &[a, b] : primative_positions) {
        auto cloned_pieces = (*pieces);              // cloned_pieces contains a clone of pieces
        move_piece(&cloned_pieces, {x, y}, {a, b});  // thats why we can modify without affecting our real board

        // If i make this move and im not under attack after moving, then im safe to do so
        if (!is_under_attack(piece_color((*pieces)[x][y]), &cloned_pieces)) {
            // white to move
            if (player.number_of_moves % 2 == 0) {
                if (piece_color((*pieces)[x][y]) == Piece::White) {
                    result.push_back({a, b});
                }

            } else {  // black to move
                if (piece_color((*pieces)[x][y]) == Piece::Black) {
                    result.push_back({a, b});
                }
            }
            // result.push_back({a, b});
        }
    }
    return result;
}

void update_squares(std::array<std::array<int, 8>, 8> *squares) {}

void update_pieces(std::array<std::array<int, 8>, 8> *pieces) {}

// Vector2 pressed_mouse_pos = {0, 0};

// void update_board(std::array<std::array<int, 8>, 8> *squares, std::array<std::array<int, 8>, 8> *pieces) {
//     Vector2 mouse_position = GetMousePosition();
//     Rectangle board_rect = Rectangle{0, 0, Constants::SQUARE_LENGTH * 8, Constants::SQUARE_LENGTH * 8};
//
//     if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
//         if (within_rectangle(mouse_position, board_rect)) {
//             pressed_mouse_pos = mouse_position;
//             Position initial_position = position_from_mouse_position(mouse_position);
//             // debug(std::format("{}, {}", inital_row_col.x, inital_row_col.y));
//             auto valid_positions = get_valid_positions(pieces, initial_position.x, initial_position.y);
//
//             (*squares)[initial_position.x][initial_position.y] ^= Square::Selected;
//
//             for (auto &[a, b] : valid_positions) {
//                 (*squares)[a][b] ^= Square::Indicator;
//             }
//         }
//     }
//     if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && pressed_mouse_pos.x) {
//         Position initial_position = position_from_mouse_position(pressed_mouse_pos);
//         auto valid_positions = get_valid_positions(pieces, initial_position.x, initial_position.y);
//
//         (*squares)[initial_position.x][initial_position.y] ^= Square::Selected;
//         for (auto &[a, b] : valid_positions) {
//             (*squares)[a][b] ^= Square::Indicator;
//         }
//
//         if (within_rectangle(mouse_position, board_rect)) {
//             Position final_position = position_from_mouse_position(mouse_position);
//             Position temp = Position{final_position.x, final_position.y};
//             if (std::find(valid_positions.begin(), valid_positions.end(), temp) != valid_positions.end()) {
//                 move_piece(pieces, initial_position, final_position);
//                 player.number_of_moves++;
//             }
//         }
//         pressed_mouse_pos = {0, 0};
//     }
// }

Vector2 prev_mouse_pos = {0, 0};
// bool should_draw_squares_now = false;

void update_board(std::array<std::array<int, 8>, 8> *squares, std::array<std::array<int, 8>, 8> *pieces) {
    Vector2 mouse_position = GetMousePosition();
    Rectangle board_rect = Rectangle{0, 0, Constants::SQUARE_LENGTH * 8, Constants::SQUARE_LENGTH * 8};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (within_rectangle(mouse_position, board_rect)) {
            if (prev_mouse_pos.x) {
                Position prev_position = position_from_mouse_position(prev_mouse_pos);

                auto prev_valid_positions = get_valid_positions(pieces, prev_position.x, prev_position.y);

                if ((*pieces)[prev_position.x][prev_position.y]) {
                    (*squares)[prev_position.x][prev_position.y] ^= Square::Selected;
                    for (auto &[a, b] : prev_valid_positions) {
                        (*squares)[a][b] ^= Square::Indicator;
                    }
                }

                Position current_position = position_from_mouse_position(mouse_position);
                if (piece_color((*pieces)[current_position.x][current_position.y]) == piece_color((*pieces)[prev_position.x][prev_position.y]) && (current_position.x != prev_position.x || current_position.y != prev_position.y)) {
                    prev_mouse_pos = mouse_position;

                    Position current_position = position_from_mouse_position(mouse_position);
                    auto current_valid_positions = get_valid_positions(pieces, current_position.x, current_position.y);

                    if ((*pieces)[current_position.x][current_position.y]) {
                        (*squares)[current_position.x][current_position.y] ^= Square::Selected;
                        for (auto &[a, b] : current_valid_positions) {
                            // debug(std::format("{} {}", a, b));
                            (*squares)[a][b] ^= Square::Indicator;
                        }
                    }
                } else {
                    if (!prev_valid_positions.empty()) {
                        if (std::find(prev_valid_positions.begin(), prev_valid_positions.end(), current_position) != prev_valid_positions.end()) {
                            move_piece(pieces, prev_position, current_position);
                            player.number_of_moves++;
                        }
                    }

                    prev_mouse_pos = {0, 0};
                }

            } else {
                prev_mouse_pos = mouse_position;

                Position current_position = position_from_mouse_position(mouse_position);
                if ((*pieces)[current_position.x][current_position.y]) {
                    auto current_valid_positions = get_valid_positions(pieces, current_position.x, current_position.y);

                    (*squares)[current_position.x][current_position.y] ^= Square::Selected;
                    for (auto &[a, b] : current_valid_positions) {
                        // debug(std::format("{} {}", a, b));
                        (*squares)[a][b] ^= Square::Indicator;
                    }
                }
            }
        }
    }
}

void draw_piece_texture(Texture2D piece_texture, Rectangle dest_rect) {}
void draw_piece_texture(Texture2D piece_texture, int x, int y) {
    Rectangle dest_rect = rectangle_from_x_y(x, y);
    DrawTexturePro(piece_texture, Rectangle{0, 0, (float)piece_texture.width, (float)piece_texture.height}, dest_rect, Vector2{0, 0}, 0, RAYWHITE);
}

void draw_pieces(std::array<std::array<int, 8>, 8> *pieces, std::vector<std::tuple<int, Texture2D>> *piece_textures) {
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            Rectangle dest_rect = rectangle_from_x_y(row, column);

            Texture2D piece_texture;
            bool found = false;
            for (const auto &[key, value] : (*piece_textures)) {
                if (key == (*pieces)[row][column]) {
                    found = true;
                    piece_texture = value;
                }
            }

            if (found) {
                draw_piece_texture(piece_texture, row, column);
            }
        }
    }
}

void draw_squares(std::array<std::array<int, 8>, 8> *squares) {
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            auto rect = rectangle_from_x_y(row, column);
            int current_square = (*squares)[row][column];
            if (square_color(current_square) == Square::Dark) {
                DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_DARK);
            } else if (square_color(current_square) == Square::Light) {
                DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_LIGHT);
            }
            // if (has_flag((*squares)[row][column], Square::Selected)) {
            if (square_is_selected(current_square)) {
                DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_SELECTED);
            }
            if (square_is_indicator(current_square)) {
                int centerX = rect.x + 0.5 * rect.width;
                int centerY = rect.y + 0.5 * rect.height;
                int radius = 0.125 * rect.width;
                DrawCircle(centerX, centerY, radius, Constants::SQUARE_SELECTED);
            }
        }
    }
}

std::array<std::array<int, 8>, 8> init_pieces(int color) {
    std::array<std::array<int, 8>, 8> pieces;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            pieces[x][y] = 0;
        }
    }
    // Load Pawns //
    for (int x = 0; x < 8; x++) {
        pieces[x][1] = player.color | Piece::Pawn;
        pieces[x][6] = opposite_color(player.color) | Piece::Pawn;
    }
    // my pieces //
    pieces[0][0] = player.color | Piece::Rook;
    pieces[1][0] = player.color | Piece::Knight;
    pieces[2][0] = player.color | Piece::Bishop;
    pieces[3][0] = player.color | Piece::Queen;
    pieces[4][0] = player.color | Piece::King;
    pieces[5][0] = player.color | Piece::Bishop;
    pieces[6][0] = player.color | Piece::Knight;
    pieces[7][0] = player.color | Piece::Rook;

    // opponents pieces //
    pieces[0][7] = opposite_color(player.color) | Piece::Rook;
    pieces[1][7] = opposite_color(player.color) | Piece::Knight;
    pieces[2][7] = opposite_color(player.color) | Piece::Bishop;
    pieces[3][7] = opposite_color(player.color) | Piece::Queen;
    pieces[4][7] = opposite_color(player.color) | Piece::King;
    pieces[5][7] = opposite_color(player.color) | Piece::Bishop;
    pieces[6][7] = opposite_color(player.color) | Piece::Knight;
    pieces[7][7] = opposite_color(player.color) | Piece::Rook;

    return pieces;
}

int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = Constants::SQUARE_LENGTH * 8;
    const int screenHeight = Constants::SQUARE_LENGTH * 8;

    InitWindow(screenWidth, screenHeight, "chess");
    SetTargetFPS(60);

    std::array<std::array<int, 8>, 8> pieces = init_pieces(player.color);

    // Squares are drawn the same regardless of what piece_color the player is playing;
    std::array<std::array<int, 8>, 8> squares;
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            if ((row + column) % 2 == 0) {
                squares[row][column] = Square::Light;
            } else {
                squares[row][column] = Square::Dark;
            }
        }
    }

    // Load textures
    std::vector<std::tuple<int, Texture2D>> piece_textures = load_textures(&pieces);

    // debug(std::format("{}", 0b0110 | 0b1000));
    // debug(std::format("AA {}", forward(1, 1)));
    // debug(std::format("{}", 0b0001));
    // debug(std::format("{}", (0b0110 | 0b1000) & 0b0001));

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        // ----------------------------------------------------------------------------------
        // debug(std::format("{}", Piece::Rook | Piece::Black));
        // update_squares(&squares);
        // update_pieces(&pieces);
        update_board(&squares, &pieces);

        // debug(std::format("NOM {}", player.number_of_moves));
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLUE);

        //
        // DrawText("Congrats! You created your first window!", 190, 200, 20,
        // LIGHTGRAY);

        draw_squares(&squares);
        draw_pieces(&pieces, &piece_textures);

        // DrawRectangle(0, 0, 100, 48, BLACK);
        // DrawText(std::format("{}", GetFPS()).c_str(), 0, 0, 24, RED);

        bool white_cant_move_anywhere = true;
        bool black_cant_move_anywhere = true;
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                int piece = (pieces)[x][y];
                if (piece) {
                    if (!get_valid_positions(&pieces, x, y).empty() && piece_color(piece) == Piece::White) {
                        white_cant_move_anywhere = false;
                    }
                    if (!get_valid_positions(&pieces, x, y).empty() && piece_color(piece) == Piece::Black) {
                        black_cant_move_anywhere = false;
                    }
                }
            }
        }
        if (white_cant_move_anywhere && is_under_attack(Piece::White, &pieces)) {
            auto measurements = MeasureText("White is checkmated", 24);
            DrawRectangle((screenWidth - measurements) / 2.0 - (0.5f * 24.0), (screenHeight / 2.0) - (0.3f * 48), measurements + 24.0, 48, BLACK);
            DrawText("White is checkmated", (screenWidth - measurements) / 2.0, screenHeight / 2.0, 24, WHITE);
        }
        if (black_cant_move_anywhere && is_under_attack(Piece::Black, &pieces)) {
            auto measurements = MeasureText("Black is checkmated", 24);
            DrawRectangle((screenWidth - measurements) / 2.0 - (0.5f * 24.0), (screenHeight / 2.0) - (0.3f * 48), measurements + 24.0, 48, BLACK);
            DrawText("Black is checkmated", (screenWidth - measurements) / 2.0, screenHeight / 2.0, 24, WHITE);
        }

        EndDrawing();

        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    unload_textures(&piece_textures);
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
