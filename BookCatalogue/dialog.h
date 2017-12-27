#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtSql>
#include <QtCore>
#include <QVBoxLayout>
//#include <QtWidgets>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

    void initBooks(QString Query);
    void AddGenreRefine(QString Query,QVBoxLayout *box);
    void AddLanguageRefine(QString Query,QVBoxLayout *box);

public:
    explicit Dialog(QWidget *parent = 0);
    const QString DRIVER = "QSQLITE";
    const QString defaultQuery = "SELECT * FROM YRB_BOOK WHERE 1 = 1";
    QString yearQuery;
    QString genreQuery;
    QString languageQuery;

    int yearCount;
    int genreCount;
    int languageCount;

    ~Dialog();

private:
    Ui::Dialog *ui;
    QSqlDatabase db;

private slots:
    void refineBox(bool toggled);
    void refineBox_2(bool toggled);
    void on_searchButton_clicked();
    void yearButtonClicked(bool toggled);
    void on_checkBox_7_toggled(bool checked);
};

#endif // DIALOG_H
