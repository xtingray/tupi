/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "qpathclipper_p.h"

#include "qbezier_p.h"
#include "qmath_p.h"

/**
   The algorithm used here is a little complicated. It's based on
   the most recent research related to set operations/clipping of
   polygons and extended a lot for paths. Introduction
   of curves makes the algorithm quite complex. Removal of all the
   special/corner cases of curves would make it a lot simpler.

   To understand the basics please read the following research
   papers:
   "Efficient clipping of arbitrary polygons"
   http://portal.acm.org/citation.cfm?id=274364&coll=portal&dl=ACM

   And

   "An Extension of Polygon Clipping To Resolve Degenerate Cases"
   http://cadanda.com/CAD_A_3_1-4_48.PDF

   The most involved parts of the algorithm are finding
   intersections, marking them and finally pathFromList method
   which is meant to combine them. Optimizations should start
   with the findIntersections method which is quadratic atm.
 */

//#define QDEBUG_CLIPPER
#ifdef QDEBUG_CLIPPER
#include <qdebug.h>
static QDebug operator<<(QDebug str, const QBezier &b)
{
    QString out = QString::fromLatin1("Bezier([%1, %2],[%3, %4],[%5, %6],[%7, %8])")
                  .arg(b.x1)
                  .arg(b.y1)
                  .arg(b.x2)
                  .arg(b.y2)
                  .arg(b.x3)
                  .arg(b.y3)
                  .arg(b.x4)
                  .arg(b.y4);
    str.nospace()<<out;
    return str;
}
#endif

class PathVertex
{
public:
    enum Degeneracy {
        KNone,
        KIntersect,
        KDegenerate
    };

    enum TraversalFlag {
        TNone,
        TEnEx,
        TExEn,
        TEn,
        TEx
    };

    enum CoupleFlag {
        NoCouple,
        FrontElement,
        RearElement
    };

    enum Direction {
        Stop,
        ForwardGo,
        BackwardGo,
        ForwardTurn,
        BackwardTurn
    };

    enum Type {
        MoveTo,
        LineTo,
        CurveTo,
        MoveCurveTo, //used when the path starts and ends at this vertex
        MoveLineTo,  //used when the part starts and ends at this vertex
        BezierIntersection,
        LineIntersection
    };
public:
    PathVertex();
    PathVertex(qreal x, qreal y, Type t);
    ~PathVertex();

    Direction eat();

    Direction forwardEat(PathVertex *prev);

    Direction backwardEat(PathVertex *prev);

    Direction turnForwardEat();

    Direction turnBackwardEat();

    void setIntersect(Degeneracy d);

    void setCode(TraversalFlag c)
    {
        code = c;
    }

    bool isCoupled() const
    {
        return (couple.info != NoCouple);
    }

    QPointF getPoint() const
    {
        return QPointF(x, y);
    }

    void  setCouple(CoupleFlag which, PathVertex *a)
    {
	couple.info = which;
	couple.link = a;
    }
public:
    PathVertex *next;
    PathVertex *prev;

    Degeneracy    intersect;
    TraversalFlag code;

    PathVertex *neighbor;

    struct CoupleInfo {
        CoupleInfo()
            : info(NoCouple),
              link(0)
        {}
        CoupleFlag  info;
        PathVertex *link;
    };
    CoupleInfo couple;

    bool cross_transfer;

    qreal x, y;
    qreal alpha;

    inline void setType(Type t)
    {
        type = t;
    }
    inline Type getType(const PathVertex *prev) const
    {
        if (type == MoveLineTo || type == MoveCurveTo) {
            if (!prev || prev == next)
                return MoveTo;
            else if (type == MoveLineTo)
                return LineTo;
            else
                return CurveTo;
        } else
            return type;
    }
    inline bool isCurveTo() const
    {
        return type == CurveTo || type == MoveCurveTo;
    }
    inline Type getRawType() const
    {
        return type;
    }
private:
    Type type;
public:
    //used only by curveto/movecurveto elements
    QPointF ctrl1, ctrl2;
};

#ifdef QDEBUG_CLIPPER
static QDebug operator<<(QDebug str, const PathVertex::TraversalFlag &b)
{
    QString out;

    switch (b) {
    case PathVertex::TNone:
        out = QString::fromLatin1("TNone");
        break;
    case PathVertex::TEnEx:
        out = QString::fromLatin1("TEnEx");
        break;
    case PathVertex::TExEn:
        out = QString::fromLatin1("TExEn");
        break;
    case PathVertex::TEn:
        out = QString::fromLatin1("TEn");
        break;
    case PathVertex::TEx:
        out = QString::fromLatin1("TEx");
        break;
    }
    str.nospace()<<out;
    return str;
}
#endif

PathVertex::PathVertex()
{
#ifdef QDEBUG_CLIPPER
    Q_ASSERT(0);
#endif
}

PathVertex::Direction PathVertex::eat()
{
    switch (code) {
    case TEnEx:
        setCode(TNone);
        if (cross_transfer)
            return ForwardTurn;
        else
            return BackwardTurn;
    case TExEn :
        setCode(TEn);
        return BackwardGo;
    case TEn :
        setCode(TNone);
        return ForwardGo;
    case TEx :
        setCode(TNone);
        return BackwardGo;
    case TNone :
        qFatal("PathVertex::eat: is this state possible?");
    }

    qFatal("Should never get here");
    return ForwardGo;
}


PathVertex::Direction PathVertex::forwardEat(PathVertex *prev)
{
    if (prev == 0)
        qFatal("clip_vertex::forward_eat: is this state possible?");

    switch (code) {
    case TEnEx :
        setCode(TEx);
        if (cross_transfer)
            return BackwardTurn;
        else
            return ForwardTurn;
    case TExEn :
        setCode(TEn);
        if (cross_transfer)
            return BackwardTurn;
        else
            return ForwardTurn;
    case TEn:
        setCode(TNone);
        if (isCoupled() && prev->isCoupled()) {
            if (prev->couple.link == this)
                return ForwardGo;
            else
                return ForwardTurn;
        }
        break;
    case TEx:
        setCode(TNone);
        break;

    case TNone :
        qFatal("PathVertex::forwardEat: is this state possible?");
    }

    if (cross_transfer)
        return BackwardTurn;
    else
        return ForwardTurn;
}


