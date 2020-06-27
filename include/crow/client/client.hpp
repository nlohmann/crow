#pragma once

#include <chrono>
#include <string>
#include <crow/types/configuration.hpp>
#include <crow/types/event.hpp>

namespace crow {

class client
{
  public:
    /*!
     * @param[in] dsn The DSN tells the SDK where to send the events to. If this
     *            value is not provided, the SDK will try to read it from the
     *            `SENTRY_DSN` environment variable. If that variable also does not
     *            exist, the SDK will just not send any events.
     * @param[in] configuration further configuration parameters
     */
    explicit client(const std::string& dsn = "",
                    types::configuration configuration = {});

    virtual ~client() = default;

    virtual void capture_event(const crow::types::event_t& event) const noexcept;
    virtual void close(std::chrono::seconds timeout) noexcept;
    virtual void flush(std::chrono::seconds timeout) noexcept;

  private:
    std::string m_public_key;
    std::string m_secret_key;
    std::string m_store_url;
    const std::string m_start_time;
    crow::types::configuration m_configuration;
};

} // namespace crow
