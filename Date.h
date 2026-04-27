#pragma once
#include <string>
 
struct Date
{
    int year{};
    int month{};
    int day{};
 
    bool IsValid() const;
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator==(const Date& other) const;
 
    // Liczba dni od 1970-01-01 (do porównań/odejmowania).
    long ToSerial() const;
 
    static Date FromSerial(long serial);
    static Date Parse(const std::string& s); // format YYYY-MM-DD
    std::string ToString() const;
};
 
int DaysBetween(const Date& from, const Date& to); // to - from
bool RangesOverlap(const Date& aStart, const Date& aEnd,
                   const Date& bStart, const Date& bEnd);