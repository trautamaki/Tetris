#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QColor>
#include <QKeyEvent>
#include <QGraphicsRectItem>
#include <QMessageBox>
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

    connect(ui->pauseButton, &QPushButton::clicked,
            this, &MainWindow::pauseGame);

    // Start game
    game();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateUI() {
    ui->pointsLabel->setText(QString::number(points_));
}

void MainWindow::pauseGame() {
    pause_ = !pause_;
    ui->pauseButton->setText(pause_ ? "Resume" : "Pause");
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if ( event->key() == Qt::Key_Down || event->key() == Qt::Key_S ) {
        fast_ = true;
        moveBlock(DOWN);
    }

    if ( event->key() == Qt::Key_Left || event->key() == Qt::Key_A ) {
        moveBlock(LEFT);
    }

    if ( event->key() == Qt::Key_Right || event->key() == Qt::Key_D ) {
        moveBlock(RIGHT);
    }

    if ( event->key() == Qt::Key_Up || event->key() == Qt::Key_W ) {
        rotateTetromino();
    }

    if ( event->key() == Qt::Key_Space ) {
        moveToBottom();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if ( event->key() == Qt::Key_Down || event->key() == Qt::Key_S ) {
        fast_ = false;
    }
}

void MainWindow::draw() {
    if ( field_.empty() ) return;

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
                                        colours.at(field_.at(x).at(y) - 2));

                graphics_.push_back(square);
            }
        }
    }
}

// Helper to check if all values are false in vector< bool >
bool allTrue(std::vector< bool >& vec) {
    for ( bool b : vec ) {
        if ( !b ) {
            return false;
        }
    }
    return true;
}

bool MainWindow::allClearBelow(int col) {
    for ( int y = 0; y < ROWS; ++y ) {
        if ( field_.at(col).at(y) > 1 ) {
            return false;
        }
    }

    return true;
}

void MainWindow::moveToBottom() {
    if ( pause_ ) return;

    bool all_the_way = false;
    int delta_y = 0;

    std::vector< int > piece_delta_y;
    std::vector< bool >piece_to_bottom(4, false);
    std::vector< int >piece_to_bottom_y(4, ROWS + 1);

    int i = 0;
    for ( int px = 0; px < 4; ++px ) {
        for ( int py = 0; py < 4; ++py ) {
            if ( current_->at(px).at(py) != 1 ) continue;

            if ( allClearBelow(position_.at(px).at(py).x) ) {
                piece_to_bottom_y.at(i) = ROWS - position_.at(px).at(py).y - 1;
                piece_to_bottom.at(i) = true;
            }

            all_the_way = allTrue(piece_to_bottom);
            if ( all_the_way ) {
                break;
            }

            int j = 0;
            for ( int x = 0; x < COLUMNS; ++x ) {
                for ( int y = 0; y < ROWS; ++y ) {
                    if ( x == position_.at(px).at(py).x &&
                         field_.at(x).at(y) > 1 ) {

                        int candidate_y = y - position_.at(px).at(py).y - 1;
                        piece_delta_y.push_back(candidate_y);
                    }
                }
                ++j;
            }
            ++i;
        }
    }

    delta_y = all_the_way ? *std::min_element(piece_to_bottom_y.begin(),
                                              piece_to_bottom_y.end())
                          : *std::min_element(piece_delta_y.begin(),
                                              piece_delta_y.end());

    for( int i = 0; i < 4; i++ ) {
        for( int j = 0; j < 4; j++ ) {
            setAbsolutePosition(i, j, position_.at(i).at(j).x,
                                      position_.at(i).at(j).y + delta_y);
        }
    }
}

