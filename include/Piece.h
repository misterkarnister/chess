#ifndef PIECE_H
#define PIECE_H
#include "structs.h"
#include "Object.h"
#include <string>
#include <vector>

/** \class Piece
 *
 *  \brief Pure render sprite. All game logic lives in chess_core
 *         (see Game::pos); this class only draws itself and reacts
 *         to the mouse.
 */
class Piece
{
    public:
        Piece();
        virtual ~Piece();
        Piece(const Piece& other) = delete;
        Piece& operator=(const Piece& other) = delete;

        /** \brief sets color, label font color; subclass supplies label/texture */
        virtual void piece_init(Color c);

        bool mouse_in();
        void hover();

        /** \brief renders highlight circles for valid_squares */
        void valid_squares_render();
        void valid_squares_empty();

        void select();
        void unselect();
        void drag();

        void position_update(Vec2i upd);
        void render();
        void cleanup();

        Text name;
        Object* the_texture;
        Color color;
        bool start_pos;
        Vec2i board_pos;
        std::vector<Vec2i> valid_squares;
        SDL_Color piece_white;
        SDL_Color piece_black;
        SDL_Color color_sdl;

    protected:
        bool selected;
};

#endif // PIECE_H
