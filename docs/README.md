# Crow - a Sentry client for C++

## Feature overview from Sentry's SDK documentation

| Feature | Implemented | Details |
| ---- | ---- | ---- |
| DSN configuration | yes | The DNS can be set as parameter to @ref crow::init and @ref crow::client::client, or via the `SENTRY_DSN` environment variable. |
| Graceful failures (e.g. Sentry server is unreachable) | no | |
| Setting attributes (e.g. tags and extra data) | yes | |
| Support for Linux, Windows and OS X (where applicable) | yes | |
| Background Sending / Non-blocking event submission | no | |
| Scopes | yes | |
| Automatic Context Data | yes |  |
| Breadcrumbs | yes | |
| Event Sampling | no | (started) |
| In-App frames | no | |
| Surrounding Source in Stack Trace | yes | |
| Desymbolication | yes | Desymbolication is done via `abi::__cxa_demangle` and the [Backward-cpp](https://github.com/bombela/backward-cpp) library. |
| Retrieve Last Event ID | no | |
| User Feedback | no | |
| Before-Send Hook | yes | A before-send hook can be set with @ref crow::set_before_send_hook. |
| Before-Breadcrumb Hook | no | |
| List Loaded Libraries | no | |
| Buffer to Disk | no | |
| HTTP Proxy | yes | Crow is using libcurl for HTTP requests, so it respects the proxy environment variables named `http_proxy`. See <https://curl.haxx.se/libcurl/c/CURLOPT_PROXY.html> for more information. |
| Automated error capturing (e.g. uncaught exception handlers) | yes | Crow can react on uncaught exceptions or raised signals. See @ref crow::install_handlers for more information. |
| Logging framework integration | no | |
| Context data helpers (e.g. setting the current user, recording breadcrumbs) | yes | |
| Event sampling | no | |
| Honor Sentry’s HTTP 429 Retry-After header | no | |
| Local variable values in stack trace (on platforms where this is possible | no | |

See <https://docs.sentry.io/development/sdk-dev/overview/> and <https://docs.sentry.io/development/sdk-dev/features/> for a complete list.
