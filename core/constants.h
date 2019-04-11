//
// Created by Robert Lexis on 2019-04-11.
//

#ifndef GAME_CONSTANT_H
#define GAME_CONSTANT_H
namespace Action {
    static const char *login = "login";
    static const char *signup = "signup";
    static const char *next_word = "next_word";
    static const char *correct_next_word = "correct_next_word";
    static const char *add = "add";
}
namespace Error {
    static const char *not_logged_in;
}
namespace ActionResult {
    static const char *login_success = "login_success";
    static const char *login_failure = "login_failure";
    static const char *signup_success = "signup_success";
    static const char *signup_failure = "signup_failure";
    static const char *next_word_success = "next_word_success";
    static const char *next_word_failure = "next_word_failure";
    static const char *correct_next_word_success = "correct_next_word_success";
    static const char *correct_next_word_failure = "correct_next_word_failure";
    static const char *add_success = "add_success";
    static const char *add_failure = "add_failure";
}

namespace Role {
    static const char *player = "player";
    static const char *examiner = "examiner";
}
#endif //GAME_CONSTANT_H
