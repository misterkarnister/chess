#include "CPU.h"
#include "object_init.h"
#include <SDL3/SDL.h>
#include <sstream>

CPU::CPU()
{
    the_depth = 0;
    max_depth = 3;
    evalulation = 0.0;
    color_evaluated = WHITE;
}

CPU::~CPU()
{
    //dtor
}
void CPU::engine_start()
{
    switch(g1.turn)
    {
    case WHITE:
        {
            color_evaluated = WHITE;
            break;
        }
    case BLACK:
        {
            color_evaluated = BLACK;
            break;
        }
    }
    for(int i =0;i<64;i++)
    {
        process[i] = g1.board[i];
    }
}
double CPU::eval_single_pos()
{
    double eval = 0.0;
    for(int i =0;i<64;i++)
    {
        if(process[i]!=nullptr)
        {
            if(process[i]->name.text_cmp("p")==0)
                { eval += process[i]->color==WHITE?1.0:-1.0;}
            else if(process[i]->name.text_cmp("R")==0)
            {eval += process[i]->color==WHITE?5.0:-5.0;}
            else if(process[i]->name.text_cmp("N")==0)
            {eval += process[i]->color==WHITE?3.0:-3.0;}
            else if(process[i]->name.text_cmp("B")==0)
            {eval += process[i]->color==WHITE?3.0:-3.0;}
            else if(process[i]->name.text_cmp("Q")==0)
            {eval += process[i]->color==WHITE?9.0:-9.0;}
        }


    }
    evalulation = eval;
    return eval;
}
void CPU::eval_render()
{
    std::stringstream ss;
    ss.str("");
    ss<<"Eval:"<<evalulation;
    Text temp;
    temp.text_load(ss.str().c_str(), t1.font_ui_get(), t1.font_ui_color_get());
    temp.render(
                t1.window_width_get() * 0.75 - temp.width_get() * 0.5,
                t1.window_height_get() * 0.15 - temp.height_get() * 0.5
                );
}
void CPU::full_eval()
{
   double _ = eval_single_pos();
}
