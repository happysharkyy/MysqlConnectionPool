#pragma once
#include <mysql.h>
#include "public.h"
#include <string>
#include <time.h>
using namespace std;

/*
ʵ��mysql�����ݿ����

*/
class Connection
{
public:
	// ��ʼ�����ݿ�����
	Connection();
	// �ͷ����ݿ�������Դ
	~Connection();
	// �������ݿ�
	bool connect(string ip, unsigned short port, string user, string password,
		string dbname);
	
	// ���²��� insert��delete��update
	bool update(string sql);
	// ��ѯ���� select
	MYSQL_RES* query(string sql);
	void refreshAliveTime() { _alivetime = clock(); };//ˢ��һ�����ӵ���ʼ�Ŀ���ʱ���ʱ��
	clock_t getAliceTime() { return clock() - _alivetime; };
private:
	MYSQL *_conn;//��ʾmysql server��һ������
	clock_t _alivetime;//��¼�������״̬��Ĵ��ʱ��
};

