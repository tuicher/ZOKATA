#include "ZokataEngine/Engine.h"
#include "ZokataLog/Log.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        ZKT::ENGINE::Engine engine;
        engine.Run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ZLOG_ERROR(std::string("Unhandled exception: ") + e.what());
        return EXIT_FAILURE;
    }
}
