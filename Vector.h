/*
 * Vector.h
 *
 *  Created on: Jan 29, 2013
 *      Author: eplguest
 *  Edited on: Mar 25, 2015
 *      Author: eplta
 */

#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "InstanceCounter.h"

namespace epl {

template <typename T>
class vector {
private:
	/*
	 * The element data is managed using four pointers
	 * sbegin : storage begin -- address of the start of the allocated storage
	 * dbegin : data begin -- address of the first initialized element in the vector
	 * dend : data end -- address (one after) the last initialized element in vector
	 * send : storage end -- address (one after) the last storage location
	 * The capacity at the front of the vector is dbegin - sbegin
	 * The capacity at the back of the vector is send - dend
	 * The number of elements is dend - dbegin
	 * The size of the storage is send - sbegin
	 */
	T* sbegin; // start of storage array
	T* send; // end of storage array
	T* dbegin; // start of data
	T* dend; // end of data
	
	const uint64_t minimum_capacity = 8;
public:
	using value_type=T;
	vector(void) {
		uint64_t capacity = minimum_capacity;
		sbegin = reinterpret_cast<T*>(operator new(capacity * sizeof(T)));
		send = sbegin + capacity;
		dbegin = dend = sbegin;

        InstanceCounter();
	}

	explicit vector(uint64_t sz) {
		uint64_t capacity = sz;
		if (sz == 0) { capacity = minimum_capacity; }
		sbegin = reinterpret_cast<T*>(operator new(capacity * sizeof(T)));
		send = sbegin + capacity;
		dbegin = dend = sbegin;
		for (uint64_t k = 0; k < sz; k += 1) {
			new (dend) T();
			++dend;
		}

        InstanceCounter();
	}

	vector(const vector<T>& that) {
        std::cout << "epl::vector copy constructor" << std::endl;
        copy(that);

        InstanceCounter();
    }

	template <typename AltType>
	vector(const vector<AltType>& that) {
		uint64_t capacity = that.size();
		if (capacity == 0) { capacity = minimum_capacity; }
		sbegin = reinterpret_cast<T*>(operator new(capacity * sizeof(T)));
		send = sbegin + capacity;
		dbegin = dend = sbegin;
		for (uint64_t k = 0; k < capacity; k += 1) {
			new (dend) T(that[k]);
			++dend;
		}

        InstanceCounter();
	}

	template <typename Iterator>
	vector(Iterator b, Iterator e) {
		constructFromIterator(b, e, typename std::iterator_traits<Iterator>::iterator_category());

        InstanceCounter();
	}

    vector(std::initializer_list<T> il) : 
        vector(il.begin(), il.end()) {
	}

	vector(vector<T>&& that) {
        move(std::move(that)); 

        InstanceCounter();
	}
	
    ~vector(void) { destroy(); }

    vector<T>& operator=(const vector<T>& that) {
		if (this != &that) {
			destroy();
			copy(that);
		}
        return *this;
	}

	vector<T>& operator=(vector<T>&& that) {
		destroy();
		move(std::move(that));
		return *this;
	}

	uint64_t size(void) const { return dend - dbegin; }

	T& operator[](uint64_t k) {
		T* p = dbegin + k;
		if (p >= dend) { throw std::out_of_range("subscript out of range"); }
		return *p;
	}

	const T& operator[](uint64_t k) const {
		const T* p = dbegin + k;
		if (p >= dend) { throw std::out_of_range("subscript out of range"); }
		return *p;
	}

	void push_back(const T& that) {
        T temp(that);
		ensure_back_capacity(1);
		new (dend) T(std::move(temp));
		++dend;
	}

	void push_back(T&& that) {
    T temp(std::move(that));
		ensure_back_capacity(1);
		new (dend) T(std::move(temp));
		++dend;
	}

	template <typename... Args>
	void emplace_back(Args... args) {
		ensure_back_capacity(1);
		new(dend) T(args...);
	}

	void push_front(const T& that) {
		ensure_front_capacity(1);
		--dbegin;
		new (dbegin) T(that);
	}

