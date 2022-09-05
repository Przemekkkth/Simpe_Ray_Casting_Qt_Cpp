#include "game.h"

float Game::fPlayerX = 14.7f;
float Game::fPlayerY = 5.09f;
float Game::fPlayerA = 0.0f;

QString Game::map;

const QString Game::PATH_TO_SILHOUETTE_PIXMAP = ":/res/silhouette.png";

Game::Game()
{
    map.append("#########.......");
    map.append("#...............");
    map.append("#.......########");
    map.append("#..............#");
    map.append("#......##......#");
    map.append("#......##......#");
    map.append("#..............#");
    map.append("###............#");
    map.append("##.............#");
    map.append("#......####..###");
    map.append("#......#.......#");
    map.append("#......#.......#");
    map.append("#..............#");
    map.append("#......#########");
    map.append("#..............#");
    map.append("################");
}
