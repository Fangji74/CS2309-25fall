#include "graph_viewer.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GraphViewer viewer;
    viewer.show();

    return app.exec();
}
