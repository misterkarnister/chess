#include "Game.h"
#include "object_init.h"
#include "constants.h"
#include <string>
#include <cmath>

Game::Game()
{
    square_dim = 64;
    players_color = WHITE;
    white_color = {225, 200, 220, 220};
    black_color = {158, 106, 100, 220};

    for(int i =0;i<64;i++)
    {
        board[i] = nullptr;
    }

    checked = false;
    checked_color = WHITE;

    mode = PVP;
    state = MAIN_MENU;

    turn = WHITE;

    sel_moves[0] = {0, 0, 0, FLAG_NONE};
    sel_count = 0;
    sel_sq = -1;

    promo_pending = false;
    promo_from = -1;
    promo_to = -1;
    awaiting_cpu = false;

    for(int c=0;c<2;c++)
        for(int t=0;t<4;t++)
            promo_counts[c][t] = 0;
}

Game::~Game()
{
    game_close();
}

/* ------------------------------------------------------------------ */
/* rendering (unchanged visuals)                                       */
/* ------------------------------------------------------------------ */

void Game::square_render(int x, int y, SDL_Color col)
{
    SDL_SetRenderDrawColor(t1.renderer_get(), col.r, col.g, col.b, col.a);
    SDL_FRect to_render=
    {
        .x = static_cast<float>(t1.window_width_get() * 0.1 + x * square_dim),
        .y = static_cast<float>(t1.window_height_get() * 0.1 + y * square_dim),
        .w = static_cast<float>(square_dim),
        .h = static_cast<float>(square_dim)
    };

    SDL_RenderRect(t1.renderer_get(), &to_render);
    SDL_RenderFillRect(t1.renderer_get(), &to_render);
}
void Game::board_render()
{
    switch(players_color)
    {
    case WHITE:
        {
            for(int i =0;i<8;i++)
            {

                for(int j=0;j<8;j++)
                {

                    SDL_Color c;
                    if(j%2==0)
                    {
                        if(i%2==0)
                            c = white_color;
                        else
                            c = black_color;
                    }
                    else
                    {
                        if(i%2==0)
                            c = black_color;
                        else
                            c = white_color;
                    }

                    square_render(i, j, c);
                }
            }
            for(int i=0;i<8;i++)
            {
                letter_render(i, WHITE);
                digit_render(i, WHITE);
                letter_render(i, BLACK);
                digit_render(i, BLACK);

            }
            break;
        }
    case BLACK:
        {
            for(int i =0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                {
                    SDL_Color c;
                    if(j%2!=0)
                    {
                        if(i%2==0)
                            c = white_color;
                        else
                            c = black_color;
                    }
                    else
                    {
                        if(i%2==0)
                            c = black_color;
                        else
                            c = white_color;
                    }

                    square_render(i, j, c);
                }
            }
            for(int i=7;i>=0;i--)
            {
                letter_render(i, WHITE);
                digit_render(i, WHITE);
                letter_render(i, BLACK);
                digit_render(i, BLACK);


            }
            break;
        }
    }

}
void Game::text_generate()
{
    const char* lets[] =
    {
        "A", "B", "C", "D", "E", "F", "G", "H"
    };
    for(int i =0;i<8;i++)
    {
        letters_text[i].text_load(lets[i], t1.font_board_get(), t1.font_board_color_get());
        digits_text[i].text_load(std::to_string(i+1).c_str(), t1.font_board_get(), t1.font_board_color_get());
    }
    white_won_text.text_load("White won!", t1.font_ui_get(), t1.font_board_color_get());
    black_won_text.text_load("Black won!", t1.font_ui_get(), t1.font_board_color_get());
    draw_text.text_load("Draw!", t1.font_ui_get(), t1.font_board_color_get());
    pvp_text.text_load("PVP", t1.font_ui_get(), {128, 0, 0, 255});
    cpu_text.text_load("CPU", t1.font_ui_get(), {128, 0, 0, 255});
}

