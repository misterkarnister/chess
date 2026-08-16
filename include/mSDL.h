#ifndef MSDL_H_INCLUDED
#define MSDL_H_INCLUDED

#include <SDL3/SDL.h>
#include <cmath>
#include <vector>

struct mSDL_Circle
{
    SDL_FPoint center;
    double radius;
};

inline void mSDL_RenderCircle(SDL_Renderer* renderer, mSDL_Circle* circ)
{
    int num = circ->radius * 2 + 1;
    SDL_FPoint* y_plus = new SDL_FPoint[num];
    SDL_FPoint* y_minus = new SDL_FPoint[num];

    int index = 0;
    double x = -(circ->radius);

    while(index<num)
    {

        double y_sol = std::sqrt(circ->radius*circ->radius-x*x);

        y_plus[index] = {static_cast<float>(circ->center.x + x), static_cast<float>(circ->center.y + y_sol)};
        y_minus[index] = {static_cast<float>(circ->center.x + x), static_cast<float>(circ->center.y - y_sol)};

        index++;
        x++;
    }

    SDL_RenderLines(renderer, y_plus, num);
    SDL_RenderLines(renderer, y_minus, num);

    delete[] y_plus;
    delete[] y_minus;
}
inline void mSDL_RenderFillCircle(SDL_Renderer* renderer, mSDL_Circle* circ)
{
    int num = circ->radius * 2 + 1;

    int index = 0;
    double x = -(circ->radius);

    while(index<num)
    {

        double y_sol = std::sqrt(circ->radius*circ->radius-x*x);
        double y1 = circ->center.y + y_sol;
        double y2 = circ->center.y - y_sol;

        SDL_RenderLine(renderer, circ->center.x + x, y1, circ->center.x + x, y2);

        index++;
        x++;
    }
}

inline bool operator==(const SDL_Color& lhs, const SDL_Color& rhs)
{
    return lhs.r == rhs.r &&
           lhs.g == rhs.g &&
           lhs.b == rhs.b &&
           lhs.a == rhs.a;
}
inline bool operator!=(const SDL_Color& lhs, const SDL_Color& rhs)
{
    return !(lhs == rhs);
}


#endif // MSDL_H_INCLUDED
