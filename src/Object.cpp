#include <SDL3_image/SDL_image.h>

#include "Object.h"
#include "object_init.h"

Object::Object()
{
    texture = nullptr;
    texture_height = 0;
    texture_width = 0;
}
Object::Object(float x, float y, const char* p)
{
    texture_load(p);
    pos.arg_set(x, y);
}
Object::~Object()
{
    destroy();
}

bool Object::texture_load(const char* path)
{
    destroy();

    SDL_Surface* surface = IMG_Load(path);
    if(surface == nullptr )
    {
        SDL_Log("Unable to load image %s! %s\n", path, SDL_GetError());
    }
    else
    {
        texture = SDL_CreateTextureFromSurface( t1.renderer_get(), surface );
        if(texture == nullptr )
        {
            SDL_Log("Unable to create texture from loaded pixels!\n%s", SDL_GetError());
        }
        else
        {
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            texture_width = surface->w;
            texture_height = surface->h;
        }


        SDL_DestroySurface( surface );
    }

    return texture != nullptr;
}

void Object::destroy()
{
    if(texture!=nullptr)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;

        texture_height = 0;
        texture_width = 0;
    }
}



void Object::render(float x, float y, double angle, SDL_FPoint* center, float width, float height)
{
    float w = width > 0.0f ? width : static_cast<float>(texture_width);
    float h = height > 0.0f ? height : static_cast<float>(texture_height);

    SDL_FRect to_render
    {
        x,
        y,
        w,
        h
    };


    SDL_RenderTextureRotated(t1.renderer_get(), texture, nullptr, &to_render, angle, center, SDL_FLIP_NONE);

    position_update({x, y});

}

SDL_Texture* Object::texture_get() const
{
    return texture;
}
int Object::width_get() const
{
    return texture_width;
}
int Object::height_get() const
{
    return texture_height;
}

void Object::position_update(Vec2f update)
{
    pos.arg_set(update.x, update.y);
}
Vec2f Object::position_get() const
{
    return pos;
}

// --- Text class implementations ---
Text::Text()
{
    texture = nullptr;
    texture_width = 0;
    texture_height = 0;

    str = nullptr;
    font = nullptr;
    font_color = {0, 0, 0, 0};
}
Text::~Text()
{
    destroy();
}

bool Text::text_load(const char* strr, TTF_Font* fontt, SDL_Color font_colorr)
{
    destroy();

    if (strr == nullptr || fontt == nullptr)
        return false;

    // SDL_ttf v3: TTF_RenderText_Solid(font, text, length, color)
    SDL_Surface* text_surface = TTF_RenderText_Solid(fontt, strr, 0, font_colorr);
    if(text_surface==nullptr)
    {
        SDL_Log("Unable to render text %s! %s\n", strr, SDL_GetError());
    }
    else
    {
        texture = SDL_CreateTextureFromSurface(t1.renderer_get(), text_surface);
        if(texture == nullptr )
        {
            SDL_Log("Unable to create texture from rendered text!\n%s", SDL_GetError());
        }
        else
        {
            texture_width = text_surface->w;
            texture_height = text_surface->h;
            str = strr;
            font = fontt;
            font_color = font_colorr;
        }

        SDL_DestroySurface(text_surface);
    }
    return texture!=nullptr;
}
void Object::color_mod(Uint8 r, Uint8 g, Uint8 b)
{
    if(texture != nullptr)
        SDL_SetTextureColorMod(texture, r, g, b);
}
int Text::text_cmp(const char* other)
{
    return strcmp(str, other);
}
bool Text::mouse_in(float m_x, float m_y)
{
    if(m_x<pos.x || m_x>pos.x+width_get() || m_y<pos.y || m_y>pos.y+height_get())
        return false;
    return true;
}
