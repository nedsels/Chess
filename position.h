#ifndef POSITION_H
#define POSITION_H

#include <string>

class Position {
public:
    Position();
    Position(char col, int row);
    Position(char col, char row);
    explicit Position(std::string pos);

    char col;
    int row;

    bool isValid();

private:
    bool validPos(char col, int row);
    bool validPos(std::string pos);
};

namespace std {
template <>
struct std::hash<Position> {
    std::size_t operator()(const Position& key) const
    {
        std::string pos;
        pos.push_back(key.col);
        pos.push_back('0' + key.row);

        return std::hash<std::string>()(pos);
    }
};
} // namespace std

inline bool operator==(Position const& a, Position const& b)
{
    return a.col == b.col && a.row == b.row;
}

inline bool operator!=(Position const& a, Position const& b)
{
    return a.col != b.col || a.row != b.row;
}

inline bool operator<(Position const& a, Position const& b)
{
    if (a.col == b.col) {
        return a.row < b.row;
    } else {
        return a.col < b.col;
    }
}

#endif
