#include <iostream>
#include <td/telegram/td_json_client.h>

int main() {
    std::cout << "Starting TDLib test...\n";

    void *client = td_json_client_create();
    if (!client) {
        std::cerr << "Failed to create TDLib client\n";
        return 1;
    }

    const char *query = R"({
        "@type": "getOption",
        "name": "version"
    })";

    td_json_client_send(client, query);

    const double timeout = 10.0;
    const char *result = td_json_client_receive(client, timeout);

    if (result) {
        std::cout << "Received: " << result << '\n';
    } else {
        std::cout << "No response within " << timeout << " seconds\n";
    }

    td_json_client_destroy(client);
    std::cout << "Done.\n";
    return 0;
}
