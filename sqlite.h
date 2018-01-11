#ifndef SQLITE_H
#define SQLITE_H

#include <QObject>
#include <QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QCoreApplication>

class sqlite : public QObject
{
    Q_OBJECT
public:
    explicit sqlite(QObject *parent = 0);
    virtual ~sqlite();

    bool Open_DB();
    bool Open_DB(QString);
    void Close_DB();
    bool ExistFieldDB(QString, QString);

    //alarm_info
    bool Create_Table_User_Info();
    bool Insert_User_Info(QString,int,int,int,QString);
    bool Select_User_Info(QString,QString,QString,QList<QVariant>&);
    bool Alter_User_Info_Field();

signals:

public slots:

private:
    QSqlDatabase m_db;
};

#endif // SQLITE_H