PathVertex::Direction PathVertex::backwardEat(PathVertex *prev)
{
    if (prev == 0)
        qFatal("PathVertex::backward_eat: is this state possible?");

    switch (code) {
    case TEnEx :
        setCode(TEn);
        if (cross_transfer)
            return ForwardTurn;
        else
            return BackwardTurn;
    case TExEn :
        setCode(TEx);
        if (cross_transfer)
            return ForwardTurn;
        else
            return BackwardTurn;
    case TEx:
        setCode(TNone);
        if (isCoupled() && prev->isCoupled()) {
            if (prev->couple.link == this)
                return BackwardGo;
            else return BackwardTurn;
        }
        break;
    case TEn:
        setCode(TNone);
        break;
    case TNone:
        qFatal("PathVertex::backward_eat: is this state possible?");;
    }
    if (cross_transfer)
        return ForwardTurn;
    else
        return BackwardTurn;
}


PathVertex::Direction PathVertex::turnForwardEat()
{
    switch (code) {
    case TEnEx:
        setCode(TNone);
        if (cross_transfer)
            return ForwardTurn;
        else
            return BackwardTurn;
    case TExEn:
        setCode(TEn);
        return BackwardGo;
    case TEn:
        setCode(TNone);
        return ForwardGo;
    case TEx:
        setCode(TNone);
        return BackwardGo;
    case TNone :
        qFatal("PathVertex::turnForwardEat: is this state possible?");
    }

    qFatal("Should never get here");
    return ForwardGo;
}


PathVertex::Direction PathVertex::turnBackwardEat()
{
    switch (code) {
    case TEnEx:
        setCode(TNone);
        if (cross_transfer)
            return BackwardTurn;
        else
            return ForwardTurn;
    case TExEn:
        setCode(TEx);
        return ForwardGo;
    case TEn:
        setCode(TNone);
        return ForwardGo;
    case TEx:
        setCode(TNone);
        return BackwardGo;
    case TNone :
        qFatal("PathVertex::turnBackwardEat: is this state possible?");
    }

    qFatal("Should never get here");
    return ForwardGo;
}


void PathVertex::setIntersect(Degeneracy d)
{
    //don't want to reset the degenerate flag
    if (intersect == KNone || intersect == KIntersect)
        intersect = d;
}

PathVertex::PathVertex(qreal xi, qreal yi, Type t)
    : next(0), prev(0), intersect(KNone),
      code(TNone), neighbor(0),
      cross_transfer(false),
      x(xi), y(yi), alpha(0), type(t)
{
}

PathVertex::~PathVertex()
{
}


struct VertexList {
public:
    static VertexList *fromPainterPath(const QPainterPath &path);
public:
    PathVertex *node;

    PathVertex *first_node;
    PathVertex *last_node;
    PathVertex *current_node;

    VertexList()
        : node(0), first_node(0),
          last_node(0), current_node(0)
    {}

    ~VertexList()
    {
        reset();
    }

    void setCurrentNode(PathVertex *a)
    {
	if (a)
            current_node = a;
	else
            qFatal("VertexList:: will crash!");
    }

    void reset()
    {
	PathVertex *a = first_node;

	while (a != 0) {
	    PathVertex *n = a->next;
	    delete a;
	    a = n;
	}

	current_node = 0;
	first_node = 0;
	last_node = 0;
    }

    void makeRing()
    {
	if (!first_node || !last_node)
            return;

	first_node->prev = last_node;
	last_node->next = first_node;
    }

    void breakRing()
    {
	if (!first_node || !last_node) return;

	first_node->prev = 0;
	last_node->next = 0;
    }

    void delNode(PathVertex *a)
    {
	if (a == 0) return;

	if (a == current_node)
	    current_node = a->prev;
	if (a->next)
	    a->next->prev = a->prev;
	if (a->prev)
	    a->prev->next = a->next;
	if (a == last_node)
	    last_node = a->prev;
	if (a == first_node)
	    first_node = a->next;
	delete a;
    }

    void appendNode(PathVertex *a)
    {
	a->prev = last_node;
	if (last_node)
	    last_node->next = a;
	if (first_node == 0)
	    first_node = a;
        last_node = a;
	current_node = a;
    }

    void insertNode(PathVertex *a, PathVertex *b)
    {
	setCurrentNode(b);

	if (current_node == b)
	    insertNode(a);
    }

    void insertNode(PathVertex *a)
    {
	if (current_node == 0) {
	    appendNode(a);
	} else {
	    a->next = current_node;

	    if (current_node) {
		a->prev = current_node->prev;
		current_node->prev = a;
	    }

	    if (a->prev) a->prev->next = a;
	    if (current_node == first_node) first_node  = a;

	    current_node = a;
	}
    }

#ifdef QDEBUG_CLIPPER
    void dump();
#endif
};

struct VertexListNavigate {

    const VertexList &h;

    PathVertex *cur;
    PathVertex *first;
    PathVertex *last;
    PathVertex *prev;

    PathVertex *lastMove;

    VertexListNavigate(const VertexList &hh)
	: h(hh), cur(hh.first_node),
          first(hh.first_node), last(hh.last_node),
          prev(hh.first_node->prev)
    {
        lastMove = cur;
    }

    ~VertexListNavigate()
    {
    }

    inline operator bool() const
    {
	return cur ? true : false;
    }

    inline void forward()
    {
	cur = cur->next ? cur->next : first;
    }

    inline void backward()
    {
	cur = cur->prev ? cur->prev : last;
    }

    inline void next()
    {
        prev = cur;
        if (cur->getType(0) == PathVertex::MoveTo && !lastMove)
            lastMove = cur;
	cur = cur ? cur->next : 0;
    }

    inline PathVertex *getNextNode() const
    {
	PathVertex *nn = cur ? cur->next: 0;
	return nn;
    }

    inline PathVertex *getPrevNode() const
    {
        return prev;
    }

    inline PathVertex *getNode() const
    {
	return cur;
    }

    inline PathVertex *getLastMove() const
    {
        return lastMove;
    }
};

