/***********************************************************************************************
created: 		2019-04-27

author:			chensong

purpose:		config
************************************************************************************************/
#include "cconfig.h"
#include <fstream>
#include "clog.h"

namespace chen {

	cconfig::cconfig():m_configs(NULL), m_values_size(0)
	{
	}


	cconfig::~cconfig()
	{
	}

	bool cconfig::init(int64 max_num, const char * file_name)
	{
		if (max_num < 1 || !file_name )
		{
			ERROR_EX_LOG(" m_config max num = %ld, cfg file ptr= NULL \n", max_num);
			return false;
		}

		if (!m_cfg_file.init(file_name))
		{
			return false;
		}
		m_configs = new cnode[max_num];
		if (!m_configs)
		{
			ERROR_EX_LOG("alloc m_config max num = %lu\n", max_num);
			return false;
		}
		

		m_values_size = max_num;
		return true;
	}

	void cconfig::destroy()
	{
	}

	void cconfig::set_string(int64 key_num,const std::string key, std::string value)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("set_string max_num = %ld, num = %ld, key = %s, value = %s\n", m_values_size, key_num, key.c_str(), value.c_str());
			return;
		}
		m_configs[key_num].init(key, EDataTypeString);
		m_configs[key_num].m_data = value;
	}

	void cconfig::set_int32(int64 key_num, const std::string key, int32 value)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("set_int32 max_num = %ld, num = %ld, key = %s, value = %d\n", m_values_size, key_num, key.c_str(), value);
			return;
		}
		m_configs[key_num].init(key, EDataTypeString);
		m_configs[key_num].m_int32 = value;
	}

	void cconfig::set_uint32(int64 key_num, const std::string key, uint32 value)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("set_uint32 max_num = %ld, num = %ld, key = %s, value = %ld\n", m_values_size, key_num, key.c_str(), value);
			return;
		}
		m_configs[key_num].init(key, EDataTypeString);
		m_configs[key_num].m_uint32 = value;
	}

	void cconfig::set_string(int64 key_num, int32, const char * key, const char * value)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("set_string max_num = %ld, num = %ld, key = %s, value = %s\n", m_values_size, key_num, key, value);
			return;
		}
		m_configs[key_num].init(key, EDataTypeString);
		m_configs[key_num].m_data = value;
	}

	void cconfig::set_int32(int64 key_num, const char * key, int32 value)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("set_int32 max_num = %ld, num = %ld, key = %s, value = %d\n", m_values_size, key_num, key, value);
			return;
		}
		m_configs[key_num].init(key, EDataTypeString);
		m_configs[key_num].m_int32 = value;
	}

	void cconfig::set_uint32(int64 key_num, const char * key, uint32 value)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("set_uint32 max_num = %ld, num = %ld, key = %s, value = %ld\n", m_values_size, key_num, key, value);
			return;
		}
		m_configs[key_num].init(key, EDataTypeString);
		m_configs[key_num].m_uint32 = value;
	}

	std::string cconfig::get_string(int64 key_num)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("get_string max_num = %ld, num = %ld\n", m_values_size, key_num);
			return "";
		}
		return m_configs[key_num].m_data;
	}

	int32 cconfig::get_int32(int64 key_num)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("get_int32 max_num = %ld, num = %ld\n", m_values_size, key_num);
			return 0;
		}
		return m_configs[key_num].m_int32;
	}

	uint32 cconfig::get_uint32(int64 key_num)
	{
		if (m_values_size < key_num)
		{
			ERROR_EX_LOG("get_int32 max_num = %ld, num = %ld\n", m_values_size, key_num);
			return 0;
		}
		return m_configs[key_num].m_uint32;
	}

	void cconfig::load_cfg_file()
	{
		for (int64 i = 0; i < m_values_size; ++i)
		{
			if (m_cfg_file.is_key(m_configs[i].m_name))
			{
				if (m_configs[i].m_type == EDataTypeNULL)
				{
					ERROR_EX_LOG(" i = %ld, type = null , name = %s\n", i, m_configs[i].m_name.c_str());
				}
				else if (m_configs[i].m_type == EDataTypeString)
				{
					m_configs[i].m_data = m_cfg_file.get_value(m_configs[i].m_name);
				}
				else if (m_configs[i].m_type == EDataTypeint)
				{
					m_configs[i].m_int32 = std::atoi(m_cfg_file.get_value(m_configs[i].m_name).c_str());
				}
				else if (m_configs[i].m_type == EDataTypeint)
				{
					m_configs[i].m_uint32 = std::atol(m_cfg_file.get_value(m_configs[i].m_name).c_str());
				}
				else
				{
					ERROR_EX_LOG(" i = %ld, type = is not find , name = %s\n", i, m_configs[i].m_name.c_str());
				}
			}
		}
		m_cfg_file.destroy();
	}

	ccfg_file::ccfg_file()
	{
	}

	ccfg_file::~ccfg_file()
	{
	}

	bool ccfg_file::init(const char * file_name)
	{

		std::ifstream  input(file_name);
		if (!input)
		{
			ERROR_EX_LOG("open config file fail file = %s\n", file_name);
			return false;
		}
		std::string line, key, value;
		while (getline(input, line))
		{
			if (_parser_line(line, key, value))
			{
				_insert_data(key, value);
				//m_configs[1].m_data = value; // 保存到map容器中的方法。
			}
		}
		if (input)
		{
			input.close();
		}
		return true;
	}

	void ccfg_file::destroy()
	{
		m_config_map.clear();
	}


	bool ccfg_file::_parser_line(const std::string & line, std::string & key, std::string & value)
	{
		if (line.empty())
		{
			return false;
		}
		std::string::size_type start_pos = 0, end_pos = line.size() - 1, pos;
		if ((pos = line.find('#')) != std::string::npos)
		{
			if (pos == 0)
			{
				return false;// #line---
			}
			end_pos = pos - 1;
		}

		std::string new_line = line.substr(start_pos, end_pos - start_pos + 1); // 删掉后半部分的注释 FIX_ME： 这里应该是减错了吧
																				// 下面pos的赋值时必要的，这样，就可在后面得到Key和value值了。
		if ((pos = new_line.find("=")) == std::string::npos) //说明前面没有 = 号
		{
			return false;
		}
		key = new_line.substr(0, pos); // 获得key
		value = new_line.substr(pos + 1, end_pos + 1 - (pos + 1)); // 获得value
		_trim(key);
		if (key.empty())
		{
			return false;
		}
		_trim(value); // 因为这里的key和value都是引用传递，可以直接被修改，所以不用返回
		return true;
	}


	bool ccfg_file::_is_space(char c)
	{
		if (c == ' ' || c == '\t')
		{
			return true;
		}
		else
		{
			return false;
		}
		return true;
	}

	bool ccfg_file::_iscommentchar(char c)
	{
		if (c == '#')
		{
			return true;
		}
		else
		{
			return false;
		}
		return true;
	}

	void ccfg_file::_trim(std::string & str)
	{
		if (str.empty())
		{
			return;
		}
		size_t i, start_pos, end_pos;
		for (i = 0; i < str.size(); i++)
		{
			if (!_is_space(str[i]))
			{
				break;
			}
		}
		if (i == str.size())
		{
			str = "";
			return;
		}
		start_pos = i; // 获取到非空格的初始位置

		for (i = str.size() - 1; i >= 0; i--)
		{
			if (!_is_space(str[i]))
			{
				break;
			}
		}
		end_pos = i;
		str = str.substr(start_pos, end_pos - start_pos + 1);
	}



	bool ccfg_file::is_key(const std::string & key) const
	{
		CCONFIG_MAP::const_iterator iter = m_config_map.find(key);
		if (iter != m_config_map.end())
		{
			return true;
		}
		return false;
	}

	std::string  ccfg_file::get_value(const std::string & key)
	{
		// TODO: 在此处插入 return 语句
		CCONFIG_MAP::const_iterator iter = m_config_map.find(key);
		if (iter != m_config_map.end())
		{
			return iter->second;
		}
		return NULL;
	}

	void ccfg_file::_insert_data(const std::string & key, const std::string & value)
	{
		if (!m_config_map.insert(std::make_pair(key, value)).second)
		{
			ERROR_EX_LOG("read cfg insert  key = %s, value = %s\n", key.c_str(), value.c_str());
		}
	}
}//namespace chen
