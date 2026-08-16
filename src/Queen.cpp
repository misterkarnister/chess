#include "Queen.h"
#include "object_init.h"
Queen::Queen()
{
    //ctor
}

Queen::~Queen()
{
    //dtor
}

void Queen::piece_init(Color c)
{

    color = c;

    switch(c)
    {
    case WHITE:
        {
            board_pos.x = g1.players_color==WHITE ? 3 : 4;
            board_pos.y =  g1.players_color==WHITE ? 7 : 0;
            g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&queen_white;


            color_sdl= piece_white;
            the_texture = &queen_white_obj;
            break;
        }
    case BLACK:
        {
            board_pos.x = g1.players_color==WHITE ? 3 : 4;
            board_pos.y =  g1.players_color==WHITE ? 0 : 7;
            g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&queen_black;

            color_sdl = piece_black;
            the_texture = &queen_black_obj;
            break;
        }
    }
    name.text_load("Q", t1.font_piece_get(), color_sdl);
    Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())};
    name.position_update(newpos);
}
void Queen::valid_squares_find()
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


    chk = {board_pos.x+1, board_pos.y+1};

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
