#include "gamescene.h"
#include <QKeyEvent>
#include <QDebug>
#include <vector>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QDir>
#include <QPainter>

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

        QImage image = QImage(m_game.RESOLUTION.width(), m_game.RESOLUTION.height(), QImage::Format_RGB32);

        for(int x = 0; x < Game::RESOLUTION.width(); x += 1)
        {
            float fRayAngle = (Game::fPlayerA - Game::fFOV/2.0f) + ((float)x / (float)Game::RESOLUTION.width()) * Game::fFOV;

            float fStepSize = 0.1f;
            float fDistanceToWall = 0.0f; //

            bool bHitWall = false;
            bool bBoundary = false;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < Game::fDepth)
            {
                fDistanceToWall += fStepSize;
                int nTestX = (int)(Game::fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(Game::fPlayerY + fEyeY * fDistanceToWall);

                if (nTestX < 0 || nTestX >= Game::nMapWidth || nTestY < 0 || nTestY >= Game::nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = Game::fDepth;
                }
                else
                {
                    if (Game::map[nTestX * Game::nMapWidth + nTestY] == '#')
                    {
                        bHitWall = true;
                        std::vector<std::pair<float, float>> p;

                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float vy = (float)nTestY + ty - Game::fPlayerY;
                                float vx = (float)nTestX + tx - Game::fPlayerX;
                                float d = sqrt(vx*vx + vy*vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(std::make_pair(d, dot));
                            }

                        sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });

                        float fBound = 0.01;
                        if (acos(p.at(0).second) < fBound) bBoundary = true;
                        if (acos(p.at(1).second) < fBound) bBoundary = true;
                        if (acos(p.at(2).second) < fBound) bBoundary = true;
                    }
                }
            }

            int nCeiling = (float)(Game::RESOLUTION.width()/2.0) - Game::RESOLUTION.height() / ((float)fDistanceToWall);
            int nFloor = Game::RESOLUTION.height()  - nCeiling;

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

        drawMap();
        drawSilhouette();
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

void GameScene::drawMap()
{
    QImage image = QImage(Game::nMapWidth, Game::nMapHeight, QImage::Format_RGB32);
    for(int x = 0; x < Game::nMapWidth; ++x)
    {
        for(int y = 0; y < Game::nMapHeight; ++y)
        {
            if(Game::map[y*Game::nMapWidth + x] == '#')
            {
                image.setPixelColor(x,y, QColor(Qt::red));
            }
            else
            {
                image.setPixelColor(x,y, QColor(Qt::yellow));
            }
        }
    }
    image.setPixelColor( (int)Game::fPlayerY, (int)Game::fPlayerX, QColor(Qt::green));
    QGraphicsPixmapItem* pItem = new QGraphicsPixmapItem(QPixmap::fromImage(image.scaled(112,112)));
    pItem->setPos(0, 0);
    addItem(pItem);
}

void GameScene::drawSilhouette()
{
    QGraphicsPixmapItem* pItem = new QGraphicsPixmapItem(QPixmap(Game::PATH_TO_SILHOUETTE_PIXMAP));
    addItem(pItem);
}

void GameScene::renderScene()
{
    static int index = 0;
    QString fileName = QDir::currentPath() + QDir::separator() + "screen" + QString::number(index++) + ".png";
    QRect rect = sceneRect().toAlignedRect();
    QImage image(rect.size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    render(&painter);
    image.save(fileName);
    qDebug() << "saved " << fileName;
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
    case Qt::Key_Z:
    {
        renderScene();
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
