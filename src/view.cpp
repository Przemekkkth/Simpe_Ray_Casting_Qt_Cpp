#include "view.h"

View::View()
    : m_gameScene(new GameScene())
{
    setScene(m_gameScene);
    resize(m_gameScene->sceneRect().width(), m_gameScene->sceneRect().height());

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