void Game::letter_render(int column, Color side)
{
    float y = 0.0f;
    switch(side)
    {
    case WHITE:
        {
            y = t1.window_height_get() * 0.09 - letters_text[column].height_get() * 0.5;
            break;
        }
    case BLACK:
        {
            y = t1.window_height_get() * 0.09 + 8 * square_dim + letters_text[column].height_get() * 0.5;
            break;
        }
    }
    switch(players_color)
    {
    case WHITE:
        {
            letters_text[column].render(t1.window_width_get() * 0.1 + (column+1) * square_dim - 0.5 * square_dim - 0.5 * letters_text[column].width_get(), y);
            break;
        }
    case BLACK:
        {
            letters_text[7 - column].render(t1.window_width_get() * 0.1 + (column+1) * square_dim - 0.5 * square_dim - 0.5 * letters_text[column].width_get(), y);
            break;
        }
    }

}
void Game::digit_render(int row, Color side)
{
    float x = 0.0f;
    switch(side)
    {
    case WHITE:
        {
            x = t1.window_width_get() * 0.09 - letters_text[row].width_get() * 0.5;
            break;
        }
    case BLACK:
        {
            x = t1.window_width_get() * 0.1 + 8 * square_dim + letters_text[row].width_get();
            break;
        }
    }
    switch(players_color)
    {
    case WHITE:
        {
            digits_text[7-row].render(x, t1.window_height_get() * 0.1 + (row+1) * square_dim - 0.5 * square_dim - 0.5 * letters_text[row].height_get());
            break;
        }
    case BLACK:
        {
            digits_text[row].render(x, t1.window_height_get() * 0.1 + (row+1) * square_dim - 0.5 * square_dim - 0.5 * letters_text[row].height_get());
            break;
        }
    }

}

void Game::game_end_render()
{

    switch(state)
    {
    case BLACK_WON:
        {
            black_won_text.render(
                                    t1.window_width_get() * 0.5 - 0.5 * black_won_text.width_get(),
                                    t1.window_height_get() * 0.5 - 0.5 * black_won_text.height_get()
                                  );
            break;
        }
    case WHITE_WON:
        {
            white_won_text.render(
                                    t1.window_width_get() * 0.5 - 0.5 * white_won_text.width_get(),
                                    t1.window_height_get() * 0.5 - 0.5 * white_won_text.height_get()
                                  );
            break;
        }
    case DRAW:
        {
            draw_text.render(
                                    t1.window_width_get() * 0.5 - 0.5 * draw_text.width_get(),
                                    t1.window_height_get() * 0.5 - 0.5 * draw_text.height_get()
                                  );
            break;
        }
    default:
        break;
    }
}

Vec2f Game::board_to_screen(Vec2i bpos, float obj_w, float obj_h)
{
    float x = t1.window_width_get() * BOARD_MARGIN + (bpos.x + 1) * square_dim - 0.5f * square_dim - 0.5f * obj_w;
    float y = t1.window_height_get() * BOARD_MARGIN + (bpos.y + 1) * square_dim - 0.5f * square_dim - 0.5f * obj_h;
    return {x, y};
}

void Game::toggle_turn()
{
    turn = (turn == WHITE) ? BLACK : WHITE;
}
void Game::main_menu_render()
{
    if(pvp_text.mouse_in(m1.position_get().x, m1.position_get().y))
        pvp_text.color_mod(127, 127, 127);
    else
        pvp_text.color_mod(255, 255, 255);
    if(cpu_text.mouse_in(m1.position_get().x, m1.position_get().y))
        cpu_text.color_mod(127, 127, 127);
    else
        cpu_text.color_mod(255, 255, 255);
    pvp_text.render(
                    t1.window_width_get() * 0.75 - pvp_text.width_get() * 0.5,
                    t1.window_height_get() * 0.15 - pvp_text.height_get() * 0.5
                    );
    cpu_text.render(
                    t1.window_width_get() * 0.75 - cpu_text.width_get() * 0.5,
                    t1.window_height_get() * 0.35 - cpu_text.height_get() * 0.5
                    );

}