VertexList *VertexList::fromPainterPath(const QPainterPath &path)
{
    VertexList *lst = new VertexList;

    bool multipleMoves = false;
    PathVertex *firstMove = 0;
    for (int i = 0; i < path.elementCount(); ++i) {
        const QPainterPath::Element &e = path.elementAt(i);
        switch(e.type) {
        case QPainterPath::MoveToElement: {
            PathVertex *lstMove = new PathVertex(e.x, e.y,
                                     PathVertex::MoveTo);
            lst->appendNode(lstMove);
            multipleMoves = firstMove;
            if (!firstMove)
                firstMove = lstMove;
            break;
        }
        case QPainterPath::LineToElement: {
            if (i == (path.elementCount() - 1) && !multipleMoves &&
                qFuzzyCompare(firstMove->x, e.x) && qFuzzyCompare(firstMove->y, e.y)) {
                firstMove->setType(PathVertex::MoveLineTo);
            } else {
                lst->appendNode(new PathVertex(e.x, e.y,
                                               PathVertex::LineTo));
            }
            break;
        }
        case QPainterPath::CurveToElement: {
#ifdef QDEBUG_CLIPPER
            Q_ASSERT(path.elementAt(i+1).type == QPainterPath::CurveToDataElement);
            Q_ASSERT(path.elementAt(i+2).type == QPainterPath::CurveToDataElement);
#endif
            if (i == (path.elementCount() - 3) && !multipleMoves &&
                qFuzzyCompare(firstMove->x, e.x) && qFuzzyCompare(firstMove->y, e.y)) {
                firstMove->setType(PathVertex::MoveCurveTo);
                firstMove->ctrl1 = QPointF(e.x, e.y);
                firstMove->ctrl2 = QPointF(path.elementAt(i+1).x, path.elementAt(i+1).y);
            } else {
                PathVertex *vtx = new PathVertex(path.elementAt(i+2).x,
                                                 path.elementAt(i+2).y,
                                                 PathVertex::CurveTo);
                vtx->ctrl1 = QPointF(e.x, e.y);
                vtx->ctrl2 = QPointF(path.elementAt(i+1).x, path.elementAt(i+1).y);
                lst->appendNode(vtx);
            }
            i += 2;
            break;
        }
        case QPainterPath::CurveToDataElement:
            Q_ASSERT(!"nodesFromPath(), bad element type");
            break;
        }
    }

    return lst;
}

#ifdef QDEBUG_CLIPPER
void VertexList::dump()
{
    PathVertex *itr = first_node;

    int i = 0;
    while (itr) {
        qDebug()<<i<<") ["<<itr->x<<", "<<itr->y<<"] "
                <<", t="<<itr->getRawType()
                <<", d="<<itr->intersect
                <<", tf="<<itr->code
                <<", cf="<<itr->couple.info
                <<", alpha="<<itr->alpha
                <<", self="<<itr
                <<", neig="<<itr->neighbor;

        ++i;
        itr = itr->next;
        if (itr == first_node)
            break;
    }
}
#endif

