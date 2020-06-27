#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <random>
#include <thread>
#include <unordered_map>
#include <crow/crow.hpp>
#include <crow/crow_config.hpp>
#include <crow/utils/utils.hpp>

// backward configuration and inclusion
#ifdef NLOHMANN_CROW_HAVE_LIBBFD
#define BACKWARD_HAS_BFD 1
#endif
#ifdef NLOHMANN_CROW_HAVE_LIBDW
#define BACKWARD_HAS_DW 1
#endif
#include <backward/backward.hpp>

#ifdef NLOHMANN_CROW_HAVE_CXA_DEMANGLE
#include <cxxabi.h> // for abi::__cxa_demangle
#endif

#ifdef NLOHMANN_CROW_HAVE_BACKTRACE
#include <execinfo.h> // for backtrace
#endif

#ifdef NLOHMANN_CROW_HAVE_DLADDR
#include <dlfcn.h> // for dladdr
#endif

#if defined(NLOHMANN_CROW_HAVE_PHTREAD_GETNAME_NP) || defined(NLOHMANN_CROW_HAVE_PHTREAD_THREADID_NP)
#include <pthread.h>
#endif

// include header for get_process_id() function
#ifdef NLOHMANN_CROW_HAVE_GETCURRENTPROCESSID
#include <processthreadsapi.h>
#else
#ifdef NLOHMANN_CROW_HAVE_GETPID
#include <unistd.h>
#endif
#endif

#ifdef NLOHMANN_CROW_HAVE_UNISTD_H
#include <unistd.h>
extern char** environ;
#endif

