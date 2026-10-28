#include <QObject>
#include <QQuickItem>
#include <QtQml>
#include <QSGFlatColorMaterial>
#include <QSGNode>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QColor>

class Q_SimpleRect : public QQuickItem{
    Q_OBJECT
    // QML_ELEMENT
    QML_NAMED_ELEMENT(BasicRectangle)

public:

    Q_SIGNAL void mouseClicked(Qt::MouseButtons buttons,QPointF pos);
    explicit Q_SimpleRect(QQuickItem* parent = nullptr){
        // qDebug()<<"ITEM CONSTRUCTOR CALLED!";
        this -> setAcceptedMouseButtons(Qt::AllButtons);
        this -> setFlag(ItemHasContents, true);

        // this ->setFlag(ItemHasContents);
    }

protected:

    QSGNode * updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) override{

        if(! oldNode)oldNode = new QSGNode();
        oldNode ->removeAllChildNodes();

        QSGGeometryNode* node = new QSGGeometryNode();
        QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 6);
        geometry ->setDrawingMode(QSGGeometry::DrawingMode::DrawTriangles);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);


        QSGFlatColorMaterial *material = new QSGFlatColorMaterial();
        material->setColor(m_color);
        // node->markDirty(QSGNode::DirtyMaterial);
        node ->setMaterial(material);
        QSGGeometry::Point2D* vertices =  geometry ->vertexDataAsPoint2D();


        // "triangle" 1:
        vertices[0].set(0, 0);
        vertices[1].set(width(), 0);
        vertices[2].set(0, height());

        // "triangle" 2:
        vertices[3].set(width(), height());
        vertices[4].set(0, height());
        vertices[5].set(width(), 0);

        // thinking about a group of 3 vertices as a triangle is pretty helpful, to me at least.

        oldNode->appendChildNode(node); // I can also cast the old node as a QSGGeometryNode, but I like adding child nodes.
        oldNode ->markDirty(QSGNode::DirtyGeometry);
        return oldNode;
    }

    void mousePressEvent(QMouseEvent *event)override{
        emit this ->mouseClicked(event ->buttons() ,event ->globalPosition());
    }

private:

    QColor m_color = QColor(0, 0, 0);

};