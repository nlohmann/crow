# Log4cplus Integration

The library contains an integration for Log4cplus that can be integrated as an appender.

```cpp
// init log4cplus and create a logger
log4cplus::Initializer initializer;
log4cplus::Logger log_ex = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("example logger"));

// create a Crow client
crow crow_client("https://abc:cde@sentry.io/1253079");

// create and add the appender
log4cplus::SharedAppenderPtr appender(new nlohmann::crow_integrations::log4cplus_appender(crow_client));
log_ex.addAppender(appender);
```

Now, all calls to Log4cplus are also sent to the Crow client. Calls to `LOG4CPLUS_TRACE`, `LOG4CPLUS_DEBUG`, `LOG4CPLUS_INFO`, and `LOG4CPLUS_WARN` create breadcrumbs, and calls to `LOG4CPLUS_ERROR` and `LOG4CPLUS_FATAL` create messages.

See `nlohmann::crow_integrations::log4cplus_appender` for more information.

Note you need to include `<crow/integrations/log4cplus.hpp>` to use the appender.
