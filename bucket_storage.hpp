#ifndef BUCKET_STORAGE_HPP
#define BUCKET_STORAGE_HPP

#include <iterator>

namespace details
{
	template< typename T >
	class Stack
	{
	  public:
		Stack() noexcept;
		~Stack();
		template< typename U >
		void push(U&& x);
		T pop();
		T peek();
		bool empty() const noexcept;
		void clear();

	  private:
		struct Node
		{
			Node* prev;
			T data;
		};
		Node* m_top;
	};

	template< typename T >
	Stack< T >::Stack() noexcept : m_top(nullptr)
	{
	}

	template< typename T >
	Stack< T >::~Stack()
	{
		while (m_top != nullptr)
		{
			pop();
		}
		m_top = nullptr;
	}

	template< typename T >
	template< typename U >
	void Stack< T >::push(U&& x)
	{
		Node* next = new Node;
		next->prev = m_top;
		next->data = std::forward< U >(x);
		m_top = next;
	}

	template< typename T >
	T Stack< T >::pop()
	{
		T res = m_top->data;
		Node* del = m_top;
		m_top = m_top->prev;
		delete del;
		return res;
	}

	template< typename T >
	T Stack< T >::peek()
	{
		if (m_top == nullptr)
		{
			return nullptr;
		}
		return m_top->data;
	}

	template< typename T >
	bool Stack< T >::empty() const noexcept
	{
		return m_top == nullptr;
	}

	template< typename T >
	void Stack< T >::clear()
	{
		while (m_top != nullptr)
		{
			pop();
		}
	}
}	 // namespace details

template< typename T >
class BucketStorage
{
	template< bool IsConst >
	class BaseIterator;
	struct Block;
	struct Element;

  public:
	using value_type = T;
	using reference = T&;
	using pointer = T*;
	using size_type = size_t;
	using const_reference = const T&;
	using difference_type = std::ptrdiff_t;
	typedef details::Stack< Block* > StackBlock;
	typedef details::Stack< size_type > StackIndexes;

	explicit BucketStorage() noexcept;
	explicit BucketStorage(size_type m_bucket_capacity) noexcept;
	~BucketStorage();

	BucketStorage(BucketStorage&& other) noexcept;
	BucketStorage(const BucketStorage& other);
	BucketStorage& operator=(BucketStorage&& other) noexcept;
	BucketStorage& operator=(const BucketStorage& other);

	using iterator = BaseIterator< false >;
	using const_iterator = BaseIterator< true >;
	iterator erase(iterator iter);
	iterator insert(const value_type& x);
	iterator insert(value_type&& x);
	iterator begin() noexcept;
	iterator end() noexcept;
	const_iterator begin() const noexcept;
	const_iterator end() const noexcept;
	const_iterator cbegin() noexcept;
	const_iterator cend() noexcept;
	size_type size() const noexcept;
	bool empty() const noexcept;
	void clear() noexcept;
	void shrink_to_fit();
	void swap(BucketStorage& other) noexcept;
	size_type capacity() const noexcept;
	iterator get_to_distance(iterator it, difference_type dist) noexcept;

  private:
	template< typename U >
	iterator insert_impl(U&& x);

	struct Element
	{
		explicit Element(size_type time);
		~Element();
		bool operator==(const Element& other);
		bool operator!=(const Element& other);
		bool operator<(const Element& other);
		bool operator>(const Element& other);
		bool operator>=(const Element& other);
		bool operator<=(const Element& other);

		size_type get_time() const;
		size_type get_pos() const;
		Block* get_block_link();
		Element* get_prev();
		Element* get_next();

		void set_time(size_type time);
		void set_pos(size_type pos);
		void set_next(Element* next);
		void set_prev(Element* prev);
		void set_block_link(Block* block_link);

	  private:
		Block* m_block_link;
		size_type m_pos;
		Element* m_next;
		Element* m_prev;
		size_type m_time;
	};

	struct Block
	{
		explicit Block(size_type m_bucket_capacity);
		~Block();
		value_type* get_data(size_type pos);
		friend class BucketStorage;

