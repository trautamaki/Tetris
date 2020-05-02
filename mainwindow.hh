#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <random>

using Tetromino = std::vector< std::vector< int > >;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    bool DEBUG = true;
    Ui::MainWindow *ui;

    QGraphicsScene* scene_;

    // Constants describing scene coordinates
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 480;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 240;

    const int SQUARE_SIDE = 20;
    const int COLUMNS = BORDER_RIGHT / SQUARE_SIDE;
    const int ROWS = BORDER_DOWN / SQUARE_SIDE;


    // How much to move a block per frame
    const int SPEED = SQUARE_SIDE;
    // How much to move a block on keypress
    const int KEYPRESS_SPEED = 2 * SPEED;
    // How much to move a block sideways on keypress
    const int LATERAL_SPEED = SQUARE_SIDE;

    // Constants for different tetrominos and the number of them
    enum Tetromino_kind { HORIZONTAL,
                          LEFT_CORNER,
                          RIGHT_CORNER,
                          SQUARE,
                          STEP_UP_RIGHT,
                          PYRAMID,
                          STEP_UP_LEFT,
                          NUMBER_OF_TETROMINOS };

    enum DIRECTIONS { LEFT, RIGHT, DOWN };
    enum OBSTACLE { NONE, WALL, FLOOR, TETROMINO };

    // For randomly selecting the next dropping tetromino
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    // Game timer
    QTimer timer_;
    /**
     * @brief updateUI
     * Update the UI elements
     */
    void updateUI();
    /**
     * @brief pauseGame
     * Pauses/resumes the game
     */
    void pauseGame();
    /**
     * @brief keyPressEvent
     * @param event
     * Track keyboard events
     */
    void keyPressEvent(QKeyEvent* event);
    /**
     * @brief keyReleaseEvent
     * @param event
     */
    void keyReleaseEvent(QKeyEvent* event);
    /**
     * @brief paintEvent
     * @param event
     */
    void draw();
    /**
     * @brief allClearBelow
     * @param row
     * @return true if there are no
     * obstacles below at given row
     */
    bool allClearBelow(int row);
    /**
     * @brief moveToBottom
     * Move tetromino as low as possible
     * in one tick
     */
    void moveToBottom();
    /**
     * @brief rotateTetromino
     */
    void rotateTetromino();
    /**
     * @brief clearRow
     * @param row
     */
    void clearRow(int row);
    /**
     * @brief checkRow
     * @param row
     */
    bool checkRow(int row);
    /**
     * @brief finishTetromino
     * Move a tetromino to be part of the floor
     */
    void finishTetromino();
    /**
     * @brief checkSpace
     * @param d: direction
     * @param r: range
     * @return 0: free
     *         1: wall
     *         2: floor
     *         3: tetromino
     */
    int checkSpace(int d, int r);
    /**
     * @brief setAbsolutePosition
     * @param p_x: x coordinate in the tetromino 4x4
     * @param p_y: y coordinate in the tetromino 4x4
     * @param x: x coordinate to go to
     * @param y: y coordinate to go to
     */
    void setAbsolutePosition(int p_x, int p_y, int x, int y);
    /**
     * @brief moveBlock
     * @param d: direction
     * Move tetromino sideways
     */
    void moveBlock(int d);
    /**
     * @brief gravity
     */
    void gravity();
    /**
     * @brief createBlock
     * @param block_id
     * Create a block with given id
     */
    void createBlock(int block_id);
    /**
     * @brief drawGrid
     */
    void drawGrid();
    /**
     * @brief gameOver
     * End the game
     */
    void gameOver();
    /**
     * @brief gameloop
     * Main loop of the game
     */
    void gameloop();
    /**
     * @brief game
     * Set up variables to begin gameloop
     */
    void game();

    std::vector< std::vector< int > >* current_;
    int current_shape_;

    struct tetromino_pos {
        int x;
        int y;
    };

    bool create_ = true;

    // Fast gravity
    bool fast_ = false;

    bool pause_ = false;
    int points_ = 0;

    std::vector< std::vector< tetromino_pos > > position_;

    // Whole game field, initialized with size COLUMNS*ROWS
    // values:
    //  0: empty
    //  1: active tetromino
    //  2..9: finished tetromino
    std::vector< std::vector< int > > field_;

    // ####
    std::vector< std::vector< int > > shape_1 = {
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 }
    };

    // #
    // ###
    std::vector< std::vector< int > > shape_2 = {
        { 0, 1, 1, 0 },
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 0, 0 }
    };

    //   #
    // ###
    std::vector< std::vector< int > > shape_3 = {
        { 0, 1, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 0 }
    };

    // ##
    // ##
    std::vector< std::vector< int > > shape_4 = {
        { 0, 0, 0, 0 },
        { 1, 1, 0, 0 },
        { 1, 1, 0, 0 },
        { 0, 0, 0, 0 }
    };

    //  ##
    // ##
    std::vector< std::vector< int > > shape_5 = {
        { 0, 1, 0, 0 },
        { 0, 1, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 0 }
    };

    //  #
    // ###
    std::vector< std::vector< int > > shape_6 = {
        { 0, 0, 1, 0 },
        { 0, 1, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 0 }
    };

    // ##
    //  ##
    std::vector< std::vector< int > > shape_7 = {
        { 0, 0, 1, 0 },
        { 0, 1, 1, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 0, 0 }
    };

    std::vector< Tetromino > types_ = { shape_1, shape_2, shape_3,
                                        shape_4, shape_5, shape_6, shape_7 };

    std::vector< QGraphicsRectItem* > graphics_;
};

#endif // MAINWINDOW_HH
