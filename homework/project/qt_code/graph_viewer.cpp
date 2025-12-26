#include "graph_viewer.h"
#include <QDebug>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QKeyEvent>
#include <QTime>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMessageBox>
#include <QtMath>
#include <QDebug>
#include <QToolTip>
#include <QGraphicsSceneMouseEvent>


ZoomGraphicsView::ZoomGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
}

void ZoomGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        QPointF scenePos = mapToScene(event->position().toPoint());
        qreal scaleFactor = 1.1;

        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1/scaleFactor, 1/scaleFactor);
        }

        // 保持鼠标位置为中心
        QPointF newScenePos = mapToScene(event->position().toPoint());
        QPointF delta = newScenePos - scenePos;
        translate(delta.x(), delta.y());

        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

// NodeItem 实现
NodeItem::NodeItem(int index, const QString& id, qreal x, qreal y, qreal radius,
                   const QPen &pen, const QBrush &brush, GraphViewer* viewer)
    : QGraphicsEllipseItem(x - radius, y - radius, radius * 2, radius * 2)//椭圆项
    , nodeIndex(index)
    , nodeId(id)
    , mainWindow(viewer)
    , updating(false)
{
    setPen(pen);//设置椭圆边框
    setBrush(brush);//设置椭圆填充颜色
    setFlag(QGraphicsItem::ItemIsMovable);//允许用户拖动
    setFlag(QGraphicsItem::ItemIsSelectable);//允许被选中
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);//发送位置变化
    setAcceptHoverEvents(true);// 启用悬停事件
    setZValue(1);//设置显示层级
    setData(0, index);//将索引存储在自定义数据中

    setFlag(QGraphicsItem::ItemIgnoresTransformations, false);//设置可移动范围

    // 设置 ToolTip（鼠标悬停时显示）
    QString toolTipText = QString("节点ID: %1\n索引: %2\n点击拖动可以移动")
                              .arg(nodeId).arg(index);
    setToolTip(toolTipText);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (updating) {//避免无限递归调用，直接返回父类
        return QGraphicsEllipseItem::itemChange(change, value);
    }

    if (change == ItemPositionHasChanged) {
        // 只在位置真正改变后通知，避免ItemPositionChange的递归
        updating = true;
        if (mainWindow) {
            mainWindow->updateNodePosition();
        }
        updating = false;
    }

    return QGraphicsEllipseItem::itemChange(change, value);
}

// GraphViewer 实现
GraphViewer::GraphViewer(QWidget *parent)
    : QMainWindow(parent)
    , updateTimer(nullptr)
    , updatePending(false)
{
    // 创建定时器，用于延迟更新
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    updateTimer->setInterval(16); // 约60fps
    connect(updateTimer, &QTimer::timeout, this, &GraphViewer::updateEdgesDelayed);

    // 设置窗口标题
    setWindowTitle("Graph Viewer - 节点图绘制工具");

    // 创建场景和视图
    scene = new QGraphicsScene(this);
    view = new ZoomGraphicsView(scene, this);

    setCentralWidget(view);//设为中央部件，占据主窗口中心

    // 设置视图属性
    view->setRenderHint(QPainter::Antialiasing);// 抗锯齿，让图形边缘平滑
    view->setDragMode(QGraphicsView::ScrollHandDrag);// 拖拽模式
    // 性能优化相关属性
    view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    view->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    view->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);

    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("文件");
    QAction *openfileAction = fileMenu->addAction("打开json文件");

    QMenu *viewMenu = menuBar->addMenu("视图");
    QAction *zoomInAction = viewMenu->addAction("放大");
    QAction *zoomOutAction = viewMenu->addAction("缩小");
    QAction *resetZoomAction = viewMenu->addAction("重置缩放");

    QMenu *displayMenu = menuBar->addMenu("显示");
    QAction *arrowDisplayAction = displayMenu->addAction("显示箭头");
    QAction *fullscreenAction = displayMenu->addAction("全屏");
    QAction *exitAction = displayMenu->addAction("退出");

    arrowDisplayAction->setCheckable(true);      // 设为可勾选
    arrowDisplayAction->setChecked(true);        // 默认勾选


    // 连接信号槽
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(openfileAction, &QAction::triggered, this, &GraphViewer::openFile);
    connect(fullscreenAction, &QAction::triggered, [this]() {
        if (isFullScreen()) {
            showNormal();
            statusBar()->showMessage("退出全屏模式");
        } else {
            showFullScreen();
            statusBar()->showMessage("已进入全屏模式 - 按ESC退出全屏");
        }
    });

    connect(zoomInAction, &QAction::triggered, [this]() {
        view->scale(1.2, 1.2);
    });

    connect(zoomOutAction, &QAction::triggered, [this]() {
        view->scale(1/1.2, 1/1.2);
    });

    connect(resetZoomAction, &QAction::triggered, [this]() {
        view->resetTransform();
    });

    connect(arrowDisplayAction, &QAction::toggled,
            this, &GraphViewer::setArrowDisplay);

    // 状态栏
    statusBar()->showMessage("就绪 - 拖动节点时边会跟随移动");

    // 设置初始窗口大小
    resize(1200, 800);

    // 初始化并绘制图
    initGraphData();
    drawGraph();

}

