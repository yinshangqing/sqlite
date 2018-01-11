#ifndef DATABASE_SQLITE_H
#define DATABASE_SQLITE_H

#include <QObject>
#include <QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

//////////////////////////////////////////////////////////////////////////
enum EM_Node_Type
{
    em_node_unit = 1,//组织机构
    em_node_nvr,//NVR
    em_node_dec,//DEC
    em_node_dvr,//DVR
    em_node_ipc,//IPC
    em_node_child_ipc,//IPC
    em_node_camera,//摄像机
    em_node_probe,//探头
    em_node_ms,//硬媒体
    em_node_enc,//编码器
    em_node_group,//分组
    em_node_alarmhost,//报警主机
    em_node_alarmhost_subsystem,//报警主机子系统
    em_node_alarmhost_zone,//报警主机防区
    em_node_hirsch_door,//赫氏门禁
    em_node_cwg_zone,//丛文防区
    em_node_anson_controller,//安森门禁控制器
    em_node_anson_door,//安森门禁门卡
    em_node_patrol_device,//巡更棒
    em_node_honeywell_alarm_zone,//霍尼韦尔报警防区
    em_node_saat_door,//国密门禁
};
//////////////////////////////////////////////////////////////////////////
enum EM_Device_Protocol_Type
{
    em_dev_sip28181,
    em_dev_onvif,
    em_dev_rtsp,
    em_dev_vsk,
    em_dev_boshi,
    em_dev_ip2000,
    em_dev_dahua,
    em_dev_hik,
    em_dev_localfile,
    em_dev_yushi,
    em_dev_yushi_ex,
    em_dev_hh,
    em_dev_xhy,
    em_dev_samsung,
    em_dev_anson,
    em_dev_patrol,
    em_dev_honeywelll_ip2000,
};
struct TAG_USER_INFO
{
    QString qstruser;
    QString qstrpassword;
    int ntype;
    qint64 llauthority;
    TAG_USER_INFO()
    {
        qstruser = "";
        qstrpassword = "";
        ntype = 1;
        llauthority = 0;
    }
};
Q_DECLARE_METATYPE(TAG_USER_INFO)
//国密门禁在sqlite数据库中的id
struct TAG_SAAT_ID
{
    QString qstrid;
    QString qstrcontrollerid;
    QString qstrdoorid;
    QString qstrdoorname;
    int ntype;
};
Q_DECLARE_METATYPE(TAG_SAAT_ID)
struct TAG_NODE_INFO
{
    QString qstrid;
    int ntype;
    QString qstrfid;
    QString qstrname;
    QString qstrip;
    int nport;
    int nport2;
    QString qstruser;
    QString qstrpassword;
    int nprotocoltype;
    QString qstrrtspmain;
    QString qstrrtspsub;
    int nchannel;
    TAG_NODE_INFO()
    {
        qstrid = "";
        ntype = em_node_nvr;
        qstrfid = "";
        qstrname = "";
        qstrip = "";
        nport = 0;
        nport2 = 5000;
        qstruser = "";
        qstrpassword = "";
        nprotocoltype = em_dev_vsk;
        qstrrtspmain = "";
        qstrrtspsub = "";
        nchannel = 0;
    }
};
Q_DECLARE_METATYPE(TAG_NODE_INFO)
struct TAG_LINK_DEC_INFO
{
    QString qstripcid;
    QString qstrdecid;
    int nscreenno;
    int npresetposition;
    TAG_LINK_DEC_INFO()
    {
        qstripcid = "";
        qstrdecid = "";
        nscreenno = 1;
        npresetposition = 0;
    }
};
Q_DECLARE_METATYPE(TAG_LINK_DEC_INFO)

struct TAG_GROUP_DEVICE_INFO
{
    QString qstrgroupid;
    QString qstripcid;
    TAG_GROUP_DEVICE_INFO()
    {
        qstrgroupid = "";
        qstripcid = "";
    }
};
Q_DECLARE_METATYPE(TAG_GROUP_DEVICE_INFO)

struct TAG_USER_PREMINSSIONS_INFO
{
    QString qstrid;
    int npremissions;
    TAG_USER_PREMINSSIONS_INFO()
    {
        qstrid = "";
        npremissions = 0;
    }
};
Q_DECLARE_METATYPE(TAG_USER_PREMINSSIONS_INFO)

//////////////////////////////////////////////////////////////////////////
struct TAG_ALARM_INFO
{
    QString qstrid;
    int nchannel;
    QString qstrtime;
    int nmaintype;
    int nsubtype;
    QString qstrinfo;
    TAG_ALARM_INFO()
    {
        qstrid = "";
        nchannel = 0;
        qstrtime = "";
        nmaintype = -1;
        nsubtype = -1;
        qstrinfo = "";
    }
};
Q_DECLARE_METATYPE(TAG_ALARM_INFO)

class database_sqlite : public QObject
{
	Q_OBJECT

public:
	database_sqlite(QObject *parent = nullptr);
	virtual ~database_sqlite();

