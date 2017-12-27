#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase(DRIVER);
    db.setHostName("localhost");
    db.setDatabaseName("database/book-catalogue.db");

    if (!db.open())
    {
       qDebug() << "Error: connection with database fail";
       exit(1);
    }
    else
    {
       qDebug() << "Database: connection ok";
    }

    initBooks(defaultQuery);
    AddGenreRefine("SELECT DISTINCT CAT FROM YRB_BOOK ORDER BY CAT", ui->genreLayout);
    AddLanguageRefine("SELECT DISTINCT LANGUAGE FROM YRB_BOOK ORDER BY LANGUAGE", ui->languageLayout);

    yearCount = 0;
    genreCount = 0;
    languageCount = 0;

    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(yearButtonClicked(bool)));
    connect(ui->checkBox_2, SIGNAL(toggled(bool)), this, SLOT(yearButtonClicked(bool)));
    connect(ui->checkBox_3, SIGNAL(toggled(bool)), this, SLOT(yearButtonClicked(bool)));
    connect(ui->checkBox_4, SIGNAL(toggled(bool)), this, SLOT(yearButtonClicked(bool)));
    connect(ui->checkBox_5, SIGNAL(toggled(bool)), this, SLOT(yearButtonClicked(bool)));
    connect(ui->checkBox_6, SIGNAL(toggled(bool)), this, SLOT(yearButtonClicked(bool)));
}

Dialog::~Dialog()
{
    db.close();
    delete ui;
}

void Dialog::initBooks(QString Query)
{
    QSqlQuery qry;
    if (qry.exec(Query))
    {
        int row = 0;
        ui->tableWidget->setRowCount(row);
        while (qry.next())
        {
            QSqlRecord record = qry.record();
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
            for (int i = 0; i < record.count(); i++)
            {
               QTableWidgetItem *itm = new QTableWidgetItem(record.value(i).toString());
               ui->tableWidget->setItem(row, i, itm);
            }
            row++;
        }
    }
}

void Dialog::AddGenreRefine(QString Query, QVBoxLayout *box)
{
    QSqlQuery qry;
    if (qry.exec(Query))
    {
        while (qry.next())
        {
            QCheckBox *refinement = new QCheckBox(qry.value(0).toString(), this);
            connect(refinement, SIGNAL(toggled(bool)), this, SLOT(refineBox(bool)));
            box->addWidget(refinement);
        }
    }
    QSpacerItem *item = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    box->addSpacerItem(item);
}

void Dialog::AddLanguageRefine(QString Query, QVBoxLayout *box)
{
    QSqlQuery qry;
    if (qry.exec(Query))
    {
        while (qry.next())
        {
            QCheckBox *refinement = new QCheckBox(qry.value(0).toString(), this);
            connect(refinement, SIGNAL(toggled(bool)), this, SLOT(refineBox_2(bool)));
            box->addWidget(refinement);
        }
    }
    QSpacerItem *item = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    box->addSpacerItem(item);
}

void Dialog::refineBox(bool toggled)
{
    QCheckBox *box = (QCheckBox *)sender();
    QString refinement = box->text();

    if (toggled)
    {
        if (genreCount > 0)
        {
            genreQuery.append( " OR ");
        }
        genreQuery.append("[cat] = '" + refinement + "'");
        genreCount++;
    }
    else
    {
        if (genreCount > 0 && genreQuery.contains(" OR [cat] = '" + refinement + "'"))
        {
            genreQuery.remove(" OR [cat] = '" + refinement + "'");
        }
        else if (genreCount > 0 && genreQuery.contains("[cat] = '" + refinement + "' OR"))
        {
            genreQuery.remove("[cat] = '" + refinement + "' OR ");
        }
        else
        {
            //genreQuery.remove("[cat] = '" + refinement + "'");
            genreQuery = "";
        }
        genreCount--;
    }
   // qDebug() << genreQuery;
}

void Dialog::refineBox_2(bool toggled)
{
    QCheckBox *box = (QCheckBox *)sender();
    QString refinement = box->text();

    if (toggled)
    {
        if (languageCount > 0)
        {
            languageQuery.append( " OR ");
        }
        languageQuery.append("[language] = '" + refinement + "'");
        languageCount++;
    }
    else
    {
        if (languageCount > 0 && languageQuery.contains(" OR [language] = '" + refinement + "'"))
        {
            languageQuery.remove(" OR [language] = '" + refinement + "'");
        }
        else if (languageCount > 0 && languageQuery.contains("[language] = '" + refinement + "' OR"))
        {
            languageQuery.remove("[language] = '" + refinement + "' OR ");
        }
        else
        {
            languageQuery = "";
        }
        languageCount--;
    }
    //qDebug() << languageQuery;
}

void Dialog::on_searchButton_clicked()
{
    QString tempQuery = defaultQuery;
    if (ui->lineEdit->text() != "")
    {
        if (ui->comboBox->currentText() == "Title")
        {
            tempQuery.append(" AND [title] LIKE '%" + ui->lineEdit->text() + "%'");
        }
        else
        {
            tempQuery.append(" AND [author] LIKE '%" + ui->lineEdit->text() + "%'");
        }
    }
    if (!yearQuery.isEmpty())
    {
        tempQuery.append(" AND (" + yearQuery + ")");
    }
    if (!genreQuery.isEmpty())
    {
        tempQuery.append(" AND (" + genreQuery + ")");
    }
    if (!languageQuery.isEmpty())
    {
        tempQuery.append(" AND (" + languageQuery + ")");
    }
    //qDebug() << tempQuery;
    initBooks(tempQuery);
}

void Dialog::yearButtonClicked(bool toggled)
{
    QCheckBox *btn = (QCheckBox *)sender();
    QString text = btn->text();
    QStringList list = text.split("-");

    QString year1 = list.first();
    QString year2 = list.last();

    if (toggled)
    {
        if (yearCount > 0)
        {
            yearQuery.append(" OR ");
        }
        yearQuery.append("([year] >= " + year1 + "AND [year] <= " + year2 + ")");
        yearCount++;
    }
    else
    {
        if (yearCount > 0 && yearQuery.contains(" OR ([year] >= " + year1 + "AND [year] <= " + year2 + ")"))
        {
            yearQuery.remove(" OR ([year] >= " + year1 + "AND [year] <= " + year2 + ")");
        }
        else if (yearCount > 0 && yearQuery.contains("([year] >= " + year1 + "AND [year] <= " + year2 + ") OR"))
        {
            yearQuery.remove("([year] >= " + year1 + "AND [year] <= " + year2 + ") OR");
        }
        else
        {
            //yearQuery.remove("([year] >= " + year1 + "AND [year] <= " + year2 + ")");
            yearQuery = "";
        }
        yearCount--;
    }
    //qDebug() << yearQuery;
}

void Dialog::on_checkBox_7_toggled(bool checked)
{
    if (checked)
    {
        if (yearCount > 0)
        {
            yearQuery.append(" OR ");
        }
        yearQuery.append("[year] < 1941");
        yearCount++;
    }
    else
    {
        if (yearCount > 0 && yearQuery.contains(" OR [year] < 1941"))
        {
            yearQuery.remove(" OR [year] < 1941");
        }
        else if (yearCount > 0 && yearQuery.contains("[year] < 1941 OR "))
        {
            yearQuery.remove("[year] < 1941 OR ");
        }
        else
        {
            yearQuery = "";
        }
        yearCount--;
    }
    //qDebug() << yearQuery;
}
