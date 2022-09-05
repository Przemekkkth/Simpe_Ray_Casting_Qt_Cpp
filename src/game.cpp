#include "game.h"

float Game::fPlayerX = 14.7f;
float Game::fPlayerY = 5.09f;
float Game::fPlayerA = 0.0f;

QString Game::map;


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
