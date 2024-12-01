#include "piece.h"
#include "board.h"

namespace Piece
{

bool Data::operator == (Data& other)
{
    return
        this->x == other.x &&
        this->y == other.y &&
        this->r == other.r &&
        this->type == other.type;
};

bool Data::operator == (const Data& other)
{
    return
        this->x == other.x &&
        this->y == other.y &&
        this->r == other.r &&
        this->type == other.type;
};

void Data::normalize()
{
    switch (this->type)
    {
    case Type::I:
        switch (this->r)
        {
        case Rotation::UP:
            break;
        case Rotation::RIGHT:
            break;
        case Rotation::DOWN:
            this->r = Rotation::UP;
            --this->x;
            break;
        case Rotation::LEFT:
            this->r = Rotation::RIGHT;
            ++this->y;
            break;
        default:
            break;
        }
        break;
    case Type::S:
        switch (r)
        {
        case Rotation::UP:
            break;
        case Rotation::RIGHT:
            break;
        case Rotation::DOWN:
            this->r = Rotation::UP;
            --this->y;
            break;
        case Rotation::LEFT:
            this->r = Rotation::RIGHT;
            --this->x;
            break;
        default:
            break;
        }
        break;
    case Type::Z:
        switch (r)
        {
        case Rotation::UP:
            break;
        case Rotation::RIGHT:
            break;
        case Rotation::DOWN:
            this->r = Rotation::UP;
            --this->y;
            break;
        case Rotation::LEFT:
            this->r = Rotation::RIGHT;
            --this->x;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
};

Data Data::get_normalize()
{
    Data copy = *this;
    copy.normalize();
    return copy;
};

}