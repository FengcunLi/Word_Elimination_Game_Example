//
// Created by Robert Lexis on 2019-03-22.
//

#ifndef TUTOR_PERSON_H
#define TUTOR_PERSON_H

#include <iostream>
#include <fstream>

//enum class Role {
//    player = 1,
//    examiner = 2
//};

class Person {
private:
    std::string name;
    std::string password;
    int level;
public:
    Person() {}

    Person(std::string name_, std::string password_) : name(std::move(name_)), password(std::move(password_)),
                                                       level(0) {
    }

    Person(Person &) = delete;

    Person &operator=(Person &) = delete;

    std::string get_name() const {
        return name;
    }

    std::string get_password() const {
        return password;
    }

    int get_level() const {
        return level;
    }

    void set_name(std::string name_) {
        name = std::move(name_);
    }

    void set_password(std::string password_) {
        password = std::move(password_);
    }

    void set_level(int level_) {
        level = level_;
    }

    virtual std::string to_string() const {
        return name + " " + password + " " + std::to_string(level);
    }

    virtual ~Person() {
        std::cout << "A person instance is destructed." << std::endl;
    }
};

class Player : public Person {
private:
    int score;
public:
    Player() : Person() {}

    Player(std::string name_, std::string password_) : Person(std::move(name_), std::move(password_)), score(0) {

    }

    void increase_score() {
        score++;
    }

    std::string to_string() const override {
        return Person::to_string() + " " + std::to_string(score);
    }

    friend std::ostream &operator<<(std::ostream &output, const Player &player) {
        output << player.to_string() << std::endl;
        return output;
    }

    friend std::istream &operator>>(std::istream &input, Player &player) {
        std::string temp_name, temp_password;
        int temp_level;
        input >> temp_name >> temp_password >> temp_level >> player.score;
        player.set_name(temp_name);
        player.set_password(temp_password);
        player.set_level(temp_level);
        return input;
    }
};

class Examiner : public Person {
private:
    int number_questions;
public:
    Examiner() : Person() {}

    Examiner(std::string name_, std::string password_) : Person(std::move(name_), std::move(password_)),
                                                         number_questions(0) {

    }

    void increase_number_questions() {
        number_questions++;
    }

    std::string to_string() const override {
        return Person::to_string() + " " + std::to_string(number_questions);
    }

    friend std::ostream &operator<<(std::ostream &output, const Examiner &examiner) {
        output << examiner.to_string() << std::endl;
        return output;
    }

    friend std::istream &operator>>(std::istream &input, Examiner &examiner) {
        std::string temp_name, temp_password;
        int temp_level;
        input >> temp_name >> temp_password >> temp_level >> examiner.number_questions;
        examiner.set_name(temp_name);
        examiner.set_password(temp_password);
        examiner.set_level(temp_level);
        return input;
    }
};

#endif //TUTOR_PERSON_H
