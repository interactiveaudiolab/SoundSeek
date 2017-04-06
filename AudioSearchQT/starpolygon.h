#ifndef STARPOLYGON_H
#define STARPOLYGON_H

#include <QPolygonF>

class StarPolygon : public QPolygonF
{
public:
    StarPolygon(int radius) {
        float halfRadius = radius * .5;
        *this << QPointF(0, radius);


        for (int i = 1; i < 5; ++i)
            *this << QPointF( radius * std::cos(0.4 * i * 3.14),
                                   radius * std::sin(0.4 * i * 3.14));
    }
};

#endif // STARPOLYGON_H