	void push_front(T&& that) {
		ensure_front_capacity(1);
		--dbegin;
		new (dbegin) T(std::move(that));
	}

	template <typename... Args>
	void emplace_front(Args... args) {
		ensure_front_capacity(1);
		--dbegin;
		new (dbegin) T(args...);
	}

	void pop_back(void) {
		if (dbegin == dend) { throw std::out_of_range("pop back from empty vector"); }
		--dend;
		dend->~T();
	}

	void pop_front(void) {
		if (dbegin == dend) { throw std::out_of_range("pop back from empty Vector"); }
		dbegin->~T();
		++dbegin;
	}

	T& front(void) {
		if (dbegin == dend) { throw std::out_of_range("front called on empty Vector"); }
		return *dbegin;
	}

	const T& front(void) const {
		if (dbegin == dend) { throw std::out_of_range("front called on empty Vector"); }
		return *dbegin;
	}

	T& back(void) {
		if (dbegin == dend) { throw std::out_of_range("back called on empty Vector"); }
		return *(dend - 1);
	}

	const T& back(void) const {
		if (dbegin == dend) { throw std::out_of_range("back called on empty Vector"); }
		return *(dend - 1);
	}

  class iterator;
	class const_iterator : public std::iterator<std::random_access_iterator_tag, T> {
		const vector<T>* parent;
		uint64_t index;
		const T* ptr;

		using Same = const_iterator;

	public:
		const_iterator(void) {
			index = 0;
			ptr = nullptr;
			parent = nullptr;
		}

		const T& operator*(void) const {
			return *ptr;
		}

		Same& operator++(void) {
			++ptr;
			++index;
			return *this;
		}

		Same operator++(int) {
			Same t(*this);
			this->operator++();
			return t;
		}

		Same& operator--(void) {
			--ptr;
			--index;
			return *this;
		}


		Same operator--(int) {
			Same t(*this);
			this->operator--();
			return t;
		}

		int64_t operator-(const const_iterator that) const {
			return this->ptr - that.ptr;
		}

		const_iterator operator+(int64_t k) {
			return iterator(parent, ptr + k);
		}

		bool operator==(const Same& that) const {
			return this->parent == that.parent
					&& this->ptr == that.ptr
					;
		}

		bool operator!=(const Same& that) const {
			return ! (*this == that);
		}

		friend vector<T>;
        friend vector<T>::iterator;

	private:
		const_iterator(const vector<T>* parent, const T* ptr) {
			this->parent = parent;
			this->ptr = ptr;
			this->index = ptr - parent->dbegin;
		}

	};

	class iterator : public const_iterator {
		using Same = iterator;
		using Base = const_iterator;
	public:
		iterator(void) {}
		
    T& operator*(void) const {
			return const_cast<T&>(const_iterator::operator*());
		}

    iterator operator+(int64_t k) {
      const_iterator::operator+(k); //calls checkRevision for us
      return iterator(const_iterator::parent, const_iterator::ptr + k);
		}
		Same& operator++(void) { Base::operator++(); return *this; }
		Same operator++(int) { Same t(*this); operator++(); return t; }
		Same& operator--(void) { Base::operator--(); return *this; }
		Same operator--(int) { Same t(*this); operator--(); return t; }
	private:
		friend vector<T>;
		iterator(const vector<T>* parent, const T* ptr) : const_iterator(parent, ptr) { }
	};

	const_iterator begin(void) const { return const_iterator(this, dbegin); }
	iterator begin(void) { return iterator(this, dbegin); }

	const_iterator end(void) const { return const_iterator(this, dend); }
	iterator end(void) { return iterator(this, dend); }

private:
	void destroy(void) {
		if (sbegin != nullptr) {
			while (dbegin != dend) {
				dbegin->~T();
				++dbegin;
			}
			operator delete(sbegin);
		}
	}

