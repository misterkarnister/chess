#include "Piece.h"
#include "object_init.h"
#include <iostream>
#include "mSDL.h"
Piece::Piece()
{
    piece_white = {255, 255, 255, 255};
    piece_black = {128, 0, 0, 255};
    selected = false;
    start_pos = true;
    checks = false;
    the_texture = nullptr;
}

Piece::~Piece()
{
}
void Piece::render()
{
    if(the_texture==nullptr || the_texture==&name)
    {
        name.render(
                name.position_get().x,
                name.position_get().y);
        return;
    }

    Vec2f npos = name.position_get();
    float center_x = npos.x + name.width_get() * 0.5f;
    float center_y = npos.y + name.height_get() * 0.5f;

    float size = static_cast<float>(g1.square_dim);
    the_texture->render(
            center_x - size * 0.5f,
            center_y - size * 0.5f,
            0.0,
            nullptr,
            size,
            size);
}
void Piece::cleanup()
{
    name.destroy();
    if(the_texture != nullptr && the_texture != &name)
        the_texture->destroy();
}
bool Piece::mouse_in()
{
    Vec2f mpos = m1.position_get();
    Vec2f ppos = name.position_get();

    if(the_texture == nullptr || the_texture == &name)
    {
        if(mpos.x<ppos.x || mpos.x>ppos.x + name.width_get() || mpos.y<ppos.y || mpos.y>ppos.y + name.height_get())
        {
            return false;
        }
        return true;
    }

    Vec2f cpos = {ppos.x + name.width_get() * 0.5f, ppos.y + name.height_get() * 0.5f};
    float size = static_cast<float>(g1.square_dim);
    Vec2f tpos = {cpos.x - size * 0.5f, cpos.y - size * 0.5f};

    if(mpos.x<tpos.x || mpos.x>tpos.x + size || mpos.y<tpos.y || mpos.y>tpos.y + size)
    {
        return false;
    }
    return true;
}
void Piece::hover()
{
    if(mouse_in())
    {
        switch(color)
        {
        case WHITE:
            {
                name.color_mod(128, 128, 128);
                break;
            }
        case BLACK:
            {
                name.color_mod(255, 255, 255);
                break;
            }
        }

    }
    else if(!selected)
    {
        switch(color)
        {
        case WHITE:
            {
                name.color_mod(piece_white.r, piece_white.g, piece_white.b);
                break;
            }
        case BLACK:
            {
                name.color_mod(piece_black.r, piece_black.g, piece_black.b);
                break;
            }
        }
    }
}

