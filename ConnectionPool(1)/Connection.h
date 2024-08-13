/*
*	MySQL���ݿ�����ӣ��Լ���ɾ�Ĳ����
*/
#pragma once
#include <mysql.h>
#include <string>
#include <ctime>
#include "public.h"

class Connection {
private:
	MYSQL* _conn;
	clock_t _initTime; //��¼ÿ�����ӵĳ�ʼʱ��
public:
	Connection();
	~Connection();

	// ����
	bool connect(std::string host, unsigned short port, std::string user, 
				 std::string passwd, std::string dbName);
	// ����
	bool update(std::string sql);
	// ��ѯ
	MYSQL_RES* query(std::string sql);
	// ��ȡ���ʱ�䣬Ĭ�ϵ�λ����
	clock_t getAliveTime() const;

};