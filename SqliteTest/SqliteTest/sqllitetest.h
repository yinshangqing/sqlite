#ifndef SQLLITETEST_H
#define SQLLITETEST_H

#include <QtWidgets/QMainWindow>
#include "ui_sqllitetest.h"

class SqlLiteTest : public QMainWindow
{
	Q_OBJECT

public:
	SqlLiteTest(QWidget *parent = 0);
	~SqlLiteTest();

private:
	Ui::SqlLiteTestClass ui;
};

#endif // SQLLITETEST_H
