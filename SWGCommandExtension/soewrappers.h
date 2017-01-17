#pragma once

#include <type_traits>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <cassert>
#include <wchar.h>
#include <vector>
#include <utility>
#include <tuple>

#define STRALLOCATOR_ADDRESS 0x012EA770 /*for sizes <= 0x80, used in strings*/
#define SOEALLOCATOR_ADDRESS 0x00AC15C0 /*for sizes > 0x80 and array boolean as false*/

#define ARRAYALLOCATOR_ADDRESS 0x00AC15E0 /*soes own allocator for generic new objects with array boolean true*/
#define ARRAYEALLOCATOR_ADDRESS 0x00AC1660

#define STRDEALLOCATOR1_ADDRESS 0x00AC1640 /*soe dealloc for sizes > 0x80 with array boolean false*/
#define STRDEALLOCATOR2_ADDRESS 0x012EA920 /*for sizes < 0x80 looks like system deallocator, used in strings*/

#define EMPTY_CONTAINER_INITIAL_OBJECTS 8

#define DEFINE_CLIENT_STATIC(x, y) static x ## & y;
#define SET_CLIENT_STATIC(x, y) decltype(x) x = *reinterpret_cast<std::add_pointer<std::remove_reference<decltype(x)>::type>::type>(y);

#define DEFINE_HOOK(ADDRESS, METHOD, ORIGINAL) typedef Hook<ADDRESS, decltype(&METHOD)> ORIGINAL; \
	typedef Hook<ADDRESS, decltype(&METHOD)> METHOD##_hook_t;

#define DEFINE_VMETHOD(ADDRESS, METHOD) \
	typedef Hook<ADDRESS, decltype(&METHOD)> METHOD##_hook_t;

