
template < typename T >
class CSingleton
{
public:
	CSingleton()
	{
	}

	static T * GetInstance()
	{
		if( ms_pInstance == NULL )
		{
			ms_pInstance = new T;
			atexit( ReleaseInstance );
		}
		return ms_pInstance;
	}
	static void ReleaseInstance()
	{
		SAFE_DELETE( ms_pInstance );	
	}

private:
	static T * ms_pInstance;
};

template< typename T >
T *	CSingleton< T >::ms_pInstance = NULL; 

