#include "Service.h"
#include "PromoCodeService.h"

 

std::string SpaService::GetName() const
{
    return "SPA (" + std::to_string(m_sessions) + " sesji)";
}

 

double SpaService::GetCost(int nights, int guests) const
{
    if (nights < 0) throw std::invalid_argument("Liczba nocy nie moze byc ujemna");
    if (guests < 1) throw std::invalid_argument("Liczba gosci musi byc co najmniej 1");
    return m_sessions * PricePerSession;
}

 

std::unique_ptr<Service> SpaService::Clone() const
{
    return std::unique_ptr<Service>(new SpaService(*this));
}

ServiceRecord SpaService::ToRecord() const
{
    ServiceRecord rec;
    rec.type = "SPA";
    rec.spaSessions = m_sessions;
    return rec;
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
    if (nights < 0) throw std::invalid_argument("Liczba nocy nie moze byc ujemna");
    if (guests < 1) throw std::invalid_argument("Liczba gosci musi byc co najmniej 1");
    return PricePerPersonPerNight() * nights * guests;
}

 

std::unique_ptr<Service> MealService::Clone() const
{
    return std::unique_ptr<Service>(new MealService(*this));
}

ServiceRecord MealService::ToRecord() const
{
    ServiceRecord rec;
    rec.type = "MEAL";
    rec.mealPlan = static_cast<int>(m_plan);
    return rec;
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

double PoolService::GetCost(int nights, int guests) const
{
    if (nights < 0) throw std::invalid_argument("Liczba nocy nie moze byc ujemna");
    if (guests < 1) throw std::invalid_argument("Liczba gosci musi byc co najmniej 1");
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

ServiceRecord PoolService::ToRecord() const
{
    ServiceRecord rec;
    rec.type = "POOL";
    rec.poolDays = m_days;
    rec.poolSunbed = m_sunbed;
    rec.poolTowel = m_towel;
    return rec;
}

std::unique_ptr<Service> MakeServiceFromRecord(const ServiceRecord& rec)
{
    if (rec.type == "SPA")
        return std::unique_ptr<Service>(new SpaService(rec.spaSessions));
    if (rec.type == "MEAL")
        return std::unique_ptr<Service>(new MealService(static_cast<MealService::Plan>(rec.mealPlan)));
    if (rec.type == "POOL")
        return std::unique_ptr<Service>(new PoolService(rec.poolDays, rec.poolSunbed, rec.poolTowel));
    if (rec.type == "PROMO")
        return std::unique_ptr<Service>(new PromoCodeService(rec.promoCode, rec.promoDiscount));
    throw std::invalid_argument("Nieznany typ uslugi w rekordzie bazy danych: " + rec.type);
}