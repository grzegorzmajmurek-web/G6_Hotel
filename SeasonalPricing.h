#pragma once
#include "Date.h"
 
enum class Season
{
    Low,
    Mid,
    High
};
 
class SeasonalPricing
{
public:
    static Season GetSeason(const Date& date);
    static double GetMultiplier(const Date& date);
    static const char* GetSeasonName(Season s);
};