/* ------------------------------------------------------------------ */
/* setup / teardown                                                    */
/* ------------------------------------------------------------------ */

void Game::game_init()
{
    core::position_init(pos);
    turn = (Color)pos.side_to_move;

    checked = false;
    checked_color = WHITE;

    sel_sq = -1;
    sel_count = 0;
    promo_pending = false;
    promo_from = -1;
    promo_to = -1;
    awaiting_cpu = false;

    for(int c=0;c<2;c++)
        for(int t=0;t<4;t++)
            promo_counts[c][t] = 0;

    text_generate();

    for(int i =0;i<8;i++)
    {
        pawns_white[i].piece_init(WHITE);
        pawns_black[i].piece_init(BLACK);

    }

    for(int i=0;i<2;i++)
    {
        rooks_white[i].piece_init(WHITE);
        rooks_black[i].piece_init(BLACK);
        knights_white[i].piece_init(WHITE);
        knights_black[i].piece_init(BLACK);
        bishops_white[i].piece_init(WHITE);
        bishops_black[i].piece_init(BLACK);
    }

    queen_white.piece_init(WHITE);
    queen_black.piece_init(BLACK);

    king_white.piece_init(WHITE);
    king_black.piece_init(BLACK);

    build_view();
}

void Game::game_close()
{
    for(int i = 0; i<8;i++)
    {
        letters_text[i].destroy();
        digits_text[i].destroy();
        pawns_white[i].cleanup();
        pawns_black[i].cleanup();

    }
    for(int i=0;i<2;i++)
    {
        rooks_white[i].cleanup();
        rooks_black[i].cleanup();
        knights_white[i].cleanup();
        knights_black[i].cleanup();
        bishops_white[i].cleanup();
        bishops_black[i].cleanup();
    }

    queen_white.cleanup();
    queen_black.cleanup();
    king_white.cleanup();
    king_black.cleanup();

    for(int i = 0; i<8;i++)
    {
        promo_queen_white[i].cleanup();
        promo_queen_black[i].cleanup();
        promo_rook_white[i].cleanup();
        promo_rook_black[i].cleanup();
        promo_bishop_white[i].cleanup();
        promo_bishop_black[i].cleanup();
        promo_knight_white[i].cleanup();
        promo_knight_black[i].cleanup();
    }

    for(int i = 0; i<4;i++)
        promo_display[i].cleanup();

    white_won_text.destroy();
    black_won_text.destroy();
    draw_text.destroy();

    pvp_text.destroy();
    cpu_text.destroy();
}

/* ------------------------------------------------------------------ */
/* view syncing                                                        */
/* ------------------------------------------------------------------ */

Piece* Game::sprite_for_piece(int8_t pc, int& pw, int& pb, int& rw, int& rb,
                              int& nw, int& nb, int& bw, int& bb)
{
    switch(pc)
    {
    case WPAWN:   return &pawns_white[pw++];
    case BPAWN:   return &pawns_black[pb++];
    case WROOK:   return &rooks_white[rw++];
    case BROOK:   return &rooks_black[rb++];
    case WKNIGHT: return &knights_white[nw++];
    case BKNIGHT: return &knights_black[nb++];
    case WBISHOP: return &bishops_white[bw++];
    case BBISHOP: return &bishops_black[bb++];
    case WQUEEN:  return &queen_white;
    case BQUEEN:  return &queen_black;
    case WKING:   return &king_white;
    case BKING:   return &king_black;
    default:      return nullptr;
    }
}

void Game::place_sprite(Piece* s)
{
    if(s == nullptr)
        return;
    Vec2f newpos = board_to_screen(s->board_pos, s->name.width_get(), s->name.height_get());
    s->name.position_update(newpos);
}

