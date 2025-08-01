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
    inline const Color SQUARE_SELECTED = GRAY;
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
    static const int None = 0;
    static const int Pawn = 1;    // 000001
    static const int Knight = 2;  // 000011
    static const int Bishop = 3;  // 000100
    static const int Rook = 4;    // 000101
    static const int Queen = 5;   // 000110
    static const int King = 6;    // 000111
                                  //
    static const int Black = 8;   // 001000
    static const int White = 16;  // 010000
};

struct Square {
    // static const int None = 0;
    static const int Selected = 1;   // 000001
    static const int Indicator = 2;  // 000001
                                     //
    static const int Dark = 8;       // 001000
    static const int Light = 16;     // 010000
};

struct Player {
    static const int None = 0;
    static const int Black = 1;
    static const int White = 2;
};

int p = Player::Black;

int forward(int from, int n) {
    switch (p) {
        case Player::Black:
            return from + n;
            break;
        case Player::White:
            return from - n;
            break;
    }
    return 0;
}

//
// enum class Square {
//   // None = 0, // 000000
//   //
//   Selected = 1, // 000001
//   Black = 8,    // 001000
//   White = 16,   // 010000
// };

void debug(std::string s) { TraceLog(LOG_INFO, s.c_str()); }

bool has_flag(int a, int b) { return (a & b) == b; }

int get_color(int a) {
    if (has_flag(a, Piece::White)) {
        return Piece::White;
    }
    if (has_flag(a, Piece::Black)) {
        return Piece::Black;
    }
    return 0;
}

bool is_opposite_color(int a, int b) { return get_color(a) != get_color(b); }
bool is_same_color(int a, int b) { return get_color(a) == get_color(b); }

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

