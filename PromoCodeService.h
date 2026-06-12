#pragma once
#include "Service.h"
#include <string>
#include <memory>

class PromoCodeService : public Service
{
public:
    PromoCodeService(std::string codeName, double discountValue);

    std::string GetName() const override;
    double GetCost(int nights, int guests) const override;
    std::unique_ptr<Service> Clone() const override;
    ServiceRecord ToRecord() const override;

    const std::string& GetCode() const { return m_codeName; }
    double GetDiscountValue() const { return m_discountValue; }

private:
    std::string m_codeName;
    double m_discountValue;
};