#include "gamescene.h"
#include <QKeyEvent>
#include <QDebug>
#include <vector>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent), m_loopSpeed(int(1000.0f/60.0f)), m_loopTime(0.0f), m_deltaTime(0.0f),
      m_leftPressed(false), m_rightPressed(false), m_forwardPressed(false), m_backwardPressed(false)
{
    setSceneRect(0,0, Game::RESOLUTION.width(), Game::RESOLUTION.height());
    connect(&m_timer, &QTimer::timeout, this, &GameScene::loop);
    m_elapsedTimer.start();
    m_timer.start(int(1000.0f/60.0f));
    setBackgroundBrush(QBrush(Qt::black));
}

void GameScene::loop()
{
    m_deltaTime = m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();

    m_loopTime += m_deltaTime;
    if( m_loopTime > m_loopSpeed)
    {
        m_loopTime -= m_loopSpeed;

        handlePlayerInput();
        qDebug() << "X " << Game::fPlayerX << " Y " << Game::fPlayerY << " A " << Game::fPlayerA;

        QImage image = QImage(m_game.RESOLUTION.width(), m_game.RESOLUTION.height(), QImage::Format_RGB32);

        for(int x = 0; x < Game::RESOLUTION.width(); x += 1)
        {
            // For each column, calculate the projected ray angle into world space
            float fRayAngle = (Game::fPlayerA - Game::fFOV/2.0f) + ((float)x / (float)Game::RESOLUTION.width()) * Game::fFOV;

            // Find distance to wall
            float fStepSize = 0.1f;		  // Increment size for ray casting, decrease to increase
            float fDistanceToWall = 0.0f; //                                      resolution

            bool bHitWall = false;		// Set when ray hits wall block
            bool bBoundary = false;		// Set when ray hits boundary between two wall blocks

            float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
            float fEyeY = cosf(fRayAngle);

            // Incrementally cast ray from player, along ray angle, testing for
            // intersection with a block
            while (!bHitWall && fDistanceToWall < Game::fDepth)
            {
                fDistanceToWall += fStepSize;
                int nTestX = (int)(Game::fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(Game::fPlayerY + fEyeY * fDistanceToWall);

                // Test if ray is out of bounds
                if (nTestX < 0 || nTestX >= Game::nMapWidth || nTestY < 0 || nTestY >= Game::nMapHeight)
                {
                    bHitWall = true;			// Just set distance to maximum depth
                    fDistanceToWall = Game::fDepth;
                }
                else
                {
                    // Ray is inbounds so test to see if the ray cell is a wall block
                    if (Game::map[nTestX * Game::nMapWidth + nTestY] == '#')
                    {
                        // Ray has hit wall
                        bHitWall = true;

                        // To highlight tile boundaries, cast a ray from each corner
                        // of the tile, to the player. The more coincident this ray
                        // is to the rendering ray, the closer we are to a tile
                        // boundary, which we'll shade to add detail to the walls
                        std::vector<std::pair<float, float>> p;

                        // Test each corner of hit tile, storing the distance from
                        // the player, and the calculated dot product of the two rays
                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++)
                            {
                                // Angle of corner to eye
                                float vy = (float)nTestY + ty - Game::fPlayerY;
                                float vx = (float)nTestX + tx - Game::fPlayerX;
                                float d = sqrt(vx*vx + vy*vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(std::make_pair(d, dot));
                            }

                        // Sort Pairs from closest to farthest
                        sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });

                        // First two/three are closest (we will never see all four)
                        float fBound = 0.01;
                        if (acos(p.at(0).second) < fBound) bBoundary = true;
                        if (acos(p.at(1).second) < fBound) bBoundary = true;
                        if (acos(p.at(2).second) < fBound) bBoundary = true;
                    }
                }
            }

            // Calculate distance to ceiling and floor
            int nCeiling = (float)(Game::RESOLUTION.width()/2.0) - Game::RESOLUTION.height() / ((float)fDistanceToWall);
            int nFloor = Game::RESOLUTION.height()  - nCeiling;

            // Shader walls based on distance
            QColor nShade = Game::SHADE_COLOR4;
            if (fDistanceToWall <= Game::fDepth / 4.0f)			nShade = Game::SHADE_COLOR0;	// Very close
            else if (fDistanceToWall < Game::fDepth / 3.0f)		nShade = Game::SHADE_COLOR1;
            else if (fDistanceToWall < Game::fDepth / 2.0f)		nShade = Game::SHADE_COLOR2;
            else if (fDistanceToWall < Game::fDepth)				nShade = Game::SHADE_COLOR3;
            else											nShade = Game::SHADE_COLOR4;		// Too far away

            if (bBoundary)		nShade = Game::SHADE_COLOR4; // Black it out


           for (int y = 0; y < Game::RESOLUTION.height(); y+=1)
            {
                // Each Row
                if(y <= nCeiling)
                {
                    image.setPixelColor(x, y, QColor(Qt::blue));
                }
                else if(y > nCeiling && y <= nFloor)
                {
                    image.setPixelColor(x, y, nShade);
                }
                else // Floor
                {
                    // Shade floor based on distance
                    float b = 1.0f - (((float)y -Game::RESOLUTION.height()/2.0f) / ((float)Game::RESOLUTION.height() / 2.0f));
                    if (b < 0.25)		nShade = Game::GROUND_COLOR0;
                    else if (b < 0.5)	nShade = Game::GROUND_COLOR1;
                    else if (b < 0.75)	nShade = Game::GROUND_COLOR2;
                    else if (b < 0.9)	nShade = Game::GROUND_COLOR3;
                    else				nShade = Game::GROUND_COLOR4;
                    image.setPixelColor(x, y, nShade);
                }
            }
        }

        clear();
        QGraphicsPixmapItem* pItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        pItem->setPos(0,0);
        addItem(pItem);
    }
}