void Game::build_view()
{
    for(int i=0;i<64;i++)
        board[i] = nullptr;

    int pw=0, pb=0, rw=0, rb=0, nw=0, nb=0, bw=0, bb=0;
    for(int sq=0; sq<64; sq++)
    {
        Piece* s = sprite_for_piece(pos.board[sq], pw, pb, rw, rb, nw, nb, bw, bb);
        if(s == nullptr)
            continue;
        board[sq] = s;
        s->position_update({sq % 8, sq / 8});
        s->valid_squares_empty();
        s->unselect();
        place_sprite(s);
    }
}

/* ------------------------------------------------------------------ */
/* input                                                               */
/* ------------------------------------------------------------------ */

bool Game::square_from_mouse(int& out_sq) const
{
    Vec2f mp = m1.position_get();
    float ox = t1.window_width_get() * BOARD_MARGIN;
    float oy = t1.window_height_get() * BOARD_MARGIN;

    const int x = static_cast<int>(std::floor((mp.x - ox) / square_dim));
    const int y = static_cast<int>(std::floor((mp.y - oy) / square_dim));

    if(x < 0 || x > 7 || y < 0 || y > 7)
        return false;
    out_sq = y*8 + x;
    return true;
}

void Game::clear_selection()
{
    if(sel_sq >= 0 && sel_sq < 64 && board[sel_sq] != nullptr)
    {
        board[sel_sq]->unselect();
        board[sel_sq]->valid_squares_empty();
    }
    sel_sq = -1;
    sel_count = 0;
}

void Game::select_square(int sq)
{
    clear_selection();
    if(board[sq] == nullptr)
        return;

    sel_sq = sq;
    board[sq]->select();

    Move_List ml;
    core::generate_moves(pos, ml);
    for(int i=0; i<ml.count; i++)
    {
        if(ml.moves[i].from != (uint8_t)sq)
            continue;
        sel_moves[sel_count++] = ml.moves[i];
        board[sq]->valid_squares.push_back({ml.moves[i].to % 8, ml.moves[i].to / 8});
    }
}

void Game::input_press()
{
    if(state != PLAYING)
        return;
    if(mode == VS_CPU && pos.side_to_move != players_color)
        return;

    if(promo_pending)
    {
        handle_promo_click();
        return;
    }

    int sq;
    if(!square_from_mouse(sq))
    {
        clear_selection();
        return;
    }
    if(board[sq] == nullptr || board[sq]->color != pos.side_to_move)
    {
        clear_selection();
        return;
    }
    select_square(sq);
}

void Game::input_motion()
{
    if(state != PLAYING)
        return;
    if(sel_sq >= 0 && board[sel_sq] != nullptr)
        board[sel_sq]->drag();
}

void Game::input_release()
{
    if(state != PLAYING)
        return;
    if(promo_pending)
        return;
    if(sel_sq < 0 || board[sel_sq] == nullptr)
    {
        sel_sq = -1;
        sel_count = 0;
        return;
    }

    const int from = sel_sq;
    Move chosen = {0, 0, 0, FLAG_NONE};
    bool found = false;

    int to;
    if(square_from_mouse(to))
    {
        for(int i=0; i<sel_count; i++)
        {
            if(sel_moves[i].to == (uint8_t)to)
            {
                chosen = sel_moves[i];
                found = true;
                break;
            }
        }
    }

    Piece* sp = board[from];
    sp->unselect();
    sp->valid_squares_empty();
    sel_sq = -1;
    sel_count = 0;

    if(!found)
    {
        place_sprite(sp);   /* snap back */
        return;
    }
    if(chosen.promo != 0)
    {
        start_promotion(chosen);
        return;
    }
    apply_move(chosen);
}

/* ------------------------------------------------------------------ */
/* applying moves                                                      */
/* ------------------------------------------------------------------ */

