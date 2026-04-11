#include <string>

enum class AmenityType {
    BASIC,
    PREMIUM,
    VIP
};

enum class PricingModel {
    FIXED,
    HOURLY,
    PER_USE
};

class Amenity {
private:
    std::string name;
    AmenityType type;
    double price;
    bool available;
    PricingModel pricingModel;

public:
    Amenity(const std::string& n, AmenityType t, double p)
        : name(n), type(t), price(p), available(true), pricingModel(PricingModel::FIXED) {}

    // Gettery
    std::string getName() const {
        return name;
    }

    double getPrice() const {
        return price;
    }

    bool isAvailable() const {
        return available;
    }

    // Setter
    void setAvailable(bool a) {
        available = a;
    }
};