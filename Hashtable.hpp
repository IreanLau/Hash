#pragma once

// K -> 线性探测
// KV -> 二次探测

enum Status
{
	EXITS,
	DELETE,
	EMPTY,
};


template<class K>
struct HashFuner
{
	size_t operator() (const K& key)
	{
		return key;
	}
};

template<>
struct HashFuner<string>
{
	static size_t BKDRHash(const char* str)
	{
		unsigned int seed = 131;//31 131 1313 13131 131313
		unsigned int hash = 0;
		while (*str)
		{
			hash = hash*seed + (*str++);
		}
		return (hash & 0x7FFFFFFF);
	}

	size_t operator()(const string& key)
	{
		return BKDRHash(key.c_str());
	}
};

template<class K,class V>
class KeyValueNode
{
public:
	K _key;
	V _value;

	KeyValueNode()
	{}
	KeyValueNode(const K& key, const V& value)
		:_key(key)
		, _value(value)
	{}
};

template<class K, class V, class HashFun = HashFuner<K> >
class HashTable
{
	typedef KeyValueNode<K, V> KVNode;
public:
	HashTable()
		:_tables(NULL)
		, _size(0)
		, _capacity(0)
		, _status(0)
	{}

	HashTable(size_t capacity)
		:_tables(new KVNode[capacity])
		, _size(0)
		, _capacity(capacity)
		, _status(new Status[capacity])
	{
		for (size_t i = 0; i < capacity; ++i)
		{
			_status[i] = EMPTY;
		}
	}

	HashTable(const HashTable<K, V>& ht)
		:_tables(NULL)
		, _status(NULL)
	{
		HashTable<K, V> newTables(ht._capacity);
		for (size_t i = 0; i < ht._capacity;++i)
		{
			if (ht._status[i] == EXITS)
			{
				newTables.Insert(ht._tables[i]);
			}
		}
		this->Swap(newTables);
	}

	HashTable<K, V>& operator=(HashTable<K, V>ht)
	{
		this->Swap(ht);
		return *this;
	}
	~HashTable()
	{
		if (_tables)
		{
			delete[] _tables;
			delete[] _status;
		}
	}

public:
	bool Insert(const K& key,const V& value)
	{
		size_t index = HashFunc0(key);
		int i = 1;
		while (_status[index] == EXITS)
		{
			index = HashFunc2(index, i++);//线性探测
		}
		_tables[index] = KVNode(key,value);
		_status[index] = EXITS;
		_size++;

		return true;
	}

	bool Remove(const K& key)
	{
		size_t index = HashFunc0(key);
		int i = 1;
		while (_tables[index]._key != EMPTY)
		{
			if (_status[index] == EXITS)
			{
				--_size;
				_status[index] = DELETE;
				return true;
			}
			else
			{
				return false;
			}
			index = HashFunc2(index, i++);
		}
		return false;
	}

	KVNode* Find(const K& key)
	{
		size_t index = HashFunc0(key);
		int i = 1;
		while (_status[index] != EMPTY)
		{
			if (_tables[index]._key == key)
			{
				if (_status[index] == EXITS)
					return &_tables[index];
				else
				{
					cout << "Delete Not Find" << endl;
					exit(1);
				}
			}
			index = HashFunc2(index, i++);
		}
		cout << "Empty Not Find" << endl;
		exit(2);
	}


	size_t HashFunc0(const K& key)
	{
		return HashFun()(key) % _capacity;
	}

	size_t HashFunc2(size_t prevValue, int i)
	{
		return (prevValue + 2 * i - 1) % _capacity;
	}

	void Swap(HashTable<K, V, HashFun>& ht)
	{
		swap(_tables, ht._tables);
		swap(_status, ht._status);
		swap(_size, ht._size);
		swap(_capacity, ht._capacity);
	}
protected:
	KVNode* _tables;			// 散列表
	size_t _size;				// 数据的个数
	size_t _capacity;			// 容量

	Status* _status;			// 数据状态表
};

void TestHashTable()
{
	HashTable<string, string> ht1(10);
	ht1.Insert("相交", "xiangjiao");

	KeyValueNode<string, string>* ret = ht1.Find("相交");
	ret->_value = "Cross";
};
