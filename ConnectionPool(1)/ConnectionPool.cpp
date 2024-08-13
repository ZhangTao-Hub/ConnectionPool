#include "ConnectionPool.h"


// 实现一个全局唯一，且线程安全的懒汉单例模式实现的线程池对象
ConnectionPool* ConnectionPool::getInstance()
{
	static ConnectionPool instance;					// 局部变量创建,自动lock和unlock
	return &instance;
}

// 消费者线程，消费可用连接
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
	// 1、拿锁
	std::unique_lock<std::mutex> lock(_mutexQue);
	// 2、连接队列为空，等待生产，最大等待时间为_connectionTimeOut毫秒
	while (_connectionQue.empty())
	{
		if (std::cv_status::timeout == cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeOut)))
		{
			if (_connectionQue.empty())
			{
				LOG("连接超时。。。连接失败");
				return nullptr;
			}
		}
	}

	// 智能指针包裹，并自定义删除器
	std::shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection* p) {
		std::unique_lock<std::mutex> lock(_mutexQue);
		_connectionQue.push(p);
		++_connectionCnt;
		});
	_connectionQue.pop();

	// 唤醒生产者线程，若连接队列为空，及时生产可用连接
	cv.notify_all();
	return sp;
}

// 生产者线程，专门负责生产可用连接
void ConnectionPool::produce()
{
	while (1)
	{
		// 1、拿锁
		std::unique_lock<std::mutex> lock(_mutexQue);
		// 2、连接队列不空，线程等待并释放锁
		while (!_connectionQue.empty())
		{
			cv.wait(lock);
		}

		// 3、连接队列为空，生产连接
		while(_connectionCnt < _maxConnectionSize)
		{
			Connection* p = new Connection();
			if (p->connect(_host, _port, _user, _passwd, _dbName))
			{
				_connectionQue.push(p);
				++_connectionCnt;
			}
		}

		// 4、通知消费
		cv.notify_all();
	}
}

// 扫描线程，用于释放超过最大空闲时间的连接
void ConnectionPool::scan()
{
	while (1)
	{

	}
}

// 加载MySQL数据库配置文件
bool ConnectionPool::_loadConfig()
{
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr)
	{
		LOG("Failed to open mysql.ini\n");
		return false;
	}
	while ( !feof(pf) )								// 未到文件末尾返回0值
	{
		// 1、读取一行，并将该行转为c++中的string
		char buf[1024] = { 0 };
		fgets(buf, 1024, pf);
		std::string line = buf;

		// 2、找到等号的位置确定key，找到\n的值确定value
		int idx1 = line.find("=");
		if (idx1 == std::string::npos)
		{
			continue;
		}
		std::string key = "", value = "";
		key = line.substr(0, idx1);
		int idx2 = line.find("\n");
		if (idx2 == std::string::npos)
		{
			continue;
		}
		value = line.substr(idx1 + 1, idx2 - idx1 - 1);

		// 3、根据对应的key值，将value值写入成员变量中
		if (key == "host")
		{
			_host = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "user")
		{
			_user = value;
		}
		else if (key == "passwd")
		{
			_passwd = value;
		}
		else if (key == "dbName")
		{
			_dbName = value;
		} 
		else if (key == "initConnectionSize")
		{
			_initConnectionSize = atoi(value.c_str());
		}
		else if (key == "maxConnectionSize")
		{
			_maxConnectionSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeOut = atoi(value.c_str());
		}
		else 
		{
			continue;
		}
	}
	fclose(pf);										// 关闭文件
	return true;
}

// 实现构造函数
ConnectionPool::ConnectionPool()
{
	// 1、判断配置文件是否正确加载
	if (!_loadConfig())
	{
		return;
	}

	// 2、创建初始数量的连接,放入连接队列中
	for (int i = 0; i < _initConnectionSize; ++i)
	{
		Connection* pConn = new Connection();
		if (pConn->connect(_host, _port, _user, _passwd, _dbName))
		{
			_connectionQue.push(pConn);
			++_connectionCnt;
		}
	}

	// 3、开启生产者线程，生产可用连接
	std::thread producer(std::bind(ConnectionPool::produce, this));
	producer.detach();

	// 4、开启扫描线程，扫描空闲连接，释放超过最大空闲时间的连接
	std::thread scanner(std::bind(ConnectionPool::scan, this));
	scanner.detach();

}


// 实现析构函数
ConnectionPool::~ConnectionPool()
{
}