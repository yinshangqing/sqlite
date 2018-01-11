#include "database_sqlite.h"
#include <QtCore/QtCore>
#include <QDebug>

bool g_bflag_remote_db;
QString g_qstr_mysql_ip;
QString g_qstr_mysql_port;
QString g_qstr_mysql_user;
QString g_qstr_mysql_password;

database_sqlite::database_sqlite(QObject *parent)
	: QObject(parent)
{

}

database_sqlite::~database_sqlite()
{
	Close_DB();
}

bool database_sqlite::Open_DB()
{
	if (m_db.isOpen())
	{
		return false;
	}

	if (g_bflag_remote_db)
	{
		if (QSqlDatabase::contains("mysqldb"))
		{
			m_db = QSqlDatabase::database("mysqldb");
		}
		else
		{
			m_db = QSqlDatabase::addDatabase("QMYSQL","mysqldb");
		}

		m_db.setHostName(g_qstr_mysql_ip);
		m_db.setPort(g_qstr_mysql_port.toInt());
		m_db.setDatabaseName("test");
		m_db.setUserName(g_qstr_mysql_user);
		m_db.setPassword(g_qstr_mysql_password);	
		if (m_db.open())  
		{
			return true;
		}
	} 
	else
	{
		if (QSqlDatabase::contains("sqlitedb2"))
		{
			m_db = QSqlDatabase::database("sqlitedb2");
		}
		else
		{
			m_db = QSqlDatabase::addDatabase("QSQLITE","sqlitedb2");
		}
		
		QString qstrpath = QCoreApplication::applicationDirPath() + "/client.db";
		m_db.setDatabaseName(qstrpath);
		if (m_db.open())  
		{
			return true;
		}  
	}
	
	QString qstrerror = m_db.lastError().text();
	qDebug() << qstrerror.toLatin1();
	return false;
}

bool database_sqlite::Open_DB(QString qstrname)
{
	if (m_db.isOpen())
	{
		return false;
	}

	if (g_bflag_remote_db)
	{
		if (QSqlDatabase::contains(qstrname))
		{
			m_db = QSqlDatabase::database(qstrname);
		}
		else
		{
			m_db = QSqlDatabase::addDatabase("QMYSQL",qstrname);
		}

		m_db.setHostName(g_qstr_mysql_ip);
		m_db.setPort(g_qstr_mysql_port.toInt());
		m_db.setDatabaseName("test");
		m_db.setUserName(g_qstr_mysql_user);
		m_db.setPassword(g_qstr_mysql_password);	
		if (m_db.open())  
		{
			qDebug() << "open mysql success";
			return true;
		}
	} 
	else
	{
		if (QSqlDatabase::contains(qstrname))
		{
			m_db = QSqlDatabase::database(qstrname);
		}
		else
		{
			m_db = QSqlDatabase::addDatabase("QSQLITE",qstrname);
		}

		QString qstrpath = QCoreApplication::applicationDirPath() + "/client.db";
		m_db.setDatabaseName(qstrpath);
		if (m_db.open())  
		{
			qDebug() << "open sqlite success";
			return true;
		}  
	}

	QString qstrerror = m_db.lastError().text();
	qDebug() << qstrerror.toLatin1();
	return false;
}

void database_sqlite::Close_DB()
{
	if (m_db.isOpen())
	{
		m_db.close();
	}
}

