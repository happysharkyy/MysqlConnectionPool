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
	//获取连接池对象实例
	static ConnectionPool* getConnectionPool();
	//给外部提供接口，从连接池中获取一个可用的空闲连接
	shared_ptr<Connection>  getConnection();

private:
	ConnectionPool();//单例 构造函数私有化
	bool LoadConfigFile();//加载配置文件
	void produceConnectionTask();//运行在独立的线程中，专门负责生产新连接
	void scannerConnectionTask();//扫描连接的线程函数


	string _ip;
	unsigned short _port;
	string _username;
	string _password;
	string _dbname;
	int _initSize;//连接池初始连接量
	int _maxSize;//最大连接量
	int _maxIdleTime;//最大空闲时间
	int _maxConnectionTimeOut;//获取连接超时时间


	queue<Connection*> _connectionQue;//存储mysql连接的队列
	mutex _queueMutex;//维护连接队列的线程安全互斥锁
	atomic_int _connectionCount;//记录所创建的connection连接的总数量
	condition_variable cv;//设置条件变量,用户连接生产线程和连接消费线程通信

};