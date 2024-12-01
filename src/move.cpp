#include "move.h"

namespace Move
{

void Map::clear()
{
    memset(this->data, 0, sizeof(u64) * 4 * 10);
};

bool Map::get(i8 x, i8 y, Piece::Rotation r)
{
    if (x < 0 || x >= 10 || y < 0 || y >= 40) {
        return true;
    }

    return this->data[static_cast<i8>(r)][x] & (1ULL << y);
};

void Map::set(i8 x, i8 y, Piece::Rotation r)
{
    if (x < 0 || x >= 10 || y < 0 || y >= 40) {
        return;
    }

    this->data[static_cast<i8>(r)][x] |= 1ULL << y;
};

void Map::unset(i8 x, i8 y, Piece::Rotation r)
{
    if (x < 0 || x >= 10 || y < 0 || y >= 40) {
        return;
    }

    this->data[static_cast<i8>(r)][x] &= ~(1ULL << y);
};

bool move_right(Piece::Data& piece, Map& map_collision)
{
    if (!map_collision.get(piece.x + 1, piece.y, piece.r)) {
        piece.x += 1;
        return true;
    }

    return false;
};

bool move_left(Piece::Data& piece, Map& map_collision)
{
    if (!map_collision.get(piece.x - 1, piece.y, piece.r)) {
        piece.x -= 1;
        return true;
    }

    return false;
};

bool move_cw(Piece::Data& piece, Map& map_collision)
{
    Piece::Rotation new_r;

    switch (piece.r)
    {
    case Piece::Rotation::UP:
        new_r = Piece::Rotation::RIGHT;
        break;
    case Piece::Rotation::RIGHT:
        new_r = Piece::Rotation::DOWN;
        break;
    case Piece::Rotation::DOWN:
        new_r = Piece::Rotation::LEFT;
        break;
    case Piece::Rotation::LEFT:
        new_r = Piece::Rotation::UP;
        break;
    }

    return Move::move_rotate(piece, new_r, map_collision);
};

bool move_ccw(Piece::Data& piece, Map& map_collision)
{
    Piece::Rotation new_r;

    switch (piece.r)
    {
    case Piece::Rotation::UP:
        new_r = Piece::Rotation::LEFT;
        break;
    case Piece::Rotation::LEFT:
        new_r = Piece::Rotation::DOWN;
        break;
    case Piece::Rotation::DOWN:
        new_r = Piece::Rotation::RIGHT;
        break;
    case Piece::Rotation::RIGHT:
        new_r = Piece::Rotation::UP;
        break;
    }

    return Move::move_rotate(piece, new_r, map_collision);
};

bool move_rotate(Piece::Data& piece, Piece::Rotation new_r, Map& map_collision)
{
    i8 srs_index = std::min(static_cast<i8>(piece.type), i8(1));

    for (i32 i = 0; i < 5; ++i) {
        i8 offset_x = get_offset_srs_x(srs_index, piece.r, i) - get_offset_srs_x(srs_index, new_r, i);
        i8 offset_y = get_offset_srs_y(srs_index, piece.r, i) - get_offset_srs_y(srs_index, new_r, i);

        if (!map_collision.get(piece.x + offset_x, piece.y + offset_y, new_r)) {
            piece.x += offset_x;
            piece.y += offset_y;
            piece.r = new_r;
            return true;
        }
    }

    return false;
};

void move_drop(Piece::Data& piece, Map& map_collision)
{
    u64 col = map_collision.data[static_cast<i8>(piece.r)][piece.x];

    piece.y = 64 - std::countl_zero(col & ((1ULL << piece.y) - 1));
};

bool is_above_stack(Piece::Data& piece, Map& map_collision)
{
    u64 col = map_collision.data[static_cast<i8>(piece.r)][piece.x];

    return piece.y >= 64 - std::countl_zero(col);
};

std::vector<Piece::Data> generate(Board& board, Piece::Type type)
{
    std::vector<Piece::Data> queue;
    std::vector<Piece::Data> locks;

    queue.reserve(1 << 12);
    locks.reserve(128);

    bool fast_mode = true;
    for (i32 i = 0; i < 10; ++i) {
        if (64 - std::countl_zero(board[i]) > 16) {
            fast_mode = false;
            break;
        }
    }

    Map map_lock = Map();
    Map map_queue = Map();
    Map map_collision = Map();

    for (i8 r = 0; r < 4; ++r) {
        for (i8 i = 0; i < 4; ++i) {
            i8 offset_x = Piece::get_offset_x(type, Piece::Rotation(r), i);
            i8 offset_y = Piece::get_offset_y(type, Piece::Rotation(r), i);

            for (i8 x = 0; x < 10; ++x) {
                u64 c = ~0ULL;

                if (x + offset_x >= 0 && x + offset_x < 10) {
                    c = board.cols[x + offset_x];
                }

                if (offset_y < 0) {
                    c = ~((~c) << (-offset_y));
                }
                else {
                    c = c >> offset_y;
                }

                map_collision.data[r][x] |= c;
            }
        }
    }

    if (fast_mode) {
        for (i8 r = 0; r < 4; ++r) {
            if (type == Piece::Type::O && r > 0) {
                break;
            }

            for (i8 x = 0; x < 10; ++x) {
                if (map_collision.data[r][x] & (1ULL << 19)) {
                    continue;
                }

                Piece::Data location = Piece::Data(x, i8(19), type, Piece::Rotation(r));
                Move::move_drop(location, map_collision);

                expand(location, map_queue, map_collision, true, queue);
                lock(location, map_lock, map_collision, locks);
            }
        }
    }
    else {
        Piece::Data init = Piece::Data(i8(4), i8(19), type, Piece::Rotation::UP);

        if (board.is_colliding(init)) {
            ++init.y;
            if (board.is_colliding(init)) {
                return {};
            }
        }

        queue.push_back(init);
        map_queue.set(init.x, init.y, init.r);
    }

    while (!queue.empty())
    {
        Piece::Data location = queue.back();
        queue.pop_back();

        expand(location, map_queue, map_collision, fast_mode, queue);
        lock(location, map_lock, map_collision, locks);
    }

    return locks;
};

void expand(Piece::Data piece, Map& map_queue, Map& map_collision, bool fast_mode, std::vector<Piece::Data>& queue)
{
    Piece::Data drop = piece;
    Move::move_drop(drop, map_collision);
    if (drop.y != piece.y && !map_queue.get(drop.x, drop.y, drop.r) && !(fast_mode && Move::is_above_stack(drop, map_collision))) {
        map_queue.set(drop.x, drop.y, drop.r);
        queue.push_back(drop);
    }

    Piece::Data right = piece;
    if (Move::move_right(right, map_collision) && !map_queue.get(right.x, right.y, right.r) && !(fast_mode && Move::is_above_stack(right, map_collision))) {
        map_queue.set(right.x, right.y, right.r);
        queue.push_back(right);
    }

    Piece::Data left = piece;
    if (Move::move_left(left, map_collision) && !map_queue.get(left.x, left.y, left.r) && !(fast_mode && Move::is_above_stack(left, map_collision))) {
        map_queue.set(left.x, left.y, left.r);
        queue.push_back(left);
    }

    if (piece.type == Piece::Type::O) {
        return;
    }

    Piece::Data cw = piece;
    if (Move::move_cw(cw, map_collision) && !map_queue.get(cw.x, cw.y, cw.r) && !(fast_mode && Move::is_above_stack(cw, map_collision))) {
        map_queue.set(cw.x, cw.y, cw.r);
        queue.push_back(cw);
    }

    Piece::Data ccw = piece;
    if (Move::move_ccw(ccw, map_collision) && !map_queue.get(ccw.x, ccw.y, ccw.r) && !(fast_mode && Move::is_above_stack(ccw, map_collision))) {
        map_queue.set(ccw.x, ccw.y, ccw.r);
        queue.push_back(ccw);
    }
};

void lock(Piece::Data piece, Map& locks_map, Map& map_collision, std::vector<Piece::Data>& locks)
{
    Move::move_drop(piece, map_collision);

    if (piece.y >= 20) {
        return;
    }

    piece.normalize();
    
    if (!locks_map.get(piece.x, piece.y, piece.r)) {
        locks.push_back(piece);
        locks_map.set(piece.x, piece.y, piece.r);
    }
};

};