	  private:
		Block* m_prev;
		Block* m_next;
		size_type m_head;
		value_type* m_arr;
		StackIndexes* m_free_pos;
		size_type m_size;
		size_type m_capacity;
	};

	class VirtualMemory
	{
	  public:
		VirtualMemory();
		~VirtualMemory();
		void push(Element* el);
		void pop();
		Element* get_end();
		Element* get_start();
		Element* get_over_end();
		void set_start(Element* el);
		void set_end(Element* el);

	  private:
		Element* m_start;
		Element* m_end;
		Element* m_over_end;
	};

	class PhysicalMemory
	{
	  public:
		explicit PhysicalMemory(size_type m_bucket_capacity);
		~PhysicalMemory();

		void push(const value_type& x, Element* el);
		void push(value_type&& x, Element* el);
		size_type size() const noexcept;
		size_type empty(Block* block_link);
		void push_free_block(Block* block_link);
		void clear_free_blocks();

	  private:
		StackBlock* m_free_blocks;
		Block* m_last_block;
		size_type m_bucket_capacity;
		void ensure_capacity();
		size_type m_size;
	};

	template< bool IsConst >
	class BaseIterator
	{
	  public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = typename std::conditional< IsConst, const T*, T* >::type;
		using reference = typename std::conditional< IsConst, const T&, T& >::type;

		explicit BaseIterator(Element* ptr);

		reference operator*() const;
		pointer operator->() const;
		BaseIterator& operator++();
		BaseIterator operator++(int);
		BaseIterator& operator--();
		BaseIterator operator--(int);
		template< bool OtherIsConst >
		bool operator==(const BaseIterator< OtherIsConst >& other) const;
		template< bool OtherIsConst >
		bool operator!=(const BaseIterator< OtherIsConst >& other) const;
		template< bool OtherIsConst >
		bool operator>(const BaseIterator< OtherIsConst >& other) const;
		template< bool OtherIsConst >
		bool operator<(const BaseIterator< OtherIsConst >& other) const;
		template< bool OtherIsConst >
		bool operator>=(const BaseIterator< OtherIsConst >& other) const;
		template< bool OtherIsConst >
		bool operator<=(const BaseIterator< OtherIsConst >& other) const;

		Element* get_current() const;

	  private:
		Element* m_current;
	};

	VirtualMemory* m_virtual_memory;
	PhysicalMemory* m_physical_memory;
	size_type m_bucket_size;
	size_type m_bucket_capacity;
};

// !BucketStorage
template< typename T >
BucketStorage< T >::BucketStorage() noexcept :
	m_physical_memory(new PhysicalMemory(64)), m_virtual_memory(new VirtualMemory()), m_bucket_size(0), m_bucket_capacity(64)
{
}

template< typename T >
BucketStorage< T >::BucketStorage(size_type m_bucket_capacity) noexcept :
	m_physical_memory(new PhysicalMemory(m_bucket_capacity)), m_virtual_memory(new VirtualMemory()), m_bucket_size(0),
	m_bucket_capacity(m_bucket_capacity)
{
}

template< typename T >
BucketStorage< T >::BucketStorage(const BucketStorage& other) :
	m_bucket_capacity(other.m_bucket_capacity), m_physical_memory(new PhysicalMemory(other.m_bucket_capacity)),
	m_virtual_memory(new VirtualMemory()), m_bucket_size(0)
{
	const_iterator temp = other.begin();
	while (temp != other.end())
	{
		insert(*temp);
		++temp;
	}
}

template< typename T >
BucketStorage< T >::BucketStorage(BucketStorage&& other) noexcept :
	m_bucket_size(other.m_bucket_size), m_bucket_capacity(other.m_bucket_capacity),
	m_physical_memory(std::move(other.m_physical_memory)), m_virtual_memory(std::move(other.m_virtual_memory))
{
	other.m_physical_memory = nullptr;
	other.m_virtual_memory = nullptr;
	other.m_bucket_size = 0;
	other.m_bucket_capacity = 0;
}

template< typename T >
BucketStorage< T >::~BucketStorage()
{
	while (!empty())
	{
		erase(begin());
	}
	delete m_physical_memory;
	delete m_virtual_memory;
}

template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(BucketStorage< T >&& other) noexcept
{
	if (this == &other)
		return *this;

	BucketStorage temp(std::move(other));
	swap(temp);

	return *this;
}

template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(const BucketStorage& other)
{
	if (this != &other)
	{
		BucketStorage temp(other);
		swap(temp);
	}
	return *this;
}

template< typename T >
template< typename U >
typename BucketStorage< T >::iterator BucketStorage< T >::insert_impl(U&& x)
{
	auto* el = new Element(m_virtual_memory->get_end()->get_time() + 1);
	try
	{
		m_physical_memory->push(std::forward< U >(x), el);
	} catch (int)
	{
		delete el;
		throw;
	}
	m_virtual_memory->push(el);
	m_bucket_size++;
	return iterator(el);
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::insert(const value_type& x)
{
	return insert_impl(x);
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::insert(value_type&& x)
{
	return insert_impl(std::move(x));
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::begin() noexcept
{
	return iterator(m_virtual_memory->get_start());
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::end() noexcept
{
	return iterator(m_virtual_memory->get_over_end());
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::begin() const noexcept
{
	return const_iterator(m_virtual_memory->get_start());
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::end() const noexcept
{
	return const_iterator(m_virtual_memory->get_over_end());
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::cbegin() noexcept
{
	if (m_virtual_memory != nullptr)
	{
		return const_iterator(m_virtual_memory->get_start());
	}
	return const_iterator(nullptr);
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::cend() noexcept
{
	if (m_virtual_memory != nullptr)
	{
		return const_iterator(m_virtual_memory->get_end()->get_next());
	}
	return const_iterator(nullptr);
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::erase(iterator iter)
{
	Element* el = iter.get_current();
	if (el == nullptr)
		return end();

	if (el->get_block_link()->m_size == el->get_block_link()->m_capacity)
	{
		m_physical_memory->push_free_block(el->get_block_link());
	}

	el->get_block_link()->m_free_pos->push(el->get_pos());
	--el->get_block_link()->m_size;

	if (el->get_prev() != nullptr)
		el->get_prev()->set_next(el->get_next());
	else
		m_virtual_memory->set_start(el->get_next());

	if (el->get_next() != nullptr)
		el->get_next()->set_prev(el->get_prev());
	else
		m_virtual_memory->set_end(el->get_prev());

	Element* next_el = el->get_next();
	el->get_block_link()->get_data(el->get_pos())->~value_type();
	m_physical_memory->empty(el->get_block_link());
	delete el;
	m_bucket_size--;
	return iterator(next_el);
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::get_to_distance(iterator it, const difference_type dist) noexcept
{
	for (difference_type i = 0; i < dist; i++)
	{
		++it;
	}
	for (difference_type i = 0; i > dist; i--)
	{
		--it;
	}
	return it;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::size() const noexcept
{
	return m_bucket_size;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::capacity() const noexcept
{
	return m_bucket_capacity * m_physical_memory->size();
}

template< typename T >
bool BucketStorage< T >::empty() const noexcept
{
	return m_bucket_size == 0;
}

template< typename T >
void BucketStorage< T >::clear() noexcept
{
	while (!empty())
		erase(begin());
	m_virtual_memory->set_start(m_virtual_memory->get_over_end());
	m_virtual_memory->set_end(m_virtual_memory->get_over_end());
	m_physical_memory->clear_free_blocks();
	m_bucket_size = 0;
}

template< typename T >
void BucketStorage< T >::shrink_to_fit()
{
	BucketStorage temp_bucket(m_bucket_capacity);
	iterator i = begin();
	while (i != end())
	{
		temp_bucket.insert(std::move(*i));
		i++;
	}
	swap(temp_bucket);
}

template< typename T >
void BucketStorage< T >::swap(BucketStorage& other) noexcept
{
	using std::swap;
	swap(m_virtual_memory, other.m_virtual_memory);
	swap(m_bucket_capacity, other.m_bucket_capacity);
	swap(m_bucket_size, other.m_bucket_size);
	swap(m_physical_memory, other.m_physical_memory);
}

//  !VirtualMemory
template< typename T >
BucketStorage< T >::VirtualMemory::VirtualMemory() : m_over_end(new Element(0))
{
	m_end = m_over_end;
	m_start = m_over_end;
}

template< typename T >
BucketStorage< T >::VirtualMemory::~VirtualMemory()
{
	Element* current = m_start;
	while (current != nullptr)
	{
		Element* next = current->get_next();
		delete current;
		current = next;
	}
}

template< typename T >
void BucketStorage< T >::VirtualMemory::push(Element* el)
{
	if (m_start == m_over_end)
	{
		m_start = el;
		m_end = el;
	}
	else
	{
		m_end->set_next(el);
		el->set_prev(m_end);
		m_end = el;
	}
	m_over_end->set_prev(m_end);
	m_end->set_next(m_over_end);
	m_over_end->set_time(m_end->get_time() + 1);
}

template< typename T >
void BucketStorage< T >::VirtualMemory::pop()
{
	if (m_end->m_prev != nullptr)
	{
		m_end = m_end->m_prev;
		delete m_end->m_next;
		m_end->m_next = nullptr;
		m_over_end->m_prev = m_end;
		m_over_end->m_time = m_end->m_time + 1;
	}
	else
	{
		m_start = m_over_end;
		m_end = m_over_end;
	}
}

template< typename T >
typename BucketStorage< T >::Element* BucketStorage< T >::VirtualMemory::get_end()
{
	return m_end;
}

template< typename T >
typename BucketStorage< T >::Element* BucketStorage< T >::VirtualMemory::get_start()
{
	return m_start;
}

template< typename T >
typename BucketStorage< T >::Element* BucketStorage< T >::VirtualMemory::get_over_end()
{
	return m_over_end;
}

template< typename T >
void BucketStorage< T >::VirtualMemory::set_start(Element* el)
{
	m_start = el;
}

template< typename T >
void BucketStorage< T >::VirtualMemory::set_end(Element* el)
{
	m_end = el;
}

// !PhysicalMemory
template< typename T >
BucketStorage< T >::PhysicalMemory::PhysicalMemory(size_type m_bucket_capacity) :
	m_last_block(nullptr), m_free_blocks(new StackBlock()), m_bucket_capacity(m_bucket_capacity), m_size(0)
{
}

template< typename T >
BucketStorage< T >::PhysicalMemory::~PhysicalMemory()
{
	delete m_free_blocks;
}

template< typename T >
void BucketStorage< T >::PhysicalMemory::push(const value_type& x, Element* el)
{
	ensure_capacity();
	auto* m_active_block = m_free_blocks->peek();
	if (!m_active_block->m_free_pos->empty())
	{
		el->set_pos(m_active_block->m_free_pos->pop());
		new (m_active_block->get_data(el->get_pos())) value_type(x);
	}
	else
	{
		new (m_active_block->get_data(m_active_block->m_head)) value_type(x);
		el->set_pos(m_active_block->m_head);
		++m_active_block->m_head;
	}
	el->set_block_link(m_active_block);
	++m_active_block->m_size;
}

template< typename T >
void BucketStorage< T >::PhysicalMemory::push(value_type&& x, Element* el)
{
	ensure_capacity();
	auto* m_active_block = m_free_blocks->peek();
	if (!m_active_block->m_free_pos->empty())
	{
		el->set_pos(m_active_block->m_free_pos->pop());
		new (m_active_block->get_data(el->get_pos())) value_type(std::move(x));
	}
	else
	{
		new (m_active_block->get_data(m_active_block->m_head)) value_type(std::move(x));
		el->set_pos(m_active_block->m_head);
		++m_active_block->m_head;
	}
	el->set_block_link(m_active_block);
	++m_active_block->m_size;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::PhysicalMemory::empty(Block* block_link)
{
	if (block_link->m_size == 0)
	{
		if (block_link->m_prev == nullptr)
		{
			if (block_link->m_next != nullptr)
			{
				block_link->m_next->m_prev = nullptr;
			}

			delete block_link;
			m_last_block = nullptr;

			m_size--;
		}
		else
		{
			m_size--;
			block_link = block_link->m_prev;
			delete block_link->m_next;
			block_link->m_next = nullptr;
			m_last_block = nullptr;
		}
	}

	return 0;
}

template< typename T >
void BucketStorage< T >::PhysicalMemory::push_free_block(Block* block_link)
{
	m_free_blocks->push(block_link);
}

template< typename T >
void BucketStorage< T >::PhysicalMemory::clear_free_blocks()
{
	m_free_blocks->clear();
}

template< typename T >
void BucketStorage< T >::PhysicalMemory::ensure_capacity()
{
	auto* m_active_block = m_free_blocks->peek();

	if (m_active_block == nullptr)
	{
		m_size++;
		m_active_block = new Block(m_bucket_capacity);
		if (m_last_block != nullptr)
		{
			m_last_block->m_next = m_active_block;
			m_active_block->m_prev = m_last_block;
			m_last_block = m_active_block;
			m_free_blocks->push(m_active_block);
		}
		else
		{
			m_last_block = m_active_block;
			m_free_blocks->push(m_active_block);
		}
	}
	if (m_active_block->m_size == m_bucket_capacity)
	{
		m_size++;
		m_free_blocks->pop();
		auto* new_block = new Block(m_bucket_capacity);
		new_block->m_prev = m_active_block;
		m_active_block->m_next = new_block;
		m_active_block = new_block;
		m_free_blocks->push(m_active_block);
	}
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::PhysicalMemory::size() const noexcept
{
	return m_size;
}

// !Block
template< typename T >
BucketStorage< T >::Block::Block(const size_type m_bucket_capacity) :
	m_prev(nullptr), m_next(nullptr), m_head(0), m_free_pos(new StackIndexes), m_size(0), m_capacity(m_bucket_capacity)
{
	m_arr = static_cast< value_type* >(operator new[](m_capacity * sizeof(value_type)));
}

template< typename T >
BucketStorage< T >::Block::~Block()
{
	if (m_arr != nullptr)
	{
		operator delete[](m_arr);
		m_arr = nullptr;
	}
	delete m_free_pos;
}

template< typename T >
typename BucketStorage< T >::value_type* BucketStorage< T >::Block::get_data(size_type pos)
{
	return &m_arr[pos];
}

// !Element
template< typename T >
BucketStorage< T >::Element::Element(size_type time) :
	m_block_link(nullptr), m_pos(0), m_next(nullptr), m_prev(nullptr), m_time(time)
{
}

template< typename T >
BucketStorage< T >::Element::~Element()
{
	if (m_prev)
	{
		m_prev->m_next = m_next;
	}
	if (m_next)
	{
		m_next->m_prev = m_prev;
	}
}

template< typename T >
bool BucketStorage< T >::Element::operator==(const Element& other)
{
	return m_time == other.m_time;
}

template< typename T >
bool BucketStorage< T >::Element::operator!=(const Element& other)
{
	return m_time != other.m_time;
}

template< typename T >
bool BucketStorage< T >::Element::operator<=(const Element& other)
{
	return m_time <= other.m_time;
}

template< typename T >
typename BucketStorage< T >::Element* BucketStorage< T >::Element::get_next()
{
	return m_next;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::Element::get_time() const
{
	return m_time;
}

template< typename T >
void BucketStorage< T >::Element::set_time(size_type time)
{
	m_time = time;
}

template< typename T >
void BucketStorage< T >::Element::set_pos(const size_type pos)
{
	m_pos = pos;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::Element::get_pos() const
{
	return m_pos;
}

template< typename T >
void BucketStorage< T >::Element::set_block_link(Block* block_link)
{
	m_block_link = block_link;
}

template< typename T >
typename BucketStorage< T >::Block* BucketStorage< T >::Element::get_block_link()
{
	return m_block_link;
}

template< typename T >
void BucketStorage< T >::Element::set_next(Element* next)
{
	m_next = next;
}

template< typename T >
void BucketStorage< T >::Element::set_prev(Element* prev)
{
	m_prev = prev;
}

template< typename T >
typename BucketStorage< T >::Element* BucketStorage< T >::Element::get_prev()
{
	return m_prev;
}

template< typename T >
bool BucketStorage< T >::Element::operator<(const Element& other)
{
	return m_time < other.m_time;
}

template< typename T >
bool BucketStorage< T >::Element::operator>(const Element& other)
{
	return m_time > other.m_time;
}

template< typename T >
bool BucketStorage< T >::Element::operator>=(const Element& other)
{
	return m_time >= other.m_time;
}

// !Iterator

template< typename T >
template< bool IsConst >
BucketStorage< T >::BaseIterator< IsConst >::BaseIterator(Element* ptr) : m_current(ptr)
{
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::template BaseIterator< IsConst >::reference
	BucketStorage< T >::BaseIterator< IsConst >::operator*() const
{
	return *m_current->get_block_link()->get_data(m_current->get_pos());
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::template BaseIterator< IsConst >::pointer
	BucketStorage< T >::BaseIterator< IsConst >::operator->() const
{
	return m_current->get_block_link()->get_data(m_current->get_pos());
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::template BaseIterator< IsConst >& BucketStorage< T >::BaseIterator< IsConst >::operator++()
{
	m_current = m_current->get_next();
	return *this;
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::template BaseIterator< IsConst > BucketStorage< T >::BaseIterator< IsConst >::operator++(int)
{
	BaseIterator tmp = *this;
	++(*this);
	return tmp;
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::template BaseIterator< IsConst >& BucketStorage< T >::BaseIterator< IsConst >::operator--()
{
	m_current = m_current->get_prev();
	return *this;
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::template BaseIterator< IsConst > BucketStorage< T >::BaseIterator< IsConst >::operator--(int)
{
	BaseIterator tmp = *this;
	--(*this);
	return tmp;
}

template< typename T >
template< bool IsConst >
template< bool OtherIsConst >
bool BucketStorage< T >::BaseIterator< IsConst >::operator>=(const BaseIterator< OtherIsConst >& other) const
{
	return *m_current >= *other.m_current;
}

template< typename T >
template< bool IsConst >
template< bool OtherIsConst >
bool BucketStorage< T >::BaseIterator< IsConst >::operator<=(const BaseIterator< OtherIsConst >& other) const
{
	return *m_current <= *other.m_current;
}

template< typename T >
template< bool IsConst >
typename BucketStorage< T >::Element* BucketStorage< T >::BaseIterator< IsConst >::get_current() const
{
	return m_current;
}

template< typename T >
template< bool IsConst >
template< bool OtherIsConst >
bool BucketStorage< T >::BaseIterator< IsConst >::operator==(const BaseIterator< OtherIsConst >& other) const
{
	return *m_current == *other.get_current();
}

template< typename T >
template< bool IsConst >
template< bool OtherIsConst >
bool BucketStorage< T >::BaseIterator< IsConst >::operator!=(const BaseIterator< OtherIsConst >& other) const
{
	return *m_current != *other.get_current();
}

template< typename T >
template< bool IsConst >
template< bool OtherIsConst >
bool BucketStorage< T >::BaseIterator< IsConst >::operator>(const BaseIterator< OtherIsConst >& other) const
{
	return *m_current > *other.m_current;
}

template< typename T >
template< bool IsConst >
template< bool OtherIsConst >
bool BucketStorage< T >::BaseIterator< IsConst >::operator<(const BaseIterator< OtherIsConst >& other) const
{
	return *m_current < *other.m_current;
}

#endif /* BUCKET_STORAGE_HPP */
