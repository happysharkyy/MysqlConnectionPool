// CommonConnetionPool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "public.h"
#include "Connection.h"
#include "CommonConnectionPool.h"

using namespace std;
int main()
{
	/*
	Connection conn;
	char sql[1024] = { 0 };
	sprintf(sql,"insert into user(name,age,sex) values('%s',%d,'%s')","zhang san",20,"male");
	conn.connect("127.0.0.1",3307,"root","123456","chat");
	conn.update(sql);
	std::cout << "Hello Wor	ld!\n";
	*/
	//ConnectionPool *cp;
	clock_t begin = clock();
	for (int i = 0; i < 1000; i++) {
		/*
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
		conn.connect("127.0.0.1", 3307, "root", "123456", "chat");
		conn.update(sql);
		*/
		ConnectionPool *cp = ConnectionPool::getConnectionPool();
		shared_ptr<Connection> sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
		sp->update(sql);
	}
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	/*
	thread t1([]() {
		ConnectionPool *cp = ConnectionPool::getConnectionPool();
		
			for (int i = 0; i < 1000; i++) {
				char sql[1024] = { 0 };
				sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
				shared_ptr<Connection> sp = cp->getConnection();
				sp->update(sql);
			}
		});
	thread t2([]() {
			for (int i = 0; i < 1000; i++) {
				char sql[1024] = { 0 };
				sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
				shared_ptr<Connection> sp = cp->getConnection();
				sp->update(sql);
			}
		});
	thread t3([]() {
			for (int i = 0; i < 1000; i++) {
				char sql[1024] = { 0 };
				sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
				shared_ptr<Connection> sp = cp->getConnection();
				sp->update(sql);
			}
		});

	t1.join();
	t2.join();
	t3.join();
	*/
	return 0;
}
