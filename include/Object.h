/** \author Igor Solny
 *
 * \file Object.h
 *
 * \brief Wszystko co związane z obiektami w grze
 *
 */


#ifndef OBJECT_H
#define OBJECT_H

#include "../include/SDL3/SDL.h"
#include "../include/SDL3_ttf/SDL_ttf.h"

#include "structs.h"
/** \class Object
 *
 *  \brief Klasa obiektu w grze
 */
class Object
{
public:

    /** \brief Konstruktor - inicjacja właściwości klasy
     *
     *
     */
    Object();

    /** \brief Konstruktor z inicjacją tekstury i pozycji
     *
     * \param x float - pozycja x obiektu
     * \param y float - pozycja y obiektu
     * \param p const char* - ścieżka do tekstury obiektu
     *
     */
    Object(float x, float y, const char* p);

    /** \brief Destruktor - dealokacja zasobów + wyzerowanie właściwości
     *
     *
     */
    ~Object();

    /** \brief Ładuje teksturę z pilku
     *
     * \param path const char* - ścieżka do pliku (podajemy linuxowo: "folder/plik")
     * \return bool - prawda jeśli udało się załadować plik, fałsz jeśli się nie udało
     *
     */
    bool texture_load( const char* path );

    /** \brief dealokacja zasobów
     *
     * \return void
     *
     */
    void destroy();

    /** \brief render obiektu na ekran (kordynaty liczone od lewego gornego rogu zawsze, oś y, rośnie w dół okna!)
     *
     * \param x float - współrzędna x okna
     * \param y float - współrzędna y okna
     * \param 0.0 double angle - kąt obrotu
     * \param nullptr SDL_FPoint* center - środek obrotu
     * \return void
     *
     */
    void render( float x, float y, double angle = 0.0, SDL_FPoint* center = nullptr);

    /** \brief getter
     *
     * \return SDL_Texture* - wskaźnik do tekstury
     *
     */
    SDL_Texture* texture_get() const;

    /** \brief getter
     *
     * \return int - szerokość obiektu
     *
     */
    int width_get() const;

    /** \brief getter
     *
     * \return int - wysokość obiektu
     *
     */
    int height_get() const;

    /** \brief update pozycji tekstury
     *
     * \param update Vec2f - wektor pozycji do przypisania
     * \return void
     *
     */
    void position_update(Vec2f update);

    /** \brief getter
     *
     * \return Vec2f - pozycja
     *
     */
    Vec2f position_get() const;

    void color_mod(Uint8 r, Uint8 g, Uint8 b);


   // bool load_check();

protected:
    SDL_Texture* texture;/**< wskaźnik do tekstury */
    int texture_width;/**< szerokość tekstury */
    int texture_height;/**< wysokość tekstury */
    Vec2f pos;/**< pozycja tekstury */
};

/** \class Text
 *
 *  \brief Pochodna klasy Object, określa jak tekst renderuje sie w grze
 */
class Text : public Object
{
public:
    /** \brief Konstruktor
     *
     *
     */
    Text();

    /** \brief Destruktor
     *
     *
     */
    ~Text();

    /** \brief Generowanie tekstu
     *
     * \param strr const char* - napis
     * \param fontt TTF_Font* - czcionka
     * \param font_colorr SDL_Color - kolor czcionki
     * \return bool
     *
     */
    bool text_load(const char* strr, TTF_Font* fontt, SDL_Color font_colorr);

    int text_cmp(const char* other);
    const char* str;/**< aktualny napis */
protected:

private:

    TTF_Font* font;/**< aktualna czcionka */
    SDL_Color font_color;/**< aktualny kolor czcionki */
};


#endif // OBJECT_H