#define SETVTABLEENTRY(NEWVTABLE, METHOD) \
	METHOD##_hook_t::hookStorage_t::newMethod = &METHOD; \
	NEWVTABLE[METHOD##_hook_t::hookStorage_t::address / sizeof(void*) + 1] =  *((uint32_t*)((void*)&METHOD##_hook_t::hookStorage_t::newMethod));

#define SETVTABLE(NEWVTABLE) *((uint32_t**)this) = NEWVTABLE + 1;

#define SETVTABLEOFFSET(NEWVTABLE, OFFSET) *((uint32_t**)this + (OFFSET / sizeof(uint32_t))) = NEWVTABLE + 1;

#define INITIALIZE_VTABLE_DATA(NEWVTABLE) static int initialize_##NEWVTABLE = initVtableData(NEWVTABLE, this, sizeof(NEWVTABLE));

#define INITIALIZE_VTABLE_DATAFROM(NEWVTABLE, SRC) static int initialize_##NEWVTABLE = initVtableDataFrom(NEWVTABLE, (void*)SRC, sizeof(NEWVTABLE));

#define DEFINE_VTABLE(NEWVTABLE) static uint32_t NEWVTABLE[];

#define GENERATE_HOOK_TYPE_OLD(x) decltype(&x)
#define DEFINE_HOOOK_OLD(x, y, z) typedef GENERATE_HOOK_TYPE(y) z ## _t; z ## _t z = (decltype(&y)) x;*/
#define GENERATE_HOOK_THIS_TYPE_OLD(CLASS, METHOD, RETURNTYPE, ...) HookThis<CLASS ## *, decltype(& ## CLASS ## :: ## METHOD), RETURNTYPE, ##__VA_ARGS__ ## >
#define SET_HOOK_THISCALL_OLD(CLASS, METHOD, RETURNTYPE, HOOKOBJECT, ...) \
	typedef GENERATE_HOOK_THIS_TYPE(CLASS, METHOD, RETURNTYPE, ##__VA_ARGS__) HOOKOBJECT;
#define DEFINE_HOOK_THISCALL_OLD(ADDRESS, CLASS, RETURNTYPE, METHOD, ...) \
	template<typename ThisType, typename C, typename ReturnType, typename ... Args> \
	C HookThis<ThisType, C, ReturnType, Args...>::newMethod = & ## CLASS ## :: ## METHOD; \
	\
	template<typename ThisType, typename C, typename ReturnType, typename ... Args> \
	typename HookThis<ThisType, C, ReturnType, Args...>::original_t HookThis<ThisType, C, ReturnType, Args...>::original = (typename HookThis<ThisType, C, ReturnType, Args...>::original_t)(ADDRESS); \
	\
	typedef GENERATE_HOOK_THIS_TYPE_OLD(CLASS, METHOD, RETURNTYPE, ##__VA_ARGS__) CLASS ## _ ## METHOD ## _hook_t;


template <uint32_t N> constexpr uint32_t compile_time() {
	return N;
}

namespace soe {
	template<class F>
	struct function_traits;

	template<class F>
	struct function_traits;

	// function pointer
	template<class R, class... Args>
	struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)> {
	};

	template<class R, class... Args>
	struct function_traits<R(Args...)> {
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct argument {
			static_assert(N < arity, "error: invalid parameter index.");
			using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
		};
	};

	// member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&, Args...)> {
	};

	// const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&, Args...)> {
	};

	// member object pointer
	template<class C, class R>
	struct function_traits<R(C::*)> : public function_traits<R(C&)> {
	};

	// functor
	template<class F>
	struct function_traits {
	private:
		using call_type = function_traits<decltype(&F::type::operator())>;
	public:
		using return_type = typename call_type::return_type;

		static constexpr std::size_t arity = call_type::arity - 1;

		template <std::size_t N>
		struct argument {
			static_assert(N < arity, "error: invalid parameter index.");
			using type = typename call_type::template argument<N + 1>::type;
		};
	};

	template<class F>
	struct function_traits<F&> : public function_traits<F> {
	};

	template<class F>
	struct function_traits<F&&> : public function_traits<F> {
	};

	typedef void*(__cdecl* strallocator_t)(uint32_t);
	typedef void*(__cdecl* strallocator2_t)(uint32_t);
	typedef void(__cdecl* strdeallocator1_t)(void*);
	typedef void(__cdecl* strdeallocator2_t)(void*, uint32_t);

	template<typename storage_t>
	class allocator {
	public:
		constexpr allocator() {
		}

		static storage_t* allocate(std::size_t objectCount) {
			static strallocator_t strallocator1 = reinterpret_cast<strallocator_t>(SOEALLOCATOR_ADDRESS);

			return reinterpret_cast<storage_t*>(strallocator1(objectCount * sizeof(storage_t)));
		}

		static void deallocate(storage_t* address, std::size_t objectCount) {
			static strdeallocator1_t deall1 = reinterpret_cast<strdeallocator1_t>(STRDEALLOCATOR1_ADDRESS);
			deall1(address);
		}
	};

	template<typename storage_t>
	class string_allocator : public allocator<storage_t> {
	public:
		constexpr string_allocator() {
		}

		static storage_t* allocate(std::size_t objectCount) {
			if ((objectCount * sizeof(storage_t)) > 0x80) {
				static strallocator_t strallocator1 = reinterpret_cast<strallocator_t>(SOEALLOCATOR_ADDRESS);

				return reinterpret_cast<storage_t*>(strallocator1(objectCount * sizeof(storage_t)));
			} else {
				static strallocator2_t strallocator2 = reinterpret_cast<strallocator2_t>(STRALLOCATOR_ADDRESS);

				return reinterpret_cast<storage_t*>(strallocator2(objectCount * sizeof(storage_t)));
			}
		}

		static void deallocate(storage_t* address, std::size_t objectCount) {
			if ((objectCount * sizeof(storage_t)) > 0x80) {
				static strdeallocator1_t deall1 = reinterpret_cast<strdeallocator1_t>(STRDEALLOCATOR1_ADDRESS);
				deall1(address);
			} else {
				static strdeallocator2_t deall = reinterpret_cast<strdeallocator2_t>(STRDEALLOCATOR2_ADDRESS);
				deall(address, objectCount * sizeof(storage_t));
			}
		}
	};

	template<typename storage_t, class Allocator = allocator<storage_t>>
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

		//gotta find the reallocs.. for now doing new allocate + copy instead on expand
		void ensureCapacity(std::size_t newSize);

	public:
		container_base() {
			start = finish = Allocator::allocate(EMPTY_CONTAINER_INITIAL_OBJECTS);
			endOfStorage = start + EMPTY_CONTAINER_INITIAL_OBJECTS;
		}

		explicit container_base(const uint32_t initialAllocObjectCount) {
			start = finish = Allocator::allocate(initialAllocObjectCount);
			endOfStorage = start + initialAllocObjectCount;
		}

		container_base(std::initializer_list<storage_t> list);
		container_base(const container_base& c);
		container_base(container_base&& c) noexcept;

		~container_base();

		container_base& operator=(const container_base& c);
		container_base& operator=(container_base&& c) noexcept;

		iterator find(const storage_t& obj, std::size_t pos = 0) noexcept {
			return std::find(std::begin(*this) + pos, std::end(*this), obj);
		}

		const_iterator find(const storage_t& obj, std::size_t pos = 0) const noexcept {
			return std::find(std::begin(*this) + pos, std::end(*this), obj);
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

		std::size_t size() const noexcept {
			return finish - start;
		}

		std::size_t capacity() const noexcept {
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
	void container_base<storage_t, Allocator>::ensureCapacity(std::size_t newSize) {
		std::size_t oldSize = endOfStorage - start;
		std::size_t oldFinishOffset = finish - start;

		if (newSize <= oldSize)
			return;

		newSize = (std::max)(newSize, oldSize * 2);

		storage_t* newStorage = Allocator::allocate(newSize);

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

		Allocator::deallocate(start, endOfStorage - start);

		start = newStorage;
		endOfStorage = newStorage + newSize;
		finish = start + oldFinishOffset;
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>::container_base(std::initializer_list<storage_t> list) {
		const uint32_t incomingSize = list.size();

		start = Allocator::allocate(incomingSize);
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

		start = finish = Allocator::allocate(incomingSize);
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

			Allocator::deallocate(start, endOfStorage - start);
		}
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>& container_base<storage_t, Allocator>::operator=(const container_base& c) {
		if (this == &c)
			return *this;

		destroyRange(0, size());
		Allocator::deallocate(start, endOfStorage - start);

		uint32_t incomingSize = c.endOfStorage - c.start;

		start = finish = Allocator::allocate(incomingSize);
		endOfStorage = start + incomingSize;

		copyFrom(c.start, incomingSize);

		return *this;
	}

	template <typename storage_t, class Allocator>
	container_base<storage_t, Allocator>& container_base<storage_t, Allocator>::operator=(container_base&& c) noexcept {
		if (this == &c)
			return *this;

		destroyRange(0, size());
		Allocator::deallocate(start, endOfStorage - start);

		start = c.start;
		finish = c.finish;
		endOfStorage = c.endOfStorage;

		c.start = nullptr;
		c.finish = nullptr;
		c.endOfStorage = nullptr;

		return *this;
	}

	template <typename storage_t>
	class stringbase_t : public container_base<storage_t, string_allocator<storage_t> > {
	public:
		typedef container_base<storage_t, string_allocator<storage_t> > base_t;
		const static std::size_t npos = -1;

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

		void push_back(const storage_t& element);

		template<class ReturnType>
		ReturnType substr(std::size_t pos = 0, std::size_t len = npos) const;

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
	void stringbase_t<storage_t>::push_back(const storage_t& element) {
		base_t::ensureCapacity(base_t::size() + 2);

		base_t::push_back(0);

		*(base_t::end() - 1) = element;
		*base_t::end() = 0;
	}

	template <typename storage_t>
	template <class ReturnType>
	ReturnType stringbase_t<storage_t>::substr(std::size_t beginIndex, std::size_t len) const {
		auto count = base_t::size();

		if (beginIndex > count)
			throw std::exception("out of bounds");
		else if (len != npos && (len - beginIndex) > count) {
			len = count - beginIndex;
		} else if (beginIndex == count)
			return ReturnType();

		if (len == npos)
			return ReturnType(base_t::begin() + beginIndex);
		else
			return ReturnType(base_t::begin() + beginIndex, len);
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

		string substr(std::size_t pos = 0, std::size_t len = npos) const {
			return stringbase_t<char>::substr<string>(pos, len);
		}

		std::string to_stdstring() const {
			return std::string(c_str());
		}

		string& operator=(const string& s);

		string operator+ (const string& rhs) const;
		string operator+ (const char* rhs) const;

		string& operator+=(const string& rhs);
		string& operator+=(const char* rhs);

		bool operator==(const string& str) const;
		bool operator==(const char* str) const;

		std::size_t find(char str, std::size_t pos = 0) const noexcept {
			auto it = stringbase_t::find(str, pos);

			if (it != stringbase_t::end()) {
				return it - stringbase_t::begin();
			} else {
				return npos;
			}
		}

		//std::size_t hash() const;
		
		uint32_t hashCode() const;

		static constexpr uint32_t hashCode(const char* string, uint32_t startCRC = 0xFFFFFFFF) {
			return *string ?
				hashCode(string + 1, crctable[((startCRC >> 24) ^ (uint8_t)(*string)) & 0xFF] ^ (startCRC << 8))
				: ~startCRC;
		}

		static constexpr uint32_t crctable[256] = {
			0x0000000,
			0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B,
			0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6,
			0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
			0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC,
			0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F,
			0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A,
			0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
			0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58,
			0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033,
			0xA4AD16EA, 0xA06C0B5D, 0xD4326D90, 0xD0F37027, 0xDDB056FE,
			0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
			0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4,
			0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 0x34867077, 0x30476DC0,
			0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5,
			0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
			0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 0x7897AB07,
			0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C,
			0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1,
			0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
			0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B,
			0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698,
			0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D,
			0x94EA7B2A, 0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
			0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 0xC6BCF05F,
			0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34,
			0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80,
			0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
			0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A,
			0x58C1663D, 0x558240E4, 0x51435D53, 0x251D3B9E, 0x21DC2629,
			0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C,
			0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
			0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E,
			0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65,
			0xEBA91BBC, 0xEF68060B, 0xD727BBB6, 0xD3E6A601, 0xDEA580D8,
			0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
			0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2,
			0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71,
			0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74,
			0x857130C3, 0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
			0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21,
			0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A,
			0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E, 0x18197087,
			0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
			0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D,
			0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 0xC5A92679, 0xC1683BCE,
			0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB,
			0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
			0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4, 0x89B8FD09,
			0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
			0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF,
			0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4,
		};
	};

	bool operator==(const char* s, const soe::string& s2);
	string operator+(const char* s, const soe::string& s2);

	class unicode : public stringbase_t<wchar_t> {
	protected:
		void fromAscii(const char* cstring);

	public:
		static_assert(sizeof(wchar_t) == sizeof(uint16_t),
			"soe unicode implementation runs on 2 bytes");

		DEFINE_CLIENT_STATIC(const unicode, empty_string);

		unicode();
		unicode(unicode&& s) noexcept;
		explicit unicode(const uint32_t initialCapacity);

		unicode(const unicode& s);
		unicode(const char* cstring);
		unicode(const char* cstring, uint32_t length);

		unicode(const wchar_t* cstring);
		unicode(const wchar_t* cstring, uint32_t length);

		std::size_t find(wchar_t str, std::size_t pos = 0) const noexcept {
			auto it = stringbase_t::find(str, pos);

			if (it != stringbase_t::end()) {
				return it - stringbase_t::begin();
			} else {
				return npos;
			}
		}

		string toAscii() const;

		unicode substr(std::size_t pos = 0, std::size_t len = npos) const {
			return stringbase_t<wchar_t>::substr<unicode>(pos, len);
		}

		std::size_t find(const unicode& str, std::size_t pos = 0) const noexcept;

		std::wstring to_stdwstring() const {
			return std::wstring(c_str());
		}

		bool operator==(const unicode& str) const;
		bool operator==(const char* str) const;
		bool operator==(const wchar_t* str) const;

		unicode& operator=(const unicode& s);
		unicode operator+ (const unicode& rhs) const;

		unicode& operator+=(const unicode& rhs);
		unicode& operator+=(const wchar_t*);
	};

	bool operator==(const char* s, const soe::unicode& s2);
	unicode operator+(const wchar_t* s, const soe::unicode& s2);

	template <typename T> using vector = container_base<T>;

	template<typename T>
	std::vector<T> to_stdvector(const soe::vector<T>& incoming) {
		auto size = incoming.size();

		if (size == 0)
			return std::vector<T>();

		std::vector<T> vec(incoming.size());

		for (const auto& obj : incoming) {
			vec.push_back(obj);
		}

		return vec;
	}

}
