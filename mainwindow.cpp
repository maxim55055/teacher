#include <QSettings>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStringListModel>

#include <QDebug>

#include <QJsonArray>

#include <QPushButton>
#include <QModelIndex>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "components/requester/requester.h"
#include "config.h"
#include <QGuiApplication>
#include <QScreen>

#include "journalpage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("NagaevM", "headmanApp");
    ui->studentName->setStyleSheet("background-color: transparent;");
    ui->classesTable->setStyleSheet("background-color: transparent;");
    ui->studentName->setText(settings.value("name").toString());

    QString urlreq = "classes/today/teacher/%1";
    Requester* r = new Requester();
    r->initRequester("10.0.2.2", 8081, nullptr);
    r->sendRequest(urlreq.arg(settings.value("auth_token").toString()),
    [this, r](const QJsonObject &data) {
        QJsonArray dataArr = data["data"].toArray();

        if (dataArr.size() == 0) {
            QStandardItemModel* model = new QStandardItemModel(1, 1, this);
            model->setItem(0, 0, new QStandardItem("Занятий нет"));
            return;
        }
        QStandardItemModel* model = new QStandardItemModel(dataArr.size() ,3, this);
        for (int i =0; i < dataArr.size(); i++) {
            QJsonObject lessonObj = dataArr[i].toObject();
            model->setItem(i, 0, new QStandardItem(lessonObj["time"].toString()));
            //model->setItem(i, 1, new QStandardItem(lessonObj["subject"].toString()+"\n"+lessonObj["teacher"].toString()));
            model->setItem(i, 2, new QStandardItem(lessonObj["place"].toString()));
        }

        model->setHeaderData(0, Qt::Horizontal, "Время");
        model->setHeaderData(1, Qt::Horizontal, "Дисциплина\nпреподаватель");
        model->setHeaderData(2, Qt::Horizontal, "Аудитория");

        ui->classesTable->setModel(model);

        for (int i =0; i < dataArr.size(); i++) {
            QJsonObject lessonObj = dataArr[i].toObject();
            QPushButton *button = new QPushButton(lessonObj["subject"].toString()+"\n"+lessonObj["teacher"].toString(), this);
            QModelIndex classesIndex = model->index(i, 1);
            ui->classesTable->setIndexWidget(classesIndex, button);
            int id = lessonObj.value("id").toInt();
            connect(button, &QPushButton::clicked, [=]() {
                JournalPage* journal= new JournalPage(nullptr, lessonObj["id"].toInt());
                this->hide();
                journal->show();
            });
        }
        ui->classesTable->resizeColumnsToContents();
        ui->classesTable->verticalHeader()->setDefaultSectionSize(50);

        r->deleteLater();
    },
    [this, r](const QVariant &data) {
        qDebug() << data << "-----------------";
                QStandardItemModel* model = new QStandardItemModel(1, 1, this);
                model->setItem(0, 0, new QStandardItem("Сервак упал"));
                ui->classesTable->setModel(model);
                r->deleteLater();
    },
    Requester::Type::GET, QVariantMap());

}

MainWindow::~MainWindow()
{
    delete ui;
}