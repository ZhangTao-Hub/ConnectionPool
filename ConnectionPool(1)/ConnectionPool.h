/*
*	MySQL连接池的实现
*	1、实现一个全局唯一，且线程安全的懒汉单例模式线程池
*	2、实现线程池的四大基本属性，初始连接数量，最大连接数量，连接超时时间，最大空闲时间
*	3、使用生产者消费者模型
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
										
	std::string _host;													// mysql主机
	unsigned short _port;												// mysql端口号，默认3306
	std::string _user;													// mysql用户名
	std::string _passwd;												// mysql密码
	std::string _dbName;												// mysql所使用的数据库

	int _initConnectionSize;											// 初始连接数量
	int _maxConnectionSize;												// 最大连接数量
	int _maxIdleTime;													// 最大空闲时间
	int _connectionTimeOut;												// 连接超时时长
	std::atomic<int> _connectionCnt;									// 记录连接的总个数(线程安全)

	std::mutex _mutexQue;												// 队列互斥量，确保队列线程安全
	std::queue<Connection*> _connectionQue;								// 存储连接的连接队列
	std::condition_variable cv;											// 线程间通信和同步

	void produce();														// 生产者线程调用函数
	void scan();														// 扫描线程调用函数

	bool _loadConfig();													// 加载配置文件的函数
	ConnectionPool();													// 单例#1 构造函数私有
	
public:
	ConnectionPool(const ConnectionPool&) = delete;						// 单例#2 禁用拷贝构造函数
	ConnectionPool& operator=(const ConnectionPool&) = delete;			// 单例#3 禁用赋值运算符
	~ConnectionPool();

	static ConnectionPool* getInstance();								// 获取线程池实例
	std::shared_ptr<Connection> getConnection();						// 获取一个可用连接
};