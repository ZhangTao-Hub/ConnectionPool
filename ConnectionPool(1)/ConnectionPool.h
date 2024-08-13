/*
*	MySQL���ӳص�ʵ��
*	1��ʵ��һ��ȫ��Ψһ�����̰߳�ȫ����������ģʽ�̳߳�
*	2��ʵ���̳߳ص��Ĵ�������ԣ���ʼ��������������������������ӳ�ʱʱ�䣬������ʱ��
*	3��ʹ��������������ģ��
*/
#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include <chrono>
#include <condition_variable>
#include "Connection.h"

class ConnectionPool {
private:
										
	std::string _host;													// mysql����
	unsigned short _port;												// mysql�˿ںţ�Ĭ��3306
	std::string _user;													// mysql�û���
	std::string _passwd;												// mysql����
	std::string _dbName;												// mysql��ʹ�õ����ݿ�

	int _initConnectionSize;											// ��ʼ��������
	int _maxConnectionSize;												// �����������
	int _maxIdleTime;													// ������ʱ��
	int _connectionTimeOut;												// ���ӳ�ʱʱ��
	std::atomic<int> _connectionCnt;									// ��¼���ӵ��ܸ���(�̰߳�ȫ)

	std::mutex _mutexQue;												// ���л�������ȷ�������̰߳�ȫ
	std::queue<Connection*> _connectionQue;								// �洢���ӵ����Ӷ���
	std::condition_variable cv;											// �̼߳�ͨ�ź�ͬ��

	void produce();														// �������̵߳��ú���
	void scan();														// ɨ���̵߳��ú���

	bool _loadConfig();													// ���������ļ��ĺ���
	ConnectionPool();													// ����#1 ���캯��˽��
	
public:
	ConnectionPool(const ConnectionPool&) = delete;						// ����#2 ���ÿ������캯��
	ConnectionPool& operator=(const ConnectionPool&) = delete;			// ����#3 ���ø�ֵ�����
	~ConnectionPool();

	static ConnectionPool* getInstance();								// ��ȡ�̳߳�ʵ��
	std::shared_ptr<Connection> getConnection();						// ��ȡһ����������
};