/*  Pokemon Automation Bot Base - Client Example
 * 
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 * 
 */

#include <chrono>
#include <sstream>
#include <iostream>
#include <mutex>
#include "Common/Cpp/PrettyPrint.h"
#include "Logging.h"

namespace PokemonAutomation{

std::string current_time(){
    //  Based off of: https://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c

    using namespace std;
    using namespace std::chrono;
    typedef duration<int, ratio_multiply<hours::period, ratio<24> >::type> days;
    system_clock::time_point now = system_clock::now();
    system_clock::duration tp = now.time_since_epoch();
    days d = duration_cast<days>(tp);
    tp -= d;
    hours h = duration_cast<hours>(tp);
    tp -= h;
    minutes m = duration_cast<minutes>(tp);
    tp -= m;
    seconds s = duration_cast<seconds>(tp);
    tp -= s;
    auto micros = 1000000 * tp.count() * system_clock::duration::period::num / system_clock::duration::period::den;
    time_t tt = system_clock::to_time_t(now);
//    tm utc_tm = *gmtime(&tt);
    tm local_tm = *localtime(&tt);

    std::stringstream ss;
    ss << local_tm.tm_year + 1900 << '-';
    ss << to_string_padded(2, local_tm.tm_mon + 1) << '-';
    ss << to_string_padded(2, local_tm.tm_mday) << ' ';
    ss << to_string_padded(2, local_tm.tm_hour) << ':';
    ss << to_string_padded(2, local_tm.tm_min) << ':';
    ss << to_string_padded(2, local_tm.tm_sec) << '.';
    ss << to_string_padded(6, micros);

    return ss.str();
}




std::mutex logging_lock;

void log(const std::stringstream& ss){
    log(ss.str());
}
void log(const std::string& msg){
    std::lock_guard<std::mutex> lg(logging_lock);
    std::cout << current_time() << " - " << msg << std::endl;
}





}

