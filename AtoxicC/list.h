#pragma once

template <typename T>
class CELL
{
public:
	CELL(const T& data, size_t index, CELL* previous)
	{
		m_data = data;
		m_index = index;
		m_previous = previous;
	}
	T& data()
	{
		return m_data;
	}
	size_t index() const
	{
		return m_index;
	}
	void setIndex(size_t index)
	{
		m_index = index;
	}
	CELL* previous()
	{
		return m_previous;
	}
	void setPrevious(CELL* previous)
	{
		m_previous = previous;
	}
private:	
	T m_data;
	CELL* m_previous;
	size_t m_index;
};

template<typename T>
class list
{
public:
	list(){
		lastCell = nullptr;
		count = 0;
	}
	list(T const &object)
	{
		lastCell = createCell(object);
	}
	list(const list &object)
	{
		this->deleteAll();
		for (int i = 0; i < object.size(); i++)
		{
			this->add(object.at(i));
		}
	}
	~list()
	{
		if(count > 0)
		{
			CELL<T>* current[2] = { lastCell,lastCell->previous() };
			while (current[1] != nullptr)
			{
				delete current[0];
				current[0] = current[1];
				current[1] = current[1]->previous();
			}
		}
	}
	void deleteAll()
	{
		if (count > 0)
		{
			CELL<T>* current[2] = { lastCell,lastCell->previous() };
			while (current[1] != nullptr)
			{
				delete current[0];
				current[0] = current[1];
				current[1] = current[1]->previous();
			}
		}
		count = 0;
		lastCell = nullptr;
	}
	T at(size_t index) const
	{
		CELL<T>* current = lastCell;
		if (current->index() == index)
			return current->data();
		while (current->previous() != nullptr)
		{
			current = current->previous();
			if (current->index() == index)
				return current->data();
		}
		return T{ 0 };
	}
	void set(size_t index, const T &data)
	{
		if (index < count)
		{
			CELL<T>* current = lastCell;
			if (current->index() == index)
				current->data() = data;
			while (current->previous() != nullptr)
			{
				current = current->previous();
				if (current->index() == index)
					current->data() = data;
			}
		}
	}
	size_t add(const T &data)
	{
		lastCell = createCell(data);
		return count-1;
	}
	void del(size_t index)
	{
		if (index >= count)
		{
			return;
		}
		else if (count == 1)
		{
			delete lastCell;
			count = 0;
			lastCell = nullptr;
			return;
		}
		else
		{
			count--;
			CELL<T>* current = lastCell;
			if (current->index() == index)
			{
				lastCell = lastCell->previous();
				delete current;
				return;
			}
			current->setIndex(current->index() - 1);
			while (current->previous() != nullptr)
			{
				current = current->previous();
				if (current->index() == index + 1)
				{
					CELL<T>* toDelete = current->previous();
					current->setPrevious(current->previous()->previous());
					delete toDelete;
					current->setIndex(current->index() - 1);
					break;
				}
				current->setIndex(current->index() - 1);
			}
		}
	}
	size_t size() const
	{
		return count;
	}
	CELL<T>* getPtr() {
		return lastCell;
	}
	T& operator[](size_t index)
	{
		CELL<T>* current = lastCell;
		if (current->index() == index)
			return current->data();
		while (current->previous() != nullptr)
		{
			current = current->previous();
			if (current->index() == index)
				return current->data();
		}
	}
	void copy(const list &othr)
	{
		this->deleteAll();
		for (int i = 0; i < othr.size(); i++)
		{
			this->add(othr.at(i));
		}
	}
	list& operator=(const list &othr)
	{
		this->deleteAll();
		for (int i = 0; i < othr.size(); i++)
		{
			this->add(othr.at(i));
		}
		return *this;
	}


private:
	CELL<T>* createCell(const T &data)
	{
		CELL<T>* rep = new CELL<T>({ data,count,lastCell });
		count++;
		return rep;
	}
	
	CELL<T>* lastCell;
	size_t count;
};

