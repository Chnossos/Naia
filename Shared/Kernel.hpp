#ifndef KERNEL_HPP_INCLUDED
#define KERNEL_HPP_INCLUDED

#include "md5.h"

struct MD5Hash
{
	md5_byte_t * hash (const std::string & str)
	{
		md5_state_t state;
		md5_init (&state);
		md5_append (&state, (const md5_byte_t *)str.data(), str.size());
		md5_byte_t * digest = new md5_byte_t[16];
		md5_finish (&state, digest);
		return digest;
	}

	std::string hash_to_string (md5_byte_t * digest)
	{
		std::string result;
		if (digest)
		{
			for (int i = 0 ; i < 16 ; ++i)
				result += digest[i];
		}
		return result;
	}
};

struct Kernel : public Singleton<Kernel>
{
	friend class Singleton<Kernel>;

	/*Kernel ()
	{
		InitRandomNumberGenerators();
	}

	~Kernel ()
	{
		CleanupRandomNumberGenerators();
	}*/

	Log log;
	MD5Hash md5;
	Random random;
	Naia::Config config;
	ThreadPool threadpool;
};

#define kernel Kernel::instance()

#endif // KERNEL_HPP_INCLUDED