void GameScene::handlePlayerInput()
{
    if(m_forwardPressed)
    {
        Game::fPlayerX += sinf(Game::fPlayerA) * Game::fSpeed;
        Game::fPlayerY += cosf(Game::fPlayerA) * Game::fSpeed;
        if (Game::map[(int)Game::fPlayerX * Game::nMapWidth + (int)Game::fPlayerY] == '#')
        {
            Game::fPlayerX -= sinf(Game::fPlayerA) * Game::fSpeed;
            Game::fPlayerY -= cosf(Game::fPlayerA) * Game::fSpeed;
        }
    }
    else if(m_backwardPressed)
    {
        Game::fPlayerX -= sinf(Game::fPlayerA) * Game::fSpeed;
        Game::fPlayerY -= cosf(Game::fPlayerA) * Game::fSpeed;
        if (Game::map[(int)Game::fPlayerX * Game::nMapWidth + (int)Game::fPlayerY] == '#')
        {
            Game::fPlayerX += sinf(Game::fPlayerA) * Game::fSpeed;
            Game::fPlayerY += cosf(Game::fPlayerA) * Game::fSpeed;
        }
    }

    if(m_leftPressed)
    {
        Game::fPlayerA -= (Game::fSpeed * 0.75f);
    }
    else if(m_rightPressed)
    {
        Game::fPlayerA += (Game::fSpeed * 0.75f);
    }
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    if(true)
    switch(event->key())
    {
    case Qt::Key_W:
    {
        m_forwardPressed = true;
    }
        break;
    case Qt::Key_S:
    {
        m_backwardPressed = true;
    }
        break;
    case Qt::Key_A:
    {
        m_leftPressed = true;
    }
        break;
    case Qt::Key_D:
    {
        m_rightPressed = true;
    }
        break;
    }

    QGraphicsScene::keyPressEvent(event);
}

void GameScene::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_W:
    {
        m_forwardPressed = false;
    }
        break;
    case Qt::Key_S:
    {
        m_backwardPressed = false;
    }
        break;
    case Qt::Key_A:
    {
        m_leftPressed = false;
    }
        break;
    case Qt::Key_D:
    {
        m_rightPressed = false;
    }
        break;
    }
    QGraphicsScene::keyReleaseEvent(event);
}