GraphViewer::~GraphViewer()
{
    // 清理资源
    if (updateTimer) {
        updateTimer->stop();
    }
}

void GraphViewer::initGraphData()
{
    // 清除旧数据
    nodes.clear();
    edges.clear();

    // 绘制结点
    QStringList nodeIds = nodeMap.keys();
    int nodeCount = nodeIds.size();

    if (nodeCount <= 25) {
        // 小数量节点用圆形布局
        qreal centerX = 400;
        qreal centerY = 300;
        qreal radius = 200;

        for (int i = 0; i < nodeCount; ++i) {
            qreal angle = 2 * M_PI * i / nodeCount;
            qreal x = centerX + radius * qCos(angle);
            qreal y = centerY + radius * qSin(angle);
            nodes.append(QPointF(x, y));

            // 存储映射
            nodeIdToIndexMap[nodeIds[i]] = i;
            nodeIndexToIdMap[i] = nodeIds[i];
        }
    }
    else {
        // 初始化随机位置
        qsrand(QTime::currentTime().msec());
        for (int i = 0; i < nodeCount; ++i) {
            qreal x = 100 + qrand() % 800;
            qreal y = 100 + qrand() % 600;
            nodes.append(QPointF(x, y));
            //更新映射关系
            nodeIdToIndexMap[nodeIds[i]] = i;
            nodeIndexToIdMap[i] = nodeIds[i];
        }
    }

    // 绘制边
    for(auto it = adjacencyList.constBegin(); it != adjacencyList.constEnd(); ++it){
        QString sourceId = it.key();

        QVector<adjacencyNode> targetNodes = it.value();
        // 若不存在直接跳过
        if (!nodeIdToIndexMap.contains(sourceId)) continue;
        int sourceIndex = nodeIdToIndexMap[sourceId];
        for (const adjacencyNode &targetNode : targetNodes) {
            if (nodeIdToIndexMap.contains(targetNode.id)) {
                int targetIndex = nodeIdToIndexMap[targetNode.id];
                QPair<int, int> edge(sourceIndex, targetIndex);
                if (!edges.contains(edge)) {
                        edges.append(edge);
                    }
                }
            }
    }
}


void GraphViewer::openFile() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择JSON文件",
        "",
        "JSON文件 (*.json)"
        );

    if (!filePath.isEmpty()) {
        loadJson(filePath);
    }
    initGraphData();
    drawGraph();
}

void GraphViewer::loadJson(const QString &filePath) {
    try {
        // 清空当前图形
        clearGraph();

        // 读取JSON文件
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("无法打开文件");
        }

        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            throw std::runtime_error("JSON格式错误：应为数组");
        }

        QJsonArray nodesArray = doc.array();

        // 构建节点映射
        for (const QJsonValue &value : nodesArray) {
            if (value.isObject()) {
                QJsonObject node = value.toObject();
                if (node.contains("id") && node["id"].isString()) {
                    QString nodeId = node["id"].toString();
                    nodeMap[nodeId] = node;
                }
            }
        }

        // 构建邻接表（删去了excluded_set排除属性部分）
        for (const QJsonValue &value : nodesArray) {
            if (value.isObject()) {
                QJsonObject node = value.toObject();
                if (node.contains("id") && node["id"].isString()) {
                    QString nodeId = node["id"].toString();
                   QVector<adjacencyNode> neighbors = getNeighbors(node,nodeId);
                    adjacencyList[nodeId] = neighbors;
                }
            }
        }

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "错误", QString("加载失败: %1").arg(e.what()));
    }
}

