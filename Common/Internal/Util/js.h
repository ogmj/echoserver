//
//  File:       jstring.h
//
//  Functions:  js::sprint
//  Functions:  js::format
//
//  by:     ogmj
//

#pragma once

namespace js
{
	namespace str
	{
		template< typename _Elem > struct str_api {};

		template<>
		struct str_api< char >
		{
			static int vsnprintf( char * _Dest, size_t len, const char *fmt, va_list & args )		{ return _vsnprintf_s( _Dest, len, len-1, fmt, args ); }
		};

		template<>
		struct str_api< wchar_t >
		{
			static int vsnprintf( wchar_t * _Dest, size_t len, const wchar_t *fmt, va_list & args )	{ return _vsnwprintf_s( _Dest, len, len/sizeof(wchar_t)-1, fmt, args ); }
		};

		template< typename _Elem >
		inline void sprintf( std::basic_string< _Elem > & str, const _Elem* fmt, ... )
		{
			_Elem buf[128];
			va_list args;
			va_start( args, fmt );

			_Elem* p = buf;
			size_t buf_len = sizeof(buf)/sizeof(_Elem);

			while ( -1 == str_api< _Elem >::vsnprintf( p, buf_len, fmt, args ) )
			{
				if ( p != buf ) delete [] p;

				buf_len *= 2;

				p = new _Elem[ buf_len ];
			}

			str = p;

			if ( p != buf ) delete [] p;
		}

		template< typename _Elem >
		inline std::basic_string< _Elem > format( const _Elem* fmt, ... )
		{
			_Elem buf[128];
			va_list args;
			va_start( args, fmt );

			_Elem* p = buf;
			size_t buf_len = sizeof(buf)/sizeof(_Elem);

			while ( -1 == str_api< _Elem >::vsnprintf( p, buf_len, fmt, args ) )
			{
				if ( p != buf ) delete [] p;

				buf_len *= 2;

				p = new _Elem[ buf_len ];
			}

			std::basic_string< _Elem > strTemp = p;

			if ( p != buf ) delete [] p;

			return strTemp;
		}
	};
};

namespace js	{ using namespace js::str;	};