void database_sqlite::ExistFieldDB()
{
	if (g_bflag_remote_db)
	{
	} 
	else
	{
		if (!isExistField("user_info","authority"))
		{
			Alter_User_Authority_Field();
		}

		if (!isExistField("link_dec_info","link_preset_position"))
		{
			Alter_LinkDecInfo_LinkPresetPosition_Field();
		}

		// !isExistTable("alarm_info") 判断失败, 返回 Driver not loaded
		if (!isExistField("alarm_info","channel"))
		{
			Create_Table_Alarm_Info();
			Alter_Alarm_Info_Channel_Field();
		}
	}
}
//判断数据库中是否存在该表 
bool database_sqlite::isExistTable(const QString& strTableName)  
{
	QSqlQuery query;  
	QString strSql = QString("SELECT 1 FROM sqlite_master where type = 'table' and  name = '%1'").arg(strTableName);  
	bool bRet = query.exec(strSql);  
	if(query.next())  
	{
		int nResult = query.value(0).toInt();//有表时返回1，无表时返回null  
		if(nResult)  
		{  
			return true;  
		}  
	}  

	QSqlError xErr = query.lastError();
	int nERr = xErr.type();
	QString strErr = xErr.text();
	qDebug() << xErr;
	return false;  
} 
//判断表中是否含有某字段
bool database_sqlite::isExistField(const QString& strTableName, const QString& strFieldName)
{  
	QSqlQuery query(m_db);  
	QString strSql = QString("SELECT 1 FROM sqlite_master where type = 'table' and name= '%1' and sql like '%%2%'").arg(strTableName).arg(strFieldName);
	query.exec(strSql);
	if(query.next())  
	{  
		int nResult = query.value(0).toInt();//有此字段时返回1，无字段时返回null  
		if(nResult)  
		{  
			return true;  
		}  
	}  
	
	qDebug() << query.lastError();
	return false;  
}
//创建用户信息表
bool database_sqlite::Create_Table_User_Info()  
{
	QString strSql = "CREATE TABLE [user_info] ([name] CHAR(256) NOT NULL ON CONFLICT REPLACE, [password] CHAR(64) NOT NULL ON CONFLICT REPLACE DEFAULT 12345, [type] INT(4) NOT NULL ON CONFLICT REPLACE DEFAULT 1, [authority] INT64 DEFAULT 0)";  
	QSqlQuery query(m_db);
	if (query.exec(strSql))
	{
		return true;
	}	
	
	qDebug() << query.lastError();
	return false;  
}
//创建报警信息表
bool database_sqlite::Create_Table_Alarm_Info()
{
	QString strSql = "CREATE TABLE [alarm_info] ([id] CHAR(64) NOT NULL ON CONFLICT REPLACE,[channel] INT64 DEFAULT 0, [datetime] DATETIME, [root_type] INT64 DEFAULT 0,[child_type] INT64 DEFAULT 0, [info] CHAR(1024))";
	QSqlQuery query(m_db);
	if (query.exec(strSql))
	{
		return true;
	}	

	QSqlError xErr = query.lastError();
	int nERr = xErr.type();
	QString strErr = xErr.text();
	qDebug() << xErr;
	return false;  
}
/************************************************************************/
/* 用户信息表的操作接口 */
/************************************************************************/
//插入权限字段
bool database_sqlite::Alter_User_Authority_Field()
{
	QSqlQuery query(m_db);  
	QString strSql = QString("ALTER TABLE 'user_info' ADD 'authority' INT64 DEFAULT 0");
	if(query.exec(strSql))  
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询所有的用户信息
bool database_sqlite::Select_All_User_Info(QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from user_info");
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			QString qstruser = query.value(0).toString();
			QString qstrpassword = query.value(1).toString();
			int ntype = query.value(2).toInt();
			qint64 llauthority = query.value(3).toLongLong();

			TAG_USER_INFO tag;
			tag.qstruser = qstruser;
			tag.qstrpassword = qstrpassword;
			tag.ntype = ntype;
			tag.llauthority = llauthority;

			tlist.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
bool database_sqlite::Table_Saat_Node_Info_Select(QString qstrcontrollerid,QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select id from node_info where rtsp_main=%1").arg(qstrcontrollerid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			QString qstrid = query.value(0).toString();

			TAG_SAAT_ID tag;
			tag.qstrid = qstrid;

			tlist.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}

bool database_sqlite::Table_Saat_Node_Info_Select(QString qstrcontrollerid,QString qstrdoorid,int ntype,QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select id,name from node_info where rtsp_main='%1' and rtsp_sub='%2' and type='%3'").arg(qstrcontrollerid).arg(qstrdoorid).arg(ntype);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			QString qstrid = query.value(0).toString();
			QString qstrname = query.value(1).toString();
			
			TAG_SAAT_ID tag;
			tag.qstrid = qstrid;
			tag.qstrcontrollerid = qstrcontrollerid;
			tag.qstrdoorid = qstrdoorid;
			tag.qstrdoorname = qstrname;
			tag.ntype = ntype;

			tlist.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询指定用户信息
bool database_sqlite::Select_User_Authority(QString qstrname,QVariant& qvar)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from user_info where name = '%1'").arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			QString qstruser = query.value(0).toString();
			QString qstrpassword = query.value(1).toString();
			int ntype = query.value(2).toInt();
			qint64 llauthority = query.value(3).toLongLong();

			TAG_USER_INFO tag;
			tag.qstruser = qstruser;
			tag.qstrpassword = qstrpassword;
			tag.ntype = ntype;
			tag.llauthority = llauthority;

			qvar = QVariant::fromValue(tag);
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询用户名称是否存在
bool database_sqlite::Select_User_Name(QString qstrname)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select name from user_info where name = '%1'").arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
//根据用户名查询用户密码
bool database_sqlite::Select_User_Password(QString qstrname,QString& qstrpassword)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select password from user_info where name = '%1'").arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			qstrpassword = query.value(0).toString();
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//根据用户名查询用户类型
bool database_sqlite::Select_User_Type(QString qstrname,int& ntype)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select type from user_info where name = '%1'").arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			ntype = query.value(0).toInt();
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//插入用户信息
bool database_sqlite::Insert_User_Info(QString qstrname,QString qstrpassword,int ntype,qint64 llauthority)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("insert into user_info (name,password,type,authority) values ('%1','%2','%3','%4')").arg(qstrname).arg(qstrpassword).arg(ntype).arg(llauthority);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//根据用户名更新用户信息
bool database_sqlite::Update_User_Info(QString qstrname,QString qstrpassword,int ntype,qint64 llauthority)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("update user_info set password = '%1',type = '%2',authority = '%3' where name = '%4'").arg(qstrpassword).arg(ntype).arg(llauthority).arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}
	else
	{
		qDebug() << query.lastError();
	}

	return false;
}
//根据用户名删除指定用户
bool database_sqlite::Delete_User_Info(QString qstrname)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from user_info where name = '%1'").arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{			
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
/************************************************************************/
/* 节点信息表的操作接口 */
/************************************************************************/
//查询所有的根组织节点和子组织节点
bool database_sqlite::Table_Node_Info_Select_Unit_Node(QVector<QVariant>& qvecfatherunit,QVector<QVariant>& qvecchildunit)
{
	qvecfatherunit.clear();
	qvecchildunit.clear();

	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where type = %1").arg(em_node_unit);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			QString qstrid = query.value(0).toString();		
			if (qstrid.isEmpty())
			{
				continue;
			}

			QString qstrfid = query.value(1).toString();
			QString qstrname = query.value(2).toString();
			QString qstrip = query.value(3).toString();
			int nport = query.value(4).toInt();
			int nchannel = query.value(5).toInt();
			int ntype = query.value(6).toInt();
			int nport2 = query.value(7).toInt();
			QString qstruser = query.value(8).toString();
			QString qstrpassword = query.value(9).toString();
			int nprotocoltype = query.value(10).toInt();
			QString qstrrtspmain = query.value(11).toString();
			QString qstrrtspsub = query.value(12).toString();

			TAG_NODE_INFO tag;
			tag.qstrid = qstrid;
			tag.ntype = ntype;
			tag.qstrfid = qstrfid;
			tag.qstrname = qstrname;
			tag.qstrip = qstrip;
			tag.nport = nport;
			tag.nport2 = nport2;
			tag.qstruser = qstruser;
			tag.qstrpassword = qstrpassword;
			tag.nprotocoltype = nprotocoltype;
			tag.qstrrtspmain = qstrrtspmain;
			tag.qstrrtspsub = qstrrtspsub;
			tag.nchannel = nchannel;

			if (qstrfid.isEmpty())
			{
				qvecfatherunit.push_back(QVariant::fromValue(tag));
			}
			else
			{
				qvecchildunit.push_back(QVariant::fromValue(tag));
			}		
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询所有的组节点
bool database_sqlite::Table_Node_Info_Select_Group_Node(QVector<QVariant>& qvecfathergroup,QVector<QVariant>& qvecchildgroup)
{
	qvecfathergroup.clear();
	qvecchildgroup.clear();

	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where type = %1").arg(em_node_group);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			QString qstrid = query.value(0).toString();		
			if (qstrid.isEmpty())
			{
				continue;
			}

			QString qstrfid = query.value(1).toString();
			QString qstrname = query.value(2).toString();
			QString qstrip = query.value(3).toString();
			int nport = query.value(4).toInt();
			int nchannel = query.value(5).toInt();
			int ntype = query.value(6).toInt();
			int nport2 = query.value(7).toInt();
			QString qstruser = query.value(8).toString();
			QString qstrpassword = query.value(9).toString();
			int nprotocoltype = query.value(10).toInt();
			QString qstrrtspmain = query.value(11).toString();
			QString qstrrtspsub = query.value(12).toString();

			TAG_NODE_INFO tag;
			tag.qstrid = qstrid;
			tag.ntype = ntype;
			tag.qstrfid = qstrfid;
			tag.qstrname = qstrname;
			tag.qstrip = qstrip;
			tag.nport = nport;
			tag.nport2 = nport2;
			tag.qstruser = qstruser;
			tag.qstrpassword = qstrpassword;
			tag.nprotocoltype = nprotocoltype;
			tag.qstrrtspmain = qstrrtspmain;
			tag.qstrrtspsub = qstrrtspsub;
			tag.nchannel = nchannel;

			if (tag.qstrfid.isEmpty())
			{
				qvecfathergroup.push_back(QVariant::fromValue(tag));
			}
			else
			{
				qvecchildgroup.push_back(QVariant::fromValue(tag));
			}
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//联合节点表和权限表查询当前用户的权限设备
bool database_sqlite::SelectDeviceNodeFormJoinTwoTableForName(QString qstrname,QVector<QVariant>& qvec)
{
	qvec.clear();

	QSqlQuery query(m_db);
	QString qstrcmd;
	if (qstrname == "admin")
	{
		qstrcmd = QString("select * from node_info where type != %1 and type != %2").arg(em_node_unit).arg(em_node_group);
	}
	else
	{
		qstrcmd = QString("select a.* from node_info AS a join user_premissions_info AS b where a.id = b.id and b.name = '%1'").arg(qstrname);
	}
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			QString qstrid = query.value(0).toString();	
			if (qstrid.isEmpty())
			{
				continue;
			}

			QString qstrfid = query.value(1).toString();
			QString qstrname = query.value(2).toString();
			QString qstrip = query.value(3).toString();
			int nport = query.value(4).toInt();
			int nchannel = query.value(5).toInt();
			int ntype = query.value(6).toInt();
			int nport2 = query.value(7).toInt();
			QString qstruser = query.value(8).toString();
			QString qstrpassword = query.value(9).toString();
			int nprotocoltype = query.value(10).toInt();
			QString qstrrtspmain = query.value(11).toString();
			QString qstrrtspsub = query.value(12).toString();

			TAG_NODE_INFO tag;
			tag.qstrid = qstrid;
			tag.ntype = ntype;
			tag.qstrfid = qstrfid;
			tag.qstrname = qstrname;
			tag.qstrip = qstrip;
			tag.nport = nport;
			tag.nport2 = nport2;
			tag.qstruser = qstruser;
			tag.qstrpassword = qstrpassword;
			tag.nprotocoltype = nprotocoltype;
			tag.qstrrtspmain = qstrrtspmain;
			tag.qstrrtspsub = qstrrtspsub;
			tag.nchannel = nchannel;

			qvec.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//插入节点
bool database_sqlite::Table_Node_Info_Insert(QVariant qvar)
{
	TAG_NODE_INFO tag = qvar.value<TAG_NODE_INFO>();
	QString qstrid = tag.qstrid;
	QString qstrname = tag.qstrname;
	QString qstrfid = tag.qstrfid;
	int ntype = tag.ntype;
	QString qstrip = tag.qstrip;
	int nport1 = tag.nport;
	int nport2 = tag.nport2;
	int nprotocoltype = tag.nprotocoltype;
	QString qstruser = tag.qstruser;
	QString qstrpassword = tag.qstrpassword;
	QString qstrmainrtsp = tag.qstrrtspmain;
	QString qstrsubrtsp = tag.qstrrtspsub;
	int nchannel = tag.nchannel;

	QSqlQuery query(m_db);
	QString qstrcmd = QString("insert into node_info (id,name,fid,type,ip,port,port2,user,password,protocol,rtsp_main,rtsp_sub,channel) values ('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13')").arg(qstrid).arg(qstrname).arg(qstrfid).arg(ntype).arg(qstrip).arg(nport1).arg(nport2).arg(qstruser).arg(qstrpassword).arg(nprotocoltype).arg(qstrmainrtsp).arg(qstrsubrtsp).arg(nchannel);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//更新国密门禁节点信息
bool database_sqlite::Table_Saat_Node_Info_Update(QVariant qvar)
{
	TAG_NODE_INFO tag = qvar.value<TAG_NODE_INFO>();
	QString qstrid = tag.qstrid;
	QString qstrfid = tag.qstrfid;
	QString qstrip = tag.qstrip;
	int nport = tag.nport;
	int nport2 = tag.nport2;
	int ntype = tag.ntype;
	int nprotocoltype = tag.nprotocoltype;
	QString qstruser = tag.qstruser;
	QString qstrpassword = tag.qstrpassword;
	QString qstrmainrtsp = tag.qstrrtspmain;
	QString qstrsubrtsp = tag.qstrrtspsub;
	int nchannel = tag.nchannel;

	QSqlQuery query(m_db);
	QString qstrcmd = QString("update node_info set fid='%1',ip='%2',port='%3',channel='%4',port2='%5',user='%6',password='%7',protocol='%8' where rtsp_main='%9' and rtsp_sub='%10' and type='%11'")
					.arg(qstrfid).arg(qstrip).arg(nport).arg(nchannel).arg(nport2).arg(qstruser).arg(qstrpassword).arg(nprotocoltype).arg(qstrmainrtsp).arg(qstrsubrtsp).arg(ntype);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//更新节点信息,名称不变,通道不变,父节点不变
bool database_sqlite::Table_Node_Info_Update_Ex(QVariant qvar,QString qstrid)
{
	TAG_NODE_INFO tag = qvar.value<TAG_NODE_INFO>();
	int ntype = tag.ntype;
	QString qstrip = tag.qstrip;
	int nport1 = tag.nport;
	int nport2 = tag.nport2;
	QString qstruser = tag.qstruser;
	QString qstrpassword = tag.qstrpassword;
	int nprotocoltype = tag.nprotocoltype;
	QString qstrmainrtsp = tag.qstrrtspmain;
	QString qstrsubrtsp = tag.qstrrtspsub;

	QSqlQuery query(m_db);
	QString qstrcmd = QString("update node_info set type = '%1',port = '%2',port2 = '%3',user = '%4',password = '%5',protocol = '%6',rtsp_main = '%7',rtsp_sub = '%8',ip = '%9' where id = '%10'").arg(ntype).arg(nport1).arg(nport2).arg(qstruser).arg(qstrpassword).arg(nprotocoltype).arg(qstrmainrtsp).arg(qstrsubrtsp).arg(qstrip).arg(qstrid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//更新节点信息,名称不变,父节点不变
bool database_sqlite::Table_Node_Info_Update_Ex_Ex(QVariant qvar,QString qstrid)
{
	TAG_NODE_INFO tag = qvar.value<TAG_NODE_INFO>();
	int ntype = tag.ntype;
	QString qstrip = tag.qstrip;
	int nport1 = tag.nport;
	int nport2 = tag.nport2;
	QString qstruser = tag.qstruser;
	QString qstrpassword = tag.qstrpassword;
	int nprotocoltype = tag.nprotocoltype;
	int nchannel = tag.nchannel;
	QString qstrsubrtsp = tag.qstrrtspsub;

	QSqlQuery query(m_db);
	QString qstrcmd = QString("update node_info set type = '%1',port = '%2',port2 = '%3',user = '%4',password = '%5',protocol = '%6',rtsp_sub = '%7',ip = '%8',channel = '%9' where id = '%10'").arg(ntype).arg(nport1).arg(nport2).arg(qstruser).arg(qstrpassword).arg(nprotocoltype).arg(qstrsubrtsp).arg(qstrip).arg(nchannel).arg(qstrid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//根据ID更新节点信息
bool database_sqlite::Table_Node_Info_Update(QVariant qvar)
{
	TAG_NODE_INFO tag = qvar.value<TAG_NODE_INFO>();	
	QString qstrid = tag.qstrid;
	QString qstrname = tag.qstrname;
	QString qstrfid = tag.qstrfid;
	int ntype = tag.ntype;
	QString qstrip = tag.qstrip;
	int nport1 = tag.nport;
	int nport2 = tag.nport2;
	QString qstruser = tag.qstruser;
	QString qstrpassword = tag.qstrpassword;
	int nprotocoltype = tag.nprotocoltype;
	QString qstrmainrtsp = tag.qstrrtspmain;
	QString qstrsubrtsp = tag.qstrrtspsub;
	int nchannel = tag.nchannel;

	QSqlQuery query(m_db);
	QString qstrcmd = QString("update node_info set name = '%1',fid = '%2',type = '%3',ip = '%4',port = '%5',port2 = '%6',user = '%7',password = '%8',protocol = '%9',rtsp_main = '%10',rtsp_sub = '%11',channel = '%12' where id = '%13'").arg(qstrname).arg(qstrfid).arg(ntype).arg(qstrip).arg(nport1).arg(nport2).arg(qstruser).arg(qstrpassword).arg(nprotocoltype).arg(qstrmainrtsp).arg(qstrsubrtsp).arg(nchannel).arg(qstrid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//更新节点的名称
bool database_sqlite::Table_Node_Info_Update_Ex_Only_Name(QString qstrid,QString qstrname)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("update node_info set name = '%1' where id = '%2'").arg(qstrname).arg(qstrid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//通过ID获取节点信息
bool database_sqlite::Table_Node_Info_Select(QString qstrid,QVariant& qvar)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where id = '%1'").arg(qstrid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			QString qstrid = query.value(0).toString();			
			QString qstrfid = query.value(1).toString();
			QString qstrname = query.value(2).toString();
			QString qstrip = query.value(3).toString();
			int nport = query.value(4).toInt();
			int nchannel = query.value(5).toInt();
			int ntype = query.value(6).toInt();
			int nport2 = query.value(7).toInt();
			QString qstruser = query.value(8).toString();
			QString qstrpassword = query.value(9).toString();
			int nprotocoltype = query.value(10).toInt();
			QString qstrrtspmain = query.value(11).toString();
			QString qstrrtspsub = query.value(12).toString();

			TAG_NODE_INFO tag;
			tag.qstrid = qstrid;
			tag.ntype = ntype;
			tag.qstrfid = qstrfid;
			tag.qstrname = qstrname;
			tag.qstrip = qstrip;
			tag.nport = nport;
			tag.nport2 = nport2;
			tag.qstruser = qstruser;
			tag.qstrpassword = qstrpassword;
			tag.nprotocoltype = nprotocoltype;
			tag.qstrrtspmain = qstrrtspmain;
			tag.qstrrtspsub = qstrrtspsub;
			tag.nchannel = nchannel;

			qvar = QVariant::fromValue(tag);

			return true;
		}		
	}

	qDebug() << query.lastError();
	return false;
}
//根据父节点ID和通道查询ID信息
bool database_sqlite::Table_Node_Info_Select(QString qstrfid,int nchannel,QString& qstrid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select id from node_info where fid = '%1' and channel = '%2'").arg(qstrfid).arg(nchannel);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			qstrid = query.value(0).toString();

			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
//针对宇视不判断channel,判断rescode
bool database_sqlite::Table_Node_Info_Select(QString qstrfid,QString qstrrescode,QString& qstrid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select id from node_info where fid = '%1' and rtsp_main = '%2'").arg(qstrfid).arg(qstrrescode);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			qstrid = query.value(0).toString();

			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
//根据父节点ID,通道号,类型查询ID信息
bool database_sqlite::Table_Node_Info_Select(QString qstrfid,int nchannel,int ntype,QString& qstrid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where fid = '%1' and type = '%2' and channel = '%3'").arg(qstrfid).arg(ntype).arg(nchannel);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			qstrid = query.value(0).toString();

			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
//根据父节点ID,通道号,类型查询节点是否存在
bool database_sqlite::Table_Node_Info_Select(QString qstrfid,int nchannel,int ntype)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where fid = '%1' and type = '%2' and channel = '%3'").arg(qstrfid).arg(ntype).arg(nchannel);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
//根据IP地址和设备类型来匹配节点是否存在
// bool database_sqlite::Table_Node_Info_Select(QString qstrip,int ntype1,int ntype2,int ntype3,int ntype4)
// {
// 	QSqlQuery query(m_db);
// 	QString qstrcmd = QString("select * from node_info where ip = '%1' and (type = '%2' or type = '%3' or type = '%4' or type = '%5')").arg(qstrip).arg(ntype1).arg(ntype2).arg(ntype3).arg(ntype4);
// 	query.prepare(qstrcmd);
// 	if(query.exec())
// 	{
// 		if(query.next())
// 		{
// 			return true;
// 		}
// 	}
// 
// 	qDebug() << query.lastError();
// 	return false;
// }
//根据IP地址和设备类型来匹配节点是否存在
bool database_sqlite::Table_Node_Info_Select(QString qstrip,int ntype1,int ntype2,int ntype3,int ntype4,int ntype5)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where ip = '%1' and (type = '%2' or type = '%3' or type = '%4' or type = '%5' or type = '%6')").arg(qstrip).arg(ntype1).arg(ntype2).arg(ntype3).arg(ntype4).arg(ntype5);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
QString database_sqlite::Table_Node_Info_Select(int nchannel,QString qstrfid)
{
	QSqlQuery query(m_db);
	QString qstrid;
	QString qstrcmd = QString("select * from node_info where fid = '%1' and channel = '%2'").arg(qstrfid).arg(nchannel);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			qstrid = query.value(0).toString();
		}
	}

	qDebug() << query.lastError();
	return qstrid;
}
bool database_sqlite::Table_Node_Info_Select(QString qstrip,int ntype)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where ip = '%1' and type = '%2'").arg(qstrip).arg(ntype);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}

//根据通道号,类型查询节点是否存在
bool database_sqlite::Table_Node_Info_Select(int nchannel,int ntype)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where type = '%1' and channel = '%2'").arg(ntype).arg(nchannel);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}

QString database_sqlite::Table_Node_Info_Select(QString qstrfield,QString qstrmain,QString qstrsub,int ntype)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select %1 from node_info where type = '%2' and rtsp_main = '%3' and rtsp_sub = '%4'").arg(qstrfield).arg(ntype).arg(qstrmain).arg(qstrsub);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			return query.value(0).toString();
		}
	}

	qDebug() << query.lastError();
	return "";
}

bool database_sqlite::Table_Node_Info_Select(QString qstrControllerID,QString qstrDoorID,int ntype)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from node_info where type = '%1' and rtsp_main = '%2' and rtsp_sub = '%3'").arg(ntype).arg(qstrControllerID).arg(qstrDoorID);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
//删除节点信息
bool database_sqlite::Table_Node_Info_Delete(QString qstrid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from node_info where id = '%1'").arg(qstrid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//删除拥有子节点的设备时,连带删除所有子节点
bool database_sqlite::Table_Node_Info_Delete_Father_Node(QString qstrid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from node_info where id = '%1' or fid = '%2'").arg(qstrid).arg(qstrid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询所有子节点的ID
bool database_sqlite::Table_Node_Info_Select_Child_Node(QString qstrfid,QList<QString>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select id from node_info where fid = '%1'").arg(qstrfid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			tlist.push_back(query.value(0).toString());
		}
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
/************************************************************************/
/* 联动信息表的操作接口 */
/************************************************************************/
////插入预置位字段
bool database_sqlite::Alter_LinkDecInfo_LinkPresetPosition_Field()
{
	QSqlQuery query(m_db);  
	QString strSql = QString("ALTER TABLE 'link_dec_info' ADD 'link_preset_position' INT DEFAULT 0");
	if(query.exec(strSql))  
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询当前IPC的联动信息
bool database_sqlite::Select_Link_Dec_Info(QString qstripcid,QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select link_ipc_id,link_dec_id,link_dec_screen,link_preset_position from link_dec_info where ipc_id = '%1'").arg(qstripcid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			QString qstrlinkipcid = query.value(0).toString();
			QString qstrlinkdecid = query.value(1).toString();
			int nlinkdecscreen = query.value(2).toInt();
			int npresetposition = query.value(3).toInt();

			TAG_LINK_DEC_INFO tag;
			tag.qstripcid = qstrlinkipcid;
			tag.qstrdecid = qstrlinkdecid;
			tag.nscreenno = nlinkdecscreen;
			tag.npresetposition = npresetposition;

			tlist.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//删除联动信息
bool database_sqlite::Delete_Link_Dec_Info(QString qstripcid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from link_dec_info where ipc_id = '%1'").arg(qstripcid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//插入联动信息
bool database_sqlite::Insert_Link_Dec_Info(QString qstripcid,QString qstrlinkipcid,QString qstrlinkdecid,int nscreenno)
{
	if (Find_Link_Dec_Info(qstripcid,qstrlinkipcid,qstrlinkdecid,nscreenno))
	{
		return true;
	} 

	QSqlQuery query(m_db);
	QString qstrcmd = QString("insert into link_dec_info (ipc_id,link_ipc_id,link_dec_id,link_dec_screen) values ('%1','%2','%3','%4')").arg(qstripcid).arg(qstrlinkipcid).arg(qstrlinkdecid).arg(nscreenno);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
bool database_sqlite::Insert_Link_Dec_Info(QString qstripcid,QString qstrlinkipcid,QString qstrlinkdecid,int nscreenno,int npresetposition)
{
	if (Find_Link_Dec_Info(qstripcid,qstrlinkipcid,qstrlinkdecid,nscreenno,npresetposition))
	{
		return true;
	} 

	QSqlQuery query(m_db);
	QString qstrcmd = QString("insert into link_dec_info (ipc_id,link_ipc_id,link_dec_id,link_dec_screen,link_preset_position) values ('%1','%2','%3','%4','%5')").arg(qstripcid).arg(qstrlinkipcid).arg(qstrlinkdecid).arg(nscreenno).arg(npresetposition);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}

bool database_sqlite::Insert_Link_Dec_Info(QString qstripcid,QVariant qvar)
{
	TAG_LINK_DEC_INFO tag = qvar.value<TAG_LINK_DEC_INFO>();
	QString qstrlinkipcid = tag.qstripcid;
	QString qstrlinkdecid = tag.qstrdecid;
	int nscreenno = tag.nscreenno;
	int npresetposition = tag.npresetposition;

	if (Find_Link_Dec_Info(qstripcid,qstrlinkipcid,qstrlinkdecid,nscreenno,npresetposition))
	{
		return true;
	} 

	QSqlQuery query(m_db);
	QString qstrcmd = QString("insert into link_dec_info (ipc_id,link_ipc_id,link_dec_id,link_dec_screen,link_preset_position) values ('%1','%2','%3','%4','%5')").arg(qstripcid).arg(qstrlinkipcid).arg(qstrlinkdecid).arg(nscreenno).arg(npresetposition);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询联动信息
bool database_sqlite::Find_Link_Dec_Info(QString qstripcid,QString qstrlinkipcid,QString qstrlinkdecid,int nscreenno)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from link_dec_info where ipc_id = '%1' and link_ipc_id = '%2' and link_dec_id = '%3' and link_dec_screen = '%4'").arg(qstripcid).arg(qstrlinkipcid).arg(qstrlinkdecid).arg(nscreenno);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
bool database_sqlite::Find_Link_Dec_Info(QString qstripcid,QString qstrlinkipcid,QString qstrlinkdecid,int nscreenno,int npresetposition)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from link_dec_info where ipc_id = '%1' and link_ipc_id = '%2' and link_dec_id = '%3' and link_dec_screen = '%4' and link_preset_position = '%5'").arg(qstripcid).arg(qstrlinkipcid).arg(qstrlinkdecid).arg(nscreenno).arg(npresetposition);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		if(query.next())
		{
			return true;
		}
	}

	qDebug() << query.lastError();
	return false;
}
/************************************************************************/
/* 分组表的操作接口 */
/************************************************************************/
//删除指定分组下的所有设备
bool database_sqlite::Delete_Group(QString qstrgroupid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from group_device_info where id = '%1'").arg(qstrgroupid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//删除指定分组下的设备
bool database_sqlite::Delete_Group_Device(QString qstrgroupid,QString qstripcid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from group_device_info where id = '%1' and ipc_id = '%2'").arg(qstrgroupid).arg(qstripcid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//删除所有分组下的指定设备
bool database_sqlite::Delete_Group_Device(QString qstripcid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from group_device_info where ipc_id = '%1'").arg(qstripcid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询指定分组下的所有设备ID
bool database_sqlite::Select_Group_Device(QString qstrgroupid,QList<QString>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select ipc_id from group_device_info where id = '%1'").arg(qstrgroupid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			QString qstripcid = query.value(0).toString();		
			tlist.push_back(qstripcid);
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//插入指定设备到指定分组
bool database_sqlite::Insert_Group_Device(QString qstrgroupid,QString qstripcid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("insert into group_device_info (id,ipc_id) values ('%1','%2')").arg(qstrgroupid).arg(qstripcid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//联合<分组表>和<权限列表>查询指定用户权限下的指定分组节点下的所有设备
bool database_sqlite::SelectGroupDeviceNodeFormJoinTwoTableForName(QString qstrname,QString qstrgroupid,QList<QString>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd;
	if (qstrname == "admin")
	{
		qstrcmd = QString("select ipc_id from group_device_info where id = '%1'").arg(qstrgroupid);
	}
	else
	{
		qstrcmd = QString("select a.ipc_id from group_device_info AS a join user_premissions_info AS b where a.ipc_id = b.id and a.id = '%1' and b.name = '%2'").arg(qstrgroupid).arg(qstrname);
	}	
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			QString qstripcid = query.value(0).toString();		
			tlist.push_back(qstripcid);
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//联合<分组表>和<权限表>查询指定用户权限下的所有分组节点下的设备
bool database_sqlite::SelectAllGroupDeviceNodeFormJoinTwoTableForName(QString qstrname,QVector<QVariant>& qvec)
{
	qvec.clear();

	QSqlQuery query(m_db);
	QString qstrcmd;
	if (qstrname == "admin")
	{
		qstrcmd = QString("select * from group_device_info");
	}
	else
	{
		qstrcmd = QString("select a.id,a.ipc_id from group_device_info AS a join user_premissions_info AS b where a.ipc_id = b.id and b.name = '%1'").arg(qstrname);
	}	
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while(query.next())
		{
			QString qstrgroupid = query.value(0).toString();
			QString qstripcid = query.value(1).toString();

			TAG_GROUP_DEVICE_INFO tag;
			tag.qstrgroupid = qstrgroupid;
			tag.qstripcid = qstripcid;

			qvec.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
/************************************************************************/
/* 设备权限表的操作接口 */
/************************************************************************/
//插入用户管理的设备
bool database_sqlite::Insert_User_Premissions_Device(QString qstrname,int npreminssinos,QString qstrid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("insert into user_premissions_info (id,premissions,name) values ('%1','%2','%3')").arg(qstrid).arg(npreminssinos).arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询指定用户管理的所有设备
bool database_sqlite::Select_User_Premissions_Device(QString qstrname,QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select * from user_premissions_info where name = '%1'").arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			QString qstrid = query.value(0).toString();			
			int npremissions = query.value(1).toInt();

			TAG_USER_PREMINSSIONS_INFO tag;
			tag.qstrid = qstrid;
			tag.npremissions = npremissions;

			tlist.push_back(QVariant::fromValue(tag));			
		}		

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询指定用户管理的所有设备ID
bool database_sqlite::Select_User_Premissions_Device_ID(QString qstrname,QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("select id from user_premissions_info where name = '%1'").arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			tlist.push_back(query.value(0));			
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//删除指定用户的管理设备
bool database_sqlite::Delete_User_Premissions_Device(QString qstrname,QString qstrid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from user_premissions_info where id = '%1' and name = '%2'").arg(qstrid).arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//删除所有用户管理的指定设备
bool database_sqlite::Delete_All_User_Premissions_Device(QString qstrid)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from user_premissions_info where id = '%1'").arg(qstrid);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//删除指定用户管理的所有设备
bool database_sqlite::Delete_User_Premissions(QString qstrname)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("delete from user_premissions_info where name = '%1'").arg(qstrname);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//插入报警信息
bool database_sqlite::Insert_Alarm_Info(QString qstrid,int nchannel,int nroottype,int nchildtype,QString qstrinfo)
{
	QSqlQuery query(m_db);
	QString qstrcmd = QString("insert into alarm_info (id,channel,datetime,root_type,child_type,info) values ('%1','%2',datetime('now','localtime'),'%3','%4','%5')").arg(qstrid).arg(nchannel).arg(nroottype).arg(nchildtype).arg(qstrinfo);
	query.prepare(qstrcmd);
	if(query.exec())
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;
}
//查询报警信息
bool database_sqlite::Select_Alarm_Info(QString qstrid,QString qstrstime,QString qstretime,QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd;
	if (qstrid.isEmpty())
	{
		qstrcmd = QString("select id,channel,datetime,root_type,child_type,info from alarm_info where datetime > '%2' and datetime < '%3'").arg(qstrstime).arg(qstretime);
	}
	else
	{
		qstrcmd = QString("select id,channel,datetime,root_type,child_type,info from alarm_info where id = '%1' and datetime > '%2' and datetime < '%3'").arg(qstrid).arg(qstrstime).arg(qstretime);
	}
	
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			TAG_ALARM_INFO tag;
			tag.qstrid = query.value(0).toString();
			tag.nchannel = query.value(1).toInt();
			tag.qstrtime = query.value(2).toString();
			tag.nmaintype = query.value(3).toInt();
			tag.nsubtype = query.value(4).toInt();
			tag.qstrinfo = query.value(5).toString();

			tlist.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}

bool database_sqlite::Select_Alarm_Info(QString qstrid,QString qstrstime,QString qstretime,int nroottype,QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd;
	if (qstrid.isEmpty())
	{
		qstrcmd = QString("select id,channel,datetime,root_type,child_type,info from alarm_info where root_type = '%2' and datetime > '%3' and datetime < '%4'").arg(nroottype).arg(qstrstime).arg(qstretime);
	}
	else
	{
		qstrcmd = QString("select id,channel,datetime,root_type,child_type,info from alarm_info where id = '%1' and root_type = '%2' and datetime > '%3' and datetime < '%4'").arg(qstrid).arg(nroottype).arg(qstrstime).arg(qstretime);
	}
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			TAG_ALARM_INFO tag;
			tag.qstrid = query.value(0).toString();
			tag.nchannel = query.value(1).toInt();
			tag.qstrtime = query.value(2).toString();
			tag.nmaintype = query.value(3).toInt();
			tag.nsubtype = query.value(4).toInt();
			tag.qstrinfo = query.value(5).toString();

			tlist.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}

bool database_sqlite::Select_Alarm_Info(QString qstrid,QString qstrstime,QString qstretime,int nroottype,int nchildtype,QList<QVariant>& tlist)
{
	QSqlQuery query(m_db);
	QString qstrcmd;
	if (qstrid.isEmpty())
	{
		qstrcmd = QString("select id,channel,datetime,root_type,child_type,info from alarm_info where root_type = '%2' and child_type = '%3' and datetime > '%4' and datetime < '%5'").arg(nroottype).arg(nchildtype).arg(qstrstime).arg(qstretime);
	}
	else
	{
		qstrcmd = QString("select id,channel,datetime,root_type,child_type,info from alarm_info where id = '%1' and root_type = '%2' and child_type = '%3' and datetime > '%4' and datetime < '%5'").arg(qstrid).arg(nroottype).arg(nchildtype).arg(qstrstime).arg(qstretime);
	}
	query.prepare(qstrcmd);
	if(query.exec())
	{
		while (query.next())
		{
			TAG_ALARM_INFO tag;
			tag.qstrid = query.value(0).toString();
			tag.nchannel = query.value(1).toInt();
			tag.qstrtime = query.value(2).toString();
			tag.nmaintype = query.value(3).toInt();
			tag.nsubtype = query.value(4).toInt();
			tag.qstrinfo = query.value(5).toString();

			tlist.push_back(QVariant::fromValue(tag));
		}

		return true;
	}

	qDebug() << query.lastError();
	return false;
}

//插入通道字段
bool database_sqlite::Alter_Alarm_Info_Channel_Field()
{
	QSqlQuery query(m_db);  
	QString strSql = QString("ALTER TABLE 'alarm_info' ADD 'channel' INT64 DEFAULT 0");
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

//插入国密门信息进sqlite
bool database_sqlite::Insert_Saat_Door_Info(QString qstrcontrollerid,QString qstrdoorid,QString qstrdoorname,int ntype)
{
	QSqlQuery query(m_db);  
	QString qstruuid = QUuid::createUuid().toString().remove(QRegExp("[{,},-]"));
	QString qstrcmd = QString("insert into node_info values('%1','','%2','','','','%3','','','','','%4','%5')").arg(qstruuid).arg(qstrdoorname).arg(ntype).arg(qstrcontrollerid).arg(qstrdoorid);
	query.prepare(qstrcmd);
	if(query.exec())  
	{
		return true;
	}

	qDebug() << query.lastError();
	return false;  
}
