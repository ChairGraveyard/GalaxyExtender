#pragma once

#include <type_traits>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <cassert>
#include <wchar.h>

#define STRALLOCATOR_ADDRESS 0x012EA770 /*for sizes <= 0x80, used in strings*/
#define SOEALLOCATOR_ADDRESS 0x00AC15C0 /*for sizes > 0x80 and array boolean as false*/

#define VECTORALLOCATOR_ADDRESS 0x00AC15E0 /*soes own allocator for generic new objects with array boolean true*/

#define STRDEALLOCATOR1_ADDRESS 0x00AC1640 /*soe dealloc for sizes > 0x80 with array boolean false*/
#define STRDEALLOCATOR2_ADDRESS 0x012EA920 /*for sizes < 0x80 looks like system deallocator, used in strings*/

#define EMPTY_CONTAINER_INITIAL_OBJECTS 8

namespace soe {
	typedef void*(__cdecl* strallocator_t)(uint32_t);
	typedef void*(__cdecl* strallocator2_t)(uint32_t);
	typedef void(__cdecl* strdeallocator1_t)(void*);
	typedef void(__cdecl* strdeallocator2_t)(void*, uint32_t);

	template<typename storage_t>
	class StringAllocator {
	public:
		constexpr StringAllocator() {
		}

		static storage_t* alloc(int objectCount) {
			if ((objectCount * sizeof(storage_t)) > 0x80) {
				static strallocator_t strallocator1 = reinterpret_cast<strallocator_t>(SOEALLOCATOR_ADDRESS);

				return reinterpret_cast<storage_t*>(strallocator1(objectCount * sizeof(storage_t)));
			} else {
				static strallocator2_t strallocator2 = reinterpret_cast<strallocator2_t>(STRALLOCATOR_ADDRESS);

				return reinterpret_cast<storage_t*>(strallocator2(objectCount * sizeof(storage_t)));
			}
		}

		static void free(storage_t* address, int objectCount) {
			if ((objectCount * sizeof(storage_t)) > 0x80) {
				static strdeallocator1_t deall1 = reinterpret_cast<strdeallocator1_t>(STRDEALLOCATOR1_ADDRESS);
				deall1(address);
			} else {
				static strdeallocator2_t deall = reinterpret_cast<strdeallocator2_t>(STRDEALLOCATOR2_ADDRESS);
				deall(address, objectCount * sizeof(storage_t));
			}
		}
	};

	template<typename storage_t>
	class DefaultSOEAllocator {
	public:
		constexpr DefaultSOEAllocator() {
		}

		static storage_t* alloc(int objectCount) {
			static strallocator_t strallocator1 = reinterpret_cast<strallocator_t>(SOEALLOCATOR_ADDRESS);

			return reinterpret_cast<storage_t*>(strallocator1(objectCount * sizeof(storage_t)));
		}

		static void free(storage_t* address, int objectCount) {
			static strdeallocator1_t deall1 = reinterpret_cast<strdeallocator1_t>(STRDEALLOCATOR1_ADDRESS);
			deall1(address);
		}
	};

	template<typename storage_t, class Allocator = DefaultSOEAllocator<storage_t>>
	class container_base {
	public:
		typedef storage_t* iterator;
		typedef const storage_t* const_iterator;
	protected:
		storage_t* start;
		storage_t* finish;
		storage_t* endOfStorage;

		void copyFrom(const storage_t* from, int objectCount);

		template<typename ... Args>
		void createElement(uint32_t index, Args&&... obj) {
			// ReSharper disable once CppNonReclaimedResourceAcquisition
			new (&(start[index])) storage_t(std::forward<Args>(obj)...);
		}

		template<typename ... Args>
		static void createElement(storage_t* address, Args&&... obj) {
			// ReSharper disable once CppNonReclaimedResourceAcquisition
			new (&(*address)) storage_t(std::forward<Args>(obj)...);
		}

		void destroyElement(int index) {
			if (!std::is_trivially_copyable<storage_t>::value) {
				(&(start[index]))->~storage_t();
			}
		}

		void destroyRange(int from, int to) {
			if (!std::is_trivially_copyable<storage_t>::value) {
				for (int i = from; i < to; ++i) {
					destroyElement(i);
				}
			}
		}

		//gotta find the reallocs.. for now doing new alloc + copy instead on expand
		void ensureCapacity(uint32_t newSize);

	public:
		container_base() {
			start = finish = Allocator::alloc(EMPTY_CONTAINER_INITIAL_OBJECTS);
			endOfStorage = start + EMPTY_CONTAINER_INITIAL_OBJECTS;
		}

