#include <crow/crow.hpp>

int main()
{
    auto c = crow("https://fad7ed01056940969a519aba36dc0b2f:3787b21e465845a09d781ab9eb048ae7@sentry.io/1253079");
    std::cout << "created client" << std::endl;

    c.add_breadcrumb(__DATE__ " " __TIME__);
    std::cout << "added breadcrumb" << std::endl;

    const char* job_number = getenv("APPVEYOR_JOB_NUMBER");
    if (job_number != nullptr)
    {
        c.add_extra_context({{"job_number", job_number}});
        c.add_tags_context({{"ci", "appveyor"}});
    }
    else
    {
        job_number = getenv("TRAVIS_JOB_NUMBER");
        const char* build_number = getenv("APPVEYOR_BUILD_NUMBER");
        if (job_number)
        {
            c.add_extra_context({{"job_number", job_number}});
            c.add_tags_context({{"ci", "travis"}});
            if (build_number)
            {
                c.add_extra_context({{"build_number", build_number}});
            }
        }
    }

    c.add_extra_context({{"NLOHMANN_CROW_CMAKE_SYSTEM_NAME", NLOHMANN_CROW_CMAKE_SYSTEM_NAME}});
    c.add_extra_context({{"NLOHMANN_CROW_CMAKE_SYSTEM_VERSION", NLOHMANN_CROW_CMAKE_SYSTEM_VERSION}});
    c.add_extra_context({{"NLOHMANN_CROW_OS_RELEASE", NLOHMANN_CROW_OS_RELEASE}});
    c.add_extra_context({{"NLOHMANN_CROW_OS_VERSION", NLOHMANN_CROW_OS_VERSION}});
    c.add_extra_context({{"NLOHMANN_CROW_VER", NLOHMANN_CROW_VER}});

    c.capture_message(NLOHMANN_CROW_CXX);
    std::cout << "sent message" << std::endl;

    std::cout << "last event id: " << c.get_last_event_id() << std::endl;
}
