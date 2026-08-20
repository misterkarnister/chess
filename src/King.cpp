#include "King.h"
#include "object_init.h"

King::King()
{
    start_pos = true;
}

King::~King() = default;

void King::piece_init(Color c)
{

    color = c;

    switch(c)
    {
    case WHITE:
    {
        board_pos.x = g1.players_color==WHITE ? 4 : 3;
        board_pos.y =  g1.players_color==WHITE ? 7 : 0;
        g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&king_white;


        color_sdl= piece_white;
        the_texture = &king_white_obj;
        break;
    }
    case BLACK:
    {
        board_pos.x = g1.players_color==WHITE ? 4 : 3;
        board_pos.y = g1.players_color==WHITE ? 0 : 7;
        g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&king_black;

        color_sdl = piece_black;
        the_texture = &king_black_obj;
        break;
    }
    }
    name.text_load("K", t1.font_piece_get(), color_sdl);
    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
    name.position_update(newpos);
}
void King::valid_squares_find()
{
    valid_squares_empty();
    Vec2i mods[] =
    {

        {
            -1, 0
            },
        {
            -1, 1
            },
        {
            0, 1
        },
        {
            1, 1
        },
        {
            1, 0
        },
        {
            1, -1
        },
        {
            0, -1
        },
        {
            -1, -1
            }
    };
    for(Vec2i mod : mods)
    {
        Vec2i chk = board_pos.add(mod);
        if(chk.x>=0 && chk.x<8 && chk.y>=0 && chk.y<8)
        {
            if(g1.square_free(chk) || g1.board[chk.y*8 + chk.x]->color!=color)
            {
                valid_squares.push_back(chk);
            }

        }
    }
    if(start_pos)
    {
        Rook* my_rooks[2] = {(color == WHITE) ? &rooks_white[0] : &rooks_black[0],
                             (color == WHITE) ? &rooks_white[1] : &rooks_black[1]};

        for(int r = 0; r < 2; r++)
        {
            if(!my_rooks[r]->start_pos)
                continue;

            int dx = my_rooks[r]->board_pos.x - board_pos.x;
            if(dx == 0)
                continue;

            int step = dx > 0 ? 1 : -1;

            bool clear = true;
            for(int x = board_pos.x + step; x != my_rooks[r]->board_pos.x; x += step)
            {
                Vec2i chk = {x, board_pos.y};
                if(!g1.square_free(chk))
                {
                    clear = false;
                    break;
                }
            }

            if(clear)
            {
                valid_squares.push_back({board_pos.x + 2 * step, board_pos.y});
            }
        }
    }

}

void King::move(Vec2i pos)
{
    g1.board[pos.y*8 + pos.x] = g1.board[board_pos.y*8 + board_pos.x];
    g1.board[board_pos.y*8 + board_pos.x] = nullptr;

    // castle
    switch(g1.players_color)
    {
    case WHITE:
    {
        if(pos.x-board_pos.x==2)
        {
            g1.board[pos.y*8+pos.x-1] = g1.board[pos.y*8 + pos.x+1];
            g1.board[pos.y*8+pos.x+1] = nullptr;
            Vec2f rookpos = g1.board_to_screen({pos.x-1, pos.y}, name.width_get(), name.height_get());
            g1.board[pos.y*8+pos.x-1]->name.position_update(rookpos);
            g1.board[pos.y*8+pos.x-1]->position_update({pos.x-1, pos.y});
        }
        else if(pos.x-board_pos.x==-2)
        {
            g1.board[pos.y*8+pos.x+1] = g1.board[pos.y*8 + pos.x-2];
            g1.board[pos.y*8+pos.x-2] = nullptr;
            Vec2f rookpos = g1.board_to_screen({pos.x+1, pos.y}, name.width_get(), name.height_get());
            g1.board[pos.y*8+pos.x+1]->name.position_update(rookpos);
            g1.board[pos.y*8+pos.x+1]->position_update({pos.x+1, pos.y});
        }
        break;
    }
    case BLACK:
    {
        if(pos.x-board_pos.x==2)
        {
            g1.board[pos.y*8+pos.x-1] = g1.board[pos.y*8 + pos.x+2];
            g1.board[pos.y*8+pos.x+2] = nullptr;
            Vec2f rookpos = g1.board_to_screen({pos.x-1, pos.y}, name.width_get(), name.height_get());
            g1.board[pos.y*8+pos.x-1]->name.position_update(rookpos);
            g1.board[pos.y*8+pos.x-1]->position_update({pos.x-1, pos.y});

        }
        else if(pos.x-board_pos.x==-2)
        {
            g1.board[pos.y*8+pos.x+1] = g1.board[pos.y*8 + pos.x-1];
            g1.board[pos.y*8+pos.x-1] = nullptr;
            Vec2f rookpos = g1.board_to_screen({pos.x+1, pos.y}, name.width_get(), name.height_get());
            g1.board[pos.y*8+pos.x+1]->name.position_update(rookpos);
            g1.board[pos.y*8+pos.x+1]->position_update({pos.x+1, pos.y});
        }
        break;
    }
    }

    position_update(pos);

    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
    name.position_update(newpos);



    start_pos = false;

    g1.black_valid_squares=0;
    g1.white_valid_squares=0;
    g1.toggle_turn();

    c1.engine_start();
    c1.full_eval();
}
