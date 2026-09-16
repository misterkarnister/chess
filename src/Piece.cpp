#include "Piece.h"
#include "object_init.h"
#include "constants.h"
#include "mSDL.h"
#include <iostream>
Piece::Piece()
{
    piece_white = {255, 255, 255, 255};
    piece_black = {128, 0, 0, 255};
    selected = false;
    start_pos = true;
    the_texture = nullptr;
    color = WHITE;
    color_sdl = piece_white;
}
Piece::~Piece() = default;
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

void Piece::valid_squares_render()
{
    if(selected)
    {

        SDL_SetRenderDrawColor(t1.renderer_get(), 74, 153, 57, 180);
        float margin_x = t1.window_width_get() * BOARD_MARGIN;
        float margin_y = t1.window_height_get() * BOARD_MARGIN;
        int len = valid_squares.size();
        for(int i=0;i<len;i++)
        {
        int vx = valid_squares[i].x, vy = valid_squares[i].y;
        if(g1.players_color == BLACK)
        {
            vx = 7 - vx;
            vy = 7 - vy;
        }

        mSDL_Circle to_render =
        {
            .center = {
                static_cast<float>(margin_x + vx * g1.square_dim + 0.5 * g1.square_dim),
                static_cast<float>(margin_y + vy * g1.square_dim + 0.5 * g1.square_dim)
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
    valid_squares.clear();
}

void Piece::position_update(Vec2i upd)
{
    board_pos = {upd.x, upd.y};
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
