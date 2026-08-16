#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"


class Bishop : public Piece
{
    public:
        Bishop();
        virtual ~Bishop();
        Bishop(const Bishop& other) = delete;
        Bishop& operator=(const Bishop& other) = delete;
        void piece_init(Color c) override;
        void valid_squares_find() override;

    protected:

    private:
};

#endif // BISHOP_H
