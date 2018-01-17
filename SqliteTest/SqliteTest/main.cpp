#include "sqllitetest.h"
#include <QtWidgets/QApplication>
#include <QCoreApplication>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QVariantList>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//添加数据库
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
	database.setDatabaseName("GoodYun.db");

	//打开数据库
	if(database.open())
	{
		qDebug()<<"database opened !";

		QSqlQuery sql_query;
		QString create_sql = "create table if not exists aut(id int, name varchar(20), address varchar(30))";
		QString insert_sql = "insert into aut values(?, ?, ?)";
		QString delete_sql = "delete from aut where id = 1";
		QString update_sql = "update aut set name = 'haoyunqing' where id = 2";
		QString select_all_sql = "select * from aut";
		QString drop_sql = "drop table aut";

		sql_query.prepare(create_sql);
		
		if(!sql_query.exec())//执行失败返回false
		{
			qDebug()<<"Table Create failed !";
			qDebug()<<"Error_Code: "<<sql_query.lastError();
		}
		else
		{
			qDebug()<<"Table Created Successfully !";

			//插入数据
			sql_query.prepare(insert_sql);

			QVariantList GroupIDs;
			GroupIDs.append(0);
			GroupIDs.append(1);
			GroupIDs.append(2);

			QVariantList GroupNames;
			GroupNames.append("zhangsan");
			GroupNames.append("lisi");
			GroupNames.append("wangwu");

			QVariantList GroupAddress;
			GroupAddress.append("beijing");
			GroupAddress.append("shanghai");
			GroupAddress.append("guangzhou");

			sql_query.addBindValue(GroupIDs);
			sql_query.addBindValue(GroupNames);
			sql_query.addBindValue(GroupAddress);

			if(!sql_query.execBatch())
			{
				qDebug()<<"Error_Code: "<<sql_query.lastError();
			}
			else
			{
				qDebug()<<"Insert into Table Successfully !";
			}

			//查询所有记录
			sql_query.prepare(select_all_sql);
			if(!sql_query.exec())
			{
				qDebug()<<"Error_Code: "<<sql_query.lastError();
			}
			else
			{
				while(sql_query.next())
				{
					int id = sql_query.value(0).toInt();
					QString name = sql_query.value(1).toString();
					QString address = sql_query.value(2).toString();
					qDebug()<<QString("ID:%1 Name:%2 Address:%3").arg(id).arg(name).arg(address);
				}
			}

			//删除某条数据
			sql_query.prepare(delete_sql);
			if(!sql_query.exec())
			{
				qDebug()<<"delete fail !";
				qDebug()<<"Error_Code: "<<sql_query.lastError();
			}
			else
			{
				//删除之后再查询一遍
				sql_query.prepare(select_all_sql);
				if(!sql_query.exec())
				{
					qDebug()<<"Error_Code: "<<sql_query.lastError();
				}
				else
				{
					qDebug()<<"delete success ! ";
					while(sql_query.next())
					{
						int id = sql_query.value(0).toInt();
						QString name = sql_query.value(1).toString();
						QString address = sql_query.value(2).toString();
						qDebug()<<QString("ID:%1 Name:%2 Address:%3").arg(id).arg(name).arg(address);
					}
				}
			}

			//更新数据
			sql_query.prepare(update_sql);
			if(!sql_query.exec())
			{
				qDebug()<<"update fail !";
				qDebug()<<"Error_Code: "<<sql_query.lastError();
			}
			else
			{
				//更新之后再查询一遍
				sql_query.prepare(select_all_sql);
				if(!sql_query.exec())
				{
					qDebug()<<"Error_Code: "<<sql_query.lastError();
				}
				else
				{
					qDebug()<<"update success ! ";
					while(sql_query.next())
					{
						int id = sql_query.value(0).toInt();
						QString name = sql_query.value(1).toString();
						QString address = sql_query.value(2).toString();
						qDebug()<<QString("ID:%1 Name:%2 Address:%3").arg(id).arg(name).arg(address);
					}
				}
			}

			//删除表
			sql_query.prepare(drop_sql);
			if(!sql_query.exec())
			{
				qDebug()<<"drop fail ! ";
				qDebug()<<"Error_Code: "<<sql_query.lastError();
			}
			else
			{
				sql_query.prepare(select_all_sql);
				if(!sql_query.exec())
				{
					qDebug()<<"drop success ! ";
				}
				else
				{
					qDebug()<<"drop fail ! ";
				}
			}
		}
		database.close();
	}
	SqlLiteTest w;
	w.show();
	return a.exec();
}
