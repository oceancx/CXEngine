#ifndef _ASIO_KCP_LOG_HPP__
#define _ASIO_KCP_LOG_HPP__

//#include <g2log.h>
#include <ostream>
#include <fstream>
#include <utility>
//#include <muduo/base/Logging.h>
//#include <muduo/base/LogFile.h>
#include "logger.h"


// using g2log for asio_kcp_server. Because it's asynchronous and thread safe.
// g2log doc: http://www.codeproject.com/Articles/288827/g-log-An-efficient-asynchronous-logger-using-Cplus


// using muduo for kcp packet log. Because I want to split the logic log and the package log.



// log level: DEBUG, INFO, WARNING, FATAL

// do not use following four macro directly.
// {
    #define AK_LOG      cxlog_err       // LOG(FATAL) << "This message is FATAL. After log flush -> Abort()";
    #define AK_LOGF     cxlog_warn      // LOGF(WARNING, "Printf-style syntax is also %s", "available");
    #define AK_LOG_IF   cxlog_err    // LOG_IF(FATAL, (2>3)) << "This message is not FATAL";
    #define AK_LOGF_IF  cxlog_warn

    // CHECK AND CHECKF will exit the program. same as FATAL
    //#define AK_CHECK CHECK
    //#define AK_CHECKF CHECKF    // CHECKF(foo(), "if 'false == foo()' then the %s is broken: FATAL), "contract");
// }


// Please using the macro below,
//

struct nullbuf_t : std::streambuf {
    char buf[256];
    int overflow(int c)
    {
        this->setp(this->buf, this->buf + 256);
        return std::char_traits<char>::not_eof(c);
    }
};

class nullstream_creator_t {
public:
    nullstream_creator_t() : ostrm_(&buf_) {}
    std::ostream& get_ostrm() {return ostrm_;}
    nullbuf_t buf_;
    std::ostream ostrm_;
};


#define AK_DEBUG_LOG    AK_LOG(DEBUG)
//#define AK_DEBUG_LOG nullstream_creator_t().get_ostrm()


#define AK_INFO_LOG     AK_LOG(INFO)
#define AK_WARNING_LOG  AK_LOG(WORNING)
#define AK_FATAL_LOG    AK_LOG(FATAL)

#define AK_MUDUO_LOG_INFO LOG_INFO  // log structure from muduo

#define AK_UDP_PACKET_LOG AK_MUDUO_LOG_INFO
#define AK_ASK_PACKET_LOG AK_MUDUO_LOG_INFO

#endif // _ASIO_KCP_LOG_HPP__
