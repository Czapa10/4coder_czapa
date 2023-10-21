#include <cstdint>
#include <cstring>
#include <cctype>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

#define null nullptr
#define global static
#define fn static auto

#define ForCount(_Count) for(u32 Index = 0; Index < Count; ++Index)

constexpr u32 DefaultStringSize = 512;

template<u32 Size = DefaultStringSize>
struct static_string
{
    char Characters[Size];
    u64 Count;
    
    void operator+=(char C)
    { Characters[Count++] = C; }
};

template<u32 ResSize = DefaultStringSize>
fn ToStaticString
(String_Const_u8 String)
{
    static_string<ResSize> Res;
    if(String.size <= ResSize)
    {
        memcpy(Res.Characters, String.str, String.size);
        Res.Count = String.size;
    }
    return Res;
}

template<class static_string_type>
fn ToStringConstU8
(const static_string_type& String)
{
    String_Const_u8 Res;
    Res.str = (u8*)String.Characters;
    Res.size = String.Count;
    return Res;
}

fn ToStringConstU8
(const char* Str)
{
	String_Const_u8 Res;
	Res.str = (u8*)Str;
	Res.size = strlen(Str);
	return Res;
}

template<class code> struct _defer 
{
    code Code;
    _defer(code Code) : Code(Code) {}
    ~_defer() { Code(); }
};

template<class code> _defer<code> MakeDefer(code Code) 
{ return _defer<code>(Code); }

#define _Defer1(x, y) x##y
#define _Defer2(x, y) _Defer1(x, y)
#define _Defer3(x)    _Defer2(x, __COUNTER__)
#define defer(code)   auto _Defer3(_defer_) = MakeDefer([&](){code;})

