#ifndef JOURNALPAGE_H
#define JOURNALPAGE_H

#include <QMainWindow>
#include <QJsonArray>

namespace Ui {
class JournalPage;
}

class JournalPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit JournalPage(QWidget *parent = nullptr);
    JournalPage(QWidget *parent = nullptr, int rec_id = 0);
    ~JournalPage();
public slots:
    void submitJournal(QJsonArray objects, int class_id);

private:
    Ui::JournalPage *ui;
    QJsonArray m_students;
};

#endif // JOURNALPAGE_H
