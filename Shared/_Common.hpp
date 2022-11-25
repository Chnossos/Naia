#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define BOOST_THREAD_USE_LIB
#define BOOST_SYSTEM_NO_DEPRECATED
#define SERVER_DECL
#define SCRIPT_DECL

// Includes
//#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>
//#include <mutex>
#include <set>
#include <sstream>
#include <string>
//#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
#include <winsock2.h>
#include <windows.h>

#include <boost/algorithm/string.hpp> /// Pour boost::trim()
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/function.hpp>
//#include <boost/enable_shared_from_this.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
//#include <boost/tr1/unordered_map.hpp>
#include <boost/thread.hpp>

#include <mysql.h>

#if COMPILER == COMPILER_MICROSOFT

#define I64FMT "%016I64X"
#define I64FMTD "%I64u"
#define SI64FMTD "%I64d"
#define snprintf _snprintf
#define atoll __atoi64

#else

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define I64FMT "%016llX"
#define I64FMTD "%llu"
#define SI64FMTD "%lld"

#endif

#if COMPILER != COMPILER_GNU
typedef signed __int64 uint64;
typedef signed __int32 uint32;
typedef signed __int16 int16;
typedef signed __int8 int8;

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;
typedef unsigned __int8 uint8;

#endif

// Namespaces
using namespace std;
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

// Typedefs
typedef boost::shared_ptr<boost::asio::io_service> io_ptr;
typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
typedef boost::shared_ptr<boost::thread> thread_ptr;

#include "Singleton.hpp"
#include "Constants.hpp"
#include "ProgressUnit.hpp"
#include "Utils.hpp"

#include "Config.hpp"
#include "ConfigDefault.hpp"
#include "Log.hpp"
#include "LanguageManager.hpp"
#include "Database.hpp"
#include "PacketHeader.hpp"
#include "Packet.hpp"
#include "md5.h"
#include "Timer.hpp"
#include "MersenneTwister.hpp"

#include "ThreadPool.hpp"

#include "Kernel.hpp"

#include "Connector.hpp"

#endif // COMMON_H_INCLUDED