static qreal dist(qreal x1, qreal y1, qreal x2, qreal y2)
{
    return qSqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


static inline QBezier bezierFromNodes(PathVertex *p1, PathVertex *p2)
{
    if (p2->isCurveTo()) {
        return QBezier::fromPoints(QPointF(p1->x, p1->y),
                                   p2->ctrl1,
                                   p2->ctrl2,
                                   QPointF(p2->x, p2->y));
    } else {
        return QBezier::fromPoints(QPointF(p1->x, p1->y),
                                   p1->getPoint(),
                                   p2->getPoint(),
                                   QPointF(p2->x, p2->y));
    }
}

static inline QBezier reverseBezier(const QBezier &b)
{
    return QBezier::fromPoints(QPointF(b.x4, b.y4),
                               QPointF(b.x3, b.y3),
                               QPointF(b.x2, b.y2),
                               QPointF(b.x1, b.y1));
}

static inline bool isBezierBetween(const PathVertex *prev,
                                   const PathVertex *v,
                                   QBezier &bezier)
{
    const PathVertex *start = prev;
    const PathVertex *end = v;

    bool reverse = false;
    if (v->next == prev) {
        reverse = true;
        start = v;
        end = prev;
    } else if (v->next == prev->neighbor) {
        reverse = true;
        start = v;
        end = prev->neighbor;
    } else if (v->prev == prev->neighbor) {
        start = prev->neighbor;
    }

    const PathVertex *origStart = start;
    const PathVertex *origEnd = end;
    while (origStart->prev && origStart->getRawType() == PathVertex::BezierIntersection)
        origStart = origStart->prev;
    while (origEnd->next && origEnd->getRawType() == PathVertex::BezierIntersection)
        origEnd = origEnd->next;

    if (!origEnd->isCurveTo())
        return false;

    QBezier origBez = QBezier::fromPoints(origStart->getPoint(),
                                          origEnd->ctrl1,
                                          origEnd->ctrl2,
                                          origEnd->getPoint());
#ifdef QDEBUG_CLIPPER
    qDebug()<<"************* OrigOrigBez("
            <<origBez.x1<<", "<<origBez.y1 <<"  ,  "
            <<origBez.x2<<", "<<origBez.y2 <<"  ,  "
            <<origBez.x3<<", "<<origBez.y3 <<"  ,  "
            <<origBez.x4<<", "<<origBez.y4 <<")";
#endif

    qreal palpha = 0, nalpha = 1;
    if (start->getRawType() == PathVertex::BezierIntersection)
        palpha = start->alpha;
    if (end->getRawType() == PathVertex::BezierIntersection)
        nalpha = end->alpha;
#ifdef QDEBUG_CLIPPER
    qDebug()<<"\t"<<palpha<<nalpha;
    Q_ASSERT(palpha < nalpha);
#endif
    bezier = origBez.bezierOnInterval(palpha, nalpha);

    if (reverse)
        bezier = reverseBezier(bezier);

    return true;
}

static inline bool vertexAlreadyIntersected(PathVertex *v, qreal alpha)
{
    return (v->intersect != PathVertex::KNone &&
            (qFuzzyCompare(alpha, qreal(1.)) ||
             qFuzzyCompare(alpha, qreal(0.))));
}

static inline bool tryInjectingBezier(const PathVertex *prev,
                                      const PathVertex *v,
                                      QPainterPath &path)
{
    QBezier bezier;
    if (!isBezierBetween(prev, v, bezier))
        return false;
    //qDebug()<<"************* Bezier("
    //        <<bezier.x1<<", "<<bezier.y1 <<"  ,  "
    //        <<bezier.x2<<", "<<bezier.y2 <<"  ,  "
    //        <<bezier.x3<<", "<<bezier.y3 <<"  ,  "
    //        <<bezier.x4<<", "<<bezier.y4 <<")";

    path.cubicTo(QPointF(bezier.x2, bezier.y2),
                 QPointF(bezier.x3, bezier.y3),
                 QPointF(bezier.x4, bezier.y4));

    return true;
}

static inline QBezier bezierOutOfIntersection(const PathVertex *prev,
                                              const PathVertex *curr)
{
#ifdef QDEBUG_CLIPPER
    Q_ASSERT(curr &&
             (curr->getRawType() == PathVertex::BezierIntersection ||
              curr->getRawType() == PathVertex::CurveTo ||
              curr->getRawType() == PathVertex::MoveCurveTo));
#endif

    QBezier bezier = QBezier::fromPoints(prev->getPoint(),
                                         curr->ctrl1,
                                         curr->ctrl2,
                                         curr->getPoint());
    qreal alpha = (curr->getType(prev) == PathVertex::CurveTo)
                  ? 1.0 : curr->alpha;

    bezier = bezier.bezierOnInterval(prev->alpha,
                                     alpha);

    if (qFuzzyCompare(bezier.x4, prev->x) &&
        qFuzzyCompare(bezier.y4, prev->y))
        return reverseBezier(bezier);
    else
        return bezier;
}

class QPathClipper::Private
{
public:
    Private()
        : subject(0),
          clipper(0)
    {
    }
    Private(const QPainterPath &s,
            const QPainterPath &c)
        : subjectPath(s),
          clipPath(c),
          subject(0), clipper(0)
    {
    }
    ~Private()
    {
        delete subject;
        delete clipper;
    }

    inline bool isEdgeIn(PathVertex *a, PathVertex *b)
    {
	PathVertex *c = a->neighbor;
	PathVertex *d = b->neighbor;

	if (!c || !d) return 0;
        //qDebug()<<"isedge in "<<a<<b;
	if ((c->next == d && d->prev == c) ||
	    (c->prev == d && d->next == c)) {
	    return true;
        }
	return false;
    }

    void makeRing()
    {
        subject->makeRing();
        clipper->makeRing();
    }
    void breakRing()
    {
        subject->breakRing();
        clipper->breakRing();
    }

    PathVertex *getUnprocessed()
    {
        for (VertexListNavigate dh(*subject); dh ; dh.next()) {
            PathVertex *cur = dh.getNode();
            PathVertex::TraversalFlag now = cur->code;

            if (now != PathVertex::TNone) {
                if (cur->isCoupled()) {

                    PathVertex::CoupleFlag which = cur->couple.info;
                    PathVertex *link = cur->couple.link;

                    if (link->code == PathVertex::TNone) continue;

                    if (which == PathVertex::FrontElement &&
                        now == PathVertex::TEn) continue;
                    if (which == PathVertex::RearElement &&
                        now == PathVertex::TEx) continue;

                    return cur;
                } else
                    return cur;
            }
        }
        return 0;
    }


    bool walkResultingPath(PathVertex  *start,
                           PathVertex  *&prev_code_owner,
                           PathVertex  *&current,
                           PathVertex::Direction &traversal_stat,
                           QList<PathVertex*> &notebook)
    {
        if (current == start && traversal_stat != PathVertex::Stop) {
            traversal_stat = PathVertex::Stop;
            return false;
        }

        if (current->code != PathVertex::TNone) {

            switch (traversal_stat) {
            case PathVertex::Stop:
                traversal_stat = current->eat();
                notebook.append(current);
                prev_code_owner = current;
                break;
            case PathVertex::ForwardTurn:
                traversal_stat = current->turnForwardEat();
                prev_code_owner = current;
                break;
            case PathVertex::BackwardTurn:
                traversal_stat = current->turnBackwardEat();
                prev_code_owner = current;
                break;
            case PathVertex::ForwardGo:
                traversal_stat = current->forwardEat(prev_code_owner);
                prev_code_owner = current;
                break;
            case PathVertex::BackwardGo:
                traversal_stat = current->backwardEat(prev_code_owner);
                prev_code_owner = current;
                break;
            default:
                qFatal("PathClipper::walkPat: unexpected state!!");
            }
        }

        //qDebug()<<"current is "<<current<<traversal_stat<<current->code
        //        <<current->getPoint();
        switch (traversal_stat) {
        case PathVertex::BackwardTurn:
        case PathVertex::ForwardTurn:
            current = current->neighbor;
            break;
        case PathVertex::ForwardGo:
            current = current->next;
            notebook.append(current);
            break;
        case PathVertex::BackwardGo:
            current = current->prev;
            notebook.append(current);
            break;
        default:
            qWarning("ignoring flag...");
        }

        return true;
    }

    enum PointTest {
        AinsideB,
        AoutsideB
    };

    void getExpressions(PointTest &op1,
                        PointTest &op2)
    {
        if (op == BoolAnd) {
            op1 = AinsideB;
            op2 = AinsideB;
        } else if (op == BoolOr) {
            op1 = AoutsideB;
            op2 = AoutsideB;
        } else if (op == BoolSub) {
            op1 = AoutsideB;
            op2 = AinsideB;
        } else if (op == BoolInSub) {
            op1 = AinsideB;
            op2 = AoutsideB;
        }
    }

    enum PointLocation {
        LOut,
        LIn,
        LOn
    };

    PointLocation classifyPointLocation(const QPointF &point,
                                        const QPainterPath &B,
                                        PointTest op)
    {
        bool isContained = B.contains(point);
        //qDebug()<<"\tB  "<<point<<", contained = "<<isContained;
        if (op == AinsideB) {
            if (isContained)
                return LIn;
            else
                return LOut;
        }

        if (op == AoutsideB) {
            if (isContained)
                return LOut;
            else
                return LIn;
        }
        qFatal("Should never get here!");
        return LOut;
    }

    static inline QPointF midPoint(PathVertex *one, PathVertex *two)
    {
        if (two->getRawType() != PathVertex::BezierIntersection &&
            !two->isCurveTo()) {
            qreal xMid = (one->x+two->x)/2;
            qreal yMid = (one->y+two->y)/2;
            return QPointF(xMid, yMid);
        } else {
            QBezier bezier;
            if (!isBezierBetween(one, two, bezier)) {
                //This happens when a line intersects a bezier. the intersection
                //on the line is still marked as BezierIntersection.
                //qWarning("Couldn't form a bezier curve out of bezier-vertices!");
                return QPointF((one->x+two->x)/2,
                               (one->y+two->y)/2);
            }
            QPointF pt = bezier.midPoint();
            //qDebug()<<"\t"<<bezier;
            //qDebug()<<qSetRealNumberPrecision(12)<<"\t point between"<<one<<two<<pt;
            return pt;
        }
    }

    PathVertex::TraversalFlag generateCode(PathVertex::TraversalFlag code,
                                           PathVertex *prev,
                                           PathVertex *cur,
                                           PathVertex *next,
                                           const QPainterPath & B_p,
                                           PointTest op)
    {
        if (cur->intersect == PathVertex::KNone)
            return PathVertex::TNone;

        PointLocation  prev_s;
        PointLocation  next_s;

        QPointF  prev_p  = prev->getPoint();
        QPointF  cur_p   = cur->getPoint();
        QPointF  next_p  = next->getPoint();


        if (isEdgeIn(prev, cur)) {
            prev_s = LOn;
        } else {
            if (code == PathVertex::TEx || code == PathVertex::TEnEx)
                prev_s = LOut;
            else if (code == PathVertex::TEn || code == PathVertex::TExEn)
                prev_s = LIn;
            else
                prev_s = classifyPointLocation(midPoint(prev, cur), B_p, op);

            //Q_ASSERT(prev_s == classifyPointLocation(midPoint(prev, cur), B_p, op));
        }

        if (isEdgeIn(cur, next))
            next_s = LOn;
        else {
            next_s = classifyPointLocation(midPoint(cur, next), B_p, op);
        }
#ifdef QDEBUG_CLIPPER
        qDebug()<<"XXX Generating code = "<<prev_p<<cur_p<<next_p<<prev_s<<next_s;
#endif
        if (prev_s == LOn  && next_s == LOn)  return PathVertex::TNone;
        if (prev_s == LOn  && next_s == LOut) return PathVertex::TEx;
        if (prev_s == LOn  && next_s == LIn)  return PathVertex::TEn;
        if (prev_s == LOut && next_s == LOn)  return PathVertex::TEn;
        if (prev_s == LIn  && next_s == LOn)  return PathVertex::TEx;
        if (prev_s == LIn  && next_s == LOut) return PathVertex::TEx;
        if (prev_s == LOut && next_s == LIn)  return PathVertex::TEn;
        if (prev_s == LIn  && next_s == LIn)  return PathVertex::TExEn;
        if (prev_s == LOut && next_s == LOut) return PathVertex::TEnEx;

        return PathVertex::TNone;
    }

    void markForBooleanOperation()
    {
        PointTest which1=AinsideB, which2=AinsideB;//and is the default
        getExpressions(which1, which2);

        makeRing();

        PathVertex *start, *cur, *prev, *next;

        VertexListNavigate subjItr(*subject);
        start = cur = subjItr.getNode();
        prev = subjItr.getPrevNode();
        next = subjItr.getNextNode();

        PathVertex::TraversalFlag prev_code = PathVertex::TNone;

        ///////////////////////////////////////////////////////////////////
        /// subject of the coding is subject against the region of clipper
        while (true) {
            PathVertex::TraversalFlag code = generateCode(prev_code, prev,
                                                          cur, next, clipPath, which1);
            if (code != PathVertex::TNone)
                prev_code = code;

            cur->setCode(code);

            subjItr.next();
            prev = cur;
            cur	 = subjItr.getNode();
            next = subjItr.getNextNode();

            if (cur == start) break;
        }
        VertexListNavigate clipItr(*clipper);
        start = cur = clipItr.getNode();
        prev = clipItr.getPrevNode();
        next = clipItr.getNextNode();

        prev_code = PathVertex::TNone;
        //qDebug()<<"//////////////////////////////////////////////////////";
        ///////////////////////////////////////////////////////////////////
        /// subject of the coding is clipper against the region of subject
        while (true) {
            PathVertex::TraversalFlag code = generateCode(prev_code, prev,
                                                          cur, next, subjectPath, which2);

            if (code != PathVertex::TNone) prev_code = code;

            cur->setCode(code);

            clipItr.next();
            prev = cur;
            cur	 = clipItr.getNode();
            next = clipItr.getNextNode();

            if (cur == start) break;
        }

        breakRing();
    }

    inline bool getNextWhichHasCode(VertexListNavigate &ln,
                                    PathVertex *start)
    {
	while (1) {
	    ln.forward();
	    PathVertex *cur = ln.getNode();
	    PathVertex::TraversalFlag code = cur->code;

	    if (cur == start)                return false;
	    if (code != PathVertex::TNone)   return true;
	}
    }

    inline bool triArea(const QPointF &a,
                        const QPointF &b,
                        const QPointF &c)
    {
        return 0.5 * (a.x() * (b.y() - c.y()) +
                      b.x() * (c.y() - a.y()) +
                      c.x() * (a.y() - b.y()));
    }

    void encodeCrossTransfer(PathVertex *c)
    {
        QPointF p  = c->getPoint();
        QPointF p1 = c->prev->getPoint();
        QPointF p2 = c->next->getPoint();
        QPointF q1 = c->neighbor->prev->getPoint();
        QPointF q2 = c->neighbor->next->getPoint();

        qreal t1 = triArea(q2, p1, q1);
        qreal t2 = triArea(p2, p1, q1);

        if (t1 * t2 < 0) {
            c->cross_transfer = true;
            c->neighbor->cross_transfer = true;
        }
    }

    void findCouplesAndCrossTransfers()
    {
        makeRing();

        PathVertex *cur, *start, *next;
        VertexListNavigate ln(*subject);

        start = ln.getNode();

        if (getNextWhichHasCode(ln, start)) {

            start = cur = ln.getNode();

            PathVertex::TraversalFlag cur_code = cur->code, next_code;

            if (cur_code == PathVertex::TEnEx || cur_code == PathVertex::TExEn)
                encodeCrossTransfer(cur);

            while (true) {
                int will_be_continued = getNextWhichHasCode(ln, start);
                next = ln.getNode();
                next_code = next->code;

                if (cur == next) break;

                if (cur_code == PathVertex::TEn || cur_code == PathVertex::TEx) {
                    if (cur_code == next_code) {
                        cur->setCouple(PathVertex::FrontElement, next);
                        next->setCouple(PathVertex::RearElement, cur);
                    }
                }

                cur = next;
                cur_code = next_code;

                if (!will_be_continued) break;

                if (cur_code == PathVertex::TEnEx || cur_code == PathVertex::TExEn)
                    encodeCrossTransfer(cur);
            }
        }

        VertexListNavigate ln1(*clipper);

        start = ln1.getNode();

        if (getNextWhichHasCode(ln1, start)) {

            start = cur = ln1.getNode();

            PathVertex::TraversalFlag cur_code = cur->code, next_code;

            while (true) {
                int will_be_continued = getNextWhichHasCode(ln1, start);
                next = ln1.getNode();
                next_code = next->code;

                if (cur == next) break;

                if (cur_code == PathVertex::TEn || cur_code == PathVertex::TEx) {
                    if (cur_code == next_code) {
                        cur->setCouple(PathVertex::FrontElement, next);
                        next->setCouple(PathVertex::RearElement, cur);
                    }
                }
                cur = next;
                cur_code = next_code;

                if (!will_be_continued) break;
            }
        }


        breakRing();
    }

    struct Intersection
    {
        Intersection(qreal xx, qreal yy,
                     PathVertex::Type t,
                     qreal ap, qreal aq,
                     PathVertex *pp1, PathVertex *pp2,
                     PathVertex *qq1, PathVertex *qq2,
                     VertexList *&pLst, VertexList *&qLst)
            : x(xx), y(yy), type(t),
              alpha_p(ap), alpha_q(aq),
              p1(pp1), p2(pp2), q1(qq1), q2(qq2),
              pList(pLst), qList(qLst)
        {
#ifdef QDEBUG_CLIPPER
            qDebug()<<qSetRealNumberPrecision(12)<<"Intersection at "<<xx<<yy
                    <<"between : "<<p1->getPoint()<<" and "<<p2->getPoint()
                    <<" --- "<<q1->getPoint()<<" and "<<q2->getPoint()
                    <<" at "<<alpha_p<<alpha_q;
#endif
        }
        qreal x, y;
        PathVertex::Type type;
        qreal alpha_p, alpha_q;

        PathVertex *p1, *p2;
        PathVertex *q1, *q2;

        VertexList *pList;
        VertexList *qList;

        PathVertex *intersection(qreal alpha,
                                 PathVertex *one, PathVertex *two,
                                 VertexList &lst, bool &created) const
        {
            PathVertex *v = 0;
            created = false;
            if (qFuzzyCompare(alpha, 0)) {
                v = one;
            } else if (qFuzzyCompare(alpha, 1)) {
                v = two;
            } else {
                v = new PathVertex(x, y, type);
                v->alpha = alpha;
                one = one->next;
                while (one && one != two &&
                       (one->intersect != PathVertex::KNone &&
                        alpha > one->alpha))
                    one = one->next;
                if (one)
                    lst.insertNode(v, one);
                else
                    lst.appendNode(v);
                created = true;
            }
            return v;
        }
        void insert() const
        {
            PathVertex::Degeneracy d = (qFuzzyCompare(alpha_p, 0) ||
                                        qFuzzyCompare(alpha_q, 0) ||
                                        qFuzzyCompare(alpha_p, 1) ||
                                        qFuzzyCompare(alpha_q, 1))
                                       ? PathVertex::KDegenerate
                                       : PathVertex::KIntersect;
            bool  newlyCreated1 = false, newlyCreated2 = false;
            PathVertex *sinter = intersection(alpha_p, p1, p2, *pList, newlyCreated1);
            PathVertex *cinter = intersection(alpha_q, q1, q2, *qList, newlyCreated2);

            if (!newlyCreated1) {
                if (vertexAlreadyIntersected(sinter, alpha_p)) {
                    if (newlyCreated2) {
                        //qDebug()<<"----- 1 already intersected = "<<sinter->getPoint();
                        qList->delNode(cinter);
                        return;
                    }
                }
            }
            if (!newlyCreated2) {
                if (vertexAlreadyIntersected(cinter, alpha_q)) {
                    if (newlyCreated1) {
                        //qDebug()<<"----- 2 already intersected = "<<cinter->getPoint();
                        pList->delNode(sinter);
                        return;
                    }
                }
            }

            sinter->neighbor = cinter;
            cinter->neighbor = sinter;
            sinter->intersect = d;
            cinter->intersect = d;

            if (p2->isCurveTo()) {
                sinter->ctrl1 = p2->ctrl1;
                sinter->ctrl2 = p2->ctrl2;
            }
            if (q2->isCurveTo()) {
                cinter->ctrl1 = q2->ctrl1;
                cinter->ctrl2 = q2->ctrl2;
            }
        }
    };
    QList<Intersection> intersections;

    bool intersectBeziers(PathVertex *p1, PathVertex *p2,
                          PathVertex *q1, PathVertex *q2)
    {
        QBezier one = bezierFromNodes(p1, p2);
        QBezier two = bezierFromNodes(q1, q2);

        QVector< QList<qreal> > inters = QBezier::findIntersections(one, two);

        //qDebug()<<"\tIntersecting: one = "<<one
        //        <<"\n\t\t two = "<<two
        //        <<"\n\t\t == "<<inters[0]<<inters[1];
        if (inters[0].isEmpty())
            return false;
        const QList<qreal> &alpha_ps = inters[0];
        const QList<qreal> &alpha_qs = inters[1];
        int count = alpha_ps.size();

        for (int i = 0; i < count; ++i) {
            qreal alpha_p = alpha_ps[i];
            qreal alpha_q = alpha_qs[i];
            QPointF pt = one.pointAt(alpha_p);

            intersections.append(
                Intersection(pt.x(), pt.y(), PathVertex::BezierIntersection,
                             alpha_p, alpha_q,
                             p1, p2, q1, q2,
                             subject, clipper));
        }
        return true;
    }

    bool intersectLines(PathVertex *p1, PathVertex *p2,
                        PathVertex *q1, PathVertex *q2)
    {
        qreal x, y, tp, tq, par;

        par = ((p2->x - p1->x)*(q2->y - q1->y) -
               (p2->y - p1->y)*(q2->x - q1->x));

        if (qFuzzyCompare(par, qreal(0.0))) {
            //FIXME
            //qDebug("possibly skipping coinciding edges...");
            return false;        /* parallel lines */
        }

        tp = ((q1->x - p1->x)*(q2->y - q1->y) -
              (q1->y - p1->y)*(q2->x - q1->x))/par;
        tq = ((p2->y - p1->y)*(q1->x - p1->x) -
              (p2->x - p1->x)*(q1->y - p1->y))/par;

        if (tp<0 || tp>1 || tq<0 || tq>1)
            return false;

        //qDebug().nospace()<<"........ ["<<QPointF(p1->x, p1->y)<<QPointF(p2->x, p2->y)
        //                  <<"], ["<<QPointF(q1->x, q1->y)<<QPointF(q2->x, q2->y)<<"]"
        //                  <<", p = "<<par<<", tp = "<<tp<<", tq = "<<tq;

        x = p1->x + tp*(p2->x - p1->x);
        y = p1->y + tp*(p2->y - p1->y);


        //qDebug()<<"1 Vertex"<<p1->x<<p1->y<<tp;
        //qDebug()<<"2 Vertex"<<p2->x<<p2->y<<tp;
        //qDebug()<<"+++++++++++++++ "<<x<<y;

        qreal nalpha_p = dist(p1->x, p1->y, x, y) /
                         dist(p1->x, p1->y, p2->x, p2->y);
        qreal nalpha_q = dist(q1->x, q1->y, x, y) /
                         dist(q1->x, q1->y, q2->x, q2->y);

        intersections.append(
            Intersection(x, y, PathVertex::LineIntersection, nalpha_p, nalpha_q,
                         p1, p2, q1, q2, subject, clipper));

        return true;
    }

    void intersectEdges(PathVertex *a, PathVertex *b,
                        PathVertex *c, PathVertex *d)
    {
#ifdef QDEBUG_CLIPPER
        Q_ASSERT(a->intersect == PathVertex::KNone);
        Q_ASSERT(b->intersect == PathVertex::KNone);
        Q_ASSERT(c->intersect == PathVertex::KNone);
        Q_ASSERT(d->intersect == PathVertex::KNone);
#endif

        if (b->isCurveTo() || d->isCurveTo()) {
            intersectBeziers(a, b, c, d);
        } else {
            intersectLines(a, b, c, d);
        }
    }
    bool doEdgesIntersect(PathVertex *a, PathVertex *b,
                          PathVertex *c, PathVertex *d)
    {
#ifdef QDEBUG_CLIPPER
        Q_ASSERT(a->intersect == PathVertex::KNone);
        Q_ASSERT(b->intersect == PathVertex::KNone);
        Q_ASSERT(c->intersect == PathVertex::KNone);
        Q_ASSERT(d->intersect == PathVertex::KNone);
#endif

        if (b->isCurveTo() || d->isCurveTo()) {
            return intersectBeziers(a, b, c, d);
        } else {
            return intersectLines(a, b, c, d);
        }
    }

    void findIntersections()
    {
        QRectF subjControl = subjectPath.controlPointRect();
        QRectF clipControl = clipPath.controlPointRect();

        if (!subjControl.intersects(clipControl)) {
            // no way we could intersect
            return;
        }

        for (VertexListNavigate subj(*subject); subj ; subj.next()) {
            PathVertex *a = subj.getNode();
            PathVertex *b = (subj.getNextNode())?subj.getNextNode():subj.getLastMove();
            if (!b)
                break;

            for (VertexListNavigate obj(*clipper); obj ; obj.next()) {
                PathVertex *c = obj.getNode();
                PathVertex *d = (obj.getNextNode())?obj.getNextNode():obj.getLastMove();;
                if (!d)
                    break;

                intersectEdges(a, b,
                               c, d);
            }
        }
        for (QList<Intersection>::const_iterator itr = intersections.constBegin();
             itr != intersections.constEnd(); ++itr) {
            const Intersection &inter = (*itr);
            inter.insert();
        }
    }

    bool areIntersecting()
    {
        QRectF subjControl = subjectPath.controlPointRect();
        QRectF clipControl = clipPath.controlPointRect();

        bool intersects = false;
        if (!subjControl.intersects(clipControl)) {
            // no way we could intersect
            return intersects;
        }

        for (VertexListNavigate subj(*subject); subj ; subj.next()) {
            PathVertex *a = subj.getNode();
            PathVertex *b = (subj.getNextNode())?subj.getNextNode():subj.getLastMove();
            if (!b)
                break;

            for (VertexListNavigate obj(*clipper); obj ; obj.next()) {
                PathVertex *c = obj.getNode();
                PathVertex *d = (obj.getNextNode())?obj.getNextNode():obj.getLastMove();;
                if (!d)
                    break;

                intersects = doEdgesIntersect(a, b,
                                              c, d);
                if (intersects)
                    return true;
            }
        }
        return intersects;
    }

    QPainterPath pathFromList(const QList<PathVertex*> &lst)
    {
        QList<PathVertex*>::const_iterator itr;
        QPainterPath path;

        int i = 0;
        const PathVertex *prev = 0;
        //qDebug()<<lst;
        for (itr = lst.constBegin(); itr != lst.constEnd(); ++itr) {
            const PathVertex *const v = *itr;
            QList<PathVertex*>::const_iterator nextItr = itr;
            ++nextItr;
#ifdef QDEBUG_CLIPPER
            qDebug()<<i<<")Vtx = "<<v->x<<","<<v->y<<" | "
                    <<v->getRawType() <<", "<<v;
#endif
            ++i;
            if (!path.elementCount()) {
                path.moveTo(v->getPoint());
                prev = v;
                continue;
            }
            if (prev && qFuzzyCompare(prev->x, v->x) &&
                qFuzzyCompare(prev->y, v->y)) {
                prev = v;
                continue;
            }
            if (tryInjectingBezier(prev, v, path)) {
                prev = v;
                continue;
            }
            bool traversingReverse = (prev &&
                                      (prev == v->next || (prev->neighbor && prev->neighbor == v->next)));
            bool prevWasBezier = (prev && prev->getRawType() == PathVertex::CurveTo &&
                                  v->next == prev);
#ifdef QDEBUG_CLIPPER
            qDebug()<<"\t TR = "<<traversingReverse<<", wasBezier = "
                    <<prevWasBezier;
            qDebug()<<prev<<v->next<<prev->neighbor<<v->next;
#endif
            switch(v->getType(prev)) {
            case PathVertex::MoveTo:
                if (prevWasBezier && traversingReverse) {
                    QBezier bezier = bezierOutOfIntersection(v, prev);
                    bezier = reverseBezier(bezier);
                    path.cubicTo(QPointF(bezier.x2, bezier.y2),
                                 QPointF(bezier.x3, bezier.y3),
                                 QPointF(bezier.x4, bezier.y4));
                } else {
                    path.lineTo(v->getPoint());
                }
                break;
            case PathVertex::LineTo:
                if (prevWasBezier && traversingReverse) {
                    QBezier bezier = bezierOutOfIntersection(v, prev);
                    bezier = reverseBezier(bezier);
                    path.cubicTo(QPointF(bezier.x2, bezier.y2),
                                 QPointF(bezier.x3, bezier.y3),
                                 QPointF(bezier.x4, bezier.y4));
                } else {
                    path.lineTo(v->getPoint());
                }
                break;
            case PathVertex::CurveTo: {
                if (prevWasBezier && traversingReverse) {
                    QBezier bezier = bezierOutOfIntersection(v, prev);
                    bezier = reverseBezier(bezier);
                    path.cubicTo(QPointF(bezier.x2, bezier.y2),
                                 QPointF(bezier.x3, bezier.y3),
                                 QPointF(bezier.x4, bezier.y4));
                } else if (traversingReverse) {
                    path.lineTo(v->getPoint());
                } else {
                    QBezier bezier = bezierOutOfIntersection(prev, v);
                    path.cubicTo(QPointF(bezier.x2, bezier.y2),
                                 QPointF(bezier.x3, bezier.y3),
                                 QPointF(bezier.x4, bezier.y4));
                }
            }
                break;
            case PathVertex::LineIntersection:
                if (prevWasBezier && traversingReverse) {
                    QBezier bezier = bezierOutOfIntersection(v, prev);
                    bezier = reverseBezier(bezier);
                    path.cubicTo(QPointF(bezier.x2, bezier.y2),
                                 QPointF(bezier.x3, bezier.y3),
                                 QPointF(bezier.x4, bezier.y4));
                } else {
                    path.lineTo(v->getPoint());
                }
                break;
            case PathVertex::BezierIntersection: {
                //since injecting failed, this should mean
                //we're traversing a region of lines
                path.lineTo(v->getPoint());
            }
                break;
            case PathVertex::MoveLineTo:
                qFatal("unhandled element");
                break;
            case PathVertex::MoveCurveTo:
                qFatal("unhandled element");
                break;
            default:
                qFatal("Unrecognized Vertex type");
            }
            prev = v;
        }
        return path;
    }


    QPainterPath subjectPath;
    QPainterPath clipPath;
    Operation    op;

    VertexList *subject;
    VertexList *clipper;
};

QPathClipper::QPathClipper()
    : k(new Private)
{
}


QPathClipper::QPathClipper(const QPainterPath &subject,
                           const QPainterPath &clip)
    : k(new Private)
{
    setSubjectPath(subject);
    setClipPath(clip);
}

QPathClipper::~QPathClipper()
{
    delete k;
    k = 0;
}

void QPathClipper::setSubjectPath(const QPainterPath &path)
{
    k->subjectPath = path;
    delete k->subject;
    k->subject = VertexList::fromPainterPath(path);
}


QPainterPath QPathClipper::subjectPath() const
{
    return k->subjectPath;
}


void QPathClipper::setClipPath(const QPainterPath &path)
{
    k->clipPath = path;
    delete k->clipper;
    k->clipper = VertexList::fromPainterPath(path);
}


QPainterPath QPathClipper::clipPath() const
{
    return k->clipPath;
}

QPainterPath QPathClipper::clip(Operation op)
{
    k->op = op;

#ifdef QDEBUG_CLIPPER
    qDebug("xxxxxxxxxxxxxxxxxxxxxxxxx");
    k->subject->dump();
    k->clipper->dump();
    qDebug("uuuuuyyyyyyyyyyyyyyyyyyyyy");
#endif

    k->findIntersections();

    if (k->intersections.isEmpty()) { //no intersections
        bool clipInSubject = k->subjectPath.contains(k->clipPath.elementAt(0));
        bool subjectInClip = k->clipPath.contains(k->subjectPath.elementAt(0));
        QPainterPath result;
        switch(k->op) {
        case QPathClipper::BoolAnd:
            if (clipInSubject)
                result = k->clipPath;
            else if (subjectInClip)
                result = k->subjectPath;
            break;
        case QPathClipper::BoolOr:
            if (clipInSubject)
                result = k->subjectPath;
            else if (subjectInClip)
                result = k->clipPath;
            else {
                result.addPath(k->subjectPath);
                result.addPath(k->clipPath);
            }
            break;
        case QPathClipper::BoolSub:
            if (subjectInClip) {
                return QPainterPath();
            } else if (clipInSubject) {
                result = k->subjectPath;
                result.addPath(k->clipPath);
            } else {
                result = k->subjectPath;
            }
            break;
        case QPathClipper::BoolInSub:
            if (clipInSubject || subjectInClip) {
                result = k->clipPath;
                result.addPath(k->subjectPath);
            } else {
                result = k->clipPath;
            }
            break;
        }
        return result;
    }

    k->markForBooleanOperation();

    k->findCouplesAndCrossTransfers();

#ifdef QDEBUG_CLIPPER
    k->subject->dump();
    k->clipper->dump();
#endif

    k->makeRing();

    PathVertex *current, *start, *prev_code_owner = 0;

    QPainterPath result;

    while (true) {
        current = 0;

        k->breakRing();
        current = k->getUnprocessed();
        k->makeRing();

        if (!current) break;

        start = current;
        bool not_over = true;

        PathVertex::Direction traversal_stat = PathVertex::Stop;

        int count = 0;
        QList<PathVertex*> vertices;
        while (not_over)
		{
            not_over = k->walkResultingPath(start, prev_code_owner,
                                            current, traversal_stat, vertices);
			if( count >= 2000 )
			{
				return result;
			}
			count++;
		}

        result.addPath(k->pathFromList(vertices));
    }

    k->breakRing();

    return result;
}

bool QPathClipper::intersect()
{
    return k->areIntersecting();
}

bool QPathClipper::contains()
{
    bool intersect = k->areIntersecting();

    //we have an intersection clearly we can't be fully contained
    if (intersect)
        return false;

    //if there's no intersections the path is already completely outside
    //or fully inside. if the first element of the clip is inside then
    //due to no intersections, the rest will be inside as well...
    return k->subjectPath.contains(k->clipPath.elementAt(0));
}

