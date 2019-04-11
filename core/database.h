//
// Created by Robert Lexis on 2019-03-22.
//

#ifndef GAME_DATABASE_H
#define GAME_DATABASE_H

#include <fstream>
#include "person.h"

bool file_exists(const std::string &filename);

class DataBase {
public:
    virtual bool
    signup(std::string name, std::string password, std::string &msg, std::shared_ptr<Person> &person_ptr) = 0;

    virtual bool update_person(Person &person, std::string &msg) = 0;

    virtual bool
    login(std::string name, std::string password, std::string &msg, std::shared_ptr<Person> &person_ptr) = 0;
};

class FileDataBase : public DataBase {
public:
    FileDataBase(std::string filename_, const char *role_) : filename(std::move(filename_)), role(role_) {
        if (!file_exists(filename)) {
            std::cout << filename << " does not exist. Create a new one." << std::endl;
            std::ofstream file(filename);
            file.close();
        }
    }

    bool signup(std::string name, std::string password, std::string &msg, std::shared_ptr<Person> &person_ptr) override;

    bool update_person(Person &person, std::string &msg) override;

    bool login(std::string name, std::string password, std::string &msg, std::shared_ptr<Person> &person_ptr) override;

private:
    std::string filename;
    const char *role;
};

class WordSet {
public:
    virtual bool add(std::string word, std::string &msg) = 0;
};

class FileWordSet : public WordSet {
public:
    explicit FileWordSet(std::string filename_) : filename(std::move(filename_)) {
        if (!file_exists(filename)) {
            std::cout << filename << " does not exist. Create a new one." << std::endl;
            std::ofstream file(filename);
            file.close();
        }
    }

    bool add(std::string word, std::string &msg) override;

private:
    std::string filename;
};

#endif //GAME_DATABASE_H