// 复用原项目中已有的 getNeighbors 逻辑，添加连接属性的返回
QVector<GraphViewer::adjacencyNode> GraphViewer::getNeighbors(const QJsonObject &node,
                                         const QString &currentNodeId)
{
    QVector<adjacencyNode> neighbors;

    if (node.isEmpty()) {
        return neighbors;
    }

    // 深度优先遍历整个 JSON，寻找所有字符串形式的 id
    findIdsDeep(QJsonValue(node), neighbors, currentNodeId,"");

    return neighbors;
}

void GraphViewer::findIdsDeep(const QJsonValue &value,
                                 QVector<adjacencyNode> &neighbors,
                                 const QString &currentNodeId,
                                 const QString &property)
{
    if (value.isObject()) {
        QJsonObject obj = value.toObject();

        for (auto it = obj.begin(); it != obj.end(); ++it) {
            QString key = it.key();
            QJsonValue val = it.value();

            // 处理当前值
            if (val.isString()) {
                QString str = val.toString();
                // 检查是否是 id 且不是自身且在 nodeMap 中
                if (isId(str) && str != currentNodeId && nodeMap.contains(str)) {
                    int i;
                    for(i=0;i<neighbors.size();i++){
                        if(neighbors[i].id == str){
                            // 如果neighbors中已有对应节点
                            neighbors[i].propertys.insert(key);// 插入属性
                            break;
                        }
                    }
                    if(i==neighbors.size()){
                        // 没有对应节点则新建
                        adjacencyNode new_node;
                        new_node.id = str;
                        new_node.propertys.insert(key);
                        neighbors.push_back(new_node);
                    }
                }
            }
            // 递归处理嵌套结构
            else if (val.isObject() || val.isArray()) {
                findIdsDeep(val, neighbors, currentNodeId, key);
            }
        }
    }
    else if (value.isArray()) {
        QJsonArray arr = value.toArray();

        for (const QJsonValue &elem : arr) {
            if (elem.isObject() || elem.isArray()) {
                findIdsDeep(elem, neighbors, currentNodeId, property);
            }
            else if (elem.isString()) {
                QString str = elem.toString();
                if (isId(str) && str != currentNodeId && nodeMap.contains(str)) {
                    int i;
                    for(i=0;i<neighbors.size();i++){
                        if(neighbors[i].id == str){
                            // 如果neighbors中已有对应节点
                            neighbors[i].propertys.insert(property);// 插入属性，这里的property是上一层的属性名
                            break;
                        }
                    }
                    if(i==neighbors.size()){
                        // 没有对应节点则新建
                        adjacencyNode new_node;
                        new_node.id = str;
                        new_node.propertys.insert(property);
                        neighbors.push_back(new_node);
                    }
                }
            }
        }
    }
    // 基本类型（非对象、非数组）不需要进一步处理
}

// 与utils.cpp中实现相同，改用了QT的数据类型
bool GraphViewer::isId(const QString &str) {
    bool num_flag = 0;  //标记是否出现数字
    bool char_flag = 0; //标记是否出现字母
    for (QChar c : str) {
        if (!((c <= '9' && c >= '0') || (c <= 'z' && c >= 'a') || (c == '.'))) {
            return 0; //检查到id字符串中不可能出现的值
        }
        if ((c <= '9' && c >= '0')) {
            num_flag = 1;
        } else if ((c <= 'z' && c >= 'a')) {
            char_flag = 1;
        } else if (c == '.') {
            //空语句
        } else {
            return 0; //其他字符直接返回0
        }
    }

    if (num_flag && char_flag &&
        (str.length() == 15 || str.length() == 16)) { //观察样例发现id长度为15或16
        return 1;
    } else {
        return 0;
    }
}

