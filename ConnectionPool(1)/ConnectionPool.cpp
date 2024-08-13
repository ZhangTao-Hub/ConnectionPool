#include "ConnectionPool.h"


// ʵ��һ��ȫ��Ψһ�����̰߳�ȫ����������ģʽʵ�ֵ��̳߳ض���
ConnectionPool* ConnectionPool::getInstance()
{
	static ConnectionPool instance;					// �ֲ���������,�Զ�lock��unlock
	return &instance;
}

// �������̣߳����ѿ�������
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
	// 1������
	std::unique_lock<std::mutex> lock(_mutexQue);
	// 2�����Ӷ���Ϊ�գ��ȴ����������ȴ�ʱ��Ϊ_connectionTimeOut����
	while (_connectionQue.empty())
	{
		if (std::cv_status::timeout == cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeOut)))
		{
			if (_connectionQue.empty())
			{
				LOG("���ӳ�ʱ����������ʧ��");
				return nullptr;
			}
		}
	}

	// ����ָ����������Զ���ɾ����
	std::shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection* p) {
		std::unique_lock<std::mutex> lock(_mutexQue);
		_connectionQue.push(p);
		++_connectionCnt;
		});
	_connectionQue.pop();

	// �����������̣߳������Ӷ���Ϊ�գ���ʱ������������
	cv.notify_all();
	return sp;
}

// �������̣߳�ר�Ÿ���������������
void ConnectionPool::produce()
{
	while (1)
	{
		// 1������
		std::unique_lock<std::mutex> lock(_mutexQue);
		// 2�����Ӷ��в��գ��̵߳ȴ����ͷ���
		while (!_connectionQue.empty())
		{
			cv.wait(lock);
		}

		// 3�����Ӷ���Ϊ�գ���������
		while(_connectionCnt < _maxConnectionSize)
		{
			Connection* p = new Connection();
			if (p->connect(_host, _port, _user, _passwd, _dbName))
			{
				_connectionQue.push(p);
				++_connectionCnt;
			}
		}

		// 4��֪ͨ����
		cv.notify_all();
	}
}

// ɨ���̣߳������ͷų���������ʱ�������
void ConnectionPool::scan()
{
	while (1)
	{

	}
}

// ����MySQL���ݿ������ļ�
bool ConnectionPool::_loadConfig()
{
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr)
	{
		LOG("Failed to open mysql.ini\n");
		return false;
	}
	while ( !feof(pf) )								// δ���ļ�ĩβ����0ֵ
	{
		// 1����ȡһ�У���������תΪc++�е�string
		char buf[1024] = { 0 };
		fgets(buf, 1024, pf);
		std::string line = buf;

		// 2���ҵ��Ⱥŵ�λ��ȷ��key���ҵ�\n��ֵȷ��value
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

		// 3�����ݶ�Ӧ��keyֵ����valueֵд���Ա������
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
	fclose(pf);										// �ر��ļ�
	return true;
}

// ʵ�ֹ��캯��
ConnectionPool::ConnectionPool()
{
	// 1���ж������ļ��Ƿ���ȷ����
	if (!_loadConfig())
	{
		return;
	}

	// 2��������ʼ����������,�������Ӷ�����
	for (int i = 0; i < _initConnectionSize; ++i)
	{
		Connection* pConn = new Connection();
		if (pConn->connect(_host, _port, _user, _passwd, _dbName))
		{
			_connectionQue.push(pConn);
			++_connectionCnt;
		}
	}

	// 3�������������̣߳�������������
	std::thread producer(std::bind(ConnectionPool::produce, this));
	producer.detach();

	// 4������ɨ���̣߳�ɨ��������ӣ��ͷų���������ʱ�������
	std::thread scanner(std::bind(ConnectionPool::scan, this));
	scanner.detach();

}


// ʵ����������
ConnectionPool::~ConnectionPool()
{
}