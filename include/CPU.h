#ifndef CPU_H
#define CPU_H

#include "structs.h"

class Piece;

class CPU
{
    public:
        CPU();
        virtual ~CPU();
        CPU(const CPU& other) = delete;
        CPU& operator=(const CPU& other) = delete;
        double eval_single_pos();
        void full_eval();
        void engine_start();
        void eval_render();

    protected:

    private:
        double  the_depth;
        double  max_depth;
        double  evalulation;
        Piece*  process[64];
        Color color_evaluated;
};

#endif // CPU_H
