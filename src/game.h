#ifndef GAME_H
#define GAME_H

#include <QSize>
#include <QString>
#include <QColor>
class Game
{
public:
    Game();
    constexpr static int nMapWidth = 16;
    constexpr static int nMapHeight = 16;
    constexpr static int GRID_SIZE = 45;
    constexpr static const QSize RESOLUTION = QSize(nMapWidth*GRID_SIZE, nMapHeight*GRID_SIZE);
    static float fPlayerX;
    static float fPlayerY;
    static float fPlayerA;
    constexpr static const float fFOV = 3.14159f / 4.0f;
    constexpr static const float fDepth = 16.0f;
    constexpr static const float fSpeed = 0.1f;

    static QString map;

    constexpr static const QColor SKY_COLOR0 = QColor(237, 235, 235);


    constexpr static const QColor SHADE_COLOR0 = QColor(237, 235, 235);
    constexpr static const QColor SHADE_COLOR1 = QColor(186, 184, 184);
    constexpr static const QColor SHADE_COLOR2 = QColor(138, 135, 135);
    constexpr static const QColor SHADE_COLOR3 = QColor( 59,  58,  58);
    constexpr static const QColor SHADE_COLOR4 = QColor(  0,   0,   0);

    constexpr static const QColor GROUND_COLOR0 = QColor(217, 139, 67);
    constexpr static const QColor GROUND_COLOR1 = QColor(161, 108, 59);
    constexpr static const QColor GROUND_COLOR2 = QColor( 97,  74, 53);
    constexpr static const QColor GROUND_COLOR3 = QColor( 36,  31, 26);
    constexpr static const QColor GROUND_COLOR4 = QColor(  0,   0,  0);
};

#endif // GAME_H
