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
    // inline const Color SQUARE_SELECTED = Color{123, 252, 185, 128};
    inline const Color SQUARE_SELECTED = PINK;
    inline const Color TRANSPARENT = Color{0, 0, 0, 0};
}  // namespace Constants

namespace Moves {
    inline const std::array<int, 2> Queen = {2, 2};
}

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

bool within_rectangle(Vector2 mouse_position, Rectangle r) {
    //
    return (mouse_position.x >= (r.x)) && (mouse_position.x <= (r.x + r.width)) && (mouse_position.y >= r.y) && (mouse_position.y <= (r.y + r.width));
}

Rectangle rectangle_from_row_column(int row, int column) {
    // TODO: bounds checking
    return Rectangle{
        (float)row * Constants::SQUARE_LENGTH,
        (float)(7 - column) * Constants::SQUARE_LENGTH,
        Constants::SQUARE_LENGTH,
        Constants::SQUARE_LENGTH,
    };
};

Vector2 row_col_from_mouse_position(Vector2 mouse_position) {
    // auto x = ((int)trunc(mouse_position.x) - Board::OFFSET) / LENGTH;
    // auto y = ((int)trunc(mouse_position.y) - Board::OFFSET) / LENGTH;
    auto row = ((int)trunc(mouse_position.x)) / Constants::SQUARE_LENGTH;
    auto y = ((int)trunc(mouse_position.y)) / Constants::SQUARE_LENGTH;
    auto col = 7 - y;
    return Vector2{(float)row, (float)col};
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
Vector2 transpose(std::array<std::array<int, 8>, 8> *pieces, Vector2 initial_position, int drow, int dcol) {
    int piece = (*pieces)[initial_position.x][initial_position.y];
    Vector2 final;

    switch (piece_color(piece)) {
        case Piece::Black:
            final = {initial_position.x + drow, initial_position.y + dcol};
            break;
        case Piece::White:
            final = {initial_position.x + drow, initial_position.y - dcol};
            break;
    }
    return final;
}

int forward(int color, int col, int dcol) {
    switch (color) {
        case Piece::Black:
            return col + dcol;
            break;
        case Piece::White:
            return col - dcol;
            break;
    }
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

std::vector<std::array<int, 2>> get_squares_in_directions(std::array<std::array<int, 8>, 8> *pieces, int starting_row, int starting_column, std::vector<Direction> directions) {
    //
    int starting_piece = (*pieces)[starting_row][starting_column];

    std::vector<std::array<int, 2>> result;
    for (auto &direction : directions) {
        Vector2 temp_pos = Vector2{(float)starting_row, (float)starting_column};
        // debug(std::format("Bee  {}", temp_pos.y));
        switch (direction) {
            case (Direction::North):
                while (temp_pos.y < 7) {
                    temp_pos.y++;

                    auto p = (*pieces)[starting_row][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({(int)starting_row, (int)temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({starting_row, (int)temp_pos.y});
                }
                break;
            case (Direction::East):
                while (temp_pos.x < 7) {
                    temp_pos.x++;
                    auto p = (*pieces)[temp_pos.x][starting_column];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({(int)temp_pos.x, (int)starting_column});
                        }
                        break;
                    }
                    result.push_back({(int)temp_pos.x, starting_column});
                }

                break;
            case (Direction::South):
                while (temp_pos.y > 0) {
                    temp_pos.y--;
                    auto p = (*pieces)[starting_row][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({(int)starting_row, (int)temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({starting_row, (int)temp_pos.y});
                    // debug(std::format("{}", temp_pos.y));
                }

                break;
            case (Direction::West):
                while (temp_pos.x > 0) {
                    temp_pos.x--;
                    auto p = (*pieces)[temp_pos.x][starting_column];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({(int)temp_pos.x, (int)starting_column});
                        }
                        break;
                    }
                    result.push_back({(int)temp_pos.x, starting_column});
                    // debug(std::format("{}", temp_pos.y));
                }
                break;
            case (Direction::NorthEast):
                while (temp_pos.y < 7 && temp_pos.x < 7) {
                    temp_pos.x++;
                    temp_pos.y++;
                    auto p = (*pieces)[temp_pos.x][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                    // debug(std::format("{}", temp_pos.y));
                }
                break;
            case (Direction::SouthEast):
                while (temp_pos.y > 0 && temp_pos.x < 7) {
                    temp_pos.x++;
                    temp_pos.y--;
                    auto p = (*pieces)[temp_pos.x][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                }
                break;
            case (Direction::SouthWest):
                while (temp_pos.y > 0 && temp_pos.x > 0) {
                    temp_pos.x--;
                    temp_pos.y--;
                    auto p = (*pieces)[temp_pos.x][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                        }
                        break;
                    }

                    result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                }
                break;
            case (Direction::NorthWest):
                while (temp_pos.y < 7 && temp_pos.x > 0) {
                    temp_pos.x--;
                    temp_pos.y++;
                    auto p = (*pieces)[temp_pos.x][temp_pos.y];
                    if (p) {
                        if (piece_color(p) != piece_color(starting_piece)) {
                            result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                        }
                        break;
                    }
                    result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                }
                break;
        }
    }

    return result;
}

typedef struct Position {
    int x;
    int y;
} Position;

bool position_is_within_board(std::array<int, 2> position) { return (position[0]); }

/* Generic function for any piece */
bool is_valid_move(std::array<std::array<int, 8>, 8> *pieces, Vector2 starting_position, Vector2 ending_position) {
    int starting_piece = (*pieces)[starting_position.x][starting_position.y];
    int ending_piece = (*pieces)[ending_position.x][ending_position.y];

    if (!(ending_position.x >= 0 && ending_position.x <= 7 && ending_position.y >= 0 && ending_position.y <= 7)) {
        return false;
    }

    if (piece_color(starting_piece) == piece_color(ending_piece)) {
        return false;
    }

    return true;
}

std::vector<std::array<int, 2>> get_knight_moves(std::array<std::array<int, 8>, 8> *pieces, int row, int column) {
    std::vector<std::array<int, 2>> result;

    std::array<std::array<int, 2>, 8> candidates = {{
        // top
        {row - 2, column + 1},
        {row - 1, column + 2},
        {row + 1, column + 2},
        {row + 2, column + 1},
        // bottom
        {row - 2, column - 1},
        {row - 1, column - 2},
        {row + 1, column - 2},
        {row + 2, column - 1},
    }};
    for (const auto &[x, y] : candidates) {
        if (is_valid_move(pieces, Vector2{(float)row, (float)column}, Vector2{(float)x, (float)y})) {
            result.push_back({x, y});
        };
    }
    return result;
}

std::vector<std::array<int, 2>> get_king_moves(std::array<std::array<int, 8>, 8> *pieces, int row, int column) {
    std::vector<std::array<int, 2>> result;

    std::array<std::array<int, 2>, 8> candidates = {{
        // top
        {row - 1, column + 1},
        {row, column + 1},
        {row + 1, column + 1},
        //  >:3
        {row - 1, column},
        {row + 1, column},

        // bottom
        {row - 1, column - 1},
        {row, column - 1},
        {row + 1, column - 1},
    }};
    for (const auto &[x, y] : candidates) {
        if (is_valid_move(pieces, Vector2{(float)row, (float)column}, Vector2{(float)x, (float)y})) {
            result.push_back({x, y});
        };
    }

    // TODO: Casting O-O and O-O-O
    return result;
}

std::vector<std::array<int, 2>> get_pawn_moves(std::array<std::array<int, 8>, 8> *pieces, int row, int column) {
    std::vector<std::array<int, 2>> result;

    int piece = (*pieces)[row][column];

    Vector2 inital_position = Vector2{(float)row, (float)column};
    Vector2 possible_position;

    // If moving up one is a ___ position
    possible_position = Vector2{(float)row, (float)forward(piece_color(piece), column, 1)};
    if (is_valid_move(pieces, inital_position, possible_position)) {
        // If theres nothing in front
        if ((*pieces)[possible_position.x][possible_position.y] == Piece::None) {
            // i can move up one
            result.push_back({(int)possible_position.x, (int)possible_position.y});

            // additionally, if im on the second column
            bool im_on_the_second_column = (piece_color(piece) == Piece::White && column == 6) || (piece_color(piece) == Piece::Black && column == 1);
            if (im_on_the_second_column) {
                // and theres nobody there
                possible_position = Vector2{(float)row, (float)forward(piece_color(piece), column, 2)};
                if (is_valid_move(pieces, inital_position, possible_position)) {
                    if ((*pieces)[possible_position.x][possible_position.y] == Piece::None) {
                        // i can move up two
                        result.push_back({(int)possible_position.x, (int)possible_position.y});
                    }
                }
            }
        }
    }

    // auto rc = transpose(pieces, {(float)row, (float)column}, 0, 1);
    // auto t = (*pieces)[rc.x][rc.y];
    // ;
    // if (!t) {
    //     result.push_back({(int)rc.x, (int)rc.y});
    // }

    // If theres nothing in front, and if it's on the second file, it can move 2 square forward
    //
    // if (is_valid_move(pieces, inital_position, possible_position)) {
    //     if ((*pieces)[possible_position.x][possible_position.y] == Piece::None) {
    //         possible_position = Vector2{(float)row, (float)forward(piece_color(piece), column, 2)};
    //     }
    // }
    // if (is_valid_move(pieces, inital_position, possible_position)) {
    //     if ((*pieces)[possible_position.x][possible_position.y] == Piece::None) {
    //         result.push_back({(int)possible_position.x, (int)possible_position.y});
    //     }
    // }

    // if (nobody_in_front && possible_position_is_valid && im_on_the_second_column) {
    //     result.push_back({(int)row, (int)forward(piece_color(piece), column, 2)});
    // }

    // If theres a piece of the opposite color on the top left/right, it can eat it
    bool can_move_to_tl = is_valid_move(pieces, {(float)row, (float)column}, {(float)(row - 1), (float)forward(piece_color(piece), column, 1)}) && (*pieces)[row - 1][(float)forward(piece_color(piece), column, 1)];
    if (can_move_to_tl) {
        bool tl_has_opposite_color = piece_color(piece) != piece_color((*pieces)[row - 1][forward(piece_color(piece), column, 1)]);
        if (tl_has_opposite_color) {
            result.push_back({(int)row - 1, (int)forward(piece_color(piece), column, 1)});
        }
    }
    //
    //
    // auto rctl = transpose(pieces, {(float)row, (float)column}, -1, 1);
    // auto tl = (*pieces)[rctl.x][rctl.y];
    // if (tl) {
    //     if (piece_color(piece) != piece_color(tl)) {
    //         result.push_back({(int)rctl.x, (int)rctl.y});
    //     }
    // }
    auto rctr = transpose(pieces, {(float)row, (float)column}, 1, 1);
    auto tr = (*pieces)[rctr.x][rctr.y];
    if (tr) {
        if (piece_color(piece) != piece_color(tr)) {
            result.push_back({(int)rctr.x, (int)rctr.y});
        }
    }

    // TODO: En passant

    return result;
}

std::vector<std::array<int, 2>> get_primative_moves(std::array<std::array<int, 8>, 8> *pieces, int starting_row, int starting_column) {
    std::vector<std::array<int, 2>> result;

    int starting_piece = (*pieces)[starting_row][starting_column];

    std::vector<Direction> d;
    switch (piece_type(starting_piece)) {
        // case Piece::Pawn:
        // case Piece::Knight:
        case (Piece::Bishop):
            d = Directions::Bishop;
            break;
        case (Piece::Rook):
            d = Directions::Rook;
            break;
        case (Piece::Queen):
            d = Directions::Queen;
            break;
            // case Piece::King:
    }
    if (!d.empty()) {
        auto destinations = get_squares_in_directions(pieces, starting_row, starting_column, d);
        result.insert(result.end(), destinations.begin(), destinations.end());
        return result;
    }

    if (piece_type(starting_piece) == Piece::Pawn) {
        auto moves = get_pawn_moves(pieces, starting_row, starting_column);
        result.insert(result.end(), moves.begin(), moves.end());
    } else if (piece_type(starting_piece) == Piece::Knight) {
        auto moves = get_knight_moves(pieces, starting_row, starting_column);
        result.insert(result.end(), moves.begin(), moves.end());
    } else if (piece_type(starting_piece) == Piece::King) {
        auto moves = get_king_moves(pieces, starting_row, starting_column);
        result.insert(result.end(), moves.begin(), moves.end());
    }

    // debug(std::format("finaly result {}", result));
    return result;
}

std::vector<std::array<int, 2>> get_attacking_moves(std::array<std::array<int, 8>, 8> *pieces, int starting_row, int starting_column) {
    // TODO: pawn moving forward is primative but not attacking
    return get_primative_moves(pieces, starting_row, starting_column);
}

bool is_under_attack(int color, std::array<std::array<int, 8>, 8> *pieces) {
    std::vector<std::array<int, 2>> all_attacking_squares;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            int piece = (*pieces)[x][y];
            if (piece) {
                if (piece_color(piece) != color) {
                    auto moves = get_primative_moves(pieces, x, y);
                    if (piece_type(piece) == Piece::Pawn) {
                        // TODO: what if you erase(null?)
                        // if (is_valid_move(pieces, {(float)x, (float)y}, {(float)x, (float)forward(pieces, y, 1)})) {
                        //     std::array<int, 2> a = {x, forward(pieces, y, 1)};
                        //
                        //     moves.erase(std::find(moves.begin(), moves.end(), a));
                        // }
                        // std::array<int, 2> b = {x, forward(pieces, y, 2)};
                        // moves.erase(std::find(moves.begin(), moves.end(), b));
                    }
                    all_attacking_squares.insert(all_attacking_squares.end(), moves.begin(), moves.end());
                }
            }
        }
    }

    debug(std::format("000000"));
    for (auto &[x, y] : all_attacking_squares) {
        debug(std::format("attacking positions {} {}", x, y));
        if (piece_type((*pieces)[x][y]) == Piece::King && (piece_color((*pieces)[x][y]) == color)) {
            return true;
        }
    }
    return false;
};

void move_piece(std::array<std::array<int, 8>, 8> *pieces, int i_row, int i_col, int f_row, int f_col) {
    (*pieces)[f_row][f_col] = (*pieces)[i_row][i_col];
    if ((i_row != f_row) || (i_col != f_col)) {
        (*pieces)[i_row][i_col] = Piece::None;
    }
}

std::vector<std::array<int, 2>> get_valid_moves(std::array<std::array<int, 8>, 8> *pieces, int starting_row, int starting_column) {
    //
    std::vector<std::array<int, 2>> result;
    auto primative_moves = get_primative_moves(pieces, starting_row, starting_column);
    for (auto &[x, y] : primative_moves) {
        auto cloned_pieces = (*pieces);                                   // cloned_pieces contains a clone of pieces
        move_piece(&cloned_pieces, starting_row, starting_column, x, y);  // thats why we can modify without affecting our real board

        // If i make this move and im not under attack after moving, then im safe to do so
        if (!is_under_attack(piece_color((*pieces)[starting_row][starting_column]), &cloned_pieces)) {
            result.push_back({x, y});
        }
    }
    return result;
}

void update_squares(std::array<std::array<int, 8>, 8> *squares) {
    // TODO:
    //
    // Vector2 mouse_position = GetMousePosition();
    // Rectangle board_rect = Rectangle{0, 0, Constants::SQUARE_LENGTH * 8, Constants::SQUARE_LENGTH * 8};
    // if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    //     if (within_rectangle(mouse_position, board_rect)) {
    //         Vector2 a = row_col_from_mouse_position(mouse_position);
    //
    //         debug(std::format("bf: {}", (*squares)[a.x][a.y]));
    //
    //         (*squares)[a.x][a.y] ^= Square::Selected;
    //         // if (has_flag((*squares)[a.x][a.y], Square::Selected)) {
    //         //     (*squares)[a.x][a.y] &= ~Square::Selected;
    //         // } else {
    //         //     (*squares)[a.x][a.y] |= Square::Selected;
    //         // }
    //
    //         debug(std::format("af: {}", (*squares)[a.x][a.y]));
    //     }
    // }
}

void update_pieces(std::array<std::array<int, 8>, 8> *pieces) {}

Vector2 pressed_mouse_pos = {0, 0};

void update_board(std::array<std::array<int, 8>, 8> *squares, std::array<std::array<int, 8>, 8> *pieces) {
    Vector2 mouse_position = GetMousePosition();
    Rectangle board_rect = Rectangle{0, 0, Constants::SQUARE_LENGTH * 8, Constants::SQUARE_LENGTH * 8};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (within_rectangle(mouse_position, board_rect)) {
            pressed_mouse_pos = mouse_position;
            Vector2 inital_row_col = row_col_from_mouse_position(mouse_position);
            // debug(std::format("{}, {}", inital_row_col.x, inital_row_col.y));

            auto valid_moves = get_valid_moves(pieces, inital_row_col.x, inital_row_col.y);

            (*squares)[inital_row_col.x][inital_row_col.y] ^= Square::Selected;

            for (auto &[r, c] : valid_moves) {
                (*squares)[r][c] ^= Square::Indicator;
            }
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && pressed_mouse_pos.x) {
        Vector2 inital_row_col = row_col_from_mouse_position(pressed_mouse_pos);

        auto valid_moves = get_valid_moves(pieces, inital_row_col.x, inital_row_col.y);

        (*squares)[inital_row_col.x][inital_row_col.y] ^= Square::Selected;
        for (auto &[r, c] : valid_moves) {
            (*squares)[r][c] ^= Square::Indicator;
        }

        if (within_rectangle(mouse_position, board_rect)) {
            Vector2 final_row_col = row_col_from_mouse_position(mouse_position);

            std::array<int, 2> y = {(int)final_row_col.x, (int)final_row_col.y};

            if (std::find(valid_moves.begin(), valid_moves.end(), y) != valid_moves.end()) {
                move_piece(pieces, inital_row_col.x, inital_row_col.y, final_row_col.x, final_row_col.y);
            }
        }

        pressed_mouse_pos = {0, 0};
    }
}

void draw_piece_texture(Texture2D piece_texture, Rectangle dest_rect) {}
// void draw_piece_texture(Texture2D piece_texture) {
//     Rectangle dest_rect = rectangle_from_row_column(row, column);
//     DrawTexturePro(piece_texture, Rectangle{0, 0, (float)piece_texture.width, (float)piece_texture.height}, dest_rect, Vector2{0, 0}, 0, RAYWHITE);
// }
void draw_piece_texture(Texture2D piece_texture, int row, int column) {
    Rectangle dest_rect = rectangle_from_row_column(row, column);
    DrawTexturePro(piece_texture, Rectangle{0, 0, (float)piece_texture.width, (float)piece_texture.height}, dest_rect, Vector2{0, 0}, 0, RAYWHITE);
}

void draw_pieces(std::array<std::array<int, 8>, 8> *pieces, std::vector<std::tuple<int, Texture2D>> *piece_textures) {
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            Rectangle dest_rect = rectangle_from_row_column(row, column);

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
            auto rect = rectangle_from_row_column(row, column);
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
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = Constants::SQUARE_LENGTH * 8;
    const int screenHeight = Constants::SQUARE_LENGTH * 8;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    std::array<std::array<int, 8>, 8> pieces;

    std::array<std::array<int, 8>, 8> squares;
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            pieces[row][column] = 0;

            if ((row + column) % 2 == 0) {
                squares[row][column] = Square::Light;
            } else {
                squares[row][column] = Square::Dark;
            }
        }
    }

    // Load Dark Pieces //
    for (int row = 0; row < 8; row++) {
        pieces[row][1] = Piece::Black | Piece::Pawn;
    }
    pieces[0][0] = Piece::Black | Piece::Rook;
    pieces[1][0] = Piece::Black | Piece::Knight;
    pieces[2][0] = Piece::Black | Piece::Bishop;
    pieces[3][0] = Piece::Black | Piece::Queen;
    pieces[4][0] = Piece::Black | Piece::King;
    pieces[5][0] = Piece::Black | Piece::Bishop;
    pieces[6][0] = Piece::Black | Piece::Knight;
    pieces[7][0] = Piece::Black | Piece::Rook;

    // Load White Pieces //
    for (int x = 0; x < 8; x++) {
        pieces[x][6] = Piece::White | Piece::Pawn;
    }
    pieces[0][7] = Piece::White | Piece::Rook;
    pieces[1][7] = Piece::White | Piece::Knight;
    pieces[2][7] = Piece::White | Piece::Bishop;
    pieces[3][7] = Piece::White | Piece::Queen;
    pieces[4][7] = Piece::White | Piece::King;
    pieces[5][7] = Piece::White | Piece::Bishop;
    pieces[6][7] = Piece::White | Piece::Knight;
    pieces[7][7] = Piece::White | Piece::Rook;

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

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLUE);

        //
        // DrawText("Congrats! You created your first window!", 190, 200, 20,
        // LIGHTGRAY);

        draw_squares(&squares);
        draw_pieces(&pieces, &piece_textures);

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
