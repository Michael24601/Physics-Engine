
#include "util.h"

using namespace pe;


int pe::generateRandomNumber(int min, int max) {

    /*
        The random deviceand rng can be reused, which speeds up the
        function.
    */
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}


real pe::generateRandomNumber(real min, real max) {

    /*
        The random deviceand rng can be reused, which speeds up the
        function.
    */
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<real> distribution(min, max);
    return distribution(gen);
}