#include "Rook.h"
#include "object_init.h"
#include <iostream>
Rook::Rook()
{
    start_pos = true;
}

Rook::~Rook()
{
    //dtor
}
void Rook::piece_init(Color c)
{

    color = c;
    static int white_ind = 0;
    static int black_ind = 0;
    switch(c)
    {
    case WHITE:
        {
            board_pos.x = white_ind==0? 0 : 7;
            board_pos.y =  g1.players_color==WHITE ? 7 : 0;
            g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&rooks_white[white_ind];

            white_ind++;
            color_sdl= piece_white;
            the_texture = &rook_white_obj;
            break;
        }
    case BLACK:
        {
            board_pos.x = black_ind==0? 0:7;
            board_pos.y =  g1.players_color==WHITE ? 0 : 7;
            g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&rooks_black[black_ind];
            black_ind++;
            color_sdl = piece_black;
            the_texture = &rook_black_obj;
            break;
        }
    }
    name.text_load("R", t1.font_piece_get(), color_sdl);
    Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())};
    name.position_update(newpos);
}
void Rook::valid_squares_find()
{
    valid_squares_empty();

    Vec2i chk = {board_pos.x+1, board_pos.y};

    while(chk.x<8 && g1.square_free(chk))
    {
        valid_squares.push_back(chk);
        chk.x++;
    }
    if(chk.x<8 && g1.board[chk.y*8+chk.x]->color!=color)
    {
        valid_squares.push_back(chk);
    }


    chk = {board_pos.x-1, board_pos.y};
    while(chk.x>=0 && g1.square_free(chk))
    {
        valid_squares.push_back(chk);
        chk.x--;
    }
    if(chk.x>=0 && g1.board[chk.y*8+chk.x]->color!=color)
    {
        valid_squares.push_back(chk);
    }


    chk = {board_pos.x, board_pos.y+1};
    while(chk.y<8 && g1.square_free(chk))
    {
        valid_squares.push_back(chk);
        chk.y++;
    }
    if(chk.y<8 && g1.board[chk.y*8+chk.x]->color!=color)
    {
         valid_squares.push_back(chk);
    }



    chk = {board_pos.x, board_pos.y-1};
    while(chk.y>=0 && g1.square_free(chk))
    {
        valid_squares.push_back(chk);
        chk.y--;
    }
    if(chk.y>=0 && g1.board[chk.y*8+chk.x]->color!=color)
    {
        valid_squares.push_back(chk);
    }


}
void Rook::move(Vec2i pos)
{


    g1.board[pos.y*8 + pos.x] = g1.board[board_pos.y*8 + board_pos.x];
    g1.board[board_pos.y*8 + board_pos.x] = nullptr;
    position_update(pos);

    Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())};
    name.position_update(newpos);

    start_pos = false;

    g1.black_valid_squares=0;
    g1.white_valid_squares=0;
    g1.turn==WHITE? g1.turn = BLACK : g1.turn = WHITE;

}
