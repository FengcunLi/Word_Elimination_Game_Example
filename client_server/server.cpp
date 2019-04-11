//
// Created by Robert Lexis on 2019-03-22.
//
#include "../core/person.h"
#include "../core/database.h"
#include "../core/utils.h"
#include "../core/constants.h"
#include <iostream>
#include <map>
#include <memory>
#include <poll.h>
#include <unistd.h>

#define MAX_USER_NUMBER 100
#define READ_BUFFER_SIZE 1024
#define WRITE_BUFFER_SIZE 1024

FileDataBase player_db("player_db.txt", Role::player);
FileDataBase examiner_db("examiner_db.txt", Role::examiner);
FileWordSet word_set("word_set.txt");

struct ClientData {
    sockaddr_in address;
    char read_buf[READ_BUFFER_SIZE];
    char write_buf[WRITE_BUFFER_SIZE];
    std::shared_ptr<Person> person_ptr;

    void write_buf_from_string(const std::string &str) {
        memset(write_buf, '\0', WRITE_BUFFER_SIZE);
        int i = 0;
        for (auto c : str) {
            write_buf[i] = c;
            i++;
        }
    }
};

void when_a_client_arrive(int listen_fd, pollfd poll_fds[], std::map<int, ClientData> &client_data_map,
                          int &online_user_num) {
    auto client = accept(listen_fd);
    int client_fd = client.first;
    sockaddr_in client_address = client.second;

    if (client_fd < 0) {
        std::cout << "Accept a client failed. Errno is " << errno << std::endl;
        return;
    } else {
        auto ip_port = get_ip_port(client_address);
        std::cout << "Accept a client from ip: " << ip_port.first << " port: " << ip_port.second
                  << std::endl;
    }

    if (online_user_num >= MAX_USER_NUMBER) {
        const char *info = "Too many users.";
        std::cout << info << std::endl;
        send(client_fd, info, strlen(info), 0);
        close(client_fd);
        return;
    }

    online_user_num++;
    setnonblock(client_fd);
    poll_fds[online_user_num].fd = client_fd;
    poll_fds[online_user_num].events = POLLIN | POLLHUP | POLLERR;
    poll_fds[online_user_num].revents = 0;
    client_data_map[client_fd].address = client_address;
    std::cout << "Current online user number is " << online_user_num << std::endl;
}

void when_poll_err(int client_fd, sockaddr_in client_address) {
    auto ip_port = get_ip_port(client_address);
    std::cout << "Get an error from socket fd: " << client_fd << " ip: " << ip_port.first << " port: " << ip_port.second
              << std::endl;

    char error[100];
    memset(error, '\0', 100);
    socklen_t len = sizeof(error);
    if (getsockopt(client_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
        std::cout << "Get socket option failed." << std::endl;
    }
}


void parse_read_buf(ClientData &client_data) {
    std::string request = std::string(client_data.read_buf);
    std::vector<std::string> parts = split(request, '#');
    std::string action = parts[0];
    std::string msg;

    if (action == Action::login || action == Action::signup) {
        std::string role = parts[1];
        std::string name = parts[2];
        std::string password = parts[3];
        DataBase *db_ptr = nullptr;
        if (role == Role::player) {
            db_ptr = &player_db;
        } else if (role == Role::examiner) {
            db_ptr = &examiner_db;
        } else {
            throw std::runtime_error("Invalid role value");
        }

        if (action == Action::login) {
            if (client_data.person_ptr == nullptr) {
                std::shared_ptr<Person> person_ptr;
                if (db_ptr->login(name, password, msg, client_data.person_ptr)) {
                    msg = join({ActionResult::login_success, msg}, '#');
                } else {
                    msg = join({ActionResult::login_failure, msg}, '#');
                }
            } else {
                msg = join({ActionResult::login_failure, "This examiner is already online."}, '#');
            }
        } else if (action == Action::signup) {
            if (client_data.person_ptr == nullptr) {
                std::shared_ptr<Person> person_ptr;
                if (db_ptr->signup(name, password, msg, client_data.person_ptr)) {
                    msg = join({ActionResult::signup_success, msg}, '#');
                } else {
                    msg = join({ActionResult::signup_failure, msg}, '#');
                }
            } else {
                msg = join({ActionResult::signup_failure, "This examiner is already online."}, '#');
            }
        }

        client_data.write_buf_from_string(msg);
        return;
    }

    if (client_data.person_ptr == nullptr) {
        msg = join({Error::not_logged_in, "This player has not logged in."}, '#');
        client_data.write_buf_from_string(msg);
        return;
    }

    if (action == Action::next_word || Action::correct_next_word || Action::add) {
        if (action == Action::next_word) {
            std::string next_word = "Jack";
            msg = join({ActionResult::next_word_success, next_word}, '#');
        } else if (action == Action::correct_next_word) {
            std::string next_word = "Jack";
            msg = join({ActionResult::next_word_success, next_word}, '#');
            dynamic_cast<Player *>(client_data.person_ptr.get())->increase_score();
        } else if (action == Action::add) {
            std::string new_word = parts[1];
            if (word_set.add(new_word, msg)) {
                dynamic_cast<Examiner *>(client_data.person_ptr.get())->increase_number_questions();
                std::string tmp_msg;
                examiner_db.update_person(*client_data.person_ptr, tmp_msg);
                msg = join({ActionResult::add_success, msg}, '#');
            } else {
                msg = join({ActionResult::add_failure, msg}, '#');
            }
        }
        client_data.write_buf_from_string(msg);
        return;
    } else {
        throw std::runtime_error("Invalid action value");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " ip_address port_number" << std::endl;
        return 1;
    }

    const char *ip = argv[1];
    int port = std::atoi(argv[2]);
    sockaddr_in address = get_address(ip, port);

    int listen_fd = init_server(address);


    std::map<int, ClientData> client_data_map;

    pollfd poll_fds[1 + MAX_USER_NUMBER];

    for (int i = 1; i <= MAX_USER_NUMBER; i++) {
        poll_fds[i].fd = -1;
        poll_fds[i].events = 0;
    }

    poll_fds[0].fd = listen_fd;
    poll_fds[0].events = POLLIN | POLLERR;
    poll_fds[0].revents = 0;

    int online_user_num = 0;
    int ret;
    while (true) {
        std::cout << "A poll round." << std::endl;
        ret = poll(poll_fds, 1 + online_user_num, -1);

        if (ret < 0) {
            std::cout << "Poll error." << std::endl;
            break;
        }

        for (int i = 0; i < 1 + online_user_num; i++) {
            pollfd &poll_fd = poll_fds[i];
            int &fd = poll_fd.fd;
            if ((fd == listen_fd) && (poll_fd.revents & POLLIN)) {
                when_a_client_arrive(listen_fd, poll_fds, client_data_map, online_user_num);
                continue;
            }

            ClientData &client_data = client_data_map[fd];
            if (poll_fd.revents & POLLERR) {
                when_poll_err(fd, client_data.address);
            } else if (poll_fd.revents & POLLHUP) {
                auto ip_port = get_ip_port(client_data.address);
                client_data_map.erase(fd);
                close(fd);
                poll_fds[i] = poll_fds[online_user_num];
                i--;
                online_user_num--;
                std::cout << "The client from socket fd: " << fd << " ip: " << ip_port.first << " port: "
                          << ip_port.second << " has gone." << std::endl;
            } else if (poll_fd.revents & POLLIN) {
                auto ip_port = get_ip_port(client_data.address);
                memset(client_data.read_buf, '\0', READ_BUFFER_SIZE);
                ssize_t n = recv(fd, client_data.read_buf, READ_BUFFER_SIZE - 1, 0);
                std::cout << "Get " << n << " bytes from " << fd
                          << " ip: " << ip_port.first << " port: "
                          << ip_port.second << std::endl;
                if (n < 0) {
                    if (errno != EAGAIN) {
                        client_data_map.erase(fd);
                        close(fd);
                        poll_fds[i] = poll_fds[online_user_num];
                        i--;
                        online_user_num--;
                        std::cout << "The server cut the connection to socket fd: " << fd << " ip: "
                                  << ip_port.first << " port: "
                                  << ip_port.second << std::endl;
                    }
                } else if (n == 0) {
                    std::cout << "Useless round." << std::endl;
                } else {
                    parse_read_buf(client_data);
                    poll_fd.events &= ~POLLIN;
                    poll_fd.events |= POLLOUT;
                }
            } else if (poll_fd.revents & POLLOUT) {
                ssize_t n = send(fd, client_data.write_buf, strlen(client_data.write_buf), 0);
                if (size_t(n) != strlen(client_data.write_buf)) {
                    std::cout << "send error " << n << " " << strlen(client_data.write_buf) << std::endl;
                }
                std::cout << fd << " sent out " << n << " bytes" << std::endl;
                poll_fd.events &= ~POLLOUT;
                poll_fd.events |= POLLIN;
            } else {
                std::cout << "Useless..." << std::endl;
            }
        }
    }
}