/** \author Igor Solny
 *
 * \file Technical.h
 *
 * \brief Technikalia SDL
 */

#ifndef TECHNICAL_H
#define TECHNICAL_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

/** \enum Errors_technical
 *
 *  \brief Błędy przy inicjacji SDL
 */
enum Errors_technical
{
    NO_ERROR = 0x000,
    ERR_SDL_INIT = 0x001,
    ERR_SDL_CREATE_WINDOW = 0x002,
    ERR_SDL_GET_WINDOW_SIZE = 0x003,
    ERR_SDL_CREATE_RENDERER = 0x004,
    ERR_TTF_INIT = 0x005
};


/** \class Technical
 *
 *  \brief Technikalia, głównie SDL
 */
class Technical
{
    public:

        /** \brief Konstruktor - inicjacja właściwości klasy
         *
         *
         */
        Technical();

        /** \brief Destruktor
         *
         *
         */
        ~Technical();



        SDL_Window* window_get() const;

        int window_width_get() const;

        int window_height_get() const;

        SDL_Renderer* renderer_get() const;

        Errors_technical err_code_get() const;


        TTF_Font* font_board_get() const;

        SDL_Color font_board_color_get() const;

        TTF_Font* font_piece_get() const;

        TTF_Font* font_ui_get() const;
        SDL_Color font_ui_color_get() const;

        int fps_target_get() const;

        bool init();
        bool media();
        void close();

    private:
        SDL_Window* window;/**< Wskaźnik do okna gry */
        int window_width;/**< Szerokość okna gry */
        int window_height;/**< Wysokość okna gry */

        SDL_Renderer* renderer;/**< Wskaźnik do renderera okna gry */

        Errors_technical err_code;/**< Błąd przy inicjacji */

        TTF_Font* font_board;
        SDL_Color font_board_color;

        TTF_Font* font_piece;

        TTF_Font* font_ui;
        SDL_Color font_ui_color;

        int fps_target;/**< docelowa liczba fpsów */

};

#endif // TECHNICAL_H
