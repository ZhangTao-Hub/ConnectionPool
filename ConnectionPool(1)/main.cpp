# include "Connection.h"
# include "ConnectionPool.h"
# include <sstream>
# include <string>
# include <ctime>

int main()
{
	clock_t begin = clock();

	std::thread t1([]() 
		{
			ConnectionPool* cp = ConnectionPool::getInstance();
			for (int i = 0; i < 2500; ++i)
			{
				std::shared_ptr<Connection> sp = cp->getConnection();
				std::stringstream ss;
				ss << "insert into user(name, age, sex) values(\"zhangsan\", 10, \"male\")";
				sp->update(ss.str());
			}
		});

	std::thread t2([]() 
		{
			ConnectionPool* cp = ConnectionPool::getInstance();
			for (int i = 0; i < 2500; ++i)
			{
				std::shared_ptr<Connection> sp = cp->getConnection();
				std::stringstream ss;
				ss << "insert into user(name, age, sex) values(\"zhangsan\", 10, \"male\")";
				sp->update(ss.str());
			}
		});

	std::thread t3([]()
		{
			ConnectionPool* cp = ConnectionPool::getInstance();
			for (int i = 0; i < 2500; ++i)
			{
				std::shared_ptr<Connection> sp = cp->getConnection();
				std::stringstream ss;
				ss << "insert into user(name, age, sex) values(\"zhangsan\", 10, \"male\")";
				sp->update(ss.str());
			}
		});

	std::thread t4([]()
		{
			ConnectionPool* cp = ConnectionPool::getInstance();
			for (int i = 0; i < 2500; ++i)
			{
				std::shared_ptr<Connection> sp = cp->getConnection();
				std::stringstream ss;
				ss << "insert into user(name, age, sex) values(\"zhangsan\", 10, \"male\")";
				sp->update(ss.str());
			}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	
	clock_t end = clock();
	std::cout << (end - begin) << "ms" << std::endl;
	return 0;

#if 0
	clock_t begin = clock();
	for (int i = 0; i < 10000; ++i)
	{
		Connection conn;
		conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
		std::stringstream ss;
		ss << "insert into user(name, age, sex) values(\"zhangsan\", 20, \"male\")";
		conn.update(ss.str());
	}
	clock_t end = clock();
	std::cout << (end - begin) << "ms" << std::endl;
#endif 
}