//
// Created by Robert Lexis on 2019-03-22.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <memory>
#include "database.h"
#include "person.h"
#include "constants.h"

bool file_exists(const std::string &filename) {
    std::ifstream file(filename);
    return file.good();
}

bool
FileDataBase::signup(std::string name, std::string password, std::string &msg, std::shared_ptr<Person> &person_ptr) {
//    std::fstream file(filename, std::ios_base::in | std::ios_base::out | std::ios_base::app);
    std::fstream file(filename, std::ios_base::in | std::ios_base::out);
    if (!file.is_open()) {
        msg = "Failed to open the database file.";
        return false;
    }

    std::string person_in_string;
    while (!file.eof()) {
        std::getline(file, person_in_string);
        size_t index = person_in_string.find(' ');
        if (index != std::string::npos) {
            if (name == person_in_string.substr(0, index)) {
                msg = "The name is not available.";
                file.close();
                return false;
            }
        }
    }

//    while (std::getline(file, person_in_string)) {
//        size_t index = person_in_string.find(' ');
//        if (index != std::string::npos) {
//            if (person.get_name() == person_in_string.substr(0, index)) {
//                msg = "The name is not available.";
//                return false;
//            }
//        }
//    }

    // important!!!!!
    file.clear();
    if (role == Role::player) {
        person_ptr = std::make_shared<Player>(name, password);
    } else if (role == Role::examiner) {
        person_ptr = std::make_shared<Examiner>(name, password);
    }
    file << person_ptr->to_string() << std::endl;
    msg = "Signup successfully.";
    file.close();
    return true;
}

bool FileDataBase::update_person(Person &person, std::string &msg) {
    std::fstream file(filename, std::ios_base::in);
    if (!file.is_open()) {
        msg = "Failed to open the database file.";
        return false;
    }

    std::ofstream temp_file("file_data_base.temp");

    std::string person_in_string;
    while (!file.eof()) {
        std::getline(file, person_in_string, '\n');
        if (!person_in_string.empty()) {
            size_t index = person_in_string.find(' ');
            if (person.get_name() != person_in_string.substr(0, index)) {
                temp_file << person_in_string << std::endl;
            } else {
                temp_file << person.to_string() << std::endl;
            }
        }
    }
    file.close();
    temp_file.close();
    remove(this->filename.c_str());
    rename("file_data_base.temp", this->filename.c_str());
    msg = "Update successfully.";
    return true;
}

bool
FileDataBase::login(std::string name, std::string password, std::string &msg, std::shared_ptr<Person> &person_ptr) {
    std::fstream file(filename, std::ios_base::in | std::ios_base::out);
    if (!file.is_open()) {
        msg = "Failed to open the database file.";
        return false;
    }

    std::string person_in_string;
    while (!file.eof()) {
        std::getline(file, person_in_string);
        size_t index_1 = person_in_string.find(' ');
        size_t index_2 = person_in_string.find(' ', index_1 + 1);
        if (index_1 != std::string::npos && index_2 != std::string::npos) {
            if (name == person_in_string.substr(0, index_1) &&
                password == person_in_string.substr(index_1 + 1, index_2 - index_1 - 1)) {
                msg = "Login successfully.";
                std::stringstream ss;
                ss << person_in_string;
                if (role == Role::player) {
                    std::shared_ptr<Player> player_ptr = std::make_shared<Player>();
                    ss >> *player_ptr;
                    person_ptr = player_ptr;
                } else if (role == Role::examiner) {
                    std::shared_ptr<Examiner> examiner_ptr = std::make_shared<Examiner>();
                    ss >> *examiner_ptr;
                    person_ptr = examiner_ptr;
                }
                file.close();
                return true;
            }
        }
    }


//    while (std::getline(file, person_in_string)) {
//        size_t index = person_in_string.find(' ');
//        if (index != std::string::npos) {
//            if (person.get_name() == person_in_string.substr(0, index)) {
//                msg = "The name is not available.";
//                return false;
//            }
//        }
//    }

    msg = "User name or password is not correct.";
    file.close();
    return false;
}

bool FileWordSet::add(std::string word, std::string &msg) {
    std::fstream file(filename, std::ios_base::in | std::ios_base::out);
    if (!file.is_open()) {
        msg = "Failed to open the database file.";
        return false;
    }

    std::string temp_word;
    while (!file.eof()) {
        std::getline(file, temp_word);
        if (temp_word == word) {
            msg = "This word is already in the word set.";
            file.close();
            return false;
        }
    }
    file.clear();
    file << word << std::endl;
    file.close();
    msg = "Add " + word + " into word set successfully.";
    return true;
}