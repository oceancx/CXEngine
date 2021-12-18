#ifndef HEAP_H
#define HEAP_H 
#include <vector>
#include <memory>
#include <iostream>
#include <random>
#include <cstdio>
template <typename T>
class Heap {
#define HEAP_LEFT(i) (i*2+1)
#define HEAP_RIGHT(i) (i*2+2)
#define HEAP_PARENT(i) ((i-1)/2)

public:
	typedef T value_type;
	typedef typename std::vector<T*>::size_type size_type;

	Heap();
	//Heap(std::initializer_list<T> il){};
	~Heap();
	
	size_type size() const { return data->size(); }
	bool empty() const { return data->empty(); }

	void add(const T t);
	void remove(int index);
	void update(int index, const T t);
	size_t updateDown(int cur);
	int updateTop(int cur);
	void sort();
	void print();
	void clear();
	
	T pop();

	T& operator[](size_type i);

	//private:
	std::shared_ptr<std::vector<T>> data;
	//std::vector<T>* data;
	//void check(size_type i,const std::string &msg) const;
};



template <typename T>
Heap<T>::Heap()
{
	data = std::make_shared<std::vector<T>>();
}

template <typename T>
Heap<T>::~Heap()
{

}
template <typename T>
void Heap<T>::clear()
{
	(*data).clear();
}

template <typename T>
size_t Heap<T>::updateDown(int cur)
{
	int max_cur = (static_cast<int>( size() )- 1) / 2;
	
	while (cur <= max_cur)
	{
		int child_left = HEAP_LEFT(cur);
		int child_right = HEAP_RIGHT(cur);
		bool b_left = child_right >= size() ?
			true : (*data)[child_left]->key < (*data)[child_right]->key;
		int small_index = b_left ? child_left : child_right;
		//printf("cur:%d small_index:%d\n", cur, small_index);
		if (small_index >= size())return 0;
		if ((*data)[cur]->key <= (*data)[small_index]->key)
		{
			return cur;
		}
		else
		{
			T tmp = (*data)[cur];
			(*data)[cur] = (*data)[small_index];
			(*data)[small_index] = tmp;
			cur = small_index;
		}
	}
	return size();
}

template <typename T>
int Heap<T>::updateTop(int cur)
{
	while (cur < size() && cur > 0)
	{
		if ((*data)[cur]->key < (*data)[HEAP_PARENT(cur)]->key)
		{
			T tmp = (*data)[cur];
			(*data)[cur] = (*data)[HEAP_PARENT(cur)];
			(*data)[HEAP_PARENT(cur)] = tmp;
			cur = HEAP_PARENT(cur);
		}
		else
		{
			return cur;
		}
	}
	return 0;
}


template <typename T>
void Heap<T>::add(const T t)
{
	data->push_back(t);
	int cur = static_cast<int>( data->size() - 1);
	updateTop(cur);
}

template <typename T>
void Heap<T>::remove(int index)
{
	if (!empty()) {
		T t = (*data)[size() - 1];
		data->pop_back();
		if (!empty())
		{
			(*data)[index] = t;
			int cur = index;
			if (updateTop(cur) == cur)
			{
				updateDown(cur);
			}
		}
	}
}

template <typename T>
void Heap<T>::update(int index, const T t)
{
	(*data)[index] = t;
	int cur = index;
	if (updateTop(cur) == cur)
	{
		updateDown(cur);
	}
}

template <typename T>
T Heap<T>::pop()
{
	if (size() >= 1) {
		T ret = (*data)[0];
		remove(0);
		return ret;
	}
	else {
		return{};
	}
}

template <typename T>
T& Heap<T>::operator[](size_type i)
{
	return{};
}


template <typename T>
void Heap<T>::sort()
{
	int i = 0;
	while (size() != 0)
	{
		i++;
		T a = pop();
		printf("%d ", a->key);
	}
	printf("\n");
}

template <typename T>
void Heap<T>::print()
{
	Heap<T> tmp2;
	for (int i = 0; i<size(); i++)
	{
		tmp2.add((*this->data)[i]);
	}

	tmp2.sort();
	// for(int i=0;i<size();i++)
	// {
	// 	printf("%d ", (*data)[i]->key); 
	// }
	// printf("\n");
}


#endif