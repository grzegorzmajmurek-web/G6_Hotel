#include "Date.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
 
static bool IsLeap(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}
 
static int DaysInMonth(int y, int m)
{
    static const int d[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (m < 1 || m > 12) return 0;
    if (m == 2 && IsLeap(y)) return 29;
    return d[m - 1];
}
 
bool Date::IsValid() const
{
    if (year < 1970 || month < 1 || month > 12) return false;
    return day >= 1 && day <= DaysInMonth(year, month);
}
 
long Date::ToSerial() const
{
    long days = 0;
    for (int y = 1970; y < year; ++y) days += IsLeap(y) ? 366 : 365;
    for (int m = 1; m < month; ++m) days += DaysInMonth(year, m);
    days += (day - 1);
    return days;
}
 
Date Date::FromSerial(long serial)
{
    Date d{ 1970, 1, 1 };
    while (true)
    {
        int yd = IsLeap(d.year) ? 366 : 365;
        if (serial >= yd) { serial -= yd; d.year++; } else break;
    }
    while (true)
    {
        int md = DaysInMonth(d.year, d.month);
        if (serial >= md) { serial -= md; d.month++; } else break;
    }
    d.day = static_cast<int>(serial) + 1;
    return d;
}
 
bool Date::operator<(const Date& other) const { return ToSerial() < other.ToSerial(); }
bool Date::operator<=(const Date& other) const { return ToSerial() <= other.ToSerial(); }
bool Date::operator==(const Date& other) const { return ToSerial() == other.ToSerial(); }
 
Date Date::Parse(const std::string& s)
{
    Date d{};
    char dash1 = 0, dash2 = 0;
    std::istringstream iss(s);
    iss >> d.year >> dash1 >> d.month >> dash2 >> d.day;
    if (!iss || dash1 != '-' || dash2 != '-' || !d.IsValid())
        throw std::invalid_argument("Nieprawidlowa data (oczekiwano YYYY-MM-DD): " + s);
    return d;
}
 
std::string Date::ToString() const
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << year << '-'
<< std::setw(2) << std::setfill('0') << month << '-'
<< std::setw(2) << std::setfill('0') << day;
    return oss.str();
}
 
int DaysBetween(const Date& from, const Date& to)
{
    return static_cast<int>(to.ToSerial() - from.ToSerial());
}
 
bool RangesOverlap(const Date& aStart, const Date& aEnd,
                   const Date& bStart, const Date& bEnd)
{
    // Zakresy [start, end) - dzien wyjazdu jest wolny.
    return aStart < bEnd && bStart < aEnd;
}