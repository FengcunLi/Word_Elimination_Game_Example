//
// Created by Robert Lexis on 2019-03-25.
//
#include "utils.h"

std::vector<std::string> split(const std::string &str, char delimiter) {
    std::istringstream iss(str);
    std::string part;
    std::vector<std::string> parts;
    while (std::getline(iss, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

std::string join(std::vector<std::string> parts, char delimiter) {
    std::string result;
    for (const auto &str : parts) {
        result += str;
        result += delimiter;
    }
    if (result.empty()) {
        return "";
    } else {
        return result.substr(0, result.size() - 1);
    }
}

int init_server(sockaddr_in address) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listen_fd >= 0);

    int ret;

    int reuse = 1;
    ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    assert(ret >= 0);

    ret = bind(listen_fd, (sockaddr *) &address, sizeof(address));
    assert(ret != -1);

    ret = listen(listen_fd, 100);
    assert(ret != -1);

    return listen_fd;
}

sockaddr_in get_address(const char *ip, int port) {
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
    return address;
}

std::pair<int, sockaddr_in> accept(int listen_fd) {
    sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));
    socklen_t len = sizeof(client_address);
    int client_fd = accept(listen_fd, (sockaddr *) &client_address, &len);
    return {client_fd, client_address};
}

std::pair<std::string, std::string> get_ip_port(sockaddr_in address) {
//    inet_ntoa() works for IPv4;
//    inet_ntop() works for both IPv4 and IPv6.
    const char *ip = inet_ntoa(address.sin_addr);
    int port = ntohs(address.sin_port);
    return {ip, std::to_string(port)};
}

int setnonblock(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}