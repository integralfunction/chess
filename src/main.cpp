#include <array>
#include <format>
#include <vector>

#include "raylib.h"

namespace Constants {
    inline const int SQUARE_LENGTH = 48;
    inline const Color SQUARE_LIGHT = Color{235, 236, 208, 255};
    inline const Color SQUARE_DARK = Color{119, 149, 86, 255};
    inline const Color SQUARE_SELECTED = GRAY;
    inline const Color TRANSPARENT = Color{0, 0, 0, 0};
}  // namespace Constants

// enum class Piece {
//   None = 0,   // 000000
//   Pawn = 1,   // 000001
//   Knight = 2, // 000011
//   Bishop = 3, // 000100
//   Rook = 4,   // 000101
//   Queen = 5,  // 000110
//   King = 6,   // 000111
//               //
//   Black = 8,  // 001000
//   White = 16, // 010000
// };

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
    static const int Selected = 1;  // 000001
                                    //
    static const int Dark = 8;      // 001000
    static const int Light = 16;    // 010000
};
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

Texture2D load_piece_texture(int piece) {
    std::string filename = "assets/pieces/";

    // Color
    if (piece & Piece::Black) {
        filename.append("b");
    } else if (piece & Piece::White) {
        filename.append("w");
    }

    debug(filename);
    if (has_flag(piece, Piece::Queen)) {
        // debug(std::format("{}", piece & Piece::Queen));
        filename.append("Q");
    }

    filename.append(".png");
    return LoadTexture(filename.c_str());
}

std::vector<Texture2D> load_textures(std::array<std::array<int, 8>, 8> *pieces) {
    std::vector<Texture2D> all_textures;
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

void draw_piece_texture(Texture2D piece_texture, Rectangle dest_rect) {}
// void draw_piece_texture(Texture2D piece_texture) {
//     Rectangle dest_rect = rectangle_from_row_column(row, column);
//     DrawTexturePro(piece_texture, Rectangle{0, 0, (float)piece_texture.width, (float)piece_texture.height}, dest_rect, Vector2{0, 0}, 0, RAYWHITE);
// }
void draw_piece_texture(Texture2D piece_texture, int row, int column) {
    Rectangle dest_rect = rectangle_from_row_column(row, column);
    DrawTexturePro(piece_texture, Rectangle{0, 0, (float)piece_texture.width, (float)piece_texture.height}, dest_rect, Vector2{0, 0}, 0, RAYWHITE);
}

void draw_pieces(std::array<std::array<int, 8>, 8> *pieces, std::vector<Texture2D> all_textures) {
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            Rectangle dest_rect = rectangle_from_row_column(row, column);
            switch ((*pieces)[row][column]) {
                case (Piece::Black | Piece::Queen):
                    DrawTexturePro(piece_texture, Rectangle{0, 0, (float)piece_texture.width, (float)piece_texture.height}, dest_rect, Vector2{0, 0}, 0, RAYWHITE);
            }
        }
    }
    for (Texture2D texture : all_textures) {
        draw_piece_texture(texture);
        // draw_piece_texture(texture, 1, 1);
    }
}

void move_piece(std::array<std::array<int, 8>, 8> *pieces, Vector2 initial_position, Vector2 final_position) {
    (*pieces)[final_position.x][final_position.y] = (*pieces)[initial_position.x][initial_position.y];
    (*pieces)[initial_position.x][initial_position.y] = Piece::None;
}

void update_squares(std::array<std::array<int, 8>, 8> *squares) {
    // TODO:
}

void draw_squares(std::array<std::array<int, 8>, 8> *squares) {
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            auto rect = rectangle_from_row_column(row, column);
            switch ((*squares)[row][column]) {
                case Square::Dark:
                    DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_DARK);
                case Square::Light:
                    DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_LIGHT);
                case Square::Selected:
                    DrawRectangle(rect.x, rect.y, rect.width, rect.height, Constants::SQUARE_SELECTED);
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

    pieces[1][1] = Piece::Black | Piece::Queen;

    auto all_textures = load_textures(&pieces);

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        // ----------------------------------------------------------------------------------
        // debug(std::format("{}", Piece::Rook | Piece::Black));
        update_squares(&squares);
        update_pieces(&pieces);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLUE);

        // DrawText("Congrats! You created your first window!", 190, 200, 20,
        // LIGHTGRAY);

        draw_squares(&squares);
        draw_pieces(all_textures);
        // draw_piece_texture(black_queen_texture, 1, 2);
        // TraceLog(LOG_INFO, "Z ");

        EndDrawing();

        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
