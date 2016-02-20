#ifndef TEMPLATE_VECTOR_H
#define TEMPLATE_VECTOR_H

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <initializer_list>
#include <type_traits>
#include <iterator>
#include <cstddef>


template<typename T>
class Vector
{

	//Assert that the T type is move_constructable
	//and move_assignable
	static_assert(std::is_move_assignable<T>::value &&
		std::is_move_constructible<T>::value,
		"Vector can only be instantiated"
		"with MoveConstructible and "
		"MoveAssignable types.");


private:
	T* _vector;
	size_t _size;
	size_t visibleSize;
	void init() { init(0); }
	void init(size_t);
	void init(size_t, T);
	void copy(const Vector &src);
	void swap(Vector &v);
	void reset(T);
	void resize(size_t);

public:
	class iterator :
		public std::iterator < std::random_access_iterator_tag,
		T,
		ptrdiff_t,
		T*,
		T& >
	{
	private:
		T* _ptr;

	public:
		iterator(T* ptr) { _ptr = ptr; }
		~iterator(){}

		bool operator==(const iterator& it)const { return _ptr == it._ptr; }
		bool operator!=(const iterator& it)const { return _ptr != it._ptr; }
		bool operator<(const iterator& it)const { return it._ptr - _ptr > 0; }

		iterator operator++(){ ++_ptr; return (*this); }
		iterator operator--(){ --_ptr; return (*this); }
		iterator operator-(size_t i) { _ptr -= i; return (*this); }
		iterator operator+(size_t i) { _ptr += i; return (*this); }

		std::ptrdiff_t operator-(const iterator& it){ return std::distance(it._ptr, _ptr); }

		T& operator*(){ return *_ptr; }
		
	};

	class const_iterator :
		public std::iterator < std::random_access_iterator_tag,
		T,
		std::ptrdiff_t,
		T*,
		T& >
	{
	private:
		T* _ptr;
	public:
		const_iterator(T* ptr) { _ptr = ptr; }
		~const_iterator(){}

		bool operator==(const const_iterator& it)const{ return _ptr == it._ptr; }
		bool operator!=(const const_iterator& it)const{ return _ptr != it._ptr; }
		bool operator<(const const_iterator& it)const { return it._ptr - _ptr > 0; }

		const_iterator operator++(){ ++_ptr; return (*this); }
		const_iterator operator--(){ --_ptr; return (*this); }
		const_iterator operator-(size_t i) { _ptr -= i; return (*this); }
		const_iterator operator+(size_t i) { _ptr += i; return (*this); }

		std::ptrdiff_t operator-(const const_iterator& it){ return std::distance(it._ptr, _ptr); }

		const T& operator*() const { return *_ptr; }
	};

	//Constructors
	Vector();
	Vector(Vector const&);
	Vector(Vector&&);
	Vector(std::initializer_list<T> ilist);
	~Vector();
	explicit Vector(size_t);
	explicit Vector(size_t, T);

	//Member functions
	void		push_back(T);
	void		insert(size_t, T);
	void		reset();
	void		clear();
	void		erase(std::size_t);
	iterator	begin();
	iterator	end();
	iterator	find(T const&);
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator find(T const&) const;
	size_t		size()				const;
	size_t		capacity()			const;
	T			get(size_t i)		const;

	//Operators
	const T& operator[](size_t i) const;
	T& operator[](size_t i);
	Vector& operator=(const Vector &v);
	Vector& operator=(Vector&&);
	bool operator==(const Vector &v) const;
	bool operator!=(const Vector &v) const;

	

};

using std::cout;
using std::endl;
using std::out_of_range;
using std::initializer_list;
using std::ptrdiff_t;

template <typename T>
Vector<T>::Vector() { init(0, T{}); }

template <typename T>
Vector<T>::Vector(size_t size) { init(size, T{}); }

template <typename T>
Vector<T>::Vector(size_t size, T value) { init(size, value); }

template <typename T>
Vector<T>::Vector(Vector const& src)
{
#ifdef _DEBUG
	cout << "Copy ctor" << endl;
#endif

	//Initiate a new array
	init(src.visibleSize);

	copy(src);

}

template <typename T>
Vector<T>::Vector(Vector&& oldVec)
{
#ifdef _DEBUG
	cout << "Move ctor" << endl;
#endif

	//Initialize before swap
	init();

	if (!(oldVec == *this))
		swap(oldVec);

}

template <typename T>
Vector<T>::Vector(initializer_list<T> ilist)
{
	//Initialize vector
	init(ilist.size());

	//Add all values to the vector
	int i = 0;
	for (T value : ilist)
	{
		_vector[i++] = value;
	}
}

template <typename T>
Vector<T>::~Vector()
{
	delete[](_vector);
}

template <typename T>
void Vector<T>::push_back(T value)
{
	resize(visibleSize + 1);

	_vector[visibleSize - 1] = value;

}