void GraphViewer::drawGraph()
{
    // 清除场景中的旧项目
    scene->clear();
    nodeItems.clear();
    nodeLabels.clear();
    edgeItems.clear();
    arrowItems.clear();

    // 设置场景背景
    scene->setBackgroundBrush(QBrush(QColor(240, 240, 240)));
    scene->setSceneRect(-100, -100, 1400, 1000); // 固定场景大小

    // 先绘制边（在节点下方）
    for (const auto &edge : qAsConst(edges)) {
        if (edge.first >= nodes.size() || edge.second >= nodes.size())
            continue;

        QString toolTipText = "属性：";
        QString SourceNodeId = nodeIndexToIdMap.value(edge.first, "");
        if (SourceNodeId == "") continue;
        auto adjacencyjListIt = adjacencyList.find(SourceNodeId);
        if (adjacencyjListIt != adjacencyList.end()){
            const auto& nodeList = adjacencyjListIt.value();
            for (const adjacencyNode& node : qAsConst(nodeList)) {
                int targetIndex = nodeIdToIndexMap.value(node.id, -1);
                if (targetIndex == edge.second) {
                    for (const QString& property : qAsConst(node.propertys)) {
                        toolTipText += property;
                        toolTipText += " ";
                    }
                    break;
                }
            }
        }

        QPointF start = nodes[edge.first];
        QPointF end = nodes[edge.second];

        HoverableLineItem *line = new HoverableLineItem(
            toolTipText,
            start.x(), start.y(),
            end.x(), end.y(),
            QPen(QColor(70, 130, 180), 2)
            );
        scene->addItem(line);

        line->setZValue(0);  // 底层

        line->setToolTip(toolTipText);
        line->setAcceptHoverEvents(true);

        // 添加箭头（三角形）
        QPolygonF arrowHead;
        qreal arrowSize = 5.0;  // 箭头大小

        // 计算箭头方向
        QPointF direction = end - start;
        qreal length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        QGraphicsPolygonItem *arrow = nullptr;

        direction /= length;  // 归一化

        // 计算箭头点（在终点前面一点开始画，避免覆盖节点）
        QPointF arrowStart = end - direction * 25;

        // 计算垂直方向
        QPointF perpendicular(-direction.y(), direction.x());

        // 箭头三角形
        QPointF backPoint = arrowStart;
        QPointF leftPoint = arrowStart + perpendicular * 6;
        QPointF rightPoint = arrowStart - perpendicular * 6;

        arrowHead << end << leftPoint << rightPoint;

        // 创建箭头图形项
        arrow = scene->addPolygon(
            arrowHead,
            QPen(Qt::NoPen),
            QBrush(QColor(0, 0, 0))
            );
        arrow->setZValue(1);  // 在线上方
        arrow->setToolTip("连接箭头");
        arrowItems.push_back(arrow);
        if(arrow){
            arrow->setVisible(showArrows); //设置可见性
        }

        // 存储边引用
        edgeItems[edge] = {line,arrow};
    }

    // 绘制节点
    QVector<QColor> nodeColors = {
        QColor(220, 80, 80),    // 红色
        QColor(80, 180, 80),    // 绿色
        QColor(80, 100, 220),   // 蓝色
        QColor(220, 180, 60),   // 黄色
        QColor(180, 100, 220),  // 紫色
        QColor(60, 200, 200)    // 青色
    };

    for (int i = 0; i < nodes.size(); ++i) {
        QColor nodeColor = nodeColors[i % nodeColors.size()];

        // 使用自定义NodeItem创建节点
        NodeItem *node = new NodeItem(
            i,                       // 节点索引
            nodeIndexToIdMap[i],     // 节点id
            nodes[i].x(),            // x坐标
            nodes[i].y(),            // y坐标
            20,                      // 半径（稍小一点）
            QPen(Qt::black, 1.5),    // 边框
            QBrush(nodeColor),       // 填充颜色
            this                     // 主窗口指针
            );

        scene->addItem(node);
        nodeItems[i] = node;

        // 添加节点标签
        QGraphicsTextItem *label = scene->addText(QString("%1").arg(i));
        label->setDefaultTextColor(Qt::white);
        label->setFont(QFont("Arial", 9, QFont::Bold));

        // 居中文本
        QRectF textRect = label->boundingRect();
        label->setPos(
            nodes[i].x() - textRect.width() / 2,
            nodes[i].y() - textRect.height() / 2
            );
        label->setZValue(2);// 文本在最顶层

        // 标签不接受鼠标事件，避免干扰
        label->setAcceptedMouseButtons(Qt::NoButton);

        // 存储标签引用
        nodeLabels[i] = label;
    }
}

void GraphViewer::updateNodePosition()
{
    // 使用定时器延迟更新，避免频繁刷新
    if (!updatePending) {
        updatePending = true;
        updateTimer->start();
    }
}

