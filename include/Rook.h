#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"

class Rook : public Piece
{
    public:
        Rook();
        virtual ~Rook();
        Rook(const Rook& other) = delete;
        Rook& operator=(const Rook& other) = delete;
        void piece_init(Color c) override;

    protected:

    private:
};

#endif // ROOK_H