template <typename T>
void Vector<T>::insert(size_t index, T value)
{
	if (index > visibleSize || index < 0)
		throw out_of_range("Index out of bounds.");

	else if (index == visibleSize)
		push_back(value);
	
	else {
		
		resize(visibleSize + 1);

		T tmp = _vector[index];
		_vector[index] = value;

		for (size_t i = visibleSize - 1; i > index; i--)		{

			_vector[i] = _vector[i - 1];
		}
		_vector[index + 1] = tmp;

	}
}

template <typename T>
void Vector<T>::erase(size_t index)
{
	if (index >= visibleSize)
		throw out_of_range("Index out of bounds!");

	for (size_t i = index; i < visibleSize-1; i++)
	{
		_vector[i] = _vector[i + 1];
	}

	resize(visibleSize - 1);
}



template <typename T>
size_t Vector<T>::capacity() const { return _size; }

template <typename T>
void Vector<T>::clear() { visibleSize = 0; }

template <typename T>
size_t Vector<T>::size() const { return visibleSize; }

template <typename T>
T Vector<T>::get(size_t i) const { return _vector[i]; }

template <typename T>
void Vector<T>::reset(){ reset(T{}); }

template <typename T>
typename Vector<T>::iterator Vector<T>::begin()
{
	return Vector<T>::iterator(_vector);
}

template <typename T>
typename Vector<T>::iterator Vector<T>::end()
{
	return iterator(_vector + visibleSize);
}

template <typename T>
typename Vector<T>::iterator Vector<T>::find(T const &sought)
{
	auto it = begin();

	while (it != end() && !(*it == sought))
		++it;

	return it;
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::begin() const
{
	return Vector<T>::const_iterator(_vector);
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::end() const
{
	return const_iterator(_vector + visibleSize);
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::find(T const &sought) const
{
	auto it = begin();

	while (it != end() && !(*it == sought))
		++it;

	return it;
}

template <typename T>
void Vector<T>::reset(T initValue)
{
	for (size_t i = 0; i < visibleSize; i++)
		_vector[i] = initValue;
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const
{
	if (i >= visibleSize)
		throw out_of_range("Index out of bounds!");

	return _vector[i];
}

template <typename T>
T& Vector<T>::operator[](size_t i)
{
	if (i >= visibleSize)
		throw out_of_range("Index out of bounds!");

	return _vector[i];
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T> &v)
{

#ifdef _DEBUG
	cout << "Copy assignment operator" << endl;
#endif

	//Check if this should be a copy of itself
	if ( !(v == *this))
	{
		//If copied vector is larger clean up and re-init
		if (v.visibleSize > _size) {
			delete[] _vector;
			init(v.visibleSize);
		}
		else {
			visibleSize = v.visibleSize;
			reset();
		}

		copy(v);
	}

	return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& v)
{

#ifdef _DEBUG
	cout << "Move assignment operator" << endl;
#endif

	// If assigned vector is not this
	if (v._vector != _vector){

		//Remove this vector and re-init an empty set
		delete[] _vector;
		init();

		//Swap this empty set with the assigned one
		swap(v);
	}

	return *this;
}

template <typename T>
bool Vector<T>::operator!=(const Vector<T> &v) const
{
	return !(*this == v);
}

template <typename T>
bool Vector<T>::operator==(const Vector<T> &v) const
{
	//Vectors are equal if they contain the same number of elements
	//and each of the elements are equal.
	if (visibleSize == v.visibleSize){
		for (size_t i = 0; i < visibleSize; i++)
		{
			if ( !(_vector[i] == v._vector[i]))
				return false;
		}

		return true;
	}

	return false;
}

template <typename T>
void Vector<T>::init(size_t desiredSize)
{
	init(desiredSize, T{});
}

template <typename T>
void Vector<T>::init(size_t desiredSize, T initValue)
{
	size_t size = 1;
	while (size < desiredSize)
		size *= 2;

	if (desiredSize == 0)
		size = 0;

	_vector = new T[size];
	_size = size;
	visibleSize = desiredSize;

	//Use reset to zero init vector
	reset(initValue);

#ifdef _DEBUG
	cout << "Initialized: Size=" << size << " VisibleSize=" << desiredSize << endl;
#endif
}

template <typename T>
void Vector<T>::swap(Vector &v)
{
	T* temp_vecptr = v._vector;
	size_t temp_size = v._size;
	size_t temp_vissize = v.visibleSize;

	v._vector = _vector;
	v._size = _size;
	v.visibleSize = visibleSize;

	_vector = temp_vecptr;
	_size = temp_size;
	visibleSize = temp_vissize;
}

template <typename T>
void Vector<T>::copy(const Vector &src)
{
	for (size_t i = 0; i < src.visibleSize; i++)
	{
		_vector[i] = src._vector[i];
	}
}

template <typename T>
void Vector<T>::resize(size_t sz)
{
	if (sz > _size)	{
		T* temp = _vector;
		size_t oldSize = visibleSize;

		init(sz);
		for (size_t i = 0; i < oldSize; i++)
			_vector[i] = temp[i];

		delete[] temp;
	}
	else {
		visibleSize = sz;
	}
}

#endif