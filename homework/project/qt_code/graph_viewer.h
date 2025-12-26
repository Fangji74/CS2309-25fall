#ifndef GRAPH_VIEWER_H
#define GRAPH_VIEWER_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QMap>
#include <QTimer>
#include <QHash>
#include <QPair>
#include <QSet>

class GraphViewer;

// 缩放类重写，实现中心缩放
class ZoomGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ZoomGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
};

class HoverableLineItem : public QGraphicsLineItem
{
public:
    // 构造函数
    HoverableLineItem(QString text, qreal x1, qreal y1, qreal x2, qreal y2,
                      const QPen &pen, QGraphicsItem *parent = nullptr)
        : QGraphicsLineItem(x1, y1, x2, y2, parent)
    {
        setPen(pen);
        setAcceptTouchEvents(false);// 禁用悬停
        setAcceptedMouseButtons(Qt::LeftButton);// 左键点击
    }

public:
    explicit HoverableLineItem(QGraphicsItem *parent = nullptr);

protected:
    // 重写方法，增大碰撞区域
    QPainterPath shape() const override;
    // 左键点击
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

// 自定义图形项类，用于检测位置变化
class NodeItem : public QGraphicsEllipseItem
{
public:
    NodeItem(int index, const QString& nodeId, qreal x, qreal y, qreal radius,
             const QPen &pen, const QBrush &brush, GraphViewer* viewer);

    int getIndex() const { return nodeIndex; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    int nodeIndex;
    QString nodeId;
    GraphViewer* mainWindow;
    bool updating;  // 防止递归调用
};

class GraphViewer : public QMainWindow
{
    Q_OBJECT

public:
    GraphViewer(QWidget *parent = nullptr);
    ~GraphViewer();

    // 公开方法，让NodeItem可以调用
    void updateNodePosition();

private slots:
    void setArrowDisplay(bool show);
    void updateEdgesDelayed();

private:
    struct adjacencyNode{
        QString id;
        QSet<QString> propertys;
    };
    void initGraphData();
    void openFile();
    void loadJson(const QString &filePath);
    QVector<adjacencyNode> getNeighbors(const QJsonObject &node,const QString &currentNodeId);
    void findIdsDeep(const QJsonValue &value,QVector<adjacencyNode> &neighbors,const QString &currentNodeId,const QString &property);
    bool isId(const QString &s);
    void drawGraph();
    void updateEdges();
    void clearGraph();

    // 场景和视图
    QGraphicsScene *scene;
    QGraphicsView *view;

    // 图数据
    QVector<QPointF> nodes;
    QVector<QPair<int, int>> edges;

    // 存储图形项（绘制时使用QMap）
    struct EdgeGraphics {
        HoverableLineItem *line;
        QGraphicsPolygonItem *arrow;  // 新增：存储箭头
    };
    QMap<int, NodeItem*> nodeItems;
    QMap<int, QGraphicsTextItem*> nodeLabels;
    // QMap<QPair<int, int>, QGraphicsLineItem*> edgeItems;
    QMap<QPair<int, int>, EdgeGraphics> edgeItems;
    QVector<QGraphicsPolygonItem *> arrowItems;

    // 图数据结构（unorderd_map改用QHash）
    QHash<QString, QJsonObject> nodeMap;
    QHash<QString, QVector<adjacencyNode>> adjacencyList;

    // QHash<QString, QPair<QStringList,QVector<QSet<QString>>>> adjacencyList;// QPair.first存储邻接点id列表，QPair.second存储对应的邻接点的属性列表
    QHash<QString, int> nodeIdToIndexMap;// 结点id映射到结点索引
    QMap<int, QString> nodeIndexToIdMap;// 结点索引映射到结点id

    // 定时器，用于延迟更新边，避免频繁刷新
    QTimer* updateTimer;
    bool updatePending;

    // 控制箭头显示的变量
    bool showArrows = true;
};

#endif // GRAPH_VIEWER_H