void Piece::select()
{
    if(!selected)
    {
        switch(color)
        {
        case WHITE:
            {
                name.color_mod(128, 128, 128);
                break;
            }
        case BLACK:
            {
                name.color_mod(255, 255, 255);
                break;
            }
        }
        selected = true;
    }
}
void Piece::unselect()
{
    if(selected)
    {
        switch(color)
        {
        case WHITE:
            {
                name.color_mod(piece_white.r, piece_white.g, piece_white.b);
                break;
            }
        case BLACK:
            {
                name.color_mod(piece_black.r, piece_black.g, piece_black.b);
                break;
            }
        }
        selected = false;
    }
}
void Piece::drag()
{
    if(selected)
    {
        Vec2f mpos = m1.position_get();
        Vec2f ppos = name.position_get();

        name.position_update(ppos.add(mpos.add(ppos.scalar_multiply(-1))));

    }
}
void Piece::snap()
{
    if(selected)
    {
        Vec2f mpos = m1.position_get();
        //Vec2f ppos = name.position_get();
        Vec2f fpos = {static_cast<float>(t1.window_width_get()*0.1), static_cast<float>(t1.window_height_get()*0.1)};

        Vec2f spos = mpos.add(fpos.scalar_multiply(-1));

        Vec2i chk = spos.divide_int(g1.square_dim);


        int len = valid_squares.size();
        bool found = false;
        for(int i =0;i<len;i++)
        {
            if(valid_squares[i].compare(chk))
            {
                for(int i=0;i<8;i++)
                {
                    pawns_black[i].last_double_move = false;
                    pawns_white[i].last_double_move = false;
                }
                move(chk);
                found = true;

            }

        }
        if(!found)
        {
            Vec2f oldpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())};
            name.position_update(oldpos);
        }
        else
        {
            g1.valid_squares_find_all();
        }

        unselect();
    }
}
void Piece::valid_squares_render()
{
    if(selected)
    {

        SDL_SetRenderDrawColor(t1.renderer_get(), 74, 153, 57, 180);
        Vec2f bstart = {static_cast<float>(t1.window_width_get() * 0.1), static_cast<float>(t1.window_height_get()*0.1)};
        int len = valid_squares.size();
        for(int i=0;i<len;i++)
        {

        mSDL_Circle to_render =
        {
            .center = {
                static_cast<float>(bstart.x + valid_squares[i].x * g1.square_dim + 0.5 * g1.square_dim),
                static_cast<float>(bstart.y + valid_squares[i].y * g1.square_dim + 0.5 * g1.square_dim)
            },
            .radius = 0.375 * g1.square_dim
        };

        mSDL_RenderCircle(t1.renderer_get(), &to_render);
        mSDL_RenderFillCircle(t1.renderer_get(), &to_render);
        }
    }

}
void Piece::valid_squares_empty()
{
    int len = valid_squares.size();

    for(int i=0;i<len;i++)
        valid_squares.pop_back();
}
void Piece::position_update(Vec2i upd)
{
    board_pos = {upd.x, upd.y};
}
void Piece::move(Vec2i pos)
{


    g1.board[pos.y*8 + pos.x] = g1.board[board_pos.y*8 + board_pos.x];
    g1.board[board_pos.y*8 + board_pos.x] = nullptr;
    position_update(pos);

    Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())};
    name.position_update(newpos);

    g1.black_valid_squares=0;
    g1.white_valid_squares=0;
    g1.turn==WHITE? g1.turn = BLACK : g1.turn = WHITE;

}
bool Piece::checking()
{
    std::vector<Vec2i> saved = valid_squares;
    valid_squares_find();
    int len = valid_squares.size();

    for(int i =0;i<len;i++)
    {

        if(!g1.square_free(valid_squares[i]) && g1.board[valid_squares[i].y*8+valid_squares[i].x]->name.text_cmp("K")==0 && color!=g1.board[valid_squares[i].y*8+valid_squares[i].x]->color)
        {
            checks = true;
            valid_squares = saved;
            return true;
        }

    }

    valid_squares = saved;
    return false;
}
void Piece::valid_squares_remove_checked()
{
    for(int j=(int)valid_squares.size()-1; j>=0; j--)
    {
        if(!g1.square_free(valid_squares[j]) && g1.board[valid_squares[j].y*8+valid_squares[j].x]->name.text_cmp("K")==0 && g1.board[valid_squares[j].y*8+valid_squares[j].x]->color!=color)
        {
            valid_squares.erase(valid_squares.begin()+j);
        }
    }

    int len = valid_squares.size();
    Vec2i org = board_pos;
    bool is_king = name.text_cmp("K") == 0;

    bool in_check = false;
    for(int k=0; k<64 && !in_check; k++)
    {
        if(g1.board[k]!=nullptr && g1.board[k]->color!=color)
        {
            if(g1.board[k]->checking())
            {
                in_check = true;
                break;
            }
        }
    }
    if(in_check)
    {
        for(int j=len-1; j>=0; j--)
        {
        if(is_king && abs(valid_squares[j].x - org.x) == 2 && valid_squares[j].y == org.y)
        {
            valid_squares.erase(valid_squares.begin()+j);
            len--;
        }
        }
    }

    for(int j=len-1; j>=0; j--)
    {
        int org_index = org.y*8+org.x;
        Piece* preserv = this;
        int index = valid_squares[j].y*8 + valid_squares[j].x;
        Piece* preserv2 = g1.board[index];
        g1.board[index] = g1.board[org_index];
        g1.board[org_index] = nullptr;
        position_update(valid_squares[j]);

        bool is_en_passant = false;
        Piece* en_passant_captured = nullptr;
        int en_passant_captured_index = -1;
        if(name.text_cmp("p") == 0 && preserv2 == nullptr
           && abs(valid_squares[j].x - org.x) == 1 && abs(valid_squares[j].y - org.y) == 1)
        {
            is_en_passant = true;
            if(color == g1.players_color)
                en_passant_captured_index = valid_squares[j].y*8 + 8 + valid_squares[j].x;
            else
                en_passant_captured_index = valid_squares[j].y*8 - 8 + valid_squares[j].x;
            en_passant_captured = g1.board[en_passant_captured_index];
            g1.board[en_passant_captured_index] = nullptr;
        }

        bool is_castle = (is_king && abs(valid_squares[j].x - org.x) == 2
                         && valid_squares[j].y == org.y);
        Vec2i intermediate = {0, 0};
        if(is_castle)
            intermediate = {(org.x + valid_squares[j].x) / 2, org.y};

        for(int k=0; k<64; k++)
        {
            if(g1.board[k]!=nullptr && g1.board[k]->color!=color)
            {
                std::vector<Vec2i> saved = g1.board[k]->valid_squares;
                g1.board[k]->valid_squares_find();

                bool king_attacked = g1.board[k]->checking();
                bool intermediate_attacked = false;
                if(is_castle && !king_attacked)
                {
                    for(int s=0; s<(int)g1.board[k]->valid_squares.size(); s++)
                    {
                        if(g1.board[k]->valid_squares[s].compare(intermediate))
                        {
                            intermediate_attacked = true;
                            break;
                        }
                    }
                }

                g1.board[k]->valid_squares = saved;

                if(king_attacked || intermediate_attacked)
                {
                    valid_squares.erase(valid_squares.begin()+j);
                    len--;
                    break;
                }
            }
        }
        if(is_en_passant)
            g1.board[en_passant_captured_index] = en_passant_captured;
        g1.board[org_index] = preserv;
        g1.board[index] = preserv2;
        g1.board[org_index]->position_update(org);
    }
}