void GraphViewer::updateEdgesDelayed()
{
    updateEdges();
    updatePending = false;
}

void GraphViewer::updateEdges()
{
    // 避免空指针
    if (scene->items().isEmpty()) return;

    // 限制更新频率
    static QTime lastUpdate = QTime::currentTime();
    if (lastUpdate.msecsTo(QTime::currentTime()) < 10) {
        return; // 10ms内只更新一次
    }
    lastUpdate = QTime::currentTime();

    // 暂停场景更新，提高性能
    scene->blockSignals(true);

    // 更新所有边的位置
    for (auto it = edgeItems.begin(); it != edgeItems.end(); ++it) {
        QPair<int, int> edge = it.key();
        QGraphicsLineItem *line = it.value().line;
        QGraphicsPolygonItem *arrow = it.value().arrow;

        if (!line) continue;

        // 获取当前节点的位置
        NodeItem *startNode = nodeItems.value(edge.first, nullptr);
        NodeItem *endNode = nodeItems.value(edge.second, nullptr);

        if (startNode && endNode) {
            // 获取节点的中心位置（场景坐标）
            QPointF startCenter = startNode->sceneBoundingRect().center();
            QPointF endCenter = endNode->sceneBoundingRect().center();

            // 更新线的位置
            line->setLine(startCenter.x(), startCenter.y(),
                          endCenter.x(), endCenter.y());

            // 更新箭头的位置
            if (arrow) {
                QPointF direction = endCenter - startCenter;
                qreal length = std::hypot(direction.x(), direction.y());

                direction /= length;
                QPointF perpendicular(-direction.y(), direction.x());

                // 计算新箭头位置
                QPointF arrowTip = endCenter - direction * 25;
                QPointF leftPoint = arrowTip + perpendicular * 6;
                QPointF rightPoint = arrowTip - perpendicular * 6;

                // 更新箭头多边形
                QPolygonF newArrowHead;
                newArrowHead << endCenter << leftPoint << rightPoint;
                arrow->setPolygon(newArrowHead);

                arrow->setVisible(showArrows); //设置可见性
            }
        }
    }

    // 更新节点标签的位置
    for (auto it = nodeItems.begin(); it != nodeItems.end(); ++it) {
        int nodeIndex = it.key();
        NodeItem *node = it.value();
        QGraphicsTextItem *label = nodeLabels.value(nodeIndex, nullptr);

        if (node && label) {
            // 获取节点的中心位置（场景坐标）
            QPointF nodeCenter = node->sceneBoundingRect().center();

            // 更新标签位置
            QRectF textRect = label->boundingRect();
            label->setPos(
                nodeCenter.x() - textRect.width() / 2,
                nodeCenter.y() - textRect.height() / 2
                );

            // 更新节点数据
            nodes[nodeIndex] = nodeCenter;
        }
    }

    // 恢复场景更新
    scene->blockSignals(false);

    // 重绘视图
    view->viewport()->update();
}

void GraphViewer::setArrowDisplay(bool show)
{
    showArrows = show;

    // 立即更新显示
    for(auto arrow:arrowItems) {
        if(arrow){
            arrow->setVisible(showArrows);
        }
    }
}

// 清空现有图
void GraphViewer::clearGraph() {
    scene->clear();
    nodeMap.clear();
    adjacencyList.clear();
    nodes.clear();
    edges.clear();
    nodeItems.clear();
    nodeLabels.clear();
    edgeItems.clear();
    arrowItems.clear();
}

HoverableLineItem::HoverableLineItem(QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    // 启用悬停事件
    setAcceptHoverEvents(true);
}

QPainterPath HoverableLineItem::shape() const
{
    // 创建比视觉线更宽的碰撞区域
    QPainterPathStroker stroker;
    stroker.setWidth(20.0);  // 20像素碰撞区域
    stroker.setCapStyle(Qt::RoundCap);

    QPainterPath path;
    path.moveTo(line().p1());
    path.lineTo(line().p2());

    return stroker.createStroke(path);
}

void HoverableLineItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 获取全局光标位置
        QPoint globalPos = event->screenPos();

        // 显示ToolTip（会像悬停ToolTip一样显示）
        QToolTip::showText(globalPos, toolTip(), nullptr, QRect(), 3000);

    }

    QGraphicsLineItem::mousePressEvent(event);
}
