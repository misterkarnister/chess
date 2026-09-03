#ifndef GAME_H
#define GAME_H
#include <SDL3/SDL.h>
#include <string>
#include "structs.h"
#include "chess_core.h"
#include "Piece.h"

/** \class Game
 *
 *  \brief Owns the engine core Position (single source of truth) and
 *         the sprite view. Input events are translated into core Moves,
 *         applied via apply_move(), and the sprites are updated to
 *         mirror the resulting position.
 */
class Game
{
    public:
        Game();
        virtual ~Game();

        Color players_color;
        void board_render();
        void pieces_render();

        void game_init();
        void game_close();
        int square_dim;
        SDL_Color white_color;
        SDL_Color black_color;

        /* ---- engine core state (single source of truth) ---- */
        Position pos;

        /* view map: square index -> sprite */
        Piece* board[64];

        void square_render(int x, int y, SDL_Color col = {0, 0, 0, 255});

        /* ---- input, called from main event loop ---- */
        void input_press();     /* mouse button down  */
        void input_motion();    /* mouse motion       */
        void input_release();   /* mouse button up    */

        /** \brief applies a legal move to the core and syncs sprites */
        bool apply_move(Move m);

        bool checked;
        Color checked_color;

        Game_Mode mode;
        Game_State state;

        void game_judge();
        void game_end_render();

        void main_menu_render();

        Color turn;

        Vec2f board_to_screen(Vec2i bpos, float obj_w, float obj_h);
        void toggle_turn();

        /* promotion chooser state */
        bool promo_pending;
        bool awaiting_cpu;

    protected:

    private:

        void text_generate();
        void letter_render(int column, Color side);
        void digit_render(int row, Color side);

        void build_view();
        void place_sprite(Piece* s);
        Piece* sprite_for_piece(int8_t pc, int& pw, int& pb, int& rw, int& rb,
                                int& nw, int& nb, int& bw, int& bb);
        Piece* acquire_promo_sprite(int8_t promo_code);
        bool square_from_mouse(int& out_sq) const;
        void select_square(int sq);
        void clear_selection();
        void start_promotion(const Move& m);
        void handle_promo_click();
        void cancel_promotion();
        void render_promotion_choices();
        void refresh_check_state();

        Move sel_moves[CORE_MAX_MOVES];
        int  sel_count;
        int  sel_sq;

        int promo_from;
        int promo_to;
        int promo_counts[2][4];   /* used pool slots [color][Q,R,B,N] */
};

#endif // GAME_H
