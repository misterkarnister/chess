#include "King.h"
#include "object_init.h"

King::King()
{
    start_pos = true;
}

King::~King()
{
    //dtor
}

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
        break;
    }
    case BLACK:
    {
        board_pos.x = g1.players_color==WHITE ? 4 : 3;
        board_pos.y = g1.players_color==WHITE ? 0 : 7;
        g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&king_black;

        color_sdl = piece_black;
        break;
    }
    }
    name.text_load("K", t1.font_piece_get(), color_sdl);
    Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())};
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
        switch(g1.players_color)
        {
        case WHITE:
        {
            bool castler = true;
            if(rooks_white[0].start_pos)
            {
                Vec2i mods_long[] = {{-3, 0}, {-2, 0}, {-1, 0}};
                for(int i = 0; i < 3; i++)
                {
                    Vec2i chk = board_pos.add(mods_long[i]);
                    if(!g1.square_free(chk))
                    {
                        castler = false;
                        break;
                    }
                }
                if(castler)
                {
                    valid_squares.push_back({board_pos.x - 2, board_pos.y});
                }
            }

            if(rooks_white[1].start_pos)
            {
                castler = true;
                Vec2i mods_short[] = {{1, 0}, {2, 0}};
                for(int i = 0; i < 2; i++)
                {
                    Vec2i chk = board_pos.add(mods_short[i]);
                    if(!g1.square_free(chk))
                    {
                        castler= false;
                        break;
                    }
                }
                if(castler)
                {
                    valid_squares.push_back({board_pos.x + 2, board_pos.y});
                }
            }

            break;
        }
        case BLACK:
        {
            bool castler = true;
            if(rooks_black[1].start_pos)
            {
                Vec2i mods_short[] = {{1, 0}, {2, 0}, {3, 0}};
                for(int i = 0; i < 3; i++)
                {
                    Vec2i chk = board_pos.add(mods_short[i]);
                    if(!g1.square_free(chk))
                    {
                        castler = false;
                        break;
                    }
                }
                if(castler)
                {
                    valid_squares.push_back({board_pos.x + 2, board_pos.y});
                }
            }
            if(rooks_black[0].start_pos)
            {
                castler = true;
                Vec2i mods_long[] = {{-2, 0}, {-1, 0}};
                for(int i = 0; i < 2; i++)
                {
                    Vec2i chk = board_pos.add(mods_long[i]);
                    if(!g1.square_free(chk))
                    {
                        castler= false;
                        break;
                    }
                }
                if(castler)
                {
                    valid_squares.push_back({board_pos.x - 2, board_pos.y});
                }
            }

            break;
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
            Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (pos.x-1+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get())
                            ,static_cast<float>(t1.window_height_get() * 0.1 + (pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())
                           };
            g1.board[pos.y*8+pos.x-1]->name.position_update(newpos);
            g1.board[pos.y*8+pos.x-1]->position_update({pos.x-1, pos.y});
        }
        else if(pos.x-board_pos.x==-2)
        {
            g1.board[pos.y*8+pos.x+1] = g1.board[pos.y*8 + pos.x-2];
            g1.board[pos.y*8+pos.x-2] = nullptr;
            Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (pos.x+1+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get())
                            ,static_cast<float>(t1.window_height_get() * 0.1 + (pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())
                           };
            g1.board[pos.y*8+pos.x+1]->name.position_update(newpos);
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
            Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (pos.x-1+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get())
                            ,static_cast<float>(t1.window_height_get() * 0.1 + (pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())
                           };
            g1.board[pos.y*8+pos.x-1]->name.position_update(newpos);
            g1.board[pos.y*8+pos.x-1]->position_update({pos.x-1, pos.y});

        }
        else if(pos.x-board_pos.x==-2)
        {
            g1.board[pos.y*8+pos.x+1] = g1.board[pos.y*8 + pos.x-1];
            g1.board[pos.y*8+pos.x-1] = nullptr;
            Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (pos.x+1+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get())
                            ,static_cast<float>(t1.window_height_get() * 0.1 + (pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())
                           };
            g1.board[pos.y*8+pos.x+1]->name.position_update(newpos);
            g1.board[pos.y*8+pos.x+1]->position_update({pos.x+1, pos.y});
        }
        break;
    }
    }

    position_update(pos);

    Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get())
                    ,static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())
                   };
    name.position_update(newpos);



    start_pos = false;

    g1.black_valid_squares=0;
    g1.white_valid_squares=0;
    g1.turn==WHITE? g1.turn = BLACK : g1.turn = WHITE;
}
