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
    if ( event->key() == Qt::Key_Down || event->key() == Qt::Key_S ) {
        //fall_block(true);
    }

    if ( event->key() == Qt::Key_Left || event->key() == Qt::Key_A ) {
        moveBlock(LEFT);
    }

    if ( event->key() == Qt::Key_Right || event->key() == Qt::Key_D ) {
        moveBlock(RIGHT);
    }

    if ( event->key() == Qt::Key_Space ) {
        //move_to_bottom();
    }
}

void MainWindow::draw() {
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);

    // Brushes
    std::vector< QBrush > colours = {
        QBrush(Qt::cyan),
        QBrush(Qt::blue),
        QBrush(QColor("orange")),
        QBrush(Qt::yellow),
        QBrush(Qt::green),
        QBrush(Qt::magenta),
        QBrush(Qt::red)
    };

    // Remove old graphics
    for ( QGraphicsRectItem* g : graphics_ ) {
        delete g;
    }

    graphics_.clear();

    // Draw the field
    for ( int x = 0; x < COLUMNS; ++x ) {
        for ( int y = 0; y < ROWS; ++y ) {
            if ( field_.at(x).at(y) == 0 ) {
                continue;
            } else if ( field_.at(x).at(y) == 1 ) {
                QGraphicsRectItem* square =
                        scene_->addRect(x*SQUARE_SIDE,
                                        y*SQUARE_SIDE,
                                        SQUARE_SIDE,
                                        SQUARE_SIDE,
                                        blackPen,
                                        colours.at(current_shape_));

                graphics_.push_back(square);
            } else if ( field_.at(x).at(y) >= 2 ) {
                QGraphicsRectItem* square =
                        scene_->addRect(x*SQUARE_SIDE,
                                        y*SQUARE_SIDE,
                                        SQUARE_SIDE,
                                        SQUARE_SIDE,
                                        blackPen,
                                        colours.at(4));

                graphics_.push_back(square);
            }
        }
    }
}

int MainWindow::checkSpace(int d) {
    // Default delty x and delta y
    int dx = 0;
    int dy = 0;

    switch ( d ) {
    case LEFT:
        dx = -1;
        break;
    case RIGHT:
        dx = 1;
        break;
    case DOWN:
        dy = 1;
        break;
    }

    for ( int px = 0; px < 4; ++px ) {
        for ( int py = 0; py < 4; ++py ) {
            // Check for walls
            if ( position_.at(px).at(py).x + dx >= COLUMNS || // Right wall
                 position_.at(px).at(py).x + dx < 0 ) {       // Left wall

                return WALL;
            }

            if ( position_.at(px).at(py).y + dy >= ROWS ) {
                return FLOOR;
            }

            // Check for other blocks
            if ( field_.at(position_.at(px).at(py).x + dx)
                       .at(position_.at(px).at(py).y + dy) > 1 ) {

                return false;
            }
        }
    }

    return NONE;
}

void MainWindow::moveBlock(int d) {
    // Default delty x and delta y
    int dx = 0;
    int dy = 0;

    switch ( d ) {
    case LEFT:
        dx = -1;
        break;
    case RIGHT:
        dx = 1;
        break;
    case DOWN:
        dy = 1;
        break;
    }

    int status = checkSpace(d);
    switch ( status ) {
    case TETROMINO:
    case WALL:
        return;
        break;
    case FLOOR:
        //finishTetromino();
        break;
    }

    // Move each piece's coordinates
    for ( int px = 0; px < 4; ++px ) {
        for ( int py = 0; py < 4; ++py ) {
            position_.at(px).at(py) = { position_.at(px).at(py).x + dx,
                                        position_.at(px).at(py).y + dy };
        }
    }

    // Clear the field
    for ( int x = 0; x < COLUMNS; ++x ) {
        for ( int y = 0; y < ROWS; ++y ) {
            if ( field_.at(x).at(y) != 1 ) {
                continue;
            } else if ( field_.at(x).at(y) == 1 ) {
                field_.at(x).at(y) = 0;
            }
        }
    }

    // Re-assign '1' to the new position
    for ( int x = 0; x < COLUMNS; ++x ) {
        for ( int y = 0; y < ROWS; ++y ) {
            for ( int px = 0; px < 4; ++px ) {
                for ( int py = 0; py < 4; ++py ) {
                    if ( (x == position_.at(px).at(py).x &&
                         y == position_.at(px).at(py).y) &&
                         current_->at(px).at(py) == 1 ) {

                        field_.at(x).at(y) = 1;
                    }
                }
            }
        }
    }

    // Redraw the field after movement
    draw();
}

void MainWindow::createBlock(int tetromino) {
    int start_x = 4;
    int start_y = 0;

    tetromino = HORIZONTAL;

    switch (tetromino) {
    case HORIZONTAL:

        for ( int x = 0; x < 4; ++x ) {
            for ( int y = 0; y < 4; ++y ) {
                position_.at(x).at(y) = { start_x + x, start_y + y };
                field_.at(start_x + x).at(start_y + y) = shape_1.at(x).at(y);
                current_shape_ = tetromino;
                current_ = &shape_1;
            }
        }

        break;
    }
}

void MainWindow::gameloop() {
    if ( create_ ) {
        create_ = !create_;
        int first = distr(randomEng);
        createBlock(first);
    }
    draw();
    moveBlock(DOWN);
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

    field_ = std::vector< std::vector< int > >
            (COLUMNS, std::vector< int >(ROWS, 0));

    position_ = std::vector< std::vector< tetromino_pos > >
            (4, std::vector< tetromino_pos >(4, { 0, 0 }));

    // Set up timer and start game loop
    timer_.setSingleShot(false);
    connect(&timer_, &QTimer::timeout, this, &MainWindow::gameloop);
    timer_.start(1000);
}