namespace crow {
namespace utils {

exception_information_t exception_information(const std::string& name)
{
    if (name == "std::bad_alloc")
    {
        return {"Type of the exceptions thrown by the standard definitions of operator new and operator new[] when they fail to allocate the requested storage space.",
                "http://www.cplusplus.com/reference/new/bad_alloc/"};
    }
    if (name == "std::domain_error")
    {
        return {"Type of the exceptions thrown as exceptions to report domain errors. Generally, the domain of a mathematical function is the subset of values that it is defined for. For example, the square root function is only defined for non-negative numbers. Thus, a negative number for such a function would qualify as a domain error.",
                "http://www.cplusplus.com/reference/stdexcept/domain_error/"};
    }
    if (name == "std::range_error")
    {
        return {"Type of the exceptions thrown as exceptions to report range errors in internal computations. It is a standard exception that can be thrown by programs. Some components of the standard library also throw exceptions of this type to signal range errors.",
                "http://www.cplusplus.com/reference/stdexcept/range_error/"};
    }
    if (name == "std::invalid_argument")
    {
        return {"Type of the exceptions thrown as exceptions to report an invalid argument. It is a standard exception that can be thrown by programs. Some components of the standard library also throw exceptions of this type to signal invalid arguments.",
                "http://www.cplusplus.com/reference/stdexcept/invalid_argument/"};
    }
    if (name == "std::length_error")
    {
        return {"Type of the exceptions thrown as exceptions to report a length error. It is a standard exception that can be thrown by programs. Some components of the standard library, such as vector and string also throw exceptions of this type to signal errors resizing.",
                "http://www.cplusplus.com/reference/stdexcept/length_error/"};
    }
    if (name == "std::overflow_error")
    {
        return {"Type of the exceptions thrown as exceptions to arithmetic overflow errors. It is a standard exception that can be thrown by programs. Some components of the standard library also throw exceptions of this type to signal range errors.",
                "http://www.cplusplus.com/reference/stdexcept/overflow_error/"};
    }
    if (name == "std::underflow_error")
    {
        return {"Type of the exceptions thrown as exceptions to arithmetic underflow errors. No component of the standard library throws exceptions of this type. It is designed as a standard exception to be thrown by programs.",
                "http://www.cplusplus.com/reference/stdexcept/underflow_error/"};
    }
    if (name == "std::bad_cast")
    {
        return {"Type of the exceptions thrown by dynamic_cast when it fails the run-time check performed on references to polymorphic class types.",
                "http://www.cplusplus.com/reference/stdexcept/bad_cast/"};
    }
    if (name == "std::runtime_error")
    {
        return {"This class defines the type of objects thrown as exceptions to report errors that can only be detected during runtime.",
                "http://www.cplusplus.com/reference/stdexcept/runtime_error/"};
    }
    if (name == "std::logic_error")
    {
        return {"This class defines the type of objects thrown as exceptions to report errors in the internal logical of the program, such as violation of logical preconditions or class invariants.",
                "http://www.cplusplus.com/reference/stdexcept/logic_error/"};
    }
    if (name == "std::out_of_range")
    {
        return {"This class defines the type of objects thrown as exceptions to report an out-of-range error. It is a standard exception that can be thrown by programs. Some components of the standard library, such as vector, deque, string and bitset also throw exceptions of this type to signal arguments out of range.",
                "http://www.cplusplus.com/reference/stdexcept/out_of_range/"};
    }

    return {"", ""};
}

signal_information_t signal_information(const int signal)
{
    switch (signal)
    {
        case 1:
            return {"SIGHUP", "Hangup detected on controlling terminal or death of controlling process"};
        case 2:
            return {"SIGINT", "Interrupt from keyboard; interactive attention signal."};
        case 3:
            return {"SIGQUIT", "Quit from keyboard."};
        case 4:
            return {"SIGILL", "Illegal instruction."};
        case 5:
            return {"SIGTRAP", "Trace/breakpoint trap."};
        case 6:
            return {"SIGABRT", "Abnormal termination; abort signal from abort(3)."};
        case 8:
            return {"SIGFPE", "Floating-point exception: erroneous arithmetic operation."};
        case 11:
            return {"SIGSEGV", "Segmentation violation: invalid memory reference."};
        case 14:
            return {"SIGALRM", "Alarm clock timer signal: alarm(2)."};
        case 15:
            return {"SIGTERM", "Termination request."};
        default:
            return {std::string("signal ") + std::to_string(signal), ""};
    }
}

// from https://stackoverflow.com/a/57519221/266378
char const* errnoname(const int errno_)
{
    switch (errno_)
    {
#ifdef E2BIG
        case E2BIG:
            return "E2BIG";
#endif
#ifdef EACCES
        case EACCES:
            return "EACCES";
#endif
#ifdef EADDRINUSE
        case EADDRINUSE:
            return "EADDRINUSE";
#endif
#ifdef EADDRNOTAVAIL
        case EADDRNOTAVAIL:
            return "EADDRNOTAVAIL";
#endif
#ifdef EADI
        case EADI:
            return "EADI";
#endif
#ifdef EADV
        case EADV:
            return "EADV";
#endif
#ifdef EAFNOSUPPORT
        case EAFNOSUPPORT:
            return "EAFNOSUPPORT";
#endif
#ifdef EAGAIN
        case EAGAIN:
            return "EAGAIN";
#endif
#ifdef EAIO
        case EAIO:
            return "EAIO";
#endif
#ifdef EALIGN
        case EALIGN:
            return "EALIGN";
#endif
#ifdef EALREADY
        case EALREADY:
            return "EALREADY";
#endif
#ifdef EASYNC
        case EASYNC:
            return "EASYNC";
#endif
#ifdef EAUTH
        case EAUTH:
            return "EAUTH";
#endif
#ifdef EBADARCH
        case EBADARCH:
            return "EBADARCH";
#endif
#ifdef EBADE
        case EBADE:
            return "EBADE";
#endif
#ifdef EBADEXEC
        case EBADEXEC:
            return "EBADEXEC";
#endif
#ifdef EBADF
        case EBADF:
            return "EBADF";
#endif
#ifdef EBADFD
        case EBADFD:
            return "EBADFD";
#endif
#ifdef EBADMACHO
        case EBADMACHO:
            return "EBADMACHO";
#endif
#ifdef EBADMSG
        case EBADMSG:
            return "EBADMSG";
#endif
#ifdef EBADR
        case EBADR:
            return "EBADR";
#endif
#ifdef EBADRPC
        case EBADRPC:
            return "EBADRPC";
#endif
#ifdef EBADRQC
        case EBADRQC:
            return "EBADRQC";
#endif
#ifdef EBADSLT
        case EBADSLT:
            return "EBADSLT";
#endif
#ifdef EBADVER
        case EBADVER:
            return "EBADVER";
#endif
#ifdef EBFONT
        case EBFONT:
            return "EBFONT";
#endif
#ifdef EBUSY
        case EBUSY:
            return "EBUSY";
#endif
#ifdef ECANCELED
        case ECANCELED:
            return "ECANCELED";
#endif
#if defined(ECANCELLED) && (!defined(ECANCELED) || ECANCELLED != ECANCELED)
        case ECANCELLED:
            return "ECANCELLED";
#endif
#ifdef ECAPMODE
        case ECAPMODE:
            return "ECAPMODE";
#endif
#ifdef ECHILD
        case ECHILD:
            return "ECHILD";
#endif
#ifdef ECHRNG
        case ECHRNG:
            return "ECHRNG";
#endif
#ifdef ECKPT
        case ECKPT:
            return "ECKPT";
#endif
#ifdef ECLONEME
        case ECLONEME:
            return "ECLONEME";
#endif
#ifdef ECOMM
        case ECOMM:
            return "ECOMM";
#endif
#ifdef ECONFIG
        case ECONFIG:
            return "ECONFIG";
#endif
#ifdef ECONNABORTED
        case ECONNABORTED:
            return "ECONNABORTED";
#endif
#ifdef ECONNREFUSED
        case ECONNREFUSED:
            return "ECONNREFUSED";
#endif
#ifdef ECONNRESET
        case ECONNRESET:
            return "ECONNRESET";
#endif
#ifdef ECORRUPT
        case ECORRUPT:
            return "ECORRUPT";
#endif
#ifdef ECVCERORR
        case ECVCERORR:
            return "ECVCERORR";
#endif
#ifdef ECVPERORR
        case ECVPERORR:
            return "ECVPERORR";
#endif
#ifdef EDEADLK
        case EDEADLK:
            return "EDEADLK";
#endif
#if defined(EDEADLOCK) && (!defined(EDEADLK) || EDEADLOCK != EDEADLK)
        case EDEADLOCK:
            return "EDEADLOCK";
#endif
#ifdef EDESTADDREQ
        case EDESTADDREQ:
            return "EDESTADDREQ";
#endif
#ifdef EDESTADDRREQ
        case EDESTADDRREQ:
            return "EDESTADDRREQ";
#endif
#ifdef EDEVERR
        case EDEVERR:
            return "EDEVERR";
#endif
#ifdef EDIRIOCTL
        case EDIRIOCTL:
            return "EDIRIOCTL";
#endif
#ifdef EDIRTY
        case EDIRTY:
            return "EDIRTY";
#endif
#ifdef EDIST
        case EDIST:
            return "EDIST";
#endif
#ifdef EDOM
        case EDOM:
            return "EDOM";
#endif
#ifdef EDOOFUS
        case EDOOFUS:
            return "EDOOFUS";
#endif
#ifdef EDOTDOT
        case EDOTDOT:
            return "EDOTDOT";
#endif
#ifdef EDQUOT
        case EDQUOT:
            return "EDQUOT";
#endif
#ifdef EDUPFD
        case EDUPFD:
            return "EDUPFD";
#endif
#ifdef EDUPPKG
        case EDUPPKG:
            return "EDUPPKG";
#endif
#ifdef EEXIST
        case EEXIST:
            return "EEXIST";
#endif
#ifdef EFAIL
        case EFAIL:
            return "EFAIL";
#endif
#ifdef EFAULT
        case EFAULT:
            return "EFAULT";
#endif
#ifdef EFBIG
        case EFBIG:
            return "EFBIG";
#endif
#ifdef EFORMAT
        case EFORMAT:
            return "EFORMAT";
#endif
#ifdef EFSCORRUPTED
        case EFSCORRUPTED:
            return "EFSCORRUPTED";
#endif
#ifdef EFTYPE
        case EFTYPE:
            return "EFTYPE";
#endif
#ifdef EHOSTDOWN
        case EHOSTDOWN:
            return "EHOSTDOWN";
#endif
#ifdef EHOSTUNREACH
        case EHOSTUNREACH:
            return "EHOSTUNREACH";
#endif
#ifdef EHWPOISON
        case EHWPOISON:
            return "EHWPOISON";
#endif
#ifdef EIDRM
        case EIDRM:
            return "EIDRM";
#endif
#ifdef EILSEQ
        case EILSEQ:
            return "EILSEQ";
#endif
#ifdef EINIT
        case EINIT:
            return "EINIT";
#endif
#ifdef EINPROG
        case EINPROG:
            return "EINPROG";
#endif
#ifdef EINPROGRESS
        case EINPROGRESS:
            return "EINPROGRESS";
#endif
#ifdef EINTEGRITY
        case EINTEGRITY:
            return "EINTEGRITY";
#endif
#ifdef EINTR
        case EINTR:
            return "EINTR";
#endif
#ifdef EINVAL
        case EINVAL:
            return "EINVAL";
#endif
#ifdef EIO
        case EIO:
            return "EIO";
#endif
#ifdef EIPSEC
        case EIPSEC:
            return "EIPSEC";
#endif
#ifdef EISCONN
        case EISCONN:
            return "EISCONN";
#endif
#ifdef EISDIR
        case EISDIR:
            return "EISDIR";
#endif
#ifdef EISNAM
        case EISNAM:
            return "EISNAM";
#endif
#ifdef EJUSTRETURN
        case EJUSTRETURN:
            return "EJUSTRETURN";
#endif
#ifdef EKEEPLOOKING
        case EKEEPLOOKING:
            return "EKEEPLOOKING";
#endif
#ifdef EKEYEXPIRED
        case EKEYEXPIRED:
            return "EKEYEXPIRED";
#endif
#ifdef EKEYREJECTED
        case EKEYREJECTED:
            return "EKEYREJECTED";
#endif
#ifdef EKEYREVOKED
        case EKEYREVOKED:
            return "EKEYREVOKED";
#endif
#ifdef EL2HLT
        case EL2HLT:
            return "EL2HLT";
#endif
#ifdef EL2NSYNC
        case EL2NSYNC:
            return "EL2NSYNC";
#endif
#ifdef EL3HLT
        case EL3HLT:
            return "EL3HLT";
#endif
#ifdef EL3RST
        case EL3RST:
            return "EL3RST";
#endif
#ifdef ELIBACC
        case ELIBACC:
            return "ELIBACC";
#endif
#ifdef ELIBBAD
        case ELIBBAD:
            return "ELIBBAD";
#endif
#ifdef ELIBEXEC
        case ELIBEXEC:
            return "ELIBEXEC";
#endif
#ifdef ELIBMAX
        case ELIBMAX:
            return "ELIBMAX";
#endif
#ifdef ELIBSCN
        case ELIBSCN:
            return "ELIBSCN";
#endif
#ifdef ELNRNG
        case ELNRNG:
            return "ELNRNG";
#endif
#ifdef ELOCKUNMAPPED
        case ELOCKUNMAPPED:
            return "ELOCKUNMAPPED";
#endif
#ifdef ELOOP
        case ELOOP:
            return "ELOOP";
#endif
#ifdef EMEDIA
        case EMEDIA:
            return "EMEDIA";
#endif
#ifdef EMEDIUMTYPE
        case EMEDIUMTYPE:
            return "EMEDIUMTYPE";
#endif
#ifdef EMFILE
        case EMFILE:
            return "EMFILE";
#endif
#ifdef EMLINK
        case EMLINK:
            return "EMLINK";
#endif
#ifdef EMOUNTEXIT
        case EMOUNTEXIT:
            return "EMOUNTEXIT";
#endif
#ifdef EMOVEFD
        case EMOVEFD:
            return "EMOVEFD";
#endif
#ifdef EMSGSIZE
        case EMSGSIZE:
            return "EMSGSIZE";
#endif
#ifdef EMTIMERS
        case EMTIMERS:
            return "EMTIMERS";
#endif
#ifdef EMULTIHOP
        case EMULTIHOP:
            return "EMULTIHOP";
#endif
#ifdef ENAMETOOLONG
        case ENAMETOOLONG:
            return "ENAMETOOLONG";
#endif
#ifdef ENAVAIL
        case ENAVAIL:
            return "ENAVAIL";
#endif
#ifdef ENEEDAUTH
        case ENEEDAUTH:
            return "ENEEDAUTH";
#endif
#ifdef ENETDOWN
        case ENETDOWN:
            return "ENETDOWN";
#endif
#ifdef ENETRESET
        case ENETRESET:
            return "ENETRESET";
#endif
#ifdef ENETUNREACH
        case ENETUNREACH:
            return "ENETUNREACH";
#endif
#ifdef ENFILE
        case ENFILE:
            return "ENFILE";
#endif
#ifdef ENFSREMOTE
        case ENFSREMOTE:
            return "ENFSREMOTE";
#endif
#ifdef ENOANO
        case ENOANO:
            return "ENOANO";
#endif
#ifdef ENOATTR
        case ENOATTR:
            return "ENOATTR";
#endif
#ifdef ENOBUFS
        case ENOBUFS:
            return "ENOBUFS";
#endif
#ifdef ENOCONNECT
        case ENOCONNECT:
            return "ENOCONNECT";
#endif
#ifdef ENOCSI
        case ENOCSI:
            return "ENOCSI";
#endif
#ifdef ENODATA
        case ENODATA:
            return "ENODATA";
#endif
#ifdef ENODEV
        case ENODEV:
            return "ENODEV";
#endif
#ifdef ENOENT
        case ENOENT:
            return "ENOENT";
#endif
#ifdef ENOEXEC
        case ENOEXEC:
            return "ENOEXEC";
#endif
#ifdef ENOIOCTL
        case ENOIOCTL:
            return "ENOIOCTL";
#endif
#ifdef ENOKEY
        case ENOKEY:
            return "ENOKEY";
#endif
#ifdef ENOLCK
        case ENOLCK:
            return "ENOLCK";
#endif
#ifdef ENOLINK
        case ENOLINK:
            return "ENOLINK";
#endif
#ifdef ENOLOAD
        case ENOLOAD:
            return "ENOLOAD";
#endif
#ifdef ENOMATCH
        case ENOMATCH:
            return "ENOMATCH";
#endif
#ifdef ENOMEDIUM
        case ENOMEDIUM:
            return "ENOMEDIUM";
#endif
#ifdef ENOMEM
        case ENOMEM:
            return "ENOMEM";
#endif
#ifdef ENOMSG
        case ENOMSG:
            return "ENOMSG";
#endif
#ifdef ENONET
        case ENONET:
            return "ENONET";
#endif
#ifdef ENOPKG
        case ENOPKG:
            return "ENOPKG";
#endif
#ifdef ENOPOLICY
        case ENOPOLICY:
            return "ENOPOLICY";
#endif
#ifdef ENOPROTOOPT
        case ENOPROTOOPT:
            return "ENOPROTOOPT";
#endif
#ifdef ENOREG
        case ENOREG:
            return "ENOREG";
#endif
#ifdef ENOSPC
        case ENOSPC:
            return "ENOSPC";
#endif
#ifdef ENOSR
        case ENOSR:
            return "ENOSR";
#endif
#ifdef ENOSTR
        case ENOSTR:
            return "ENOSTR";
#endif
#ifdef ENOSYM
        case ENOSYM:
            return "ENOSYM";
#endif
#ifdef ENOSYS
        case ENOSYS:
            return "ENOSYS";
#endif
#ifdef ENOTACTIVE
        case ENOTACTIVE:
            return "ENOTACTIVE";
#endif
#ifdef ENOTBLK
        case ENOTBLK:
            return "ENOTBLK";
#endif
#ifdef ENOTCAPABLE
        case ENOTCAPABLE:
            return "ENOTCAPABLE";
#endif
#ifdef ENOTCONN
        case ENOTCONN:
            return "ENOTCONN";
#endif
#ifdef ENOTDIR
        case ENOTDIR:
            return "ENOTDIR";
#endif
#ifdef ENOTEMPTY
        case ENOTEMPTY:
            return "ENOTEMPTY";
#endif
#ifdef ENOTNAM
        case ENOTNAM:
            return "ENOTNAM";
#endif
#ifdef ENOTREADY
        case ENOTREADY:
            return "ENOTREADY";
#endif
#ifdef ENOTRECOVERABLE
        case ENOTRECOVERABLE:
            return "ENOTRECOVERABLE";
#endif
#ifdef ENOTRUST
        case ENOTRUST:
            return "ENOTRUST";
#endif
#ifdef ENOTSOCK
        case ENOTSOCK:
            return "ENOTSOCK";
#endif
#ifdef ENOTSUP
        case ENOTSUP:
            return "ENOTSUP";
#endif
#ifdef ENOTTY
        case ENOTTY:
            return "ENOTTY";
#endif
#ifdef ENOTUNIQ
        case ENOTUNIQ:
            return "ENOTUNIQ";
#endif
#ifdef ENOUNLD
        case ENOUNLD:
            return "ENOUNLD";
#endif
#ifdef ENOUNREG
        case ENOUNREG:
            return "ENOUNREG";
#endif
#ifdef ENXIO
        case ENXIO:
            return "ENXIO";
#endif
#ifdef EOPCOMPLETE
        case EOPCOMPLETE:
            return "EOPCOMPLETE";
#endif
#if defined(EOPNOTSUPP) && (!defined(ENOTSUP) || EOPNOTSUPP != ENOTSUP)
        case EOPNOTSUPP:
            return "EOPNOTSUPP";
#endif
#ifdef EOVERFLOW
        case EOVERFLOW:
            return "EOVERFLOW";
#endif
#ifdef EOWNERDEAD
        case EOWNERDEAD:
            return "EOWNERDEAD";
#endif
#ifdef EPASSTHROUGH
        case EPASSTHROUGH:
            return "EPASSTHROUGH";
#endif
#ifdef EPATHREMOTE
        case EPATHREMOTE:
            return "EPATHREMOTE";
#endif
#ifdef EPERM
        case EPERM:
            return "EPERM";
#endif
#ifdef EPFNOSUPPORT
        case EPFNOSUPPORT:
            return "EPFNOSUPPORT";
#endif
#ifdef EPIPE
        case EPIPE:
            return "EPIPE";
#endif
#ifdef EPOWERF
        case EPOWERF:
            return "EPOWERF";
#endif
#ifdef EPROCLIM
        case EPROCLIM:
            return "EPROCLIM";
#endif
#ifdef EPROCUNAVAIL
        case EPROCUNAVAIL:
            return "EPROCUNAVAIL";
#endif
#ifdef EPROGMISMATCH
        case EPROGMISMATCH:
            return "EPROGMISMATCH";
#endif
#ifdef EPROGUNAVAIL
        case EPROGUNAVAIL:
            return "EPROGUNAVAIL";
#endif
#ifdef EPROTO
        case EPROTO:
            return "EPROTO";
#endif
#ifdef EPROTONOSUPPORT
        case EPROTONOSUPPORT:
            return "EPROTONOSUPPORT";
#endif
#ifdef EPROTOTYPE
        case EPROTOTYPE:
            return "EPROTOTYPE";
#endif
#ifdef EPWROFF
        case EPWROFF:
            return "EPWROFF";
#endif
#ifdef EQFULL
        case EQFULL:
            return "EQFULL";
#endif
#ifdef EQSUSPENDED
        case EQSUSPENDED:
            return "EQSUSPENDED";
#endif
#ifdef ERANGE
        case ERANGE:
            return "ERANGE";
#endif
#ifdef ERECYCLE
        case ERECYCLE:
            return "ERECYCLE";
#endif
#ifdef EREDRIVEOPEN
        case EREDRIVEOPEN:
            return "EREDRIVEOPEN";
#endif
#ifdef EREFUSED
        case EREFUSED:
            return "EREFUSED";
#endif
#ifdef ERELOC
        case ERELOC:
            return "ERELOC";
#endif
#ifdef ERELOCATED
        case ERELOCATED:
            return "ERELOCATED";
#endif
#ifdef ERELOOKUP
        case ERELOOKUP:
            return "ERELOOKUP";
#endif
#ifdef EREMCHG
        case EREMCHG:
            return "EREMCHG";
#endif
#ifdef EREMDEV
        case EREMDEV:
            return "EREMDEV";
#endif
#ifdef EREMOTE
        case EREMOTE:
            return "EREMOTE";
#endif
#ifdef EREMOTEIO
        case EREMOTEIO:
            return "EREMOTEIO";
#endif
#ifdef EREMOTERELEASE
        case EREMOTERELEASE:
            return "EREMOTERELEASE";
#endif
#ifdef ERESTART
        case ERESTART:
            return "ERESTART";
#endif
#ifdef ERFKILL
        case ERFKILL:
            return "ERFKILL";
#endif
#ifdef EROFS
        case EROFS:
            return "EROFS";
#endif
#ifdef ERPCMISMATCH
        case ERPCMISMATCH:
            return "ERPCMISMATCH";
#endif
#ifdef ESAD
        case ESAD:
            return "ESAD";
#endif
#ifdef ESHLIBVERS
        case ESHLIBVERS:
            return "ESHLIBVERS";
#endif
#ifdef ESHUTDOWN
        case ESHUTDOWN:
            return "ESHUTDOWN";
#endif
#ifdef ESOCKTNOSUPPORT
        case ESOCKTNOSUPPORT:
            return "ESOCKTNOSUPPORT";
#endif
#ifdef ESOFT
        case ESOFT:
            return "ESOFT";
#endif
#ifdef ESPIPE
        case ESPIPE:
            return "ESPIPE";
#endif
#ifdef ESRCH
        case ESRCH:
            return "ESRCH";
#endif
#ifdef ESRMNT
        case ESRMNT:
            return "ESRMNT";
#endif
#ifdef ESTALE
        case ESTALE:
            return "ESTALE";
#endif
#ifdef ESTART
        case ESTART:
            return "ESTART";
#endif
#ifdef ESTRPIPE
        case ESTRPIPE:
            return "ESTRPIPE";
#endif
#ifdef ESYSERROR
        case ESYSERROR:
            return "ESYSERROR";
#endif
#ifdef ETIME
        case ETIME:
            return "ETIME";
#endif
#ifdef ETIMEDOUT
        case ETIMEDOUT:
            return "ETIMEDOUT";
#endif
#ifdef ETOOMANYREFS
        case ETOOMANYREFS:
            return "ETOOMANYREFS";
#endif
#ifdef ETXTBSY
        case ETXTBSY:
            return "ETXTBSY";
#endif
#ifdef EUCLEAN
        case EUCLEAN:
            return "EUCLEAN";
#endif
#ifdef EUNATCH
        case EUNATCH:
            return "EUNATCH";
#endif
#ifdef EUSERS
        case EUSERS:
            return "EUSERS";
#endif
#ifdef EVERSION
        case EVERSION:
            return "EVERSION";
#endif
#if defined(EWOULDBLOCK) && (!defined(EAGAIN) || EWOULDBLOCK != EAGAIN)
        case EWOULDBLOCK:
            return "EWOULDBLOCK";
#endif
#ifdef EWRONGFS
        case EWRONGFS:
            return "EWRONGFS";
#endif
#ifdef EWRPROTECT
        case EWRPROTECT:
            return "EWRPROTECT";
#endif
#ifdef EXDEV
        case EXDEV:
            return "EXDEV";
#endif
#ifdef EXFULL
        case EXFULL:
            return "EXFULL";
#endif
        default:
            return "";
    }
}

crow::types::interfaces::stacktrace_t get_backtrace(int skip, std::function<bool(const std::string&)> symbol_is_external)
{
    crow::types::interfaces::stacktrace_t result{};
    backward::StackTrace st;
    backward::TraceResolver resolver;
    st.load_here(128);
    resolver.load_stacktrace(st);
    backward::SnippetFactory sf;

    for (std::size_t trace_idx = st.size(); trace_idx > 0; --trace_idx)
    {
        const auto r = resolver.resolve(st[trace_idx]);

        crow::types::interfaces::frame_t entry;
        entry.function = r.object_function;
        entry.module = r.object_filename;

        entry.filename = r.source.filename;
        entry.lineno = static_cast<int>(r.source.line);
        entry.colno = static_cast<int>(r.source.col);

        const auto& snippet = sf.get_snippet(r.source.filename, r.source.line, 5);
        for (auto& line : snippet)
        {
            if (line.first == entry.lineno)
            {
                entry.context_line = line.second;
            }
            else if (line.first < entry.lineno)
            {
                entry.pre_context.push_back(line.second);
            }
            else
            {
                entry.post_context.push_back(line.second);
            }
        }

        entry.in_app = not(r.object_function.substr(0, 5) == "std::" or
                           r.object_function.substr(0, 6) == "crow::" or
                           r.object_function.substr(0, 10) == "backward::" or
                           r.object_function.substr(0, 2) == "__" or
                           r.object_function == "start" or
                           r.object_function == "_start");

        if (entry.in_app.value() and symbol_is_external)
        {
            entry.in_app = symbol_is_external(r.object_filename);
        }

        result.frames.push_back(entry);
    }

    return result;
}

std::string pretty_name(const char* type_id_name,
                        const bool only_module)
{
    if (type_id_name == nullptr)
    {
        return "";
    }

    std::string result = type_id_name;

#ifdef NLOHMANN_CROW_HAVE_CXA_DEMANGLE
    int status;
    char* demangled_output = abi::__cxa_demangle(type_id_name, nullptr, nullptr, &status);
    if (status == 0 and demangled_output != nullptr)
    {
        result = demangled_output;
    }
    std::free(demangled_output);
#endif

    return only_module
               ? result.substr(0, result.find_first_of(':'))
               : result;
}

int get_process_id() noexcept
{
#ifdef NLOHMANN_CROW_HAVE_GETCURRENTPROCESSID
    return static_cast<int>(GetCurrentProcessId());
#else
#ifdef NLOHMANN_CROW_HAVE_GETPID
    return static_cast<int>(::getpid());
#else
#endif
    return -1;
#endif
}

std::string thread_name()
{
#ifdef NLOHMANN_CROW_HAVE_PHTREAD_GETNAME_NP
    char thread_name[64];
    pthread_getname_np(pthread_self(), thread_name, 64);
    return std::string(thread_name);
#else
    return "";
#endif
}

std::size_t thread_id()
{
#ifdef NLOHMANN_CROW_HAVE_PHTREAD_THREADID_NP
    uint64_t result;
    pthread_threadid_np(nullptr, &result);
    return result;
#else
    static std::size_t next_id{0};
    static std::unordered_map<std::thread::id, std::size_t> integer_ids;

    const auto this_id = std::this_thread::get_id();
    if (integer_ids.count(this_id) == 0)
    {
        integer_ids[this_id] = next_id;
        ++next_id;
    }

    return integer_ids[this_id];
#endif
}

void termination_handler()
{
    crow::types::interfaces::breadcrumb_value_t breadcrumb_value;
    breadcrumb_value.message = "termination handler";
    breadcrumb_value.category = "crow handler";
    crow::add_breadcrumb(breadcrumb_value);

    auto current_ex = std::current_exception();
    if (current_ex)
    {
        try
        {
            std::rethrow_exception(current_ex);
        }
        catch (const std::exception& e)
        {
            crow::capture_exception(e, false);
        }
        catch (const char* s)
        {
            types::interfaces::exception_t ex{};
            ex.type = "const char* exception";
            ex.value = s;
            ex.stacktrace = get_backtrace();
            ex.mechanism.type = "exception";
            ex.mechanism.handled = false;
            crow::capture_exception(ex);
        }
        catch (const std::string& s)
        {
            types::interfaces::exception_t ex{};
            ex.type = "std::string exception";
            ex.value = s;
            ex.stacktrace = get_backtrace();
            ex.mechanism.type = "exception";
            ex.mechanism.handled = false;
            crow::capture_exception(ex);
        }
        catch (...)
        {
            types::interfaces::exception_t ex{};
            ex.type = "unknown exception";
            ex.stacktrace = get_backtrace();
            ex.mechanism.type = "exception";
            ex.mechanism.description = "An unknown exception type has been thrown. As it does not inherit from std::exception, what() cannot be called.";
            ex.mechanism.handled = false;
            crow::capture_exception(ex);
        }
    }

    crow::internal::existing_termination_handler();
}

void signal_handler(int signal)
{
    crow::types::interfaces::breadcrumb_value_t breadcrumb_value{};
    breadcrumb_value.message = "signal handler";
    breadcrumb_value.category = "crow handler";
    crow::add_breadcrumb(breadcrumb_value);

    crow::capture_signal(signal, false);
    std::signal(signal, SIG_DFL);
    std::raise(signal);
}

int get_random_number(int lower, int upper)
{
    std::random_device random_device;
    std::default_random_engine random_engine(random_device());
    std::uniform_int_distribution<int> uniform_dist(lower, upper);
    return uniform_dist(random_engine);
}

std::string generate_uuid()
{
    std::string result(32, ' ');

    for (std::size_t i = 0; i < result.size(); ++i)
    {
        // the 12th character must be a '4'
        if (i == 12)
        {
            result[i] = '4';
        }
        else
        {
            // get a random number from 0..15
            const auto r = static_cast<char>(get_random_number(0, 15));
            if (r < 10)
            {
                result[i] = '0' + r;
            }
            else
            {
                result[i] = 'a' + r - static_cast<char>(10);
            }
        }
    }

    return result;
}

std::int64_t get_timestamp()
{
    const auto tp = std::chrono::system_clock::now();
    const auto dur = tp.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(dur).count();
}

std::string get_iso8601()
{
    std::time_t now;
    std::time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];

#ifdef NLOHMANN_CROW_HAVE_GMTIME_R
    struct tm time_data{};
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime_r(&now, &time_data));
#else
    static std::mutex m;
    std::lock_guard<std::mutex> lock(m);
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now));
#endif
    return buf;
}
json get_environment_variables()
{
    json result;

#ifdef NLOHMANN_CROW_HAVE_UNISTD_H
    for (char** var = environ; *var != nullptr; ++var)
    {
        std::string v = *var;
        const auto equals = v.find_first_of('=');
        if (equals != std::string::npos)
        {
            result[v.substr(0, equals)] = v.substr(equals + 1);
        }
    }
#endif

    return result;
}

std::string get_environment_variable(const char* name)
{
    const char* value = std::getenv(name);
    return value != nullptr ? value : "";
}

} // namespace utils
} // namespace crow