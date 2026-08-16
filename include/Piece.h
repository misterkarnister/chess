#ifndef PIECE_H
#define PIECE_H
#include "structs.h"
#include "Object.h"
#include <vector>
class Piece
{
    public:
        Piece();
        virtual ~Piece();
        Piece(const Piece& other) = delete;
        Piece& operator=(const Piece& other) = delete;

        virtual void piece_init(Color c);

        virtual void valid_squares_find();

        bool mouse_in();
        void hover();
        void valid_squares_render();
        void valid_squares_empty();
        void valid_squares_remove_checked();

        void select();
        void unselect();
        void drag();
        void snap();
        virtual void move(Vec2i pos);

        bool checking();
        bool checks;

        void promote();

        void position_update(Vec2i upd);
        void render();
        void cleanup();
        Text name;
        Object* the_texture;
        Color color;
        bool start_pos;
        Vec2i board_pos;
        std::vector<Vec2i> valid_squares;
    protected:



        SDL_Color color_sdl;

        bool selected;
        SDL_Color piece_white;
        SDL_Color piece_black;



    private:


};

#endif // PIECE_H