void MainWindow::rotateTetromino() {
    if ( current_ == nullptr || current_shape_ == SQUARE ) return;
    if ( pause_ ) return;

    std::vector< std::vector< int > >* temp =
            new std::vector< std::vector< int > >;
    *temp = *current_;

    for( int i = 0; i < 4; i++ ) {
        for( int j = 0; j < 4; j++ ) {
            temp->at(j).at(4 - i - 1) = current_->at(i).at(j);
        }
    }

    // Move to right/left if wall is in way
    int r = 0;
    int l = 0;
    for( int i = 0; i < 4; i++ ) {
        for( int j = 0; j < 4; j++ ) {
            if ( temp->at(i).at(j) != 1 ) continue;

            if ( position_.at(i).at(j).x < 0 ) {
                r++;
            } else if ( position_.at(i).at(j).x > COLUMNS - 1 ) {
                l++;
            } else if ( field_.at(position_.at(i).at(j).x).at(position_.at(i).at(j).y) > 1 ) {
                // Real block in the way. Do nothing
                return;
            }
        }
    }

    for ( int i = 0; i < r; ++i ) {
        moveBlock(RIGHT);
    }

    for ( int i = 0; i < l; ++i ) {
        moveBlock(LEFT);
    }

    current_ = temp;
    draw();
}

void MainWindow::clearRow(int row) {
    if ( DEBUG ) qDebug() << "Clearing row " << row;

    // Clear the row
    for ( int x = 0; x < COLUMNS; ++x ) {
        if ( field_.at(x).at(row) < 2 ) {
            field_.at(x).at(row) = 0;
        }
    }

    // Move rows above 'row' 1 step down
    for ( int x = 0; x < COLUMNS; ++x ) {
        for ( int y = ROWS; y > 0; --y ) {
            if ( y < row && y > 1 ) {
                field_.at(x).at(y+1) = field_.at(x).at(y);
                field_.at(x).at(y) = 0;
            }
        }
    }

    points_ += 10;
    updateUI();
}

bool MainWindow::checkRow(int row) {
    for ( int x = 0; x < COLUMNS; ++x ) {
        if ( field_.at(x).at(row) < 2 ) {
            return false;
        }
    }

    return true;
}

void MainWindow::finishTetromino() {
    for ( int px = 0; px < 4; ++px ) {
        for ( int py = 0; py < 4; ++py ) {
            if ( current_->at(px).at(py) == 0 ) {
                continue;
            }
            field_.at(position_.at(px).at(py).x)
                  .at(position_.at(px).at(py).y) = current_shape_ + 2;
        }
    }

    //create_ = true;
    current_ = nullptr;
    createBlock(distr(randomEng));
    if ( DEBUG ) qDebug() << "Tetromino finished " << current_shape_;

    for ( int y = 0; y < ROWS; ++y ) {
        if ( checkRow(y) ) {
            if ( DEBUG ) qDebug() << "Row " << y << " is to be cleared";
            clearRow(y);
        }
    }
}

int MainWindow::checkSpace(int d, int r = 1) {
    // Default delty x and delta y
    int dx = 0;
    int dy = 0;

    switch ( d ) {
    case LEFT:
        dx = -r;
        break;
    case RIGHT:
        dx = r;
        break;
    case DOWN:
        dy = r;
        break;
    }

    for ( int px = 0; px < 4; ++px ) {
        for ( int py = 0; py < 4; ++py ) {
            if ( current_->at(px).at(py) != 1 ) continue;


            // Check for walls
            if ( (position_.at(px).at(py).x + dx >= COLUMNS || // Right wall
                  position_.at(px).at(py).x + dx < 0) ) {      // Left wall

                if ( DEBUG ) qDebug() << "Movement blocked: wall";
                return WALL;
            }

            if ( position_.at(px).at(py).y + dy >= ROWS ) {

                if ( DEBUG ) qDebug() << "Movement blocked: floor";
                return FLOOR;
            }

            // Check for other blocks
            if ( (position_.at(px).at(py).x + dx > 0 &&
                  position_.at(px).at(py).x + dx < COLUMNS) &&

                 (position_.at(px).at(py).y + dy < ROWS) &&

                 (field_.at(position_.at(px).at(py).x + dx)
                        .at(position_.at(px).at(py).y + dy) > 1 ) ) {

                if ( DEBUG ) qDebug() << "Movement blocked: tetromino";
                return TETROMINO;
            }
        }
    }

    return NONE;
}

