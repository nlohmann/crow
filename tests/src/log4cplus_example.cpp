#include <crow/crow.hpp>
#include <crow/integrations/log4cplus.hpp>

log4cplus::Logger log_ex;

void bar(int i)
{
    LOG4CPLUS_INFO(log_ex, "i = " << i);

    if (i == 99)
    {
        LOG4CPLUS_ERROR(log_ex, "reached i value " << i);
    }
}

void foo()
{
    LOG4CPLUS_DEBUG(log_ex, "entered foo");

    for (int i = 0; i < 100; ++i)
    {
        bar(i);
    }

    LOG4CPLUS_DEBUG(log_ex, "left foo");
}

int main()
{
    crow::init("https://a2bfbfbd7eb74a66b799535fad454555:0d03645ed1d641ee9a74b18fcd9a83c6@sentry.io/1535511");
    crow::install_handlers();

    // init log4cplus and create a logger
    log4cplus::Initializer initializer;
    log_ex = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("example logger"));

    // create and add the appender
    log4cplus::SharedAppenderPtr appender(new nlohmann::crow_integrations::log4cplus_appender());
    log_ex.addAppender(appender);

    crow::configure_scope([](crow::scope& scope) {
        scope.set_extra("test_case", "log4cplus_example");
    });

    foo();
}