Piece* Game::acquire_promo_sprite(int8_t promo_code)
{
    const Color c = (promo_code > 0) ? WHITE : BLACK;
    const int t = (promo_code < 0) ? -promo_code : promo_code;

    Piece* s = nullptr;
    Object* tex = nullptr;
    const char* letter = "?";
    int slot = 0;

    switch(t)
    {
    case TYPE_QUEEN:
        slot = promo_counts[c][0]++;
        s = (c==WHITE) ? &promo_queen_white[slot] : &promo_queen_black[slot];
        tex = (c==WHITE) ? &queen_white_obj : &queen_black_obj;
        letter = "Q";
        break;
    case TYPE_ROOK:
        slot = promo_counts[c][1]++;
        s = (c==WHITE) ? &promo_rook_white[slot] : &promo_rook_black[slot];
        tex = (c==WHITE) ? &rook_white_obj : &rook_black_obj;
        letter = "R";
        break;
    case TYPE_BISHOP:
        slot = promo_counts[c][2]++;
        s = (c==WHITE) ? &promo_bishop_white[slot] : &promo_bishop_black[slot];
        tex = (c==WHITE) ? &bishop_white_obj : &bishop_black_obj;
        letter = "B";
        break;
    case TYPE_KNIGHT:
        slot = promo_counts[c][3]++;
        s = (c==WHITE) ? &promo_knight_white[slot] : &promo_knight_black[slot];
        tex = (c==WHITE) ? &knight_white_obj : &knight_black_obj;
        letter = "N";
        break;
    default:
        return nullptr;
    }

    if(slot >= 8)
        return nullptr;   /* pool exhausted; cannot happen in legal chess */

    s->color = c;
    s->color_sdl = (c == WHITE) ? s->piece_white : s->piece_black;
    s->the_texture = tex;
    s->start_pos = false;
    s->name.text_load(letter, t1.font_piece_get(), s->color_sdl);
    return s;
}

bool Game::apply_move(Move m)
{
    const int mover_color = pos.side_to_move;

    Position next;
    core::make_move(pos, next, m);
    pos = next;
    turn = (Color)pos.side_to_move;

    /* ---- mirror the move onto the sprite view ---- */
    Piece* mover = board[m.from];
    board[m.from] = nullptr;

    if(m.flags & FLAG_EP)
    {
        const int capsq = (mover_color == WHITE) ? m.to + 8 : m.to - 8;
        board[capsq] = nullptr;
    }

    if(m.promo != 0)
    {
        Piece* promoted = acquire_promo_sprite(m.promo);
        if(promoted != nullptr)
        {
            promoted->position_update({m.to % 8, m.to / 8});
            board[m.to] = promoted;
        }
    }
    else if(mover != nullptr)
    {
        mover->position_update({m.to % 8, m.to / 8});
        board[m.to] = mover;
    }

    if(m.flags & FLAG_CASTLE)
    {
        if(m.to > m.from)   /* kingside: h-rook -> f-file */
        {
            board[m.from+1] = board[m.from+3];
            board[m.from+3] = nullptr;
            if(board[m.from+1])
            {
                board[m.from+1]->position_update({(m.from+1) % 8, (m.from+1) / 8});
                place_sprite(board[m.from+1]);
            }
        }
        else                /* queenside: a-rook -> d-file */
        {
            board[m.from-1] = board[m.from-4];
            board[m.from-4] = nullptr;
            if(board[m.from-1])
            {
                board[m.from-1]->position_update({(m.from-1) % 8, (m.from-1) / 8});
                place_sprite(board[m.from-1]);
            }
        }
    }

    place_sprite(board[m.to]);

    clear_selection();
    refresh_check_state();
    game_judge();

    /* live eval readout in PVP (VS_CPU refreshes via its own search) */
    if(mode == PVP)
    {
        c1.engine_start();
        c1.full_eval();
    }

    if(mode == VS_CPU && state == PLAYING && pos.side_to_move != players_color)
        awaiting_cpu = true;

    return true;
}

