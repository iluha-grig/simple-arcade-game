#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Point
{
    int x;
    int y;
};

enum class MovementDir
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Player
{
    explicit Player(const Point& pos, const Image& ava) :
                    coords(pos), old_coords(coords), avatar(ava) {};
    bool Moved() const;
    void ProcessInput(MovementDir dir);
    void Draw(Image& screen, const Image& background);
    Point GetRoom() const
    {
        return room;
    }
    Point GetRoomCoords() const
    {
        return room_coords;
    }

public:

    Point coords{ .x = 32, .y = 960 };
    Point old_coords{ .x = 32, .y = 960 };
    Image avatar;
    int move_speed = 2;
    Point room{.x = 0, .y = 0};
    Point room_coords{.x = 1, .y = 1};
    bool have_key = false;
};

#endif //MAIN_PLAYER_H
