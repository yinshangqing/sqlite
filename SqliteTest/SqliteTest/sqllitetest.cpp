#include "sqllitetest.h"
#include <QCoreApplication>
#include <QTextCodec>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QVariantList>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QTime>

SqlLiteTest::SqlLiteTest(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//添加数据库
	db.setHostName("Erric");
	db.setDatabaseName("YSQ.db");
	db.setUserName("yinshangqqing");
	db.setPassword("123456");
	if(db.open())
	{
		qDebug()<<"Database opened success !";
		QSqlQuery query;
		bool success = query.exec("create table if not exists auto\
													(id int primary key,\
													 name varchar(20),\
													address varchar(30))");
		if(success)
		{
			qDebug()<<"table create success !";
		}
		else
		{
			qDebug()<<"table create fail !";
		}
		//查询
		query.exec("select * from auto");
		QSqlRecord rec = query.record();
		qDebug()<<"auto columns count: "<<rec.count();
		//插入记录
		QString insert_sql = "insert into auto values(1,'hao','beijing'),(2,'yun','shanghai'),(3,'qing','guangzhou')";
		QString select_sql = "select * from auto";
		success = query.prepare(insert_sql);
		if(success)
		{
			qDebug()<<"insert table success !";
		}
		else
		{
			qDebug()<<"insert table fail !";
			QSqlError lastError = query.lastError();
			qDebug()<<"lastError: "<<lastError;
		}
		success = query.prepare(select_sql);
		if(success)
		{
			//qDebug()<<"datas: "<<query.prepare(select_sql);
			qDebug()<<"select table success !";
			while(query.next())
			{
				int id = query.value(0).toInt();
				QString name = query.value(1).toString();
				QString address = query.value(2).toString();
				qDebug()<<QString("%1,%2,%3").arg(id).arg(name).arg(address);
			}
		}
		else
		{
			qDebug()<<"select table fail !";
			QSqlError lastError = query.lastError();
			qDebug()<<"lastError: "<<lastError;
		}
	}
	db.close();
	
}

SqlLiteTest::~SqlLiteTest()
{

}
