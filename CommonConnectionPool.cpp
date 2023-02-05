#include "CommonConnectionPool.h"
#include "public.h"



//线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool;



	return &pool;
}
bool ConnectionPool::LoadConfigFile() {
	FILE *pf = fopen("mysql.cnf", "r");
	if (pf == nullptr)
	{
		LOG("mysql.ini file is  no exist");
		return false;
	}
	while (!feof(pf))
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int index = str.find('=', 0);//0 是offset
		if (index == -1)//无效的配置项
		{
			continue;
		}
		int endidx = str.find('\n', index);
		string key = str.substr(0, index);
		string value = str.substr(index + 1, endidx - index - 1);
		cout << "key:" << key << "value" << value <<endl;
		if (key == "ip")
		{
			_ip == value;
		}
		else if(key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "maxConnectionTimeOut")
		{
			_maxConnectionTimeOut = atoi(value.c_str());
		}
	}
	return true;
}

//连接池的构造
ConnectionPool::ConnectionPool()
{
	//加载配置项
	if (!LoadConfigFile()) {
		return ;
	}
	//创建初始数量的连接
	for (int i = 0; i < _initSize; ++i)
	{
		Connection *p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();
		_connectionQue.push(p);
		_connectionCount++;

	}

	//启动一个新的线程,作为连接的生产者
	//为什么这里需要一个用到绑定器去传递连接池对象 
	//假如这里不用bind函数返回一个函数指针 不同线程独立运行的时候怎么生产连接
	//要想访问到produceConnectionTask必须传递ConnectionPool指针 否则无法调用到该方法
	thread product(std::bind(&ConnectionPool::produceConnectionTask, this));
	product.detach();

	//启动一个新的定时线程，扫描超过maxIdleTime时间的空闲线程，进行对于连接的回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();



}
void ConnectionPool::scannerConnectionTask() {
	for (;;) {

		//多少秒扫描一次
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));
		//扫描整个队列，释放多余连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCount > _initSize)
		{
			Connection *p = _connectionQue.front();
			if (p->getAliceTime() >= (_maxIdleTime*1000))
			{
				_connectionQue.pop();
				_connectionCount--;
				delete p;//释放连接
			}
			else
			{
				break;//队头的连接没有超过maxIdleTime其他连接肯定也没有超时
			}
		}

	}
}
shared_ptr<Connection> ConnectionPool::getConnection() {

	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		if (cv_status::timeout == cv.wait_for(lock, chrono::microseconds(_maxConnectionTimeOut))) 
		{
			if (_connectionQue.empty()) {
				LOG("获取空闲连接超时！");
				return nullptr;
			}
		}
		
	}
	/*
	shared_ptr智能指针析构时 会把connection资源直接delete掉 
	相当于调用connection的析构函数，connection就被close掉
	这里需要自定义share_ptr的释放资源的方式 把connection直接归还到queue中
	*/
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection *pcon) {
			//这里是在服务器的应用线程中调用的，所以一定要考虑队列的线程安全问题
			unique_lock<mutex> lock(_queueMutex);
			pcon->refreshAliveTime();
			_connectionQue.push(pcon);
		});//出队列
	_connectionQue.pop();
	if (_connectionQue.empty())//其实不判断也可以
	{
		cv.notify_all();//谁消费了队列中的最后一个connection 谁负责通知生产者生产
	}
	return sp;

}
void ConnectionPool::produceConnectionTask() {
	for (;;)
	{
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock);//队列不空，此处生产线程进入等待状态
		}
		//连接数量没有到达上限
		if (_connectionCount < _maxSize)
		{
			Connection *p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();
			_connectionQue.push(p);
			_connectionCount++;

		}

		//通知消费者线程消费连接
		cv.notify_all();
	}
}