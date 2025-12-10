// main/main.cpp
#include <iostream>

#include "../src/tgClient/TgClientTdlib.hpp"
#include "../src/facade/TgClientFacade.h"

int main(int argc, char** argv) {
    try {
        TgClientTdlib client;
        TgClientFacade facade(client);

        return facade.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] fatal error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "[tgcli] fatal error: unknown exception\n";
        return 1;
    }
}
