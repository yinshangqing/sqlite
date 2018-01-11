#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    // sqlite 中的事物
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sql.db");
    if(db.open())
    {
        qDebug() << "open success ...";
        QSqlQuery query;
        query.exec("create table student(id int primary key,name varchar(20),age int)");
        QTime *timer=new QTime;
        timer->start();
        if(query.at()<100000)
        {
            db.transaction();   //开始一个事务
            query.prepare("insert into student values(?,?,?)");
            QVariantList uid,uname,age;
            for(int i=1;i<100000;i++)
            {
                uid<<i;
                uname<<QString("%1").arg(qrand());
                age<<20+qrand()%3;
            }
            qDebug()<<"ready!now";
            query.addBindValue(uid);
            query.addBindValue(uname);
            query.addBindValue(age);
            query.execBatch();
            db.commit();    //提交
        }
        qDebug()<<timer->elapsed();
    }
    else
    {
        qDebug() << "open fail ...";
    }


    return a.exec();
}