		explicit container_base(const uint32_t initialAllocObjectCount) {
			start = finish = Allocator::alloc(initialAllocObjectCount);
			endOfStorage = start + initialAllocObjectCount;
		}

		container_base(std::initializer_list<storage_t> list);
		container_base(const container_base& c);
		container_base(container_base&& c) noexcept;

		~container_base();

		container_base& operator=(const container_base& c);
		container_base& operator=(container_base&& c) noexcept;

		storage_t* find(const storage_t& obj, std::size_t pos = 0) noexcept {
			return std::find(std::begin(*this), std::end(*this), obj);
		}

		const storage_t* find(const storage_t& obj, std::size_t pos = 0) const noexcept {
			return std::find(std::begin(*this), std::end(*this), obj);
		}

		const_iterator begin() const noexcept {
			return start;
		}

		const_iterator end() const noexcept {
			return finish;
		}

		iterator begin() noexcept {
			return start;
		}

		iterator end() noexcept {
			return finish;
		}

		const storage_t& operator[](uint32_t i) const noexcept {
			return start[i];
		}

		storage_t& operator[](uint32_t i) noexcept {
			return start[i];
		}

		const storage_t& at(uint32_t i) const {
			if (i >= size())
				throw std::exception("access out of bounds");

			return start[i];
		}

		storage_t& at(uint32_t i) {
			if (i >= size())
				throw std::exception("access out of bounds");

			return start[i];
		}

		storage_t& front() noexcept {
			return (*begin());
		}

		const storage_t& front() const noexcept {
			return (*begin());
		}

		const storage_t& back() const noexcept {
			return *(end() - 1);
		}

		storage_t& back() noexcept {
			return *(end() - 1);
		}

		uint32_t size() const noexcept {
			return finish - start;
		}

		uint32_t capacity() const noexcept {
			return endOfStorage - start;
		}

		bool empty() const noexcept {
			return start == finish;
		}

		void push_back(const storage_t& element) {
			ensureCapacity(size() + 1);

			if (std::is_trivially_copyable<storage_t>::value)
				*finish = element;
			else
				createElement(finish, element);

			++finish;
		}

		template<typename... Args>
		void emplace_back(Args&&... args) {
			ensureCapacity(size() + 1);

			createElement(finish, std::forward<Args>(args)...);

			++finish;
		}

		void pop_back() {
			assert(finish > start);

			--finish;

			destroyElement(finish - start);
		}

		void clear() noexcept {
			start = finish;
		}
	};

	template <typename storage_t, class Allocator>
	void container_base<storage_t, Allocator>::copyFrom(const storage_t* from, int objectCount) {
		if (std::is_trivially_copyable<storage_t>::value) {
			memcpy(start, from, objectCount * sizeof(storage_t));
		} else {
			for (int i = 0; i < objectCount; ++i) {
				createElement(start + i, from[i]);
			}
		}

		finish = start + objectCount;
	}

