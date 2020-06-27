#include <chrono>
#include <cstdlib>
#include <regex>
#include <utility>
#include <crow/client/client.hpp>
#include <crow/crow_config.hpp>
#include <crow/utils/utils.hpp>
#include <http/http.hpp>

namespace crow {

client::client(const std::string& dsn,
               crow::types::configuration configuration)
    : m_start_time(utils::get_iso8601())
    , m_configuration(std::move(configuration))
{
    // take given DSN or use SENTRY_DSN environment variable as fallback
    std::string effective_dns = dsn;
    if (effective_dns.empty())
    {
        auto sentry_dsn_env = std::getenv("SENTRY_DSN");
        if (sentry_dsn_env != nullptr)
        {
            effective_dns = sentry_dsn_env;
        }
    }

    // parse DSN (see https://docs.sentry.io/development/sdk-dev/overview/#parsing-the-dsn)
    if (not effective_dns.empty())
    {
        const std::regex dsn_regex("(http[s]?)://([^:]+)(:[^@]+)?@([^/]+)/([0-9]+)");
        std::smatch pieces_match;

        if (std::regex_match(effective_dns, pieces_match, dsn_regex) and pieces_match.size() == 6)
        {
            const auto scheme = pieces_match.str(1);
            m_public_key = pieces_match.str(2);

            // the secret key is optional; if it exists, we need to strip the leading colon (:)
            m_secret_key = pieces_match.str(3);
            if (not m_secret_key.empty())
            {
                m_secret_key.erase(0, 1);
            }

            const auto host = pieces_match.str(4);
            const auto project_id = pieces_match.str(5);
            m_store_url = scheme + "://" + host + "/api/" + project_id + "/store/";
        }
        else
        {
            throw std::invalid_argument("DNS " + effective_dns + " is invalid");
        }
    }
}

void client::capture_event(const crow::types::event_t& event) const noexcept
{
    try
    {
        crow::types::event_t e = event;
        e.event_id = utils::generate_uuid();
        e.timestamp = utils::get_iso8601();
        e.context.app.app_start_time = m_start_time;

        // security header
        std::string security_header = "X-Sentry-Auth: Sentry sentry_version=5,sentry_client=crow/";
        security_header += std::string(NLOHMANN_CROW_VERSION) + ",sentry_timestamp=";
        security_header += std::to_string(utils::get_timestamp());
        security_header += ",sentry_key=" + m_public_key;
        security_header += ",sentry_secret=" + m_secret_key;

        http::request r;
        r.timeout(m_configuration.http_timeout);
        r.set_header(security_header.c_str());
        r.use_compression(true);
        r.post(m_store_url, nlohmann::json(e));
    }
    catch (...)
    {}
}

void client::close(std::chrono::seconds timeout) noexcept
{
}

void client::flush(std::chrono::seconds timeout) noexcept
{
}

} // namespace crow
