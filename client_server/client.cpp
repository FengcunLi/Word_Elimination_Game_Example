//
// Created by Robert Lexis on 2019-03-22.
//

#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include "../core/utils.h"
#include "../core/database.h"
#include "../core/person.h"
#include "../core/constants.h"

#define READ_BUFFER_SIZE 1024

bool poll(pollfd poll_fds[], std::function<bool(std::vector<std::string>)> callback) {
    int ret;
    char read_buf[READ_BUFFER_SIZE];
    ssize_t n;
    ret = poll(poll_fds, 1, -1);
    std::cout << "Poll return event " << poll_fds[0].revents << std::endl;
    if (ret < 0) {
        std::cout << "Poll failed." << std::endl;
        return true;
    }

    if (poll_fds[0].revents & POLLHUP) {
        std::cout << "Server close connection." << std::endl;
        return false;
    }

    if (poll_fds[0].revents & POLLIN) {
        memset(read_buf, '\0', READ_BUFFER_SIZE);
        n = recv(poll_fds[0].fd, read_buf, READ_BUFFER_SIZE, 0);
        std::cout << "Get " << n << " bytes." << std::endl;
        auto parts = split(read_buf, '#');
        std::cout << parts[1] << std::endl;
        return callback(parts);
    }
    return true;
}

bool login_or_signup_callback(std::vector<std::string> parts) {
    if (parts[0] == ActionResult::login_success || parts[0] == ActionResult::signup_success) {
        return false;
    } else if (parts[0] == ActionResult::login_failure || parts[0] == ActionResult::signup_failure) {
        return true;
    } else {
        throw std::runtime_error("Invalid return from server.");
    }
}

bool add_callback(std::vector<std::string> parts) {
    if (parts[0] == ActionResult::add_success) {
        return true;
    } else if (parts[0] == ActionResult::add_failure) {
        return true;
    } else {
        throw std::runtime_error("Invalid return from server.");
    }
}

void login_or_signup(const char *action, const char *role, pollfd poll_fds[]) {
    ssize_t n;
    while (true) {
        std::string name, password;
        std::cout << "User name:" << std::endl;
        std::cin >> name;
        std::cout << "Password:" << std::endl;
        std::cin >> password;

        std::string request = join({action, role, name, password}, '#');
        if ((n = send(poll_fds[0].fd, request.c_str(), request.size(), 0)) != request.size()) {
            std::cout << "Send error." << std::endl;
        }
        std::cout << "Send out " << n << "/" << request.size() << " bytes." << std::endl;

        if (!poll(poll_fds, login_or_signup_callback)) {
            break;
        }
    }
}


//void play(const std::shared_ptr<Person> &player_ptr, int fd) {
//    std::cout << "Please get ready!(If you want to quit, input #X.)" << std::endl;
//    std::string msg;
//    while (true) {
//        std::string word = "Jack";
//        std::string answer;
//        std::cout << word << std::endl;
//        std::cin >> answer;
//        if (answer == "#X") {
//            break;
//        }
//        if (answer == word) {
//            dynamic_cast<Player *>(player_ptr.get())->increase_score();
//            data_base.update_person(*player_ptr, msg);
//            std::cout << msg << std::endl;
//        }
//    }
//}
//
void add(pollfd poll_fds[]) {
    int ret;
    char read_buf[READ_BUFFER_SIZE];
    ssize_t n;
    std::string msg;
    while (true) {
        std::string word;
        std::cout << "Please come up a word(If you do not want to give a word anymore, input #X.): " << std::endl;
        std::cin >> word;
        if (word == "#X") {
            break;
        }
        std::string request = join({Action::add, word}, '#');
        if ((n = send(poll_fds[0].fd, request.c_str(), request.size(), 0)) != request.size()) {
            std::cout << "Send error." << std::endl;
        }
        std::cout << "Send out " << n << "/" << request.size() << " bytes." << std::endl;

        poll(poll_fds, add_callback);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "usage: " << argv[0] << " ip_address port_number" << std::endl;
        return 1;
    }

    const char *ip = argv[1];
    int port = std::atoi(argv[2]);
    sockaddr_in server_address = get_address(ip, port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        std::cout << "connection failed." << std::endl;
        close(sockfd);
        return 1;
    }

    pollfd poll_fds[1];
    poll_fds[0].fd = sockfd;
    poll_fds[0].events = POLLIN | POLLHUP;
    poll_fds[0].revents = 0;

    int action_index;
    int role_index;
    const char *action = nullptr;
    const char *role = nullptr;
    while (true) {
        std::cout << "Welcome to the game!\n\tLogin[1]\nor\n\tCreate a new account[2]" << std::endl;
        std::cin >> action_index;
        if (action_index == 1) {
            action = Action::login;
            break;
        } else if (action_index == 2) {
            action = Action::signup;
            break;
        } else {
            continue;
        }
    }

    while (true) {
        std::cout << action << " as\n\tplayer[1]\nor\n\texaminer[2]" << std::endl;
        std::cin >> role_index;
        if (role_index == 1) {
            role = Role::player;
            break;
        } else if (role_index == 2) {
            role = Role::examiner;
            break;
        } else {
            continue;
        }
    }
    login_or_signup(action, role, poll_fds);

    if (role == Role::examiner) {
        add(poll_fds);
    } else if (role == Role::player) {

    }
}