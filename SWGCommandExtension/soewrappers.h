#pragma once

#include <type_traits>
#include <string>
#include <initializer_list>


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
			if (objectCount > 0x80) {
				static strallocator_t strallocator1 = (strallocator_t)SOEALLOCATOR_ADDRESS;

				return (storage_t*)strallocator1(objectCount * sizeof(storage_t));
			}
			else {
				static strallocator2_t strallocator2 = (strallocator2_t)STRALLOCATOR_ADDRESS;

				return (storage_t*)strallocator2(objectCount * sizeof(storage_t));
			}
		}

		static void free(storage_t* address, int objectCount) {
			if (objectCount > 0x80) {
				static strdeallocator1_t deall1 = (strdeallocator1_t)STRDEALLOCATOR1_ADDRESS;
				deall1(address);
			}
			else {
				static strdeallocator2_t deall = (strdeallocator2_t)STRDEALLOCATOR2_ADDRESS;
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
			static strallocator_t strallocator1 = (strallocator_t)SOEALLOCATOR_ADDRESS;

			return (storage_t*)strallocator1(objectCount * sizeof(storage_t));
		}

		static void free(storage_t* address, int objectCount) {
			static strdeallocator1_t deall1 = (strdeallocator1_t)STRDEALLOCATOR1_ADDRESS;
			deall1(address);
		}
	};

	template<typename storage_t, class Allocator = DefaultSOEAllocator<storage_t>>
	class container_base {
	protected:
		storage_t* start;
		storage_t* finish;
		storage_t* endOfStorage;

		void copyFrom(const storage_t* from, int objectCount) {
			if (std::is_trivially_copyable<storage_t>::value) {
				memcpy(start, from, objectCount * sizeof(storage_t));
			}
			else {
				for (int i = 0; i < objectCount; ++i) {
					createElement(start + i, from[i]);
				}
			}

			finish = start + objectCount;
		}

		void createElement(uint32_t index, const storage_t& obj) {
			new (&(start[index])) storage_t(obj);
		}

		static void createElement(storage_t* address, const storage_t& obj) {
			new (&(*address)) storage_t(obj);
		}

		void destroyElement(int index) {
			if (std::is_trivially_copyable<storage_t>::value) {
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
		void ensureCapacity(uint32_t newSize) {
			uint32_t oldSize = size();

			if (newSize <= (uint32_t)(endOfStorage - start))
				return;

			storage_t* newStorage = Allocator::alloc(newSize);

			if (std::is_trivially_copyable<storage_t>::value) {
				memcpy(newStorage, start, size() * sizeof(storage_t));
			}
			else {
				for (uint32_t i = 0; i < oldSize; ++i) {
					createElement(newStorage + i, start[i]);
				}
			}

			Allocator::free(start, endOfStorage - start);

			start = newStorage;
			endOfStorage = newStorage + newSize;
			finish = newStorage + oldSize;
		}

	public:
		container_base() {
			start = finish = Allocator::alloc(EMPTY_CONTAINER_INITIAL_OBJECTS);
			endOfStorage = start + EMPTY_CONTAINER_INITIAL_OBJECTS;
		}

		explicit container_base(const uint32_t initialAllocObjectCount) {
			start = finish = Allocator::alloc(initialAllocObjectCount);
			endOfStorage = start + initialAllocObjectCount;
		}

		container_base(std::initializer_list<storage_t> list) {
			const uint32_t incomingSize = list.size();

			start = Allocator::alloc(incomingSize);
			finish = start + incomingSize;

			endOfStorage = start + incomingSize;
			int i = 0;

			for (auto entry : list) {
				if (std::is_trivially_copyable<storage_t>::value) {
					start[i] = entry;
				}
				else {
					createElement(i, entry);
				}

				++i;
			}
		}

		container_base(const container_base& c) {
			uint32_t incomingSize = c.endOfStorage - c.start;

			start = finish = Allocator::alloc(incomingSize);
			endOfStorage = start + incomingSize;

			copyFrom(c.start, incomingSize);
		}

		~container_base() {
			destroyRange(0, size());

			Allocator::free(start, endOfStorage - start);
		}

		container_base& operator=(const container_base& c) {
			if (this == &c)
				return *this;

			Allocator::free(start, endOfStorage - start);

			uint32_t incomingSize = c.endOfStorage - c.start;

			start = finish = Allocator::alloc(incomingSize);
			endOfStorage = start + incomingSize;

			copyFrom(c.start, incomingSize);

			return *this;
		}

		const storage_t* begin() const {
			return start;
		}

		const storage_t* end() const {
			return finish;
		}

		storage_t* begin() {
			return start;
		}

		storage_t* end() {
			return finish;
		}

		const storage_t& operator[](uint32_t i) const {
			return start[i];
		}

		storage_t& operator[](uint32_t i) {
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

		storage_t& front() {
			return (*begin());
		}

		const storage_t& front() const {
			return (*begin());
		}

		const storage_t& back() const {
			return *(end() - 1);
		}

		storage_t& back() {
			return *(end() - 1);
		}

		uint32_t size() const {
			return finish - start;
		}

		bool empty() const {
			return start == finish;
		}

		void push_back(const storage_t& element) {
			ensureCapacity(size() + 1);

			if (std::is_trivially_copyable<storage_t>::value)
				*finish = element;
			else
				createElement(finish, element);

			finish++;
		}

		void pop_back() {
			finish--;

			destroyElement(finish - start);
		}

		void clear() {
			start = finish;
		}
	};

	template <typename storage_t>
	class stringbase_t : public container_base<storage_t, StringAllocator<storage_t> > {
	public:
		stringbase_t() : container_base<storage_t, StringAllocator<storage_t> >() {
			*finish = 0;
		}

		explicit stringbase_t(const uint32_t initialSize) : container_base<storage_t, StringAllocator<storage_t> >(initialSize + 1) {
			*finish = 0;
		}

		stringbase_t(const stringbase_t& s) : container_base<storage_t, StringAllocator<storage_t> >(s) {
		}

		explicit stringbase_t(const storage_t* cstring, int characterCount) : container_base<storage_t, StringAllocator<storage_t> >(characterCount + 1) {
			copyFrom(cstring, characterCount);

			*finish = 0;
		}

		stringbase_t& operator=(const stringbase_t& s) {
			if (&s == this)
				return *this;

			container_base<storage_t, StringAllocator<storage_t> >::operator=(s);

			return *this;
		}

		const storage_t* c_str() const {
			return start;
		}

		void clear() {
			container_base<storage_t, StringAllocator<storage_t> >::clear();

			*finish = 0;
		}
	};
	
	class string : public stringbase_t<char> {
	public:
		string();
		explicit string(const uint32_t initialCapacity);

		string(const string& s);
		string(const char* cstring);
		string(const char* cstring, uint32_t length);

		void push_back(const char& element);

		string& operator=(const string& s);

		bool operator==(const string& str) const;
		bool operator==(const char* str) const;
		
		uint32_t hashCode() const;
		static uint32_t hashCode(const char* str, uint32_t startCRC = 0xFFFFFFFF);
	};

	bool operator==(const char* s, const soe::string& s2);

	class unicode : public stringbase_t<unsigned short> {
	protected:
		void fromAscii(const char* cstring, uint32_t asciiSize);

	public:
		unicode();

		unicode(const unicode& s);
		unicode(const char* cstring);
		unicode(const char* cstring, uint32_t length);

		void push_back(const unsigned short& element);

		string toAscii() const;

		bool operator==(const unicode& str) const;
		bool operator==(const char* str) const;

		unicode& operator=(const unicode& s);
	};

	bool operator==(const char* s, const soe::unicode& s2);

	template <typename T> using vector = container_base<T>;

}