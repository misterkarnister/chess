#include "Bishop.h"
#include "object_init.h"
Bishop::Bishop()
{
    //ctor
}

Bishop::~Bishop()
{
    //dtor
}

void Bishop::piece_init(Color c)
{

    color = c;
    static int white_ind = 0;
    static int black_ind = 0;
    switch(c)
    {
    case WHITE:
        {
            board_pos.x = white_ind==0? 2 : 5;
            board_pos.y = g1.players_color==WHITE ? 7 : 0;
            g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&bishops_white[white_ind];

            white_ind++;
            color_sdl= piece_white;
            break;
        }
    case BLACK:
        {
            board_pos.x = black_ind==0? 2 : 5;
            board_pos.y =  g1.players_color==WHITE ? 0 : 7;
            g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&bishops_black[black_ind];
            black_ind++;
            color_sdl = piece_black;
            break;
        }
    }
    name.text_load("B", t1.font_piece_get(), color_sdl);
    Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())};
    name.position_update(newpos);
}
void Bishop::valid_squares_find()
{
    valid_squares_empty();

    Vec2i chk = {board_pos.x+1, board_pos.y+1};

    while(chk.x<8 && chk.y<8 && g1.square_free(chk))
    {
        valid_squares.push_back(chk);
        chk.x++;
        chk.y++;
    }
    if(chk.x<8 && chk.y<8 && g1.board[chk.y*8+chk.x]->color!=color)
    {
        valid_squares.push_back(chk);
    }


    chk = {board_pos.x-1, board_pos.y-1};
    while(chk.x>=0 && chk.y>=0 && g1.square_free(chk))
    {
        valid_squares.push_back(chk);
        chk.x--;
        chk.y--;
    }
    if(chk.x>=0 && chk.y>=0 && g1.board[chk.y*8+chk.x]->color!=color)
    {
        valid_squares.push_back(chk);
    }


    chk = {board_pos.x-1, board_pos.y+1};
    while(chk.x>=0 && chk.y<8 && g1.square_free(chk))
    {
        valid_squares.push_back(chk);
        chk.y++;
        chk.x--;
    }
    if(chk.x>=0 && chk.y<8 && g1.board[chk.y*8+chk.x]->color!=color)
    {
        valid_squares.push_back(chk);
    }


    chk = {board_pos.x+1, board_pos.y-1};
    while(chk.x<8 && chk.y>=0 && g1.square_free(chk))
    {
        valid_squares.push_back(chk);
        chk.y--;
        chk.x++;
    }
    if(chk.x<8 && chk.y>=0 && g1.board[chk.y*8+chk.x]->color!=color)
    {
        valid_squares.push_back(chk);
    }

}
