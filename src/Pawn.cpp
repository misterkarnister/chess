#include "Pawn.h"
#include "object_init.h"
#include "mSDL.h"
Pawn::Pawn()
{
    start_pos = true;
    last_double_move = false;
    promoting = false;
    inited = false;
}

Pawn::~Pawn()
{
}
void Pawn::piece_init(Color c)
{

    color = c;
    switch(c)
    {
    case WHITE:
    {
        board_pos.x = g1.pawn_white_idx;
        board_pos.y =  g1.players_color==WHITE ? 6 : 1;
        g1.board[g1.players_color==WHITE? 48 + g1.pawn_white_idx : 8 + g1.pawn_white_idx]=&pawns_white[g1.pawn_white_idx];

        g1.pawn_white_idx++;
        color_sdl= piece_white;
        the_texture = &pawn_white_obj;
        break;
    }
    case BLACK:
    {
        board_pos.x = g1.pawn_black_idx;
        board_pos.y =  g1.players_color==WHITE ? 1 : 6;
        g1.board[g1.players_color==WHITE? 8 + g1.pawn_black_idx : 48 + g1.pawn_black_idx]=&pawns_black[g1.pawn_black_idx];
        g1.pawn_black_idx++;
        color_sdl = piece_black;
        the_texture = &pawn_black_obj;
        break;
    }
    }
    name.text_load("p", t1.font_piece_get(), color_sdl);
    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
    name.position_update(newpos);
}
void Pawn::valid_squares_find()
{
    valid_squares_empty();
    if(color==g1.players_color)
    {
        Vec2i mod = {0, -1};
        Vec2i pot = board_pos.add(mod);
        if(pot.y>=0 && g1.square_free(pot) )
        {
            valid_squares.push_back(pot);
        }

        if(start_pos)
        {
            Vec2i mod1 = {0, -1};
            Vec2i chk1 = board_pos.add(mod1);
            if(chk1.y>=0 && g1.square_free(chk1))
            {
                mod = {0, -2};
                pot = board_pos.add(mod);
                if(pot.y>=0 && g1.square_free(pot) )
                {
                    valid_squares.push_back(pot);
                }

            }
        }

        Vec2i take1 = {-1, -1};
        Vec2i take2 = {1, -1};
        Vec2i pot1 = board_pos.add(take1);
        Vec2i pot2 = board_pos.add(take2);
        if(pot1.x>=0 && pot1.y>=0 && !g1.square_free(pot1) && g1.board[pot1.y*8+pot1.x]->color!=color)
        {
            valid_squares.push_back(pot1);

        }

        if(pot2.x<8 && pot2.y>=0 && !g1.square_free(pot2) && g1.board[pot2.y*8+pot2.x]->color!=color)
        {
            valid_squares.push_back(pot2);

        }


        Vec2i en_passant1 = {-1, 0};
        Vec2i en_passant2 = {1, 0};
        Vec2i pot3 = board_pos.add(en_passant1);
        Vec2i pot4 = board_pos.add(en_passant2);

        if(!g1.square_free(pot3) && g1.board[pot3.y*8 + pot3.x]->name.text_cmp("p")==0)
        {
            Pawn* cast = static_cast<Pawn*>(g1.board[pot3.y*8 + pot3.x]);
            if(pot3.x>=0  && cast->color!=color && cast->last_double_move)
            {
                valid_squares.push_back({pot3.x, pot3.y-1});

            }
        }
        if(!g1.square_free(pot4) && g1.board[pot4.y*8+pot4.x]->name.text_cmp("p")==0)
        {
            Pawn* cast = static_cast<Pawn*>(g1.board[pot4.y*8 + pot4.x]);
            if(pot4.x<8 && cast->color!=color && cast->last_double_move)
            {
                valid_squares.push_back({pot4.x, pot4.y-1});

            }
        }


    }
    else
    {
        Vec2i mod = {0, 1};
        Vec2i pot = board_pos.add(mod);
        if(pot.y<8 && g1.square_free(pot))
        {
            valid_squares.push_back(pot);
        }

        if(start_pos)
        {
            Vec2i mod1 = {0, 1};
            Vec2i chk1 = board_pos.add(mod1);
            if(chk1.y<8 && g1.square_free(chk1))
            {
                mod = {0, 2};
                pot = board_pos.add(mod);
                if(pot.y<8 && g1.square_free(pot))
                {
                    valid_squares.push_back(pot);
                }

            }
        }
        Vec2i take1 = {-1, 1};
        Vec2i take2 = {1, 1};
        Vec2i pot1 = board_pos.add(take1);
        Vec2i pot2 = board_pos.add(take2);
        if(pot1.x>=0 && pot1.y<8 && !g1.square_free(pot1) && g1.board[pot1.y*8+pot1.x]->color!=color)
        {
            valid_squares.push_back(pot1);
        }

        if(pot2.x<8 && pot2.y<8 && !g1.square_free(pot2) && g1.board[pot2.y*8+pot2.x]->color!=color)
        {
            valid_squares.push_back(pot2);
        }


        Vec2i en_passant1 = {-1, 0};
        Vec2i en_passant2 = {1, 0};
        Vec2i pot3 = board_pos.add(en_passant1);
        Vec2i pot4 = board_pos.add(en_passant2);
        if(!g1.square_free(pot3) && g1.board[pot3.y*8 + pot3.x]->name.text_cmp("p")==0)
        {
            Pawn* cast = static_cast<Pawn*>(g1.board[pot3.y*8 + pot3.x]);
            if(pot3.x>=0  && cast->color!=color && cast->last_double_move)
            {
                valid_squares.push_back({pot3.x, pot3.y+1});

            }
        }

        if(!g1.square_free(pot4) && g1.board[pot4.y*8+pot4.x]->name.text_cmp("p")==0)
        {
            Pawn* cast = static_cast<Pawn*>(g1.board[pot4.y*8 + pot4.x]);
            if(pot4.x<8 && cast->color!=color && cast->last_double_move)
            {
                valid_squares.push_back({pot4.x, pot4.y+1});
            }
        }

    }

}