std::vector<std::array<int, 2>> get_squares_in_directions(std::array<std::array<int, 8>, 8> *pieces, int starting_row, int starting_column, std::vector<Direction> directions) {
    std::vector<std::array<int, 2>> result;
    for (auto &direction : directions) {
        Vector2 temp_pos = Vector2{(float)starting_row, (float)starting_column};
        // debug(std::format("Bee  {}", temp_pos.y));
        switch (direction) {
            case (Direction::North):
                while (temp_pos.y < 7) {
                    temp_pos.y++;

                    auto p = (*pieces)[starting_row][temp_pos.y];
                    // If theres some piece in here
                    if (p) {
                        if (is_same_color(p, (*pieces)[starting_row][starting_column])) {
                            break;
                        }
                        if (is_opposite_color(p, (*pieces)[starting_row][starting_column])) {
                            result.push_back({(int)starting_row, (int)temp_pos.y});
                            break;
                        }
                    }

                    result.push_back({starting_row, (int)temp_pos.y});
                }
                break;
            case (Direction::East):
                while (temp_pos.x < 7) {
                    temp_pos.x++;
                    auto p = (*pieces)[temp_pos.x][starting_column];
                    // If theres some piece in here
                    if (p) {
                        if (is_same_color(p, (*pieces)[starting_row][starting_column])) {
                            break;
                        }
                        if (is_opposite_color(p, (*pieces)[starting_row][starting_column])) {
                            result.push_back({(int)temp_pos.x, (int)starting_column});
                            break;
                        }
                    }
                    result.push_back({(int)temp_pos.x, starting_column});
                    // debug(std::format("{}", temp_pos.y));
                }

                break;
            case (Direction::South):
                while (temp_pos.y > 0) {
                    temp_pos.y--;
                    auto p = (*pieces)[starting_row][temp_pos.y];
                    // If theres some piece in here
                    if (p) {
                        if (is_same_color(p, (*pieces)[starting_row][starting_column])) {
                            break;
                        }
                        if (is_opposite_color(p, (*pieces)[starting_row][starting_column])) {
                            result.push_back({(int)temp_pos.x, (int)starting_column});
                            break;
                        }
                    }
                    result.push_back({starting_row, (int)temp_pos.y});
                    // debug(std::format("{}", temp_pos.y));
                }

                break;
            case (Direction::West):
                while (temp_pos.x > 0) {
                    temp_pos.x--;
                    auto p = (*pieces)[temp_pos.x][starting_column];
                    // If theres some piece in here
                    if (p) {
                        if (is_same_color(p, (*pieces)[starting_row][starting_column])) {
                            break;
                        }
                        if (is_opposite_color(p, (*pieces)[starting_row][starting_column])) {
                            result.push_back({(int)temp_pos.x, (int)starting_column});
                            break;
                        }
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
                    // If theres some piece in here
                    if (p) {
                        if (is_same_color(p, (*pieces)[starting_row][starting_column])) {
                            break;
                        }
                        if (is_opposite_color(p, (*pieces)[starting_row][starting_column])) {
                            result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                            break;
                        }
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
                    // If theres some piece in here
                    if (p) {
                        if (is_same_color(p, (*pieces)[starting_row][starting_column])) {
                            break;
                        }
                        if (is_opposite_color(p, (*pieces)[starting_row][starting_column])) {
                            result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                            break;
                        }
                    }
                    result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                    // debug(std::format("{}", temp_pos.y));
                }
                break;
            case (Direction::SouthWest):
                while (temp_pos.y > 0 && temp_pos.x > 0) {
                    temp_pos.x--;
                    temp_pos.y--;
                    auto p = (*pieces)[temp_pos.x][temp_pos.y];
                    // If theres some piece in here
                    if (p) {
                        if (is_same_color(p, (*pieces)[starting_row][starting_column])) {
                            break;
                        }
                        if (is_opposite_color(p, (*pieces)[starting_row][starting_column])) {
                            result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                            break;
                        }
                    }
                    result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                    // debug(std::format("{}", temp_pos.y));
                }
                break;
            case (Direction::NorthWest):
                while (temp_pos.y < 7 && temp_pos.x > 0) {
                    temp_pos.x--;
                    temp_pos.y++;
                    auto p = (*pieces)[temp_pos.x][temp_pos.y];
                    // If theres some piece in here
                    if (p) {
                        if (is_same_color(p, (*pieces)[starting_row][starting_column])) {
                            break;
                        }
                        if (is_opposite_color(p, (*pieces)[starting_row][starting_column])) {
                            result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                            break;
                        }
                    }
                    result.push_back({(int)temp_pos.x, (int)temp_pos.y});
                    // debug(std::format("{}", temp_pos.y));
                }
                break;
        }
    }

    return result;
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
        if (x >= 0 && x <= 7 && y >= 0 && y <= 7) {
            if (!is_same_color((*pieces)[x][y], (*pieces)[row][column])) {
                result.push_back({x, y});
            }
        };
    }
    return result;
}

std::vector<std::array<int, 2>> get_valid_moves(std::array<std::array<int, 8>, 8> *squares, std::array<std::array<int, 8>, 8> *pieces, int starting_row, int starting_column) {
    std::vector<std::array<int, 2>> result;

    if (has_flag((*pieces)[starting_row][starting_column], Piece::Queen)) {
        auto ds = get_squares_in_directions(pieces, starting_row, starting_column, Directions::Queen);
        result.insert(result.end(), ds.begin(), ds.end());

    } else if (has_flag((*pieces)[starting_row][starting_column], Piece::Bishop)) {
        auto ds = get_squares_in_directions(pieces, starting_row, starting_column, Directions::Bishop);
        result.insert(result.end(), ds.begin(), ds.end());

    } else if (has_flag((*pieces)[starting_row][starting_column], Piece::Rook)) {
        auto ds = get_squares_in_directions(pieces, starting_row, starting_column, Directions::Rook);
        result.insert(result.end(), ds.begin(), ds.end());
    } else if (has_flag((*pieces)[starting_row][starting_column], Piece::Knight)) {
        // Knight
        auto ds = get_knight_moves(pieces, starting_row, starting_column);
        result.insert(result.end(), ds.begin(), ds.end());
        // candidates = [
        //        // top
        //        [x - 2, y + 1],
        //        [x - 1, y + 2],
        //        [x + 1, y + 2],
        //        [x + 2, y + 1],
        //        // bottom
        //        [x - 2, y - 1],
        //        [x - 1, y - 2],
        //        [x + 1, y - 2],
        //        [x + 2, y - 1],
        //    ];
        //
    } else if (has_flag((*pieces)[starting_row][starting_column], Piece::Pawn)) {
        std::vector<std::array<int, 2>> ds;
        // If there is no piece in front of the pawn, it can move
        auto t = (*pieces)[starting_row][starting_column + 1];
        if (!t) {
            ds.push_back({(int)starting_row, (int)starting_column + 1});
        }

        auto tl = (*pieces)[starting_row - 1][starting_column + 1];
        if (tl) {
            if (is_opposite_color((*pieces)[starting_row][starting_column], tl)) {
                ds.push_back({(int)starting_row - 1, (int)starting_column + 1});
            }
        }

        auto tr = (*pieces)[starting_row + 1][starting_column + 1];
        if (tr) {
            if (is_opposite_color((*pieces)[starting_row][starting_column], tr)) {
                ds.push_back({(int)starting_row + 1, (int)starting_column + 1});
            }
        }
        result.insert(result.end(), ds.begin(), ds.end());
    }

    // debug(std::format("finaly result {}", result));
    return result;
}

Vector2 row_col_from_mouse_position(Vector2 mouse_position) {
    // auto x = ((int)trunc(mouse_position.x) - Board::OFFSET) / LENGTH;
    // auto y = ((int)trunc(mouse_position.y) - Board::OFFSET) / LENGTH;
    auto row = ((int)trunc(mouse_position.x)) / Constants::SQUARE_LENGTH;
    auto y = ((int)trunc(mouse_position.y)) / Constants::SQUARE_LENGTH;
    auto col = 7 - y;
    return Vector2{(float)row, (float)col};
}

std::tuple<int, Texture2D> load_piece_texture(int piece) {
    std::string filename = "assets/pieces/";

    // Color
    if (piece & Piece::Black) {
        filename.append("b");
    } else if (piece & Piece::White) {
        filename.append("w");
    }

    if (has_flag(piece, Piece::Queen)) {
        filename.append("Q");
    } else if (has_flag(piece, Piece::Bishop)) {
        filename.append("B");
    } else if (has_flag(piece, Piece::Pawn)) {
        filename.append("P");
    } else if (has_flag(piece, Piece::King)) {
        filename.append("K");
    } else if (has_flag(piece, Piece::Knight)) {
        filename.append("N");
    } else if (has_flag(piece, Piece::Rook)) {
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

// void unload_texture(Texture2D piece_texture) {
//     UnloadTexture(piece_texture);
// }

void unload_textures(std::vector<std::tuple<int, Texture2D>> *piece_textures) {
    for (const auto &[key, value] : (*piece_textures)) {
        UnloadTexture(value);
    }
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

void move_piece(std::array<std::array<int, 8>, 8> *pieces, int i_row, int i_col, int f_row, int f_col) {
    if ((*pieces)[f_row][f_col]) {
    }
    (*pieces)[f_row][f_col] = (*pieces)[i_row][i_col];
    if ((i_row != f_row) || (i_col != f_col)) {
        (*pieces)[i_row][i_col] = Piece::None;
    }
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
            debug(std::format("{}, {}", inital_row_col.x, inital_row_col.y));

            auto valid_moves = get_valid_moves(squares, pieces, inital_row_col.x, inital_row_col.y);

            (*squares)[inital_row_col.x][inital_row_col.y] ^= Square::Selected;

            for (auto &move : valid_moves) {
                (*squares)[move[0]][move[1]] ^= Square::Indicator;
            }
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && pressed_mouse_pos.x) {
        Vector2 inital_row_col = row_col_from_mouse_position(pressed_mouse_pos);

        auto valid_moves = get_valid_moves(squares, pieces, inital_row_col.x, inital_row_col.y);

        (*squares)[inital_row_col.x][inital_row_col.y] ^= Square::Selected;

        for (auto &move : valid_moves) {
            (*squares)[move[0]][move[1]] ^= Square::Indicator;
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
            if (has_flag((*squares)[row][column], Square::Dark)) {
                DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_DARK);
            } else if (has_flag((*squares)[row][column], Square::Light)) {
                DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_LIGHT);
            }
            if (has_flag((*squares)[row][column], Square::Selected)) {
                DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_SELECTED);
            }
            if (has_flag((*squares)[row][column], Square::Indicator)) {
                int centerX = rect.x + 0.5 * rect.width;
                int centerY = rect.y + 0.5 * rect.height;
                int radius = 0.25 * rect.width;
                DrawCircle(centerX, centerY, radius, Constants::SQUARE_SELECTED);
                //
            }
        }
    }
}
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    // auto black_queen_texture = load_piece_texture(Piece::Black | Piece::Queen);
    //
    // pieces []
    // squares []

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

    pieces[5][7 - 1] = Piece::White | Piece::Pawn;

    // pieces[1][1] = Piece::Black | Piece::Queen;
    // pieces[4][5] = Piece::White | Piece::Bishop;
    // pieces[3][2] = Piece::Black | Piece::Pawn;

    std::vector<std::tuple<int, Texture2D>> all_textures = load_textures(&pieces);

    // debug(std::format("{}", 0b0110 | 0b1000));
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

        // debug(std::format("{}", get_color(Piece::None)));

        ClearBackground(BLUE);

        //
        // DrawText("Congrats! You created your first window!", 190, 200, 20,
        // LIGHTGRAY);

        draw_squares(&squares);
        draw_pieces(&pieces, &all_textures);
        // draw_piece_texture(black_queen_texture, 1, 2);

        EndDrawing();

        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    unload_textures(&all_textures);
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
