#include "sqlite.h"

sqlite::sqlite(QObject *parent)
    : QObject(parent)
{

}

sqlite::~sqlite()
{

}

bool sqlite::Open_DB()
{
    if (m_db.isOpen())
    {
        return false;
    }
    if (QSqlDatabase::contains("sqlitedb2"))
    {
        m_db = QSqlDatabase::database("sqlitedb2");
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE","sqlitedb2");
    }

    QString qstrpath = QCoreApplication::applicationDirPath() + "/sqlite.db";
    m_db.setDatabaseName(qstrpath);
    if (m_db.open())
    {
        return true;
    }


    QString qstrerror = m_db.lastError().text();
    qDebug() << qstrerror.toLatin1();
    return false;
}

bool sqlite::Open_DB(QString qstrname)
{
    if (m_db.isOpen())
    {
        return false;
    }
    if (QSqlDatabase::contains(qstrname))
    {
        m_db = QSqlDatabase::database(qstrname);
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE",qstrname);
    }

    QString qstrpath = QCoreApplication::applicationDirPath() + "/sqlite.db";
    m_db.setDatabaseName(qstrpath);
    if (m_db.open())
    {
        qDebug() << "open sqlite success";
        return true;
    }

    QString qstrerror = m_db.lastError().text();
    qDebug() << qstrerror.toLatin1();
    return false;
}

void sqlite::Close_DB()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

bool sqlite::ExistFieldDB(QString qstrtablename,QString qstrtablemember)
{
    if (!isExistField(qstrtablename,qstrtablemember))
    {
        qDebug() << qstrtablename << " table is not create !!!";
        // 此处可以开始创建
        Create_Table_User_Info();
        Alter_User_Info_Field();
    }
    else
    {
        qDebug() << qstrtablename << " table is create success!!!";
        // 表已经存在了，不需要创建了
    }
}
/*
 *  id              ---- CHAR(64) NOT NULL ON CONFLICT REPLACE
 *  name            ---- CHAR(64) NOT NULL
 *  telephone       ---- CHAR(64)
 *  datetime        ---- DATETIME
 *  info            ---- CHAR(1024))
 *
 */
bool sqlite::Create_Table_User_Info()
{
    QString strSql = "CREATE TABLE [user_info] ([id] CHAR(64) NOT NULL ON CONFLICT REPLACE, [name] CHAR(64) NOT NULL, [telephone] CHAR(64), [datetime] DATETIME, [info] CHAR(1024))";
    QSqlQuery query(m_db);
    if (query.exec(strSql))
    {
        return true;
    }

    QSqlError xErr = query.lastError();
    int nERr = xErr.type();
    QString strErr = xErr.text();
    qDebug() << strErr << "..." << nERr;
    return false;
}

bool sqlite::Select_User_Info(QString qstrid, QString qstrname, QString qstrphone, QString qstrinfo,QList<QVariant> &qlist)
{
    QSqlQuery query(m_db);
    QString qstrcmd = QString("insert into user_info (id,name,telephone,datetime,info) values ('%1','%2','%3',datetime('now','localtime'),'%4')").arg(qstrid).arg(qstrname).arg(qstrphone).arg(qstrinfo);
    query.prepare(qstrcmd);
    if(query.exec())
    {
        // qlist
        // 此处将数据加入到list表中
        // qlist.push_back(...);
        return true;
    }

    qDebug() << query.lastError();
    return false;
}

bool sqlite::Alter_User_Info_Field(QString qstraddmember)
{
    QSqlQuery query(m_db);
    QString strSql = QString("ALTER TABLE 'user_info' ADD '%1' CHAR(64)").arg(qstraddmember);
    if(query.exec(strSql))
    {
        return true;
    }

    QSqlError xErr = query.lastError();
    int nERr = xErr.type();
    QString strErr = xErr.text();
    qDebug() << xErr;
    return false;
}