void Pawn::move(Vec2i pos)
{
    if(start_pos)
        last_double_move = true;

    if(color==g1.players_color)
    {
        Vec2i mod1 = {-1, -1};
        Vec2i chk1 = board_pos.add(mod1);
        if(chk1.compare(pos) && g1.square_free(pos))
        {
            g1.board[pos.y*8 + 8 + pos.x] = nullptr;
        }
        Vec2i mod2 = {1, -1};
        Vec2i chk2 = board_pos.add(mod2);
        if(chk2.compare(pos) && g1.square_free(pos))
        {
            g1.board[pos.y*8 + 8 + pos.x] = nullptr;
        }
    }
    else
    {
        Vec2i mod1 = {-1, 1};
        Vec2i chk1 = board_pos.add(mod1);
        if(chk1.compare(pos) && g1.square_free(pos))
        {
            g1.board[pos.y*8 - 8 + pos.x] = nullptr;
        }
        Vec2i mod2 = {1, 1};
        Vec2i chk2 = board_pos.add(mod2);
        if(chk2.compare(pos) && g1.square_free(pos))
        {
            g1.board[pos.y*8 - 8 + pos.x] = nullptr;
        }
    }

    g1.board[pos.y*8 + pos.x] = g1.board[board_pos.y*8 + board_pos.x];
    g1.board[board_pos.y*8 + board_pos.x] = nullptr;
    position_update(pos);

    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
    name.position_update(newpos);

    start_pos = false;

    if((g1.players_color==color && board_pos.y==0) || (g1.players_color!=color && board_pos.y==7))
    {
        promoting = true;
    }


    g1.black_valid_squares=0;
    g1.white_valid_squares=0;
    g1.toggle_turn();

    c1.engine_start();
    c1.full_eval();
}
void Pawn::promote_render()
{
    bool modulo = board_pos.x%2==0?true:false;
    bool site  = g1.players_color==color?true:false;

    for(int i =0;i<4;i++)
        promo_display[i].piece_init(WHITE);

    SDL_Color start_color;

    if(g1.players_color==WHITE)
    {
        if(site)
        {
            if(!inited)
            {
                for(int i =0;i<4;i++)
                {
                    promo_display[i].piece_init(WHITE);
                    switch(i)
                    {
                        case 0: promo_display[i].name.text_load("Q", t1.font_piece_get(), piece_white); promo_display[i].the_texture = &queen_white_obj;break;
                        case 1: promo_display[i].name.text_load("R", t1.font_piece_get(), piece_white); promo_display[i].the_texture = &rook_white_obj;break;
                        case 2: promo_display[i].name.text_load("B", t1.font_piece_get(), piece_white); promo_display[i].the_texture = &bishop_white_obj;break;
                        case 3: promo_display[i].name.text_load("N", t1.font_piece_get(), piece_white); promo_display[i].the_texture = &knight_white_obj;break;
                    }
                }

                inited = true;
            }
            if(modulo)
            {
                start_color = g1.black_color;
            }
            else
            {
                start_color = g1.white_color;
            }
        }
        else
        {
            if(!inited)
            {
                for(int i =0;i<4;i++)
                {
                    promo_display[i].piece_init(BLACK);
                    switch(i)
                    {
                        case 0: promo_display[i].name.text_load("Q", t1.font_piece_get(), piece_black); promo_display[i].the_texture = &queen_black_obj;break;
                        case 1: promo_display[i].name.text_load("R", t1.font_piece_get(), piece_black); promo_display[i].the_texture = &rook_black_obj;break;
                        case 2: promo_display[i].name.text_load("B", t1.font_piece_get(), piece_black); promo_display[i].the_texture = &bishop_black_obj;break;
                        case 3: promo_display[i].name.text_load("N", t1.font_piece_get(), piece_black); promo_display[i].the_texture = &knight_black_obj;break;
                    }
                }

                inited = true;
            }
            if(modulo)
            {
                start_color = g1.white_color;
            }
            else
            {
                start_color = g1.black_color;//g1.white_color;
            }
        }
    }
    else
    {
        if(site)
        {
            if(!inited)
            {
                for(int i =0;i<4;i++)
                {
                    promo_display[i].piece_init(BLACK);
                    switch(i)
                    {
                        case 0: promo_display[i].name.text_load("Q", t1.font_piece_get(), piece_black); promo_display[i].the_texture = &queen_black_obj;break;
                        case 1: promo_display[i].name.text_load("R", t1.font_piece_get(), piece_black); promo_display[i].the_texture = &rook_black_obj;break;
                        case 2: promo_display[i].name.text_load("B", t1.font_piece_get(), piece_black); promo_display[i].the_texture = &bishop_black_obj;break;
                        case 3: promo_display[i].name.text_load("N", t1.font_piece_get(), piece_black); promo_display[i].the_texture = &knight_black_obj;break;
                    }
                }

                inited = true;
            }
            if(modulo)
            {
                start_color = g1.white_color;
            }
            else
            {
                start_color = g1.black_color;//g1.white_color;
            }
        }
        else
        {
            if(!inited)
            {
                for(int i =0;i<4;i++)
                {
                    promo_display[i].piece_init(WHITE);
                    switch(i)
                    {
                        case 0: promo_display[i].name.text_load("Q", t1.font_piece_get(), piece_white); promo_display[i].the_texture = &queen_white_obj;break;
                        case 1: promo_display[i].name.text_load("R", t1.font_piece_get(), piece_white); promo_display[i].the_texture = &rook_white_obj;break;
                        case 2: promo_display[i].name.text_load("B", t1.font_piece_get(), piece_white); promo_display[i].the_texture = &bishop_white_obj;break;
                        case 3: promo_display[i].name.text_load("N", t1.font_piece_get(), piece_white); promo_display[i].the_texture = &knight_white_obj;break;
                    }
                }

                inited = true;
            }
            if(modulo)
            {
                start_color = g1.black_color;//g1.white_color;
            }
            else
            {
                start_color = g1.white_color;
            }
        }
    }
    if(site)
    {
        for(int i =-2;i<=1;i++)
        {
            g1.square_render(i+board_pos.x, -1, start_color);
            promo_display[i+2].position_update({i+board_pos.x, -1});
            Vec2f newpos = g1.board_to_screen(promo_display[i+2].board_pos, promo_display[i+2].name.width_get(), promo_display[i+2].name.height_get());
            promo_display[i+2].name.position_update(newpos);
            promo_display[i+2].render();
            if(start_color==g1.black_color)
                start_color = g1.white_color;
            else
                start_color = g1.black_color;;
        }
    }
    else
    {
        for(int i =-2;i<=1;i++)
        {
            g1.square_render(i+board_pos.x, 8, start_color);

            promo_display[i+2].position_update({i+board_pos.x, 8});
            Vec2f newpos = g1.board_to_screen(promo_display[i+2].board_pos, promo_display[i+2].name.width_get(), promo_display[i+2].name.height_get());
            promo_display[i+2].name.position_update(newpos);
            promo_display[i+2].render();
            if(start_color==g1.white_color)
                start_color = g1.black_color;
            else
                start_color = g1.white_color;
        }
    }

}