void Game::refresh_check_state()
{
    checked = core::in_check_color(pos, pos.side_to_move);
    checked_color = (Color)pos.side_to_move;
}

void Game::game_judge()
{
    if(state != PLAYING)
        return;

    Move_List ml;
    core::generate_moves(pos, ml);

    if(ml.count == 0)
    {
        if(core::in_check(pos))
            state = (pos.side_to_move == WHITE) ? BLACK_WON : WHITE_WON;
        else
            state = DRAW;
    }
}

/* ------------------------------------------------------------------ */
/* promotion chooser                                                   */
/* ------------------------------------------------------------------ */

void Game::start_promotion(const Move& m)
{
    promo_pending = true;
    promo_from = m.from;
    promo_to = m.to;

    if(board[promo_from] != nullptr)
        place_sprite(board[promo_from]);   /* snap pawn home visually */

    static const char* letters[4] = {"Q", "R", "B", "N"};
    Object* tex_w[4] = {&queen_white_obj, &rook_white_obj, &bishop_white_obj, &knight_white_obj};
    Object* tex_b[4] = {&queen_black_obj, &rook_black_obj, &bishop_black_obj, &knight_black_obj};

    const Color c = (Color)pos.side_to_move;
    for(int i=0; i<4; i++)
    {
        Piece& d = promo_display[i];
        d.piece_init(c);
        d.the_texture = (c == WHITE) ? tex_w[i] : tex_b[i];
        d.name.text_load(letters[i], t1.font_piece_get(), d.color_sdl);
        d.start_pos = false;
    }
}

void Game::cancel_promotion()
{
    promo_pending = false;
}

void Game::handle_promo_click()
{
    Vec2f mp = m1.position_get();
    float ox = t1.window_width_get() * BOARD_MARGIN;
    float oy = t1.window_height_get() * BOARD_MARGIN;

    const int col = static_cast<int>(std::floor((mp.x - ox) / square_dim));
    const int row = static_cast<int>(std::floor((mp.y - oy) / square_dim));

    const int py = (promo_to / 8 == 0) ? -1 : 8;
    static const int8_t types[4] = {TYPE_QUEEN, TYPE_ROOK, TYPE_BISHOP, TYPE_KNIGHT};

    for(int i=0; i<4; i++)
    {
        const int xi = promo_to % 8 - 2 + i;
        if(xi >= 0 && xi <= 7 && col == xi && row == py)
        {
            const int8_t code = (pos.side_to_move == WHITE)
                              ? types[i]
                              : (int8_t)(-(int)types[i]);
            Move m = {(uint8_t)promo_from, (uint8_t)promo_to, code, FLAG_NONE};
            promo_pending = false;
            apply_move(m);
            return;
        }
    }
    cancel_promotion();
}

void Game::render_promotion_choices()
{
    if(!promo_pending)
        return;

    const int py = (promo_to / 8 == 0) ? -1 : 8;
    for(int i=0; i<4; i++)
    {
        const int x = promo_to % 8 - 2 + i;
        /* checker colors continue the (col+row)%2 pattern of the board;
         * comparing == 0 makes negative-modulo signs irrelevant */
        const SDL_Color cc = (((x + py) % 2 == 0)) ? white_color : black_color;
        square_render(x, py, cc);

        promo_display[i].position_update({x, py});
        place_sprite(&promo_display[i]);
        promo_display[i].hover();
        promo_display[i].render();
    }
}

/* ------------------------------------------------------------------ */
/* frame render                                                        */
/* ------------------------------------------------------------------ */

void Game::pieces_render()
{
    for(int i=0;i<64;i++)
    {
        if(board[i]!=nullptr)
        {
            board[i]->valid_squares_render();
        }
    }
    for(int i=0;i<64;i++)
    {
        if(board[i]!=nullptr)
        {
            board[i]->render();
            board[i]->hover();
        }
    }
    render_promotion_choices();
}
