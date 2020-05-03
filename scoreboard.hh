/*
 * Scoreboard -dialog
 * UI class that contains the scores
 *
 * Timi Rautamäki, 284032
 *
 */

#ifndef SCOREBOARD_HH
#define SCOREBOARD_HH

#include <QDialog>

namespace Ui {
    class ScoreBoard;
}

class ScoreBoard : public QDialog {
    Q_OBJECT

public:
    explicit ScoreBoard(std::string& filename, QWidget *parent = 0);
    ~ScoreBoard();

private:
    struct entry_ {
        QString name;
        int score;
        int min;
        int s;
    };

    Ui::ScoreBoard *ui;
    /**
     * @brief compareScores
     * @param a
     * @param b
     * @return true if a's score is larger than b's
     */
    static bool compareScores(const entry_ &a, const entry_ &b);
    /**
     * @brief readFile
     * read file and fill scoreboard
     */
    void readFile();
    /**
     * @brief Splits a string to pieces by given delimeter
     * @param string s: string to split
     * @param char delimiter
     * @param bool ignore_empty:  wheter to ignore empty
     *        substring (default false)
     * @return vector of the pieces
     */
    std::vector< std::string > split(const std::string &s,
                                     const char delimiter,
                                     bool ignore_empty = false);

    std::string filename_;
};

#endif // SCOREBOARD_HH
