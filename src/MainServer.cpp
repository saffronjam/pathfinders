#include "AppServer.h"

int main(int, char **)
{
    try
    {
        AppServer app;
        app.Run();
    }
    catch (const IException &e)
    {
        log_fatal("Type: %s What: %s", e.GetType(), e.what());
    }
    catch (const std::exception &e)
    {
        log_fatal("Type: %s What: %s", "Standard Exception", e.what());
    }
    catch (...)
    {
        log_fatal("Type: %s What: %s", "Standard Exception", "No details available");
    }
    return EXIT_SUCCESS;
}
