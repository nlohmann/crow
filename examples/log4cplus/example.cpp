#include <crow/crow.hpp>
#include <crow/integrations/log4cplus.hpp>
#include <log4cplus/log4cplus.h>

using crow = nlohmann::crow;

int main()
{
    // init log4cplus and create a logger
    log4cplus::Initializer initializer;
    log4cplus::Logger test = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("test"));

    // create a Crow client
    crow crow_client("https://fad7ed01056940969a519aba36dc0b2f:3787b21e465845a09d781ab9eb048ae7@sentry.io/1253079");

    log4cplus::SharedAppenderPtr append(new nlohmann::crow_integrations::log4cplus_appender(crow_client));
    test.addAppender(append);

    LOG4CPLUS_TRACE(test, "this is a trace message");
    LOG4CPLUS_DEBUG(test, "this is a debug message");
    LOG4CPLUS_INFO(test, "this is an info message");
    LOG4CPLUS_WARN(test, "this is a warning message");
    LOG4CPLUS_ERROR(test, "this is an error message");
    LOG4CPLUS_FATAL(test, "this is a fatal message");
}