	template <typename storage_t, class Allocator>
	void container_base<storage_t, Allocator>::ensureCapacity(uint32_t newSize) {
		uint32_t oldSize = endOfStorage - start;
		uint32_t oldFinishOffset = finish - start;

		if (newSize <= oldSize)
			return;

		newSize = (std::max)(newSize, oldSize * 2);

		storage_t* newStorage = Allocator::alloc(newSize);

		if (std::is_trivially_copyable<storage_t>::value) {
			memcpy(newStorage, start, oldSize * sizeof(storage_t));
		} else if (std::is_move_constructible<storage_t>::value) {
			for (uint32_t i = 0; i < oldSize; ++i) {
				createElement(newStorage + i, std::move(start[i]));
			}
		} else {
			for (uint32_t i = 0; i < oldSize; ++i) {
				createElement(newStorage + i, std::add_const<storage_t&>::type(start[i]));
			}
		}

		destroyRange(0, oldSize);

		Allocator::free(start, endOfStorage - start);

		start = newStorage;
		endOfStorage = newStorage + newSize;
		finish = start + oldFinishOffset;
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>::container_base(std::initializer_list<storage_t> list) {
		const uint32_t incomingSize = list.size();

		start = Allocator::alloc(incomingSize);
		finish = start + incomingSize;

		endOfStorage = start + incomingSize;
		int i = 0;

		for (auto entry : list) {
			if (std::is_trivially_copyable<storage_t>::value) {
				start[i] = entry;
			} else {
				createElement(i, entry);
			}

			++i;
		}
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>::container_base(const container_base& c) {
		uint32_t incomingSize = c.endOfStorage - c.start;

		start = finish = Allocator::alloc(incomingSize);
		endOfStorage = start + incomingSize;

		copyFrom(c.start, incomingSize);
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>::container_base(container_base&& c) noexcept {
		start = c.start;
		finish = c.finish;
		endOfStorage = c.endOfStorage;

		c.start = nullptr;
		c.finish = nullptr;
		c.endOfStorage = nullptr;
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>::~container_base() {
		if (start != nullptr) {
			destroyRange(0, size());

			Allocator::free(start, endOfStorage - start);
		}
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>& container_base<storage_t, Allocator>::operator=(const container_base& c) {
		if (this == &c)
			return *this;

		destroyRange(0, size());
		Allocator::free(start, endOfStorage - start);

		uint32_t incomingSize = c.endOfStorage - c.start;

		start = finish = Allocator::alloc(incomingSize);
		endOfStorage = start + incomingSize;

		copyFrom(c.start, incomingSize);

		return *this;
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>& container_base<storage_t, Allocator>::operator=(container_base&& c) noexcept {
		if (this == &c)
			return *this;

		destroyRange(0, size());
		Allocator::free(start, endOfStorage - start);

		start = c.start;
		finish = c.finish;
		endOfStorage = c.endOfStorage;

		c.start = nullptr;
		c.finish = nullptr;
		c.endOfStorage = nullptr;

		return *this;
	}

	template <typename storage_t>
	class stringbase_t : public container_base<storage_t, StringAllocator<storage_t> > {
	public:
		typedef container_base<storage_t, StringAllocator<storage_t> > base_t;

		stringbase_t() : base_t() {
			*base_t::end() = 0;
		}

		stringbase_t(stringbase_t&& s) noexcept : base_t(std::move(s)) {
		}

		explicit stringbase_t(const uint32_t initialSize) : base_t(initialSize + 1) {
			*base_t::end() = 0;
		}

		stringbase_t(const stringbase_t& s) : base_t(s) {
			base_t::pop_back();

			*base_t::end() = 0;
		}

		explicit stringbase_t(const storage_t* cstring, int characterCount) : base_t(characterCount + 1) {
			copyFrom(cstring, characterCount);

			*base_t::end() = 0;
		}

		void push_back(const char& element);

		stringbase_t& operator=(const stringbase_t& s);

		const storage_t* c_str() const noexcept {
			return base_t::begin();
		}

		void clear() noexcept {
			base_t::clear();

			*base_t::finish = 0;
		}

		uint32_t length() const noexcept {
			return base_t::size();
		}

	};

	template <typename storage_t>
	void stringbase_t<storage_t>::push_back(const char& element) {
		base_t::ensureCapacity(base_t::size() + 2);

		base_t::push_back(0);

		*(base_t::end() - 1) = element;
		*base_t::end() = 0;
	}

	template <typename storage_t>
	stringbase_t<storage_t>& stringbase_t<storage_t>::operator=(const stringbase_t& s) {
		if (&s == this)
			return *this;

		base_t::operator=(s);

		base_t::pop_back();
		*base_t::end() = 0;

		return *this;
	}

	class string : public stringbase_t<char> {
	public:
		string();
		explicit string(const uint32_t initialCapacity);

		string(const string& s);
		string(string&& s) noexcept;
		string(const char* cstring);
		string(const char* cstring, uint32_t length);

		std::size_t find(const string& str, std::size_t pos = 0) const noexcept;

		string& operator=(const string& s);

		string operator+ (const string& rhs) const;
		string operator+ (const char* rhs) const;

		string& operator+=(const string& rhs);
		string& operator+=(const char* rhs);

		bool operator==(const string& str) const;
		bool operator==(const char* str) const;

		//std::size_t hash() const;
		
		uint32_t hashCode() const;
		static uint32_t hashCode(const char* str, uint32_t startCRC = 0xFFFFFFFF);
	};

	bool operator==(const char* s, const soe::string& s2);
	string operator+(const char* s, const soe::string& s2);

	class unicode : public stringbase_t<wchar_t> {
	protected:
		void fromAscii(const char* cstring);

	public:
		static_assert(sizeof(wchar_t) == sizeof(uint16_t),
			"soe unicode implementation runs on 2 bytes");

		unicode();
		unicode(unicode&& s) noexcept;
		explicit unicode(const uint32_t initialCapacity);

		unicode(const unicode& s);
		unicode(const char* cstring);
		unicode(const char* cstring, uint32_t length);

		string toAscii() const;

		std::size_t find(const unicode& str, std::size_t pos = 0) const noexcept;

		bool operator==(const unicode& str) const;
		bool operator==(const char* str) const;

		unicode& operator=(const unicode& s);
		unicode operator+ (const unicode& rhs) const;
	};

	bool operator==(const char* s, const soe::unicode& s2);

	template <typename T> using vector = container_base<T>;

}
