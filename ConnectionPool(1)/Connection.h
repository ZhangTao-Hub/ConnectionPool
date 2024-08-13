/*
*	MySQL数据库的连接，以及增删改查操作
*/
#pragma once
#include <mysql.h>
#include <string>
#include <ctime>
#include "public.h"

class Connection {
private:
	MYSQL* _conn;
	clock_t _initTime; //记录每条连接的初始时间
public:
	Connection();
	~Connection();

	// 连接
	bool connect(std::string host, unsigned short port, std::string user, 
				 std::string passwd, std::string dbName);
	// 更新
	bool update(std::string sql);
	// 查询
	MYSQL_RES* query(std::string sql);
	// 获取存活时间，默认单位毫秒
	clock_t getAliveTime() const;

};