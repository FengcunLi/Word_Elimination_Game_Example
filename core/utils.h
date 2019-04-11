//
// Created by Robert Lexis on 2019-03-25.
//

#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <utility>
#include <fcntl.h>

std::vector<std::string> split(const std::string &str, char delimiter);

std::string join(std::vector<std::string> parts, char delimiter);

int init_server(sockaddr_in address);

sockaddr_in get_address(const char *ip, int port);

std::pair<int, sockaddr_in> accept(int listen_fd);

std::pair<std::string, std::string> get_ip_port(sockaddr_in address);

int setnonblock(int fd);

#endif //GAME_UTILS_H
