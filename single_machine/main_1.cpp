//
// Created by Robert Lexis on 2019-03-22.
//

#include <iostream>
#include <memory>
#include "../core/database.h"
#include "../core/person.h"
#include "../core/constants.h"

std::shared_ptr<Person> login(DataBase &data_base) {
    while (true) {
        std::string name, password;
        std::cout << "User name:" << std::endl;
        std::cin >> name;
        std::cout << "Password:" << std::endl;
        std::cin >> password;
        bool ret;
        std::string msg;
        std::shared_ptr<Person> person_ptr;
        ret = data_base.login(name, password, msg, person_ptr);
        std::cout << msg << std::endl;
        if (ret) {
            return person_ptr;
        }
    }
}

std::shared_ptr<Person> signup(DataBase &data_base) {
    while (true) {
        std::string name, password;
        std::cout << "User name:" << std::endl;
        std::cin >> name;
        std::cout << "Password:" << std::endl;
        std::cin >> password;
        bool ret;
        std::string msg;
        std::shared_ptr<Person> person_ptr;
        ret = data_base.signup(name, password, msg, person_ptr);
        std::cout << msg << std::endl;
        if (ret) {
            return person_ptr;
        }
    }
}

void play(const std::shared_ptr<Person> &player_ptr, WordSet &word_set, DataBase &data_base) {
    std::cout << "Please get ready!(If you want to quit, input #X.)" << std::endl;
    std::string msg;
    while (true) {
        std::string word = "Jack";
        std::string answer;
        std::cout << word << std::endl;
        std::cin >> answer;
        if (answer == "#X") {
            break;
        }
        if (answer == word) {
            dynamic_cast<Player *>(player_ptr.get())->increase_score();
            data_base.update_person(*player_ptr, msg);
            std::cout << msg << std::endl;
        }
    }
}

void issue(const std::shared_ptr<Person> &examiner_ptr, WordSet &word_set, DataBase &data_base) {
    bool ret;
    std::string msg;
    while (true) {
        std::string word;
        std::cout << "Please come up a word(If you do not want to give a word anymore, input #X.): " << std::endl;
        std::cin >> word;
        if (word == "#X") {
            break;
        }
        ret = word_set.add(word, msg);
        std::cout << msg << std::endl;
        if (ret) {
            dynamic_cast<Examiner *>(examiner_ptr.get())->increase_number_questions();
            ret = data_base.update_person(*examiner_ptr, msg);
            std::cout << msg << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    FileDataBase player_db("player_db.txt", Role::player);
    FileDataBase examiner_db("examiner_db.txt", Role::examiner);
    FileWordSet word_set("word_set.txt");

    int activity;
    int role;
    while (true) {
        std::cout << "Welcome to the game!\n\tLogin[1]\nor\n\tCreate a new account[2]" << std::endl;
        std::cin >> activity;
        if (activity == 1) {
            while (true) {
                std::cout << "Login as\n\tplayer[1]\nor\n\texaminer[2]" << std::endl;
                std::cin >> role;
                if (role == 1 || role == 2) {
                    break;
                }
            }
            break;
        } else if (activity == 2) {
            while (true) {
                std::cout << "Create a new account as\n\tplayer[1]\nor\n\texaminer[2]" << std::endl;
                std::cin >> role;
                if (role == 1 || role == 2) {
                    break;
                }
            }
            break;
        }
    }
    switch (activity) {
        case 1: {
            switch (role) {
                case 1: {
                    std::shared_ptr<Person> player_ptr = login(player_db);
                    // 将一个基类指针（或引用）cast 到继承类指针
                    play(player_ptr, word_set, player_db);
                    break;
                }
                case 2: {
                    std::shared_ptr<Person> examiner_ptr = login(examiner_db);
                    issue(examiner_ptr, word_set, examiner_db);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case 2: {
            switch (role) {
                case 1: {
                    std::shared_ptr<Person> player_ptr = signup(player_db);
                    play(player_ptr, word_set, player_db);
                    break;
                }
                case 2: {
                    std::shared_ptr<Person> examiner_ptr = signup(examiner_db);
                    issue(examiner_ptr, word_set, examiner_db);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}