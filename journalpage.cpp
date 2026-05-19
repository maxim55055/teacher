#include "journalpage.h"
#include "ui_journalpage.h"
#include "components/requester/requester.h"
#include "mainwindow.h"

#include <QSettings>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStringListModel>

#include <QDebug>


#include <QCheckBox>
#include <QModelIndex>
#include <QTableWidgetItem>

#include <QGuiApplication>
#include <QScreen>
#include <QTableWidget>

JournalPage::JournalPage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::JournalPage)
{
    ui->setupUi(this);
}


JournalPage::JournalPage(QWidget *parent, int class_id)
    : QMainWindow(parent)
    , ui(new Ui::JournalPage)
{
    ui->setupUi(this);

    QSettings settings("NagaevM", "headmanApp");

    // Удаляем старую таблицу, если она есть
    if (ui->journalTable) {
        delete ui->journalTable;
    }

    // Создаём новую таблицу
    QTableWidget* table = new QTableWidget(this);
    table->setObjectName("journalTable");

    // Добавляем в layout (замени "verticalLayout" на имя твоего layout)
    // Если не знаешь имя layout, используй:
    ui->verticalLayout->addWidget(table);

    // Сохраняем указатель
    ui->journalTable = table;

    // Настройка таблицы
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"ФИО", "Отсутствие"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->verticalHeader()->setVisible(false);
    table->setWordWrap(false);

    ui->lessonName->setStyleSheet("background-color: transparent;");
    ui->themeBrowser->setStyleSheet("background-color: transparent;");

    QString urlreq = "student/journalrec/teacher/%1?token=%2";
    Requester* r = new Requester(this);
    r->initRequester("10.0.2.2", 8081, nullptr);

    r->sendRequest(urlreq.arg(class_id).arg(settings.value("auth_token").toString()),
                   [this, table, r](const QJsonObject data) {
                       QJsonArray dataArr = data["data"].toArray();
                       qDebug() << "Students count:" << dataArr.size();

                       // Очищаем m_students
                       m_students = QJsonArray();

                       // Устанавливаем количество строк
                       table->setRowCount(dataArr.size());

                       for (int i = 0; i < dataArr.size(); i++) {
                           QJsonObject dataObj = dataArr[i].toObject();
                           m_students.append(dataObj);

                           // Устанавливаем имя в колонку 0
                           QTableWidgetItem* item = new QTableWidgetItem(dataObj["name"].toString());
                           item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                           table->setItem(i, 0, item);

                           // Создаём чекбокс для колонки 1
                           QCheckBox* checkBox = new QCheckBox();
                           checkBox->setStyleSheet("margin-left: 15px; margin-right: 15px;");
                           table->setCellWidget(i, 1, checkBox);

                           if(dataObj["absent"].toInt() == 1) {
                               checkBox->setChecked(true);
                           }

                           // Подключаем сигнал
                           int studentId = dataObj["id"].toInt();
                           connect(checkBox, &QCheckBox::checkStateChanged, [this, studentId, checkBox](int state) {

                               // Обновляем статус в m_students
                               for (int j = 0; j < m_students.size(); j++) {
                                   QJsonObject obj = m_students[j].toObject();
                                   if (obj["id"].toInt() == studentId) {
                                       obj["absent"] = checkBox->isChecked() ? 1 : 0;
                                       m_students[j] = obj;
                                       break;
                                   }
                               }
                               qDebug() << m_students;
                           });
                       }

                       // Настройка ширины колонок
                       table->setColumnWidth(0, 250);
                       table->setColumnWidth(1, 80);
                       table->resizeRowsToContents();

                       ui->lessonName->setText(data["name"].toString());
                       r->deleteLater();
                   },
                   [r](const QJsonObject data) {
                       qDebug() << "Error:" << data;
                       r->deleteLater();
                   },
                   Requester::Type::GET,
                   QVariantMap()
                   );

    /*connect(ui->sumbitButton, &QPushButton::clicked, this, [this, class_id]() {
        QJsonArray dataArr = m_students;
        QSettings settings("NagaevM", "headmanApp");
        QJsonObject data;
        data["token"] = settings.value("auth_token").toString();
        data["class_id"] = class_id;
        data["theme"] = ui->theme->toPlainText();
        data["data"] = dataArr;

        qDebug() << data;

        QString urlreq = "journal/fill";
        Requester *r = new Requester();
        r->initRequester("10.0.2.2", 8081, nullptr);
        r->sendRequest(urlreq,
                [=] (const QJsonObject data) {
                qDebug() << data;
                r->deleteLater();
            },
            [=] (const QJsonObject data) {
                qDebug() << data;
                qDebug() << "Всё, мне было лень делать нормальную обработку ошибки, хавай это";
                r->deleteLater();
            }, Requester::Type::POST, data.toVariantMap());
        MainWindow* w = new MainWindow();
        w->show();
        this->hide();
    });*/
    connect(ui->sumbitButton, &QPushButton::clicked, this, [this, class_id]() {
        QJsonArray dataArr = m_students;
        QSettings settings("NagaevM", "headmanApp");

        QJsonObject data;
        data["token"] = settings.value("auth_token").toString();
        data["class_id"] = class_id;
        data["comment"] = ui->comment->toPlainText();  // ← проверь имя
        data["students"] = dataArr;  // ← обязательно "students", не "data"

        qDebug() << "=== SENDING TO SERVER ===";
        qDebug() << QJsonDocument(data).toJson().toStdString().c_str();

        Requester *r = new Requester(this);
        r->initRequester("10.0.2.2", 8081, nullptr);
        r->sendRequest("/journal/fill",
                       [=](const QJsonObject response) {
                           qDebug() << "✅ Success:" << response;
                           r->deleteLater();
                           MainWindow* w = new MainWindow();
                           w->show();
                           this->hide();
                       },
                       [=](const QJsonObject error) {
                           qDebug() << "❌ Error:" << error;
                           r->deleteLater();
                       },
                       Requester::Type::POST,
                       data.toVariantMap()
                       );
    });
}

JournalPage::~JournalPage()
{
    delete ui;
}

void JournalPage::submitJournal(QJsonArray objects, int class_id) {

}