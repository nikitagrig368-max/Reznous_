#include "01_core_constants.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

// ============================================================================
// Модуль: Преобразование времени
// Содержит:
//   1. get_leap_seconds() - високосные секунды
//   2. utc_to_tt_epoch() - UTC → TT эпоха (версия 1: простой парсинг)
//   3. utc_to_tt_epoch_v2() - UTC → TT эпоха (версия 2: с поддержкой Z)
//   4. gmst_high_precision() - среднее звёздное время Гринвича
// ============================================================================

// ----------------------------------------------------------------------------
// Версия 1: определение високосных секунд (базовая)
// ----------------------------------------------------------------------------
double get_leap_seconds(double utc_epoch) {
    if (utc_epoch >= 1483228800) return 37.0;  // с 2017-01-01
    if (utc_epoch >= 1341100800) return 35.0;  // с 2012-07-01
    return 37.0;
}

// ----------------------------------------------------------------------------
// Версия 2: определение високосных секунд (расширенная)
// ----------------------------------------------------------------------------
double get_leap_seconds_v2(double utc_epoch) {
    // Расширенная таблица високосных секунд
    if (utc_epoch >= 1483228800) return 37.0;
    if (utc_epoch >= 1438291200) return 36.0;  // 2015-07-01
    if (utc_epoch >= 1341100800) return 35.0;
    if (utc_epoch >= 1293840000) return 34.0;  // 2011-01-01
    if (utc_epoch >= 1230768000) return 33.0;  // 2009-01-01
    if (utc_epoch >= 1136073600) return 32.0;  // 2006-01-01
    return 32.0;
}

// ----------------------------------------------------------------------------
// Версия 1: UTC → TT эпоха (простой парсинг)
// ----------------------------------------------------------------------------
void utc_to_tt_epoch(const std::string& utc_str, double& utc_epoch, double& tt_epoch) {
    std::tm tm = {};
    std::istringstream ss(utc_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    std::time_t time = std::mktime(&tm);

    // Поправка на часовой пояс
    auto now = std::time(nullptr);
    auto local_tm = *std::localtime(&now);
    auto utc_tm = *std::gmtime(&now);
    double tz_offset = std::difftime(std::mktime(&local_tm), std::mktime(&utc_tm));

    utc_epoch = static_cast<double>(time) - tz_offset;
    double leap_sec = get_leap_seconds(utc_epoch);
    tt_epoch = utc_epoch + leap_sec + 32.184;
}

// ----------------------------------------------------------------------------
// Версия 2: UTC → TT эпоха (с поддержкой Z-формата)
// ----------------------------------------------------------------------------
void utc_to_tt_epoch_v2(const std::string& utc_str, double& utc_epoch, double& tt_epoch) {
    std::string clean_str = utc_str;
    // Удаляем 'Z' если есть
    if (!clean_str.empty() && clean_str.back() == 'Z') {
        clean_str.pop_back();
    }

    std::tm tm = {};
    std::istringstream ss(clean_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

    // Используем timegm для прямого преобразования в UTC
    tm.tm_isdst = 0;
    std::time_t time = std::mktime(&tm);  // ВНИМАНИЕ: mktime использует локальный часовой пояс

    // Альтернативный подход: использовать _mkgmtime (нестандартный)
    // Для кроссплатформенности используем difftime подход
    auto now = std::time(nullptr);
    auto local_tm = *std::localtime(&now);
    auto utc_tm = *std::gmtime(&now);
    double tz_offset = std::difftime(std::mktime(&local_tm), std::mktime(&utc_tm));

    utc_epoch = static_cast<double>(time) - tz_offset;
    double leap_sec = get_leap_seconds_v2(utc_epoch);
    tt_epoch = utc_epoch + leap_sec + 32.184;
}

// ----------------------------------------------------------------------------
// GMST: среднее звёздное время Гринвича
// ----------------------------------------------------------------------------
double gmst_high_precision(double tt_epoch) {
    double JD_TT = (tt_epoch / 86400.0) + 2440587.5;
    double T = (JD_TT - 2451545.0) / 36525.0;

    // Формула IAU 1982
    double gmst_seconds = 24110.54841 + 8640184.812866 * T +
                          0.093104 * T*T - 6.2e-6 * T*T*T;

    double gmst_rad = std::fmod(gmst_seconds * (TWO_PI / 86400.0), TWO_PI);
    if (gmst_rad < 0) gmst_rad += TWO_PI;
    return gmst_rad;
}

// ----------------------------------------------------------------------------
// Тестирование модуля времени
// ----------------------------------------------------------------------------
#ifdef TEST_TIME_MODULE
int main() {
    std::cout << "=== Тест модуля времени ===" << std::endl;

    std::string utc_str = "2026-06-10T12:00:00";
    double utc_ep, tt_ep;

    // Тест версии 1
    utc_to_tt_epoch(utc_str, utc_ep, tt_ep);
    std::cout << "Версия 1:" << std::endl;
    std::cout << "  UTC эпоха: " << std::fixed << std::setprecision(3) << utc_ep << std::endl;
    std::cout << "  TT эпоха:  " << tt_ep << std::endl;

    // Тест версии 2
    utc_to_tt_epoch_v2(utc_str, utc_ep, tt_ep);
    std::cout << "Версия 2:" << std::endl;
    std::cout << "  UTC эпоха: " << utc_ep << std::endl;
    std::cout << "  TT эпоха:  " << tt_ep << std::endl;

    // Тест GMST
    double gmst = gmst_high_precision(tt_ep);
    std::cout << "GMST: " << gmst * RAD2DEG << " град" << std::endl;

    return 0;
}
#endif
