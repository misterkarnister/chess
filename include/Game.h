#ifndef GAME_H
#define GAME_H
#include <SDL3/SDL.h>

#include "structs.h"
#include "Piece.h"
class Game
{
    public:
        Game();
        virtual ~Game();
        Color players_color;
        void board_render();
        void pieces_render();


        void game_init();
        void game_close();
        int square_dim;
        SDL_Color white_color;
        SDL_Color black_color;
        Piece* board[64];

        bool square_free(Vec2i pos);
        void square_render(int x, int y, SDL_Color col = {0, 0, 0, 255});
        void valid_squares_find_all();
        bool checked;
        Color checked_color;

        Game_Mode mode;
        Game_State state;

        int white_valid_squares;
        int black_valid_squares;

        void game_judge();
        void game_end_render();

        Color turn;

    protected:

    private:



        void text_generate();
        void letter_render(int column, Color side);
        void digit_render(int row, Color side);


};

#endif // GAME_H
