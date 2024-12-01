#include "move.h"

int main()
{
    std::array<Piece::Type, 7> queue {
        // IOTLJSZ
        Piece::Type::I,
        Piece::Type::O,
        Piece::Type::T,
        Piece::Type::L,
        Piece::Type::J,
        Piece::Type::S,
        Piece::Type::Z
    };

    using namespace std;

    Board board;

    auto func = [&] (Board b) {
        const int count = 1000000;

        // For each piece
        for (i8 t = 0; t < 7; ++t) {
            int64_t time = 0;
            int64_t c = 0;

            std::vector<int64_t> lists;
            lists.reserve(1000000);

            for (int i = 0; i < count; ++i) {
                auto time_start = chrono::high_resolution_clock::now();
                auto m = Move::generate(b, queue[t]);
                auto time_stop = chrono::high_resolution_clock::now();

                auto dt = chrono::duration_cast<chrono::nanoseconds>(time_stop - time_start).count();

                c += m.size();
                time += dt;
                lists.push_back(dt);
            }

            // Calculate mean time & movegen count
            time = time / count;
            c = c / count;

            // Calculate stdev
            uint64_t sd = 0;
            uint64_t max = 0;
            uint64_t min = UINT64_MAX;

            for (auto dt : lists) {
                sd += (dt - time) * (dt - time);
                max = std::max(max, uint64_t(dt));
                min = std::min(min, uint64_t(dt));
            }

            sd = sd / count;

            cout << "    piece: " << to_char(queue[t]) << "    time: " << time << " ns" << "    stdev: " << std::sqrt(sd) << "    min: " << min << " ns"  << "    max: " << max << " ns" << "    count: " << c << endl;
        }
    };

    board[9] = 0b00111111;
    board[8] = 0b00111111;
    board[7] = 0b00011111;
    board[6] = 0b00000111;
    board[5] = 0b00000001;
    board[4] = 0b00000000;
    board[3] = 0b00001101;
    board[2] = 0b00011111;
    board[1] = 0b00111111;
    board[0] = 0b11111111;

    cout << "BOARD TSPIN" << endl;
    func(board);

    board[9] = 0b111111111;
    board[8] = 0b111111111;
    board[7] = 0b011111111;
    board[6] = 0b011111111;
    board[5] = 0b000111111;
    board[4] = 0b000100110;
    board[3] = 0b010000001;
    board[2] = 0b011110111;
    board[1] = 0b011111111;
    board[0] = 0b011111111;

    cout << "BOARD DT CANNON" << endl;
    func(board);
    
    board[9] = 0b000011111111;
    board[8] = 0b000011000000;
    board[7] = 0b110011001100;
    board[6] = 0b110011001100;
    board[5] = 0b110011001100;
    board[4] = 0b110011001100;
    board[3] = 0b110011001100;
    board[2] = 0b110000001100;
    board[1] = 0b110000001100;
    board[0] = 0b111111111100;

    cout << "BOARD TERRIBLE" << endl;
    func(board);

    return 0;
};