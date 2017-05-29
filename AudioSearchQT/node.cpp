/**
 * node.cpp
 *
 * Implements a node of the sound exploration graph
 * Based on Qt's Elastic Nodes example, used under the BSD License
 */

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "edge.h"
#include "node.h"
#include "graphwidget.h"
#include "common-utils.cpp"
#include "utils.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QFont>

Node::Node(GraphWidget *graphWidget)
    : graph(graphWidget), expanded(false)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(1);
    color.setHsvF(rand()%100/100.f, .1, 1);
    radius = 20;
    rating = 0;

}

void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

QList<Edge *> Node::edges() const
{
    return edgeList;
}

void Node::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this || expanded) {
        newPos = pos();
        return;
    }

    // Sum up all forces pushing this item away
    qreal xvel = 0;
    qreal yvel = 0;

    foreach (QGraphicsItem *item, scene()->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;

        QPointF vec = mapToItem(node, 0, 0);
        qreal dx = vec.x();
        qreal dy = vec.y();

        double l = 2.0 * (dx * dx + dy * dy);
        if (l > 0) {
            xvel += (dx * 150.0) / l / 2.5;
            yvel += (dy * 150.0) / l / 2.5;
        }
    }

    // Now subtract all forces pulling items together
    double weight = (edgeList.size() + 1) * 10;
    foreach (Edge *edge, edgeList) {
        QPointF vec;
        if (edge->sourceNode() == this)
            vec = mapToItem(edge->destNode(), 0, 0);
        else
            vec = mapToItem(edge->sourceNode(), 0, 0);
        xvel -= vec.x() / weight ;//* 2.;
        yvel -= vec.y() / weight ;//* 2.;
    }

    if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
        xvel = yvel = 0;

    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + radius / 2), sceneRect.right() - radius / 2));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + radius / 2), sceneRect.bottom() - radius / 2));
}

bool Node::advance()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}

QRectF Node::boundingRect() const
{
    qreal adjust = radius / 10;
    return QRectF( -1 * radius / 2 - adjust, -1 * radius / 2 - adjust, radius+3 + adjust, radius+3 + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-1 * radius / 2, -1 * radius / 2, radius, radius);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    if (rating == -1) {
        painter->setBrush(QBrush(Qt::white));
        painter->drawEllipse(-1 * radius / 2, -1 * radius / 2, radius, radius);

        painter->setPen(QPen(Qt::lightGray, 0));
        painter->drawEllipse(-1 * radius / 2, -1 * radius / 2, radius, radius);
    }
    else {
        painter->setBrush(QBrush(color));

        painter->drawEllipse(-1 * radius / 2, -1 * radius / 2, radius, radius);

        painter->setPen(QPen(Qt::black, 0));
        painter->drawEllipse(-1 * radius / 2, -1 * radius / 2, radius, radius);
    }
    if (rating > 0) {
        QPointF centerPoint = boundingRect().center();
        centerPoint.setX(centerPoint.x() - 5    );
        centerPoint.setY(centerPoint.y() + 4);
        painter->setPen(QPen(Qt::black, 0));
        painter->drawText(centerPoint,"★");
    }
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (Edge *edge, edgeList)
            edge->adjust();
        graph->itemMoved();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // if right click, pop context menu
    if (event->button() == Qt::RightButton && !expanded)
    {

    }
    else { // if left click, play audio
       graph->playAudio(audioFile);
       if (rating == -1)
            rating = 0;
    }
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    expand();
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}


void Node::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    auto *likeAction = menu.addAction("Like");

    auto *dislikeAction = menu.addAction("Dislike");

    QAction *expandAction = nullptr;

    if (!expanded)
        expandAction = menu.addAction("Show more like this");

    //auto *collapseAction = menu.addAction("Hide children");

    auto *openInFinderAction = menu.addAction("Reveal in Finder");

    auto *openInEditorAction = menu.addAction("Open File");

    auto *selectedAction = menu.exec(event->screenPos());

    if (selectedAction == likeAction)
    {
        radius += 5;
        rating++;
        graph->getAudioParent()->searchEngine.addLikedSound(QUrlToPath(audioFile), QUrlToPath(parent->getAudio()));
        update();
    }
    else if (selectedAction == dislikeAction)
    {
        rating = -1;
        update();
    }
    else if (selectedAction == expandAction)
    {
        expand();
    }
    else if (selectedAction == openInFinderAction)
    {
        revealFile(nullptr, QUrl::fromPercentEncoding(audioFile.toLocalFile().toLocal8Bit()));
    }
    else if (selectedAction == openInEditorAction)
    {
        QDesktopServices::openUrl(audioFile);
//      auto app = QFileDialog::getOpenFileName(nullptr, "Choose an Audio Editor Application", "/Applications/", "Applications (*.app)");
//        QProcess::startDetached(app, QStringList( QUrl::fromPercentEncoding(audioFile.toLocalFile().toLocal8Bit())));
    }
//    else if (selectedAction == collapseAction)
//    {
//        collapse();
//    }
}

void Node::expand()
{
    if (expanded)
        return;

    expanded = true;
    //rating++;
    radius += 5; // increase size
    color.setHsvF(color.hslHueF(), 1, 1); // increase saturation of color

    AudioRecorder *audioRecorder = graph->getAudioParent();

    audioRecorder->searchByPath(audioFile);
}

void Node::collapse()
{
    auto* scene = graph->getScene();

    for (auto *child : childlist)
    {
        scene->removeItem(child);
    }
    edgeList.clear();
}
