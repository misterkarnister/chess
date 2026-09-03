#ifndef KING_H
#define KING_H

#include "Piece.h"

class King : public Piece
{
    public:
        King();
        virtual ~King();
        King(const King& other) = delete;
        King& operator=(const King& other) = delete;
        void piece_init(Color c) override;

    protected:

    private:

};

#endif // KING_H
