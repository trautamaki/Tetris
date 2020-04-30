#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QColor>
#include <QKeyEvent>
#include <QGraphicsRectItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles.
    scene_ = new QGraphicsScene(this);

    int left_margin = 100; // x coordinate
    int top_margin = 150; // y coordinate

    ui->graphicsView->setGeometry(left_margin, top_margin,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    // Setting random engine ready for the first real call.
    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng);

    // Start game
    game();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {

/*    // Check that Tetrino is created and correct size
    if ( current_ == nullptr || current_->size() != 4 ) {
        return;
    }

    if ( event->key() == Qt::Key_Down || event->key() == Qt::Key_S ) {
        fall_block(true);
    }

    if ( event->key() == Qt::Key_Left || event->key() == Qt::Key_A ) {
        move_block(LEFT);
    }

    if ( event->key() == Qt::Key_Right || event->key() == Qt::Key_D ) {
        move_block(RIGHT);
    }

    if ( event->key() == Qt::Key_Space ) {
        move_to_bottom();
    }*/
}

void MainWindow::gameloop() {

}

void MainWindow::drawGrid() {
    QPen blackPen(Qt::gray);
    blackPen.setWidth(1);

    for ( int i = 0; i < ROWS; ++i ) {
        scene_->addLine(0,            i * SQUARE_SIDE,
                        BORDER_RIGHT, i * SQUARE_SIDE,
                        blackPen);
    }

    for ( int j = 0; j < ROWS; ++j ) {
        scene_->addLine(j * SQUARE_SIDE, 0,
                        j * SQUARE_SIDE, BORDER_DOWN,
                        blackPen);
    }
}

void MainWindow::game() {

    drawGrid();

    // Set up timer and start game loop
    timer_.setSingleShot(false);
    connect(&timer_, &QTimer::timeout, this, &MainWindow::gameloop);
    timer_.start(1000);
}
