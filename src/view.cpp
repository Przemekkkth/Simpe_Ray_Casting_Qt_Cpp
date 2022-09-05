#include "view.h"
#include <QApplication>
#include <QKeyEvent>

View::View()
    : m_gameScene(new GameScene())
{
    setScene(m_gameScene);
    resize(m_gameScene->sceneRect().width(), m_gameScene->sceneRect().height());

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void View::keyPressEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    {
        switch (event->key()) {
            case Qt::Key_Escape:
                QApplication::instance()->quit();
            break;
        }
    }
    QGraphicsView::keyPressEvent(event);
}
