#include "Mouse.h"

Mouse::Mouse()
{
    position_set(0 ,0);
    clicked = false;
}

Mouse::~Mouse()
{
    //dtor
}
void Mouse::position_set(float x, float y)
{
    position.arg_set(x, y);
}
void Mouse::mouse_handler()
{
    float x,y;
    SDL_GetMouseState(&x, &y);

    position_set(x, y);


    #ifdef DEBUG_MOUSE
    position_print();
    #endif // DEBUG_MOUSE
}
Vec2f Mouse::position_get()
{
    return position;
}
#ifdef DEBUG_MOUSE
void Mouse::position_print()
{
    SDL_Log("MOUSE:X:%0.f Y:%0.f", position.x, position.y);
}
#endif // DEBUG_MOUSE
