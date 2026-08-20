/** \author Igor Solny
 *
 * \file structs.h
 *
 * \brief Globalne struktury, enumy, itd. używane w innych plikach
 *
 */


#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

enum Color
{
    WHITE,
    BLACK
};

enum Game_Mode
{
    PVP,
    VS_CPU
};

enum Game_State
{
    PLAYING,
    WHITE_WON,
    BLACK_WON,
    DRAW,
    MAIN_MENU
};

/** \struct Vec2i
 *
 *  \brief Wektor 2 wymiary inty
 */
struct Vec2i
{
    int x;
    int y;

    /** \brief ustawienie structa
     *
     * \param xx int - x
     * \param yy int - y
     * \return void
     *
     */
    void arg_set(int xx, int yy)
    {
        x = xx;
        y = yy;
    }

    /** \brief Konstruktor bazowy - tylko by móc wstawić wektor w klasie
     *
     *
     */
    Vec2i(){};

    /** \brief Konstruktor do tworzenia wektora
     *
     * \param xx int - x
     * \param yy int - y
     *
     */
    Vec2i(int xx, int yy)
    {
        arg_set(xx, yy);
    }

    /** \brief Konstruktor do kopiowania wektora
     *
     * \param p const Vec2i& - wektor
     *
     */
    Vec2i(const Vec2i& p)
    {
        arg_set(p.x, p.y);
    }

    /** \brief Porównanie wektorów
     *
     * \param other const Vec2i& - wektor
     * \return bool - prawda jeśli są takie same, fałsz jeśli nie
     *
     */
    bool compare(const Vec2i& other)
    {
        if(other.x==x && other.y==y)
            return true;
        return false;
    }
    /** \brief Dodawanie wektorów
     *
     * \param other const Vec2i& - wektor
     * \return void
     *
     */
    Vec2i add(const Vec2i& other)
    {
        return Vec2i(x+other.x, y+other.y);
    }

    /** \brief Mnozenie przez skalar
     *
     * \param other double - skalar
     * \return Vec2i
     *
     */
    Vec2i scalar_multiply(double other)
    {
        return Vec2i(x*other, y*other);
    }

    /** \brief Dzielenie calkowite
     *
     * \param div int - dzielnik
     * \return Vec2f
     *
     */
    Vec2i divide_int(int div)
    {
        return Vec2i(x/div, y/div);
    }

    /** \brief Modulo wektoru
     *
     * \param mod int - mod
     * \return Vec2i
     *
     */
    Vec2i modulo(int mod)
    {
        return Vec2i(x%mod, y%mod);
    }
};

/** \struct Vec2f
 *
 *  \brief Wektor 2 wymiary floaty
 */
struct Vec2f
{
    float x;
    float y;

    /** \brief ustawienie wektora
     *
     * \param xx float - x
     * \param yy float - y
     * \return void
     *
     */
    void arg_set(float xx, float yy)
    {
        x = xx;
        y = yy;
    }

    /** \brief Konstruktor bazowy - tylko by móc wstawić wektor w klasie
     *
     *
     */ Vec2f(){};

    /** \brief Konstruktor do tworzenia wektora
     *
     * \param xx float - x
     * \param yy float - y
     *
     */
    Vec2f(float xx, float yy)
    {
        arg_set(xx, yy);
    }

    /** \brief Konstruktor do kopiowania wektora
     *
     * \param p const Vec2f& - wektor
     *
     */
    Vec2f(const Vec2f& p)
    {
        arg_set(p.x, p.y);
    }

    /** \brief Porównanie wektorów
     *
     * \param other const Vec2f& - wektor
     * \return bool - prawda jeśli są takie same, fałsz jeśli nie
     *
     */
    bool compare(const Vec2f& other)
    {
        if(other.x==x && other.y==y)
            return true;
        return false;
    }

    /** \brief Dodanie wektorów
     *
     * \param other const Vec2f& - wektor
     * \return Vec2f
     *
     */
    Vec2f add(const Vec2f& other)
    {
        return Vec2f(x+other.x, y+other.y);
    }

    /** \brief Mnozenie przez skalar
     *
     * \param other double - skalar
     * \return Vec2f
     *
     */
    Vec2f scalar_multiply(double other)
    {
        return Vec2f(x*other, y*other);
    }

    /** \brief Dzielenie calkowite
     *
     * \param div int - dzielnik
     * \return Vec2f
     *
     */
    Vec2i divide_int(int div)
    {
        return Vec2i((int)x/div, (int)y/div);
    }

    /** \brief Modulo wektoru
     *
     * \param mod int - mod
     * \return Vec2f
     *
     */
    Vec2i modulo(int mod)
    {
        return Vec2i((int)x%mod, (int)y%mod);
    }
};


#endif // STRUCTS_H_INCLUDED