void Piece::piece_init(Color c)
{
    color = c;

    switch(c)
    {
    case WHITE:
        {
            color_sdl= piece_white;
            break;
        }
    case BLACK:
        {

            color_sdl = piece_black;
            break;
        }
    }

}

void Piece::valid_squares_find()
{

}
void Piece::promote()
{
    if(mouse_in()&& m1.clicked)
    {
        static int wht_ind = 0;
        static int blk_ind = 0;
        Pawn* promoter = nullptr;
        for(int i =0;i<8;i++)
        {
            if(pawns_black[i].promoting)
                promoter = &pawns_black[i];
            else if(pawns_white[i].promoting)
                promoter = &pawns_white[i];
        }
        if(promoter!=nullptr)
        {
            Vec2i pos = promoter->board_pos;
            int ind = color==WHITE ? wht_ind++ : blk_ind++;

            Piece* promotion = nullptr;
            if(name.str != nullptr)
            {
                if(color==WHITE)
                {
                    if(name.text_cmp("Q")==0)       { promotion = &promo_queen_white[ind];   promotion->the_texture = &queen_white_obj; }
                    else if(name.text_cmp("R")==0)  { promotion = &promo_rook_white[ind];    promotion->the_texture = &rook_white_obj; }
                    else if(name.text_cmp("B")==0)  { promotion = &promo_bishop_white[ind];  promotion->the_texture = &bishop_white_obj; }
                    else if(name.text_cmp("N")==0)  { promotion = &promo_knight_white[ind];  promotion->the_texture = &knight_white_obj; }
                }
                else
                {
                    if(name.text_cmp("Q")==0)       { promotion = &promo_queen_black[ind];   promotion->the_texture = &queen_black_obj; }
                    else if(name.text_cmp("R")==0)  { promotion = &promo_rook_black[ind];    promotion->the_texture = &rook_black_obj; }
                    else if(name.text_cmp("B")==0)  { promotion = &promo_bishop_black[ind];  promotion->the_texture = &bishop_black_obj; }
                    else if(name.text_cmp("N")==0)  { promotion = &promo_knight_black[ind];  promotion->the_texture = &knight_black_obj; }
                }
            }

            if(promotion!=nullptr)
            {
                promotion->color = color;
                promotion->color_sdl = color==WHITE ? promotion->piece_white : promotion->piece_black;
                promotion->start_pos = false;
                promotion->board_pos = pos;

                g1.board[pos.y*8+pos.x] = promotion;

                promotion->name.text_load(name.str, t1.font_piece_get(), promotion->color_sdl);
                Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (promotion->board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * promotion->name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (promotion->board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * promotion->name.height_get())};
                promotion->name.position_update(newpos);

                promoter->promoting = false;
                promoter->inited = false;

                g1.valid_squares_find_all();
            }
        }

    }

}
