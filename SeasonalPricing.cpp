#include "SeasonalPricing.h"
 
Season SeasonalPricing::GetSeason(const Date& date)
{
    // Wysoki: czerwiec-sierpien, grudzien.
    // Sredni: kwiecien, maj, wrzesien, pazdziernik.
    // Niski: pozostale.
    int m = date.month;
    if (m == 6 || m == 7 || m == 8 || m == 12) return Season::High;
    if (m == 4 || m == 5 || m == 9 || m == 10) return Season::Mid;
    return Season::Low;
}
 
double SeasonalPricing::GetMultiplier(const Date& date)
{
    switch (GetSeason(date))
    {
        case Season::High: return 1.5;
        case Season::Mid:  return 1.2;
        case Season::Low:  return 1.0;
    }
    return 1.0;
}
 
const char* SeasonalPricing::GetSeasonName(Season s)
{
    switch (s)
    {
        case Season::High: return "Wysoki";
        case Season::Mid:  return "Sredni";
        case Season::Low:  return "Niski";
    }
    return "?";
}