#include "Service.h"

 

std::string SpaService::GetName() const
{
    return "SPA (" + std::to_string(m_sessions) + " sesji)";
}

 

double SpaService::GetCost(int /*nights*/, int /*guests*/) const
{
    return m_sessions * PricePerSession;
}

 

std::unique_ptr<Service> SpaService::Clone() const
{
    return std::unique_ptr<Service>(new SpaService(*this));
}

 

std::string MealService::GetName() const
{
    switch (m_plan)
    {
        case Plan::Breakfast: return "Posilki: sniadania";
        case Plan::HalfBoard: return "Posilki: HB (sniadanie+obiadokolacja)";
        case Plan::FullBoard: return "Posilki: FB (3 posilki)";
    }
    return "Posilki";
}

 

double MealService::PricePerPersonPerNight() const
{
    switch (m_plan)
    {
        case Plan::Breakfast: return 45.0;
        case Plan::HalfBoard: return 90.0;
        case Plan::FullBoard: return 140.0;
    }
    return 0.0;
}

 

double MealService::GetCost(int nights, int guests) const
{
    return PricePerPersonPerNight() * nights * guests;
}

 

std::unique_ptr<Service> MealService::Clone() const
{
    return std::unique_ptr<Service>(new MealService(*this));
}

std::string PoolService::GetName() const
{
    std::string name = "Basen (" + std::to_string(m_days) + " dni)";
    if (m_sunbed || m_towel) {
        name += " + dodatki (";
        if (m_sunbed) name += "lezak";
        if (m_sunbed && m_towel) name += ", ";
        if (m_towel) name += "recznik";
        name += ")";
    }
    return name;
}

double PoolService::GetCost(int /*nights*/, int guests) const
{
    double dailyCostPerGuest = 0.0;
    if (m_sunbed) dailyCostPerGuest += 20.0; // Cena lezaka za dzien na osobe
    if (m_towel) dailyCostPerGuest += 10.0;  // Cena recznika za dzien na osobe
    // Samo wejscie na basen wliczone w pobyt, platne tylko udogodnienia
    return dailyCostPerGuest * m_days * guests;
}

std::unique_ptr<Service> PoolService::Clone() const
{
    return std::unique_ptr<Service>(new PoolService(*this));
}