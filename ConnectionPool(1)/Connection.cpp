#include "Connection.h"

// 初始化数据库连接，记录连接时间
Connection::Connection(): _initTime(clock())
{
	_conn = mysql_init(nullptr);
}

// 关闭数据库连接
Connection::~Connection()
{
	if (_conn != nullptr)
	{
		mysql_close(_conn);
		_initTime = 0;
	}
		
}

// 连接MySQL服务
bool Connection::connect(std::string host, unsigned short port, std::string user, std::string passwd, std::string dbName)
{
	MYSQL* p=mysql_real_connect(_conn, host.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0);
	return p != nullptr;
}

// 更新操作
bool Connection::update(std::string sql)
{
	if (mysql_query(_conn, sql.c_str())) // 执行成功返回0，否则返回非0
	{
		LOG("更新失败！");
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(std::string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("查询失败！");
		return nullptr;
	}
	return mysql_use_result(_conn);
}

// 返回该条连接的存活时间
clock_t Connection::getAliveTime() const
{
	return clock_t() - _initTime;
}

void Connection::getCurrentTime()
{
	_initTime = clock_t();
}

