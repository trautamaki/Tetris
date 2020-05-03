/*
 * Scoreboard -dialog
 * UI class that contains the scores
 *
 * Timi Rautamäki, 284032
 *
 */

#include "scoreboard.hh"
#include "ui_scoreboard.h"
#include <algorithm>
#include <fstream>
#include <QDebug>

ScoreBoard::ScoreBoard(std::string& filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScoreBoard),
    filename_(filename) {

    ui->setupUi(this);
    readFile();
}

ScoreBoard::~ScoreBoard() {
    delete ui;
}

bool ScoreBoard::compareScores(const entry_  &a, const entry_ &b) {
    return a.score > b.score;
}

void ScoreBoard::readFile() {
    std::vector< entry_ > entries;
    std::ifstream file(filename_);

    // Create new leaderboard if not found
    if ( !file ) {
        qDebug() << "Error opening leaderboard";
    }

    std::string row;

    while ( getline(file, row) ) {
        std::vector< std::string > parts = split(row, ':');

        entries.push_back( { QString::fromStdString(parts.at(0)),
                             std::stoi(parts.at(3)),
                             std::stoi(parts.at(1)),
                             std::stoi(parts.at(2)) } );
    }

    file.close();

    std::sort(entries.begin(), entries.end(), compareScores);

    int i = 1;
    for ( entry_ e : entries ) {
        QString entry = QString("%1. %2 %3 points in %4 min and %5 s.")
                        .arg(QString::number(i))
                        .arg(e.name.leftJustified(20, ' '))
                        .arg(QString::number(e.score))
                        .arg(QString::number(e.min))
                        .arg(QString::number(e.s));

        ui->listWidget->addItem(new QListWidgetItem(entry));

        ++i;
    }
}

std::vector< std::string > ScoreBoard::split(const std::string& s,
                                             const char delimiter,
                                             bool ignore_empty /* = false*/) {

    std::vector< std::string > result;
    std::string tmp = s;

    while ( tmp.find(delimiter) != std::string::npos ) {
        std::string new_part = tmp.substr(0, tmp.find(delimiter));
        tmp = tmp.substr(tmp.find(delimiter)+1, tmp.size());

        if( not (ignore_empty and new_part.empty()) ) {
            result.push_back(new_part);
        }
    }

    if( not (ignore_empty and tmp.empty()) ) {
        result.push_back(tmp);
    }

    return result;
}