void MainWindow::setAbsolutePosition(int p_x, int p_y, int x, int y) {
    // Move piece's coordinates

    position_.at(p_x).at(p_y) = { x, y };


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

void MainWindow::moveBlock(int d) {
    if ( current_ == nullptr ) return;
    if ( pause_ ) return;

    // Default delty x and delta y
    int dx = 0;
    int dy = 0;

    int r = 1;

    switch ( d ) {
    case LEFT:
        dx = -1;
        break;
    case RIGHT:
        dx = 1;
        break;
    case DOWN:
        fast_ ? dy = 2 : dy = 1;
        fast_ ? r = 2 : r = 1;
        break;
    }

    int status = checkSpace(d, r);
    switch ( status ) {
    case TETROMINO:
        if ( d != DOWN ) {
            return;
        }
        if ( fast_ ) {
            moveToBottom();
        }
        finishTetromino();
        break;
    case WALL:
        return;
        break;
    case FLOOR:
        if ( fast_ ) {
            moveToBottom();
        }
        finishTetromino();
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

    switch ( tetromino ) {
    case HORIZONTAL:
        start_x = 4;
        start_y = -2;
        break;
    case STEP_UP_RIGHT:
    case STEP_UP_LEFT:
    case LEFT_CORNER:
    case RIGHT_CORNER:
    case PYRAMID:
        start_x = 4;
        start_y = -1;
        break;
    }

    start_x = 4;
    start_y = 0;

    if ( DEBUG ) qDebug() << "Create block " << tetromino;

    for ( int x = 0; x < 4; ++x ) {
        for ( int y = 0; y < 4; ++y ) {
            position_.at(x).at(y) = { start_x + x, start_y + y };
            field_.at(start_x + x).at(start_y + y) = types_.at(tetromino)
                                                           .at(x).at(y);
            current_shape_ = tetromino;
            current_ = &types_.at(tetromino);
        }
    }
}

void MainWindow::gameOver() {
    if ( DEBUG ) qDebug() << "Game over";
    pause_ = true;

    QMessageBox::StandardButton replay;
    QString message = QString("You had %1 points. Play again?")
            .arg(points_);
    replay = QMessageBox::question(this, "Game over",
                                   message,
                                   QMessageBox::Yes | QMessageBox::No);

    if ( replay == QMessageBox::No ) {
        qApp->exit();
    }

    if ( replay == QMessageBox::Yes ) {
        create_ = true;
        game();
        draw();
    }
}

void MainWindow::gameloop() {
    if ( !pause_ ) {
        if ( create_ ) {
            create_ = !create_;
            createBlock(distr(randomEng));
        }

        for ( int x = 0; x < 6; ++x ) {
            for ( int y = 0; y < 5; ++y ) {
                // Check if spawn zone is occupied
                //      -> game over

                if ( field_.at(3 + x).at(0 + y) > 1 ) {
                    gameOver();
                    return;
                }
            }
        }

        draw();
        moveBlock(DOWN);
    }
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
    points_ = 0;
    field_.clear();
    position_.clear();
    current_ = nullptr;
    pause_ = false;
    updateUI();
    drawGrid();

    draw();

    field_ = std::vector< std::vector< int > >
            (COLUMNS, std::vector< int >(ROWS, 0));

    position_ = std::vector< std::vector< tetromino_pos > >
            (4, std::vector< tetromino_pos >(4, { 0, 0 }));

    // Set up timer and start game loop
    timer_.setSingleShot(false);
    connect(&timer_, &QTimer::timeout, this, &MainWindow::gameloop);
    timer_.start(300);
}
