#include "Connection.h"

// ��ʼ�����ݿ����ӣ���¼����ʱ��
Connection::Connection(): _initTime(clock())
{
	_conn = mysql_init(nullptr);
}

// �ر����ݿ�����
Connection::~Connection()
{
	if (_conn != nullptr)
	{
		mysql_close(_conn);
		_initTime = 0;
	}
		
}

// ����MySQL����
bool Connection::connect(std::string host, unsigned short port, std::string user, std::string passwd, std::string dbName)
{
	MYSQL* p=mysql_real_connect(_conn, host.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0);
	return p != nullptr;
}

// ���²���
bool Connection::update(std::string sql)
{
	if (mysql_query(_conn, sql.c_str())) // ִ�гɹ�����0�����򷵻ط�0
	{
		LOG("����ʧ�ܣ�");
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(std::string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("��ѯʧ�ܣ�");
		return nullptr;
	}
	return mysql_use_result(_conn);
}

// ���ظ������ӵĴ��ʱ��
clock_t Connection::getAliveTime() const
{
	return clock_t() - _initTime;
}

void Connection::getCurrentTime()
{
	_initTime = clock_t();
}