	bool Open_DB();
	bool Open_DB(QString);
	void Close_DB();

	void ExistFieldDB();
	bool isExistTable(const QString& strTableName);
	bool isExistField(const QString& strTableName, const QString& strFieldName);
	bool Create_Table_User_Info();

	//user_info
	bool Select_User_Name(QString);
	bool Select_User_Password(QString,QString&);
	bool Select_User_Type(QString,int&);
	bool Insert_User_Info(QString,QString,int,qint64);
	bool Update_User_Info(QString,QString,int,qint64);
	bool Delete_User_Info(QString);
	bool Select_All_User_Info(QList<QVariant>&);
	bool Select_User_Authority(QString,QVariant&);
	bool Alter_User_Authority_Field();
	//end user_info

	//node_info
	bool Table_Node_Info_Select_Unit_Node(QVector<QVariant>&,QVector<QVariant>&);
	bool Table_Node_Info_Select_Group_Node(QVector<QVariant>&,QVector<QVariant>&);
	bool SelectDeviceNodeFormJoinTwoTableForName(QString,QVector<QVariant>&);
	bool Table_Node_Info_Insert(QVariant);
	bool Table_Node_Info_Update(QVariant);
	bool Table_Saat_Node_Info_Update(QVariant);
	bool Table_Node_Info_Update_Ex(QVariant,QString);
	bool Table_Node_Info_Update_Ex_Ex(QVariant,QString);
	bool Table_Node_Info_Update_Ex_Only_Name(QString,QString);
	bool Table_Node_Info_Select(QString,QVariant&);
	//bool Table_Node_Info_Select(QString,int,int,int,int);
	bool Table_Node_Info_Select(QString,int,int,int,int,int);
	bool Table_Node_Info_Select(QString,int);
	bool Table_Node_Info_Select(QString,int,QString&);
	bool Table_Node_Info_Select(QString,int,int);
	bool Table_Node_Info_Select(QString,int,int,QString&);
	bool Table_Node_Info_Select(int,int);
	bool Table_Node_Info_Select(QString,QString,QString&);
	bool Table_Node_Info_Select(QString,QString,int);
	bool Table_Node_Info_Delete(QString);	
	QString Table_Node_Info_Select(int,QString);
	bool Table_Node_Info_Delete_Father_Node(QString);
	bool Table_Node_Info_Select_Child_Node(QString,QList<QString>&);
	bool Table_Saat_Node_Info_Select(QString,QList<QVariant>&);
	bool Table_Saat_Node_Info_Select(QString,QString,int,QList<QVariant>&);
	QString Table_Node_Info_Select(QString,QString,QString,int);
	//end node_info

	//link_dec_info
	bool Select_Link_Dec_Info(QString,QList<QVariant>&);
	bool Find_Link_Dec_Info(QString,QString,QString,int);
	bool Find_Link_Dec_Info(QString,QString,QString,int,int);
	bool Insert_Link_Dec_Info(QString,QString,QString,int);
	bool Insert_Link_Dec_Info(QString,QString,QString,int,int);
	bool Insert_Link_Dec_Info(QString,QVariant);
	bool Delete_Link_Dec_Info(QString);
	bool Alter_LinkDecInfo_LinkPresetPosition_Field();
	//end link_dec_info

	//group_device_info
	bool Select_Group_Device(QString,QList<QString>&);
	bool SelectGroupDeviceNodeFormJoinTwoTableForName(QString,QString,QList<QString>&);
	bool SelectAllGroupDeviceNodeFormJoinTwoTableForName(QString,QVector<QVariant>&);
	bool Delete_Group(QString);
	bool Insert_Group_Device(QString,QString);
	bool Delete_Group_Device(QString,QString);
	bool Delete_Group_Device(QString);
	//end group_device_info

	//user_premissions_info
	bool Insert_User_Premissions_Device(QString,int,QString);
	bool Select_User_Premissions_Device(QString,QList<QVariant>&);
	bool Select_User_Premissions_Device_ID(QString,QList<QVariant>&);	
	bool Delete_User_Premissions_Device(QString,QString);
	bool Delete_All_User_Premissions_Device(QString);
	bool Delete_User_Premissions(QString);
	//end user_premissions_info

	//alarm_info
	bool Create_Table_Alarm_Info();
	bool Insert_Alarm_Info(QString,int,int,int,QString);
	bool Select_Alarm_Info(QString,QString,QString,QList<QVariant>&);
	bool Select_Alarm_Info(QString,QString,QString,int,QList<QVariant>&);
	bool Select_Alarm_Info(QString,QString,QString,int,int,QList<QVariant>&);
	bool Alter_Alarm_Info_Channel_Field();
	//end alarm_info

	bool Insert_Saat_Door_Info(QString,QString,QString,int);
private:
	QSqlDatabase m_db;
};

#endif // DATABASE_SQLITE_H