	void copy(const vector<T>& that) {
		/* there is nothing preventing me from using the "private" parts of that
		 * as I implement this function (since this and that are the same type)
		 * However... someday I might want to have a member template where that
		 * is a different vector type (i.e., that has a different T).
		 * Since I can implement this function using only the public methods
		 * on that, I get more flexibility (should I want to copy this code later)
		 */
		uint64_t capacity = that.size();
		if (capacity < minimum_capacity) { capacity = minimum_capacity; }
		sbegin = reinterpret_cast<T*>(operator new(capacity * sizeof(T)));
		dbegin = dend = sbegin;
		for (uint64_t k = 0; k < that.size(); k += 1) {
			new (dend) T(that[k]);
			++dend;
		}
	}

	void move(vector<T>&& that) {
		sbegin = that.sbegin;
		send = that.send;
		dbegin = that.dbegin;
		dend = that.dend;
		that.sbegin = that.send = that.dbegin = that.dend = nullptr;
	}

	template <typename Iterator>
	void constructFromIterator(Iterator b, Iterator e, std::random_access_iterator_tag) {
		uint64_t capacity = (uint64_t) (e - b);
		if (capacity < minimum_capacity) { capacity = minimum_capacity; }
		sbegin = reinterpret_cast<T*>(operator new(capacity * sizeof(T)));
		dbegin = dend = sbegin;
		while (b != e) {
			new (dend) T(*b);
			++dend;
			++b;
		}
	}

	template <typename Iterator>
	void constructFromIterator(Iterator b, Iterator e, std::forward_iterator_tag) {
		uint64_t capacity = minimum_capacity;
		sbegin = reinterpret_cast<T*>(operator new(capacity * sizeof(T)));
		dbegin = dend = sbegin;
		while (b != e) {
			push_back(*b);
			++b;
		}
	}

	void ensure_back_capacity(uint64_t back_capacity) {
		if (back_capacity <= (uint64_t) (send - dend)) { // sufficient capacity
			return;
		}

		/* try doubling capacity */
		uint64_t capacity = 2 * (send - sbegin);

		while (capacity < back_capacity) {
			capacity *= 2;
		}

		/* set the back_capacity to either half of the excess capacity we have
		 * or to the requested capacity (back_capacity), whichever is greater.
		 */
		uint64_t excess_capacity = capacity - size();
		if (back_capacity < excess_capacity / 2) { back_capacity = excess_capacity / 2; }

		T* new_storage = reinterpret_cast<T*>(operator new(sizeof(T) * capacity));
		T* new_data = new_storage + capacity - back_capacity - size();
		T* new_data_end = new_data;

		/* move the elements (and deconstruct the originals) */
		while (dbegin != dend) {
			new (new_data_end) T(std::move(*dbegin));
			dbegin->~T();
			++dbegin;
			++new_data_end;
		}
		operator delete(sbegin);

		sbegin = new_storage;
		send = sbegin + capacity;
		dbegin = new_data;
		dend = new_data_end;
	}

	void ensure_front_capacity(uint64_t front_capacity) {
		if (front_capacity <= (uint64_t) (dbegin - sbegin)) { // sufficient capacity
			return;
		}

		/* try doubling capacity */
		uint64_t capacity = 2 * (send - sbegin);

		while (capacity < front_capacity) {
			capacity *= 2;
		}

		/* set the back_capacity to either half of the excess capacity we have
		 * or to the requested capacity (back_capacity), whichever is greater.
		 */
		uint64_t excess_capacity = capacity - size();
		if (front_capacity < excess_capacity / 2) { front_capacity = excess_capacity / 2; }

		T* new_storage = reinterpret_cast<T*>(operator new(sizeof(T) * capacity));
		T* new_data = new_storage + front_capacity;
		T* new_data_end = new_data;

		/* move the elements (and deconstruct the originals) */
		while (dbegin != dend) {
			new (new_data_end) T(std::move(*dbegin));
			dbegin->~T();
			++dbegin;
			++new_data_end;
		}
		operator delete(sbegin);

		sbegin = new_storage;
		send = sbegin + capacity;
		dbegin = new_data;
		dend = new_data_end;
	}

};

} //epl namespace

#endif /* VECTOR_HPP_ */
