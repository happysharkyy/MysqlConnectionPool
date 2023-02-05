#pragma once


#include <string>
#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <functional>
#include <condition_variable>
#include "Connection.h"


using namespace std;
class ConnectionPool
{
public:
	//��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();
	//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
	shared_ptr<Connection>  getConnection();

private:
	ConnectionPool();//���� ���캯��˽�л�
	bool LoadConfigFile();//���������ļ�
	void produceConnectionTask();//�����ڶ������߳��У�ר�Ÿ�������������
	void scannerConnectionTask();//ɨ�����ӵ��̺߳���


	string _ip;
	unsigned short _port;
	string _username;
	string _password;
	string _dbname;
	int _initSize;//���ӳس�ʼ������
	int _maxSize;//���������
	int _maxIdleTime;//������ʱ��
	int _maxConnectionTimeOut;//��ȡ���ӳ�ʱʱ��


	queue<Connection*> _connectionQue;//�洢mysql���ӵĶ���
	mutex _queueMutex;//ά�����Ӷ��е��̰߳�ȫ������
	atomic_int _connectionCount;//��¼��������connection���ӵ�������
	condition_variable cv;//������������,�û����������̺߳����������߳�ͨ��

};