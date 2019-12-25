#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
using namespace std;
/* 散列表行数 */
#define HASHTABLE_ROWS_NUMBER 20
/* 数据数组行数，也可换成 vector */
#define DATATABLE_ROWS_NUMBER 60
/**
 * 记录结构
 */
struct Record
{
	char set = 0;
	string username;
	string telephone;
	string sex;
	string colname;
};
/**
 * 记录数据结构
 */
struct RecordData
{
	RecordData(vector<struct Record> _records, int _count)
	{
		records = _records;
		count = _count;
	}
	vector<struct Record> records;
	int count = 0;
};
/**
 * 记录结构
 */
struct HASHBlock
{
	struct Record *record = NULL;
	struct HASHBlock *next_block = NULL; // 当一个块有多条数据时
	struct HASHBlock *next = NULL;
};
bool load_data(Record *records, HASHBlock *hashblocks_username, HASHBlock *hashblocks_telephone, HASHBlock *hashblocks_colname, string data_file_path);
void add_hashblock(HASHBlock *hashblocks, Record *record_addr, string value, bool is_col);
RecordData *search_hashblock(HASHBlock *hashblocks, string type, string value);
int simple_hash(int data);
bool is_prime(int number);
int get_max_prime(int number);
unsigned int str2int(string str);
/**
 * 入口
 */
int main()
{
	/* 设置编码 */
	system("@chcp 65001>nul");
	/* 空记录数组 */
	Record records[DATATABLE_ROWS_NUMBER];
	/* 空用户名散列表 */
	HASHBlock hashblocks_username[HASHTABLE_ROWS_NUMBER];
	/* 空电话散列表 */
	HASHBlock hashblocks_telephone[HASHTABLE_ROWS_NUMBER];
	/* 空院系散列表 */
	HASHBlock hashblocks_colname[HASHTABLE_ROWS_NUMBER];
	/* 加载数据 */
	if (!load_data(records, hashblocks_username, hashblocks_telephone, hashblocks_colname, "path\\to\\data.txt"))
	{
		system("pause");
		return 1;
	}
	// system("cls");
	/* 开始选单 */
	int i;
	string op, value;
	RecordData *query_record_data;
	while (true)
	{
		cout << "欢迎使用人员信息查询工具！" << endl;
		cout << "1. 电话号码查询\t\t2. 用户名查询" << endl;
		cout << "3. 院系人员查询\t\t0. 退出" << endl;
		cout << "请选择您要执行的操作:" << endl;
		cin >> op;
		if (op == "0")
		{
			break;
		}
		else if (op == "1" || op == "2" || op == "3")
		{
			cout << "请输入查询的值：" << endl;
			cin >> value;
			cout << endl;
			if (op == "1")
			{
				query_record_data = search_hashblock(hashblocks_telephone, "telephone", value);
				cout << "电话号码查询结果如下：" << endl;
			}
			else if (op == "2")
			{
				query_record_data = search_hashblock(hashblocks_username, "username", value);
				cout << "用户名查询结果如下：" << endl ;
			}
			else if (op == "3")
			{
				query_record_data = search_hashblock(hashblocks_colname, "colname", value);
				cout << "院系查询结果如下：" << endl;
			}
			if (query_record_data->count == 0)
			{
				cout << "没有找到您查询的数据！" << endl;
			}
			else
			{
				cout << "查找到 " << query_record_data->count << " 条记录！" << endl;
				for (i = 0; i < query_record_data->records.size(); i++)
				{
					cout << "姓名: " << query_record_data->records[i].username << ", 电话: " << query_record_data->records[i].telephone << ", 院系: " << query_record_data->records[i].colname << endl;
				}
			}
			delete query_record_data;
		}
		else
		{
			cout << "没有找到您选择的操作，请重新选择！" << endl;
		}
		cout << endl
			 << endl;
	}
	/* 结束选单 */
	system("pause");
	return 0;
}
/**
 * 读取数据
 * @param records 空记录数组
 * @param hashblocks_username 空用户名散列表
 * @param hashblocks_telephone 空电话散列表
 * @param hashblocks_colname 空院系散列表
 * @param data_file_path 数据文件路径
 * @retval true 成功
 * @retval false 失败
 */
