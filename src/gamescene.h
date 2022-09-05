#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QElapsedTimer>
#include <QImage>
#include "game.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = nullptr);
private slots:
    void loop();
signals:
private:
    void handlePlayerInput();
    void drawMap();
    Game m_game;
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    const float m_loopSpeed;
    float m_loopTime, m_deltaTime;
    bool m_leftPressed, m_rightPressed, m_forwardPressed, m_backwardPressed;

    // QGraphicsScene interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // GAMESCENE_H
