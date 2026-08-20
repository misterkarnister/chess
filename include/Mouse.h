/** \author Igor Solny
 *
 * \file Mouse.h
 *
 * \brief obsługa sterowania
 */


#ifndef CONTROLS_H
#define CONTROLS_H

#include <SDL3/SDL.h>

#include "structs.h"
//#define DEBUG_MOUSE 1
/** \class Mouse
 *
 *  \brief ogólna klasa obsługi myszki
 */
class Mouse
{
    public:

        /** \brief Konstruktor - inicjacja właściwości
         *
         *
         */
        Mouse();

        ~Mouse();

        /** \brief ustawia pozycje myszki
         *
         * \param x float - współrzędna x
         * \param y float - współrzędna y
         * \return void
         *
         */
        void position_set(float x, float y);

        /** \brief update pozycji
         *
         * \return void
         *
         */
        void mouse_handler(); // celowanie sledzenie myszki

        /** \brief getter
         *
         * \return Vec2f - pozycja myszki
         *
         */
        Vec2f position_get() const;

        /** \brief DEBUG_MOUSE - wypisuje aktualna pozycję myszki
         *
         * \return void
         *
         */
        #ifdef DEBUG_MOUSE
            void position_print();
        #endif

        bool clicked;
    protected:

    private:
        Vec2f position;/**< aktualna pozycja myszki */
};


#endif // CONTROLS_H