bool load_data(Record *records, HASHBlock *hashblocks_username, HASHBlock *hashblocks_telephone, HASHBlock *hashblocks_colname, string data_file_path)
{
	cout << "# 加载数据..." << endl;
	int record_index = 0;
	string row_username, row_telephone, row_colname;
	ifstream in_filestream(data_file_path);
	if (in_filestream)
	{
		while (
			(in_filestream >> row_username) && (in_filestream >> row_telephone) && (in_filestream >> row_colname))
		{
			records[record_index].set = 1;
			cout << "$ 处理记录: {姓名: " << row_username << ", 电话: " << row_telephone << ", 院系: " << row_colname << "}" << endl;
			add_hashblock(hashblocks_username, &records[record_index], records[record_index].username = row_username, false);
			add_hashblock(hashblocks_telephone, &records[record_index], records[record_index].telephone = row_telephone, false);
			add_hashblock(hashblocks_colname, &records[record_index], records[record_index].colname = row_colname, true);
			record_index++;
		}
		in_filestream.close();
		in_filestream.clear();
		cout << "# 数据文件 \"" << data_file_path << "\" 加载完成!" << endl
			 << endl;
		return true;
	}
	cout << "# 数据文件 \"" << data_file_path << "\" 加载异常!" << endl;
	return false;
}
/**
 * 添加数据块
 * @param hashblocks 数据块组
 * @param record_addr 记录地址
 * @param value 值
 * @param is_col 是否是组
 */
void add_hashblock(HASHBlock *hashblocks, Record *record_addr, string value, bool is_col)
{
	int key = simple_hash(str2int(value));
	HASHBlock *hashblock_point = &hashblocks[key];
	cout << "> record: " << record_addr << ", hashblocks: " << hashblocks << ", value: " << value << ", key: " << key << ", hashblock: " << hashblock_point << endl;
	while (
		(hashblock_point->next != NULL) && (!is_col || hashblock_point->record->colname != value))
	{
		hashblock_point = hashblock_point->next;
	}
	if (is_col)
	{
		HASHBlock *hashblock_col_point = hashblock_point;
		while (hashblock_col_point->next_block != NULL)
		{
			hashblock_col_point = hashblock_col_point->next_block;
		}
		hashblock_col_point->record = record_addr;
		hashblock_col_point->next_block = new HASHBlock;
	}
	else
	{
		hashblock_point->record = record_addr;
	}
	hashblock_point->next = new HASHBlock;
}
/**
 * 搜索数据
 * @param hashblocks 数据块组
 * @param type 类型
 * @param value 值
 * @retval RecordData 记录数据
 */
RecordData *search_hashblock(HASHBlock *hashblocks, string type, string value)
{
	int key = simple_hash(str2int(value));
	HASHBlock *hashblock_point = &hashblocks[key];
	vector<Record> records;
	if (hashblock_point->next == NULL)
	{
		return new RecordData(records, records.size());
	}
	while (hashblock_point->next != NULL)
	{
		if (type == "username")
		{
			if (hashblock_point->record->username == value)
			{
				records.push_back(*(hashblock_point->record));
			}
		}
		else if (type == "telephone")
		{
			if (hashblock_point->record->telephone == value)
			{
				records.push_back(*(hashblock_point->record));
			}
		}
		else if (hashblock_point->record->colname == value)
		{
			records.push_back(*(hashblock_point->record));
			if(hashblock_point->next_block != NULL) {
				hashblock_point = hashblock_point->next_block;
			}
			while (hashblock_point->next_block != NULL)
			{
				records.push_back(*(hashblock_point->record));
				hashblock_point = hashblock_point->next_block;
			}
			break;
		}
		hashblock_point = hashblock_point->next;
	}
	return new RecordData(records, records.size());
}

/**
 * 简易单向字符转数字方法
 * @param string 字符串
 * @retval int 整数
 */
unsigned int str2int(string str)
{
	int ret = 0;
	for (int i = 0; i != str.length(); i++)
	{
		ret += toascii(str[i]);
	}
	return ret;
}

/**
 * 简单整数哈希实现
 * @param data 整数形式数据
 * @retval int 指纹
 */
int simple_hash(int data)
{
	return data % HASHTABLE_ROWS_NUMBER;
}

/**
 * 判断是否为素数（程序没有使用）
 * @param number 整数
 * @retval true 是素数
 * @retval false 不是素数
 */
bool is_prime(int number)
{
	float n_sqrt;
	if (number == 2 || number == 3)
	{
		return false;
	}
	if (number % 6 != 1 && number % 6 != 5)
	{
		return false;
	}
	n_sqrt = floor(sqrt((float)number));
	for (int i = 5; i <= n_sqrt; i += 6)
	{
		if (number % (i) == 0 || number % (i + 2) == 0)
		{
			return false;
		}
	}
	return true;
}
/**
 * 获取大于等于 number 的素数（程序没有使用）
 * @param number 整数
 * @retval int 素数
 * @retval 1 没有符合（X）
 */
int get_max_prime(int number)
{
	int i;
	for (i = number; i > 1; i--)
	{
		if (is_prime(i))
			return i;
	}
	return 1;
}
