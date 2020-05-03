/*
 * Tetris -game
 * UI class that contains the
 * Tetris game
 *
 * Timi Rautamäki, 284032
 *
 */

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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    /**
     * @brief updateTime
     * Updates the LCD displays above game field
     */
    void updateTime();

    void on_startButton_clicked();

    void on_scoreBoardButton_clicked();

    void on_endGameButton_clicked();

private:
    bool DEBUG = true;
    Ui::MainWindow *ui;

    QGraphicsScene* scene_;
    QGraphicsScene* next_scene_;

    std::string FILENAME = "leaders.txt";

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
    enum TETROMINO_KIND { HORIZONTAL,
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
     * Update the UI elements (points)
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
     * Track keyboard press events
     */
    void keyPressEvent(QKeyEvent* event);
    /**
     * @brief keyReleaseEvent
     * @param event
     * Track keyboard release events
     */
    void keyReleaseEvent(QKeyEvent* event);
    /**
     * @brief paintEvent
     * @param event
     * Draw the whole field
     */
    void draw();
    /**
     * @brief drawNext
     * Draw the next tetromino next to
     * the play field
     */
    void drawNext();
    /**
     * @brief allClearBelow
     * @param col
     * @return true if there are no
     *         obstacles below given row
     */
    bool allClearBelow(int col);
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
     * Clear a row and shift all above
     */
    void clearRow(int row);
    /**
     * @brief checkRow
     * @param row
     * Check if a row could be cleared
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
     * Move tetromino left/right/down
     */
    void moveBlock(int d);
    /**
     * @brief gravity
     * Move tetrommino 1 step down
     * at each tick
     */
    void gravity();
    /**
     * @brief createBlock
     * @param block_id
     * Create a block with given id
     */
    void createBlock(int tetromino);
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

    // Holds the address of the active tetromino shape
    std::vector< std::vector< int > >* current_;
    // Current shape id
    int current_shape_;
    // Next shape id
    int next_shape_;

    // Wheter to create a tetromino
    bool create_ = true;

    // Fast gravity when 'Down'-key is pressed
    bool fast_ = false;

    bool pause_ = false;
    int points_ = 0;

    // Game timer
    QTimer* clock_;
    int seconds_ = 0;
    int minutes_ = 0;

    int points_per_row_ = EASY;

    struct tetromino_pos {
        int x;
        int y;
    };

    // 4*4 that holds the information of each piece in the tetromino
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

    /*
     *  Game tuneables
     */

    // Brushes. Ordered by 'TETROMINO_KIND'
    std::vector< QBrush > colours_ = {
        QBrush(Qt::cyan),
        QBrush(Qt::blue),
        QBrush(QColor("orange")),
        QBrush(Qt::yellow),
        QBrush(Qt::green),
        QBrush(Qt::magenta),
        QBrush(Qt::red)
    };

    // Default username
    std::string username_ = "anonymous";

    // Difficulty defines the rate at which
    // tetrominos fall (clock tick delay).
    enum DIFFICULTY { EASY,
                      MEDIUM,
                      INSANE };

    std::vector< int > speeds = {
        800,
        400,
        150
    };

    std::vector< int > points = {
        10,
        15,
        25
    };

    // Default difficulty
    int difficulty_ = EASY;
};

#endif // MAINWINDOW_HH
