// ═══════════════════════════════════════════════════════════════════════════════
//  HOTEL MANAGEMENT SYSTEM — C++23 — SINGLE FILE
//  g++ -std=c++23 -o hotel hotel_system_all.cpp && ./hotel
//  clang++ -std=c++23 -o hotel hotel_system_all.cpp && ./hotel
// ═══════════════════════════════════════════════════════════════════════════════
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <expected>
#include <format>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <regex>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════════════
// §1  CURRENCY & MONEY
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

using CurrencyCode = std::array<char,3>;
constexpr CurrencyCode PLN={'P','L','N'};
constexpr CurrencyCode EUR={'E','U','R'};
constexpr CurrencyCode USD={'U','S','D'};

struct CurrencyMismatchException : std::runtime_error {
    CurrencyMismatchException(CurrencyCode a, CurrencyCode b)
        : std::runtime_error(std::string("Currency mismatch: ")
            +std::string(a.data(),3)+" vs "+std::string(b.data(),3)) {}
};

// NEVER store money as double — int64 grosze only
class Money final {
public:
    [[nodiscard]] static constexpr Money fromGrosze(int64_t g, CurrencyCode c) noexcept { return {g,c}; }
    [[nodiscard]] static Money fromDecimal(double v, CurrencyCode c) {
        return {static_cast<int64_t>(std::llround(v*100.0)), c};
    }
    [[nodiscard]] static constexpr Money zero(CurrencyCode c) noexcept { return {0,c}; }

    [[nodiscard]] Money operator+(const Money& r) const { chk(r); return {a_+r.a_,c_}; }
    [[nodiscard]] Money operator-(const Money& r) const { chk(r); return {a_-r.a_,c_}; }
    [[nodiscard]] Money operator*(double f)        const noexcept {
        return {static_cast<int64_t>(std::llround(static_cast<double>(a_)*f)),c_};
    }
    [[nodiscard]] Money operator*(int32_t m)       const noexcept { return {a_*m,c_}; }
    [[nodiscard]] Money applyDiscount(uint8_t pct) const {
        if(pct>100) throw std::invalid_argument("Discount > 100%");
        return {a_-static_cast<int64_t>(std::llround(static_cast<double>(a_)*pct/100.0)),c_};
    }
    [[nodiscard]] auto  operator<=>(const Money& r) const { chk(r); return a_<=>r.a_; }
    [[nodiscard]] bool  operator==(const Money& r)  const noexcept { return c_==r.c_&&a_==r.a_; }

    [[nodiscard]] constexpr int64_t      grosze()    const noexcept { return a_; }
    [[nodiscard]] constexpr double       toDecimal() const noexcept { return a_/100.0; }
    [[nodiscard]] constexpr CurrencyCode currency()  const noexcept { return c_; }
    [[nodiscard]] constexpr bool         isZero()    const noexcept { return a_==0; }
    [[nodiscard]] constexpr bool         isNeg()     const noexcept { return a_<0; }

    [[nodiscard]] std::string str() const {
        std::ostringstream o; std::string code(c_.data(),3);
        int64_t abs=std::abs(a_), zl=abs/100, gr=abs%100;
        if(isNeg()) o<<'-';
        auto fmtK=[](int64_t v){ std::string s=std::to_string(v); for(int p=(int)s.size()-3;p>0;p-=3) s.insert(p," "); return s; };
        if(code=="PLN") o<<fmtK(zl)<<','<<std::setfill('0')<<std::setw(2)<<gr<<" zł";
        else if(code=="EUR") o<<"€"<<fmtK(zl)<<','<<std::setfill('0')<<std::setw(2)<<gr;
        else o<<"$"<<fmtK(zl)<<'.'<<std::setfill('0')<<std::setw(2)<<gr;
        return o.str();
    }
private:
    constexpr Money(int64_t a, CurrencyCode c) noexcept : a_{a},c_{c} {}
    void chk(const Money& o) const { if(c_!=o.c_) throw CurrencyMismatchException{c_,o.c_}; }
    int64_t a_; CurrencyCode c_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §2  UUID
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

class UUID final {
public:
    [[nodiscard]] static UUID generate() {
        static thread_local std::mt19937_64 rng{std::random_device{}()};
        static thread_local std::uniform_int_distribution<uint8_t> d{0,255};
        UUID u;
        for(auto& b:u.b_) b=d(rng);
        u.b_[6]=(u.b_[6]&0x0F)|0x40;
        u.b_[8]=(u.b_[8]&0x3F)|0x80;
        return u;
    }
    [[nodiscard]] static UUID fromString(const std::string& s) {
        if(s.size()!=36||s[8]!='-'||s[13]!='-'||s[18]!='-'||s[23]!='-')
            throw std::invalid_argument("Invalid UUID: "+s);
        UUID u;
        std::string h=s.substr(0,8)+s.substr(9,4)+s.substr(14,4)+s.substr(19,4)+s.substr(24,12);
        for(size_t i=0;i<16;++i) u.b_[i]=static_cast<uint8_t>(std::stoul(h.substr(i*2,2),nullptr,16));
        return u;
    }
    [[nodiscard]] std::string toString() const {
        std::ostringstream o; o<<std::hex<<std::setfill('0');
        for(size_t i=0;i<16;++i){ if(i==4||i==6||i==8||i==10) o<<'-'; o<<std::setw(2)<<(int)b_[i]; }
        return o.str();
    }
    [[nodiscard]] bool operator==(const UUID&) const noexcept = default;
    [[nodiscard]] auto operator<=>(const UUID& r) const noexcept { return b_<=>r.b_; }
    struct Hash { size_t operator()(const UUID& id) const noexcept {
        size_t h=0; for(auto b:id.b_) h^=std::hash<uint8_t>{}(b)+0x9e3779b9+(h<<6)+(h>>2); return h;
    }};
private:
    std::array<uint8_t,16> b_{};
};

// Strong ID wrappers — prevents mixing different ID types
struct ReservationId { UUID value; };
struct GuestId       { UUID value; };
struct RoomId        { uint32_t value{}; };
struct AmenityId     { UUID value; };
struct EventId       { UUID value; };
struct UserId        { UUID value; };

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §3  ENUMS & DATE RANGE
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

enum class GuestGroupType  : uint8_t { Solo,Couple,Family,FamilyExtended,Group,Corporate,Wedding,Conference };
enum class RoomCategory    : uint8_t { Standard,Superior,Deluxe,JuniorSuite,Suite,PresidentialSuite,Penthouse };
enum class BedConfiguration: uint8_t { Single,Twin,Double,King,TwinDouble };
enum class RoomStatus      : uint8_t { Available,Occupied,Maintenance,Cleaning,OutOfOrder,Blocked };
enum class ViewType        : uint8_t { Garden,Pool,Sea,City,Mountain,Courtyard };
enum class MealPlan        : uint8_t { RoomOnly,BedAndBreakfast,HalfBoard,FullBoard,AllInclusive,AllInclusivePremium,DietarySpecial };
enum class LoyaltyTier     : uint8_t { None=0,Bronze,Silver,Gold,Platinum,Diamond };
enum class ReservationStatus:uint8_t { Pending,Confirmed,CheckedIn,CheckedOut,Cancelled,NoShow,Modified };
enum class CancellationReason:uint8_t{ GuestRequest,HotelRequest,PaymentFailure,NoShow,ForceMajeure,PolicyViolation };
enum class AmenityType     : uint8_t { Spa,Pool,Gym,Sauna,TennisCourt,Golf,ParkingIndoor,ParkingOutdoor,
                                       AirportTransfer,BabysittingService,LaundryService,RoomService,
                                       MiniBarRefill,LateCheckout,EarlyCheckin,PetFriendly };

[[nodiscard]] constexpr uint8_t loyaltyPct(LoyaltyTier t) noexcept {
    switch(t){ case LoyaltyTier::Bronze:return 2; case LoyaltyTier::Silver:return 5;
               case LoyaltyTier::Gold:return 8;   case LoyaltyTier::Platinum:return 12;
               case LoyaltyTier::Diamond:return 18; default:return 0; }
}

using Date = std::chrono::year_month_day;

struct InvalidDateRangeException : std::runtime_error {
    explicit InvalidDateRangeException(const std::string& r) : std::runtime_error("Invalid date range: "+r) {}
};

class DateRange final {
public:
    DateRange(Date in, Date out) {
        if(!in.ok())  throw InvalidDateRangeException("check-in invalid");
        if(!out.ok()) throw InvalidDateRangeException("check-out invalid");
        if(out<=in)   throw InvalidDateRangeException("check-out must be after check-in");
        auto today=std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
        if(in<today)  throw InvalidDateRangeException("check-in cannot be in the past");
        in_=in; out_=out;
    }
    [[nodiscard]] int  nightsCount() const noexcept {
        return (int)(std::chrono::sys_days{out_}-std::chrono::sys_days{in_}).count();
    }
    [[nodiscard]] bool overlaps(const DateRange& o) const noexcept { return in_<o.out_&&o.in_<out_; }
    [[nodiscard]] bool contains(const Date& d)       const noexcept { return d>=in_&&d<out_; }
    [[nodiscard]] int  daysUntilCheckin() const noexcept {
        auto today=std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
        return (int)(std::chrono::sys_days{in_}-today).count();
    }
    [[nodiscard]] bool isWithinDays(int n) const noexcept { int d=daysUntilCheckin(); return d>=0&&d<=n; }
    [[nodiscard]] Date checkIn()  const noexcept { return in_; }
    [[nodiscard]] Date checkOut() const noexcept { return out_; }
    [[nodiscard]] bool operator==(const DateRange&) const noexcept = default;
    [[nodiscard]] std::string toString() const {
        return std::format("{:04}/{:02}/{:02} -> {:04}/{:02}/{:02}",
            (int)in_.year(),(unsigned)in_.month(),(unsigned)in_.day(),
            (int)out_.year(),(unsigned)out_.month(),(unsigned)out_.day());
    }
private:
    Date in_,out_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §4  DOMAIN EVENTS
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

using Timestamp = std::chrono::time_point<std::chrono::system_clock,std::chrono::nanoseconds>;

struct GuestCount {
    uint8_t adults{1}; std::vector<uint8_t> children_ages; uint8_t seniors{0};
    [[nodiscard]] uint8_t total() const noexcept { return adults+(uint8_t)children_ages.size()+seniors; }
};

class DomainEvent {
public:
    virtual ~DomainEvent()=default;
    [[nodiscard]] virtual std::string_view eventType() const noexcept=0;
    DomainEvent(const DomainEvent&)=delete;
    DomainEvent& operator=(const DomainEvent&)=delete;
    [[nodiscard]] EventId   eventId()          const noexcept { return eid_; }
    [[nodiscard]] UUID      aggregateId()      const noexcept { return agg_; }
    [[nodiscard]] uint64_t  aggregateVersion() const noexcept { return ver_; }
    [[nodiscard]] Timestamp occurredAt()       const noexcept { return ts_; }
    [[nodiscard]] UserId    causedBy()         const noexcept { return by_; }
    [[nodiscard]] UUID      correlationId()    const noexcept { return corr_; }
protected:
    DomainEvent(UUID agg,uint64_t ver,UserId by,UUID corr)
        : eid_{EventId{UUID::generate()}},agg_{agg},ver_{ver}
        , ts_{std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now())}
        , by_{by},corr_{corr} {}
private:
    EventId eid_; UUID agg_; uint64_t ver_; Timestamp ts_; UserId by_; UUID corr_;
};

class ReservationCreated final : public DomainEvent {
public:
    struct Payload { GuestId gid; DateRange period; GuestGroupType gtype; GuestCount gcnt;
                     MealPlan meal; RoomCategory rcat; Money total; std::string channel; };
    ReservationCreated(ReservationId id,uint64_t v,UserId by,UUID c,Payload p)
        : DomainEvent{id.value,v,by,c},p_{std::move(p)} {}
    [[nodiscard]] std::string_view eventType() const noexcept override { return "ReservationCreated"; }
    [[nodiscard]] const Payload& payload() const noexcept { return p_; }
private: Payload p_;
};

class ReservationConfirmed final : public DomainEvent {
public:
    ReservationConfirmed(ReservationId id,uint64_t v,UserId by,UUID c,std::string ref)
        : DomainEvent{id.value,v,by,c},ref_{std::move(ref)} {}
    [[nodiscard]] std::string_view eventType()         const noexcept override { return "ReservationConfirmed"; }
    [[nodiscard]] const std::string& paymentRef() const noexcept { return ref_; }
private: std::string ref_;
};

class ReservationCancelled final : public DomainEvent {
public:
    ReservationCancelled(ReservationId id,uint64_t v,UserId by,UUID c,
        CancellationReason r,Money refund,std::string notes="")
        : DomainEvent{id.value,v,by,c},r_{r},refund_{refund},notes_{std::move(notes)} {}
    [[nodiscard]] std::string_view   eventType() const noexcept override { return "ReservationCancelled"; }
    [[nodiscard]] CancellationReason reason()    const noexcept { return r_; }
    [[nodiscard]] Money              refund()    const noexcept { return refund_; }
private: CancellationReason r_; Money refund_; std::string notes_;
};

class RoomAllocated final : public DomainEvent {
public:
    RoomAllocated(ReservationId id,uint64_t v,UserId by,UUID c,
        RoomId rid,RoomCategory cat,uint8_t floor,Money rate)
        : DomainEvent{id.value,v,by,c},rid_{rid},cat_{cat},floor_{floor},rate_{rate} {}
    [[nodiscard]] std::string_view eventType()    const noexcept override { return "RoomAllocated"; }
    [[nodiscard]] RoomId       roomId()           const noexcept { return rid_; }
    [[nodiscard]] RoomCategory category()         const noexcept { return cat_; }
    [[nodiscard]] uint8_t      floor()            const noexcept { return floor_; }
    [[nodiscard]] Money        ratePerNight()     const noexcept { return rate_; }
private: RoomId rid_; RoomCategory cat_; uint8_t floor_; Money rate_;
};

class CheckOutCompleted final : public DomainEvent {
public:
    CheckOutCompleted(ReservationId id,uint64_t v,UserId by,UUID c,Money total,Money extras)
        : DomainEvent{id.value,v,by,c},total_{total},extras_{extras} {}
    [[nodiscard]] std::string_view eventType()   const noexcept override { return "CheckOutCompleted"; }
    [[nodiscard]] Money finalTotal()    const noexcept { return total_; }
    [[nodiscard]] Money extrasCharged() const noexcept { return extras_; }
private: Money total_,extras_;
};

class GuestNoShow final : public DomainEvent {
public:
    GuestNoShow(ReservationId id,uint64_t v,UserId by,UUID c,Money charge)
        : DomainEvent{id.value,v,by,c},charge_{charge} {}
    [[nodiscard]] std::string_view eventType()   const noexcept override { return "GuestNoShow"; }
    [[nodiscard]] Money noShowCharge() const noexcept { return charge_; }
private: Money charge_;
};

class RoomUpgraded final : public DomainEvent {
public:
    enum class Reason:uint8_t{GuestRequest,LoyaltyBenefit,HotelInitiative,OverbookingResolution};
    RoomUpgraded(ReservationId id,uint64_t v,UserId by,UUID c,
        RoomId from,RoomId to,RoomCategory fc,RoomCategory tc,Money diff,Reason r)
        : DomainEvent{id.value,v,by,c},from_{from},to_{to},fc_{fc},tc_{tc},diff_{diff},r_{r} {}
    [[nodiscard]] std::string_view eventType() const noexcept override { return "RoomUpgraded"; }
    [[nodiscard]] RoomId fromRoom() const noexcept { return from_; }
    [[nodiscard]] RoomId toRoom()   const noexcept { return to_; }
    [[nodiscard]] Reason reason()   const noexcept { return r_; }
private: RoomId from_,to_; RoomCategory fc_,tc_; Money diff_; Reason r_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §5  DOMAIN MODEL: GuestProfile & Room
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

enum class DietaryReq:uint8_t{None,Vegan,Vegetarian,GlutenFree,Kosher,Halal};

struct GuestPrefs {
    std::optional<uint8_t> floor;
    std::optional<ViewType> view;
    std::optional<BedConfiguration> bed;
    DietaryReq dietary{DietaryReq::None};
    bool quiet{false};
};

struct StayRecord {
    ReservationId rid; DateRange period; RoomCategory cat; Money total; bool loyal;
};

class GuestProfile final {
public:
    GuestProfile(GuestId id, std::string first, std::string last,
                 std::chrono::year_month_day dob, std::string email, std::string phone,
                 LoyaltyTier tier=LoyaltyTier::None)
        : id_{id}, first_{ne(std::move(first),"first")}, last_{ne(std::move(last),"last")}
        , dob_{dob}, email_{ve(std::move(email))}, phone_{vp(std::move(phone))}, tier_{tier}
    {}

    [[nodiscard]] GuestId                    id()          const noexcept { return id_; }
    [[nodiscard]] const std::string&         firstName()   const noexcept { return first_; }
    [[nodiscard]] const std::string&         lastName()    const noexcept { return last_; }
    [[nodiscard]] std::string                fullName()    const { return first_+" "+last_; }
    [[nodiscard]] std::chrono::year_month_day dob()        const noexcept { return dob_; }
    [[nodiscard]] const std::string&         email()       const noexcept { return email_; }
    [[nodiscard]] LoyaltyTier                tier()        const noexcept { return tier_; }

    [[nodiscard]] bool hasBirthdayDuring(const DateRange& p) const noexcept {
        for(int y=(int)p.checkIn().year();y<=(int)p.checkOut().year();++y){
            std::chrono::year_month_day c{std::chrono::year{y},dob_.month(),dob_.day()};
            if(c.ok()&&p.contains(c)) return true;
        }
        return false;
    }

    [[nodiscard]] GuestProfile withTier(LoyaltyTier t) const { GuestProfile c=*this; c.tier_=t; return c; }

private:
    static std::string ne(std::string s,const char* f) {
        if(s.empty()) throw std::invalid_argument(std::string(f)+" cannot be empty");
        return s;
    }
    static std::string ve(std::string e) {
        static const std::regex re{R"(^[a-zA-Z0-9._%+\-]+@[a-zA-Z0-9.\-]+\.[a-zA-Z]{2,}$)"};
        if(!std::regex_match(e,re)) throw std::invalid_argument("Invalid email: "+e);
        return e;
    }
    static std::string vp(std::string p) {
        static const std::regex re{R"(^\+[1-9]\d{6,14}$)"};
        if(!std::regex_match(p,re)) throw std::invalid_argument("Phone must be E.164: "+p);
        return p;
    }
    GuestId id_; std::string first_,last_; std::chrono::year_month_day dob_;
    std::string email_,phone_; LoyaltyTier tier_; GuestPrefs prefs_;
    std::vector<StayRecord> stays_;
};

struct RoomAmenity { std::string name; bool free{true}; };

class Room final {
public:
    struct OccupancyCtx { float occ; DateRange period; GuestGroupType group; };

    Room(RoomId id,std::string num,RoomCategory cat,BedConfiguration bed,
         uint8_t cap,int16_t floor,double area,std::vector<ViewType> views,Money base)
        : id_{id},num_{std::move(num)},cat_{cat},bed_{bed},cap_{cap}
        , floor_{floor},area_{area},views_{std::move(views)},base_{base},status_{RoomStatus::Available}
    {
        if(!cap_)   throw std::invalid_argument("Capacity cannot be 0");
        if(area_<=0) throw std::invalid_argument("Area must be positive");
    }

    [[nodiscard]] RoomId             id()          const noexcept { return id_; }
    [[nodiscard]] const std::string& number()      const noexcept { return num_; }
    [[nodiscard]] RoomCategory       category()    const noexcept { return cat_; }
    [[nodiscard]] uint8_t            maxCapacity() const noexcept { return cap_; }
    [[nodiscard]] int16_t            floor()       const noexcept { return floor_; }
    [[nodiscard]] RoomStatus         status()      const noexcept { return status_; }
    [[nodiscard]] bool               isAvailable() const noexcept { return status_==RoomStatus::Available; }
    [[nodiscard]] bool canAccommodate(uint8_t g)   const noexcept { return g>0&&g<=cap_; }
    [[nodiscard]] Money calculateBaseRate(const DateRange&,const OccupancyCtx&) const noexcept { return base_; }

    void setStatus(RoomStatus s) {
        if(status_==RoomStatus::OutOfOrder&&s==RoomStatus::Occupied)
            throw std::logic_error("Cannot occupy OutOfOrder room");
        status_=s;
    }

private:
    RoomId id_; std::string num_; RoomCategory cat_; BedConfiguration bed_;
    uint8_t cap_; int16_t floor_; double area_; std::vector<ViewType> views_;
    Money base_; RoomStatus status_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §6  PRICING ENGINE  (Chain of Responsibility)
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

struct PriceMod { std::string name; Money before,after; std::string reason; };

class PricingContext {
public:
    explicit PricingContext(Money base) : cur_{base},orig_{base} {}
    void apply(std::string n,Money p,std::string r) { mods_.push_back({std::move(n),cur_,p,std::move(r)}); cur_=p; }
    [[nodiscard]] Money currentPrice()  const noexcept { return cur_; }
    [[nodiscard]] Money originalPrice() const noexcept { return orig_; }
    [[nodiscard]] Money totalDiscount() const noexcept { return orig_-cur_; }
    [[nodiscard]] const std::vector<PriceMod>& mods() const noexcept { return mods_; }
private:
    Money cur_,orig_; std::vector<PriceMod> mods_;
};

struct PricingInput {
    const Room& room; DateRange period; GuestGroupType group;
    GuestCount gcnt; MealPlan meal;
    float occupancy{0.5f}; LoyaltyTier loyalty{LoyaltyTier::None};
    std::optional<std::string> promo; std::string channel{"Website"};
    int group_rooms{1};
};

class IPriceModifier {
public:
    virtual ~IPriceModifier()=default;
    virtual void apply(PricingContext& ctx,const PricingInput& in) const=0;
    [[nodiscard]] virtual std::string_view name() const noexcept=0;
};

class OccupancyModifier final : public IPriceModifier {
public:
    void apply(PricingContext& ctx,const PricingInput& in) const override {
        float o=in.occupancy; double f=1.0; std::string r;
        if(o>0.90f){f=1.35;r="High demand >90% +35%";}
        else if(o>0.75f){f=1.15;r="Elevated >75% +15%";}
        else if(o>0.50f) return;
        else if(o>0.25f){f=0.90;r="Low demand -10%";}
        else{f=0.80;r="Very low -20%";}
        ctx.apply(std::string(name()),ctx.currentPrice()*f,r);
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "Occupancy"; }
};

class LengthOfStayModifier final : public IPriceModifier {
public:
    void apply(PricingContext& ctx,const PricingInput& in) const override {
        int n=in.period.nightsCount(); uint8_t p=0; std::string r;
        if(n>=30){p=15;r="30+ nights";}else if(n>=14){p=10;r="14+ nights";}
        else if(n>=7){p=5;r="7+ nights";}else return;
        ctx.apply(std::string(name()),ctx.currentPrice().applyDiscount(p),r+" -"+std::to_string(p)+"%");
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "LengthOfStay"; }
};

class EarlyBookingModifier final : public IPriceModifier {
public:
    void apply(PricingContext& ctx,const PricingInput& in) const override {
        int d=in.period.daysUntilCheckin(); uint8_t p=0; std::string r;
        if(d>=90){p=12;r="90+ days early";}else if(d>=60){p=8;r="60+ days";}
        else if(d>=30){p=4;r="30+ days";}else return;
        ctx.apply(std::string(name()),ctx.currentPrice().applyDiscount(p),r+" -"+std::to_string(p)+"%");
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "EarlyBooking"; }
};

class LastMinuteModifier final : public IPriceModifier {
public:
    explicit LastMinuteModifier(bool surcharge=true):s_{surcharge}{}
    void apply(PricingContext& ctx,const PricingInput& in) const override {
        if(in.period.daysUntilCheckin()>2) return;
        if(s_&&in.occupancy>=0.5f)
            ctx.apply(std::string(name()),ctx.currentPrice()*1.20,"Last-minute high demand +20%");
        else
            ctx.apply(std::string(name()),ctx.currentPrice().applyDiscount(20),"Last-minute fill -20%");
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "LastMinute"; }
private: bool s_;
};

class GroupRateModifier final : public IPriceModifier {
public:
    void apply(PricingContext& ctx,const PricingInput& in) const override {
        if(in.group!=GuestGroupType::Group&&in.group!=GuestGroupType::Corporate) return;
        uint8_t p=0; std::string r;
        if(in.group_rooms>=20){p=15;r="20+ rooms";}
        else if(in.group_rooms>=10){p=10;r="10+ rooms";}else return;
        ctx.apply(std::string(name()),ctx.currentPrice().applyDiscount(p),r+" -"+std::to_string(p)+"%");
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "GroupRate"; }
};

class DayOfWeekModifier final : public IPriceModifier {
public:
    void apply(PricingContext& ctx,const PricingInput& in) const override {
        int nights=in.period.nightsCount(),wknd=0;
        auto cur=std::chrono::sys_days{in.period.checkIn()};
        for(int i=0;i<nights;++i){
            auto wd=std::chrono::weekday{cur};
            if(wd==std::chrono::Friday||wd==std::chrono::Saturday) ++wknd;
            cur+=std::chrono::days{1};
        }
        if(wknd>nights/2)
            ctx.apply(std::string(name()),ctx.currentPrice()*1.10,"Weekend stay +10%");
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "DayOfWeek"; }
};

class MealPlanPricing {
public:
    struct Rates {
        Money adult{Money::zero(PLN)},child36{Money::zero(PLN)},child712{Money::zero(PLN)},senior{Money::zero(PLN)};
        Rates()=default;
        Rates(Money a,Money c36,Money c712,Money s):adult{a},child36{c36},child712{c712},senior{s}{}
    };
    explicit MealPlanPricing(std::map<MealPlan,Rates> r) : r_{std::move(r)} {}
    [[nodiscard]] Money calculate(MealPlan p,const GuestCount& g,int nights) const {
        if(p==MealPlan::RoomOnly) return Money::zero(PLN);
        auto it=r_.find(p);
        if(it==r_.end()) throw std::invalid_argument("No rate for meal plan");
        const auto& rt=it->second;
        Money t=Money::zero(PLN);
        t=t+(rt.adult*g.adults*nights);
        t=t+(rt.senior*g.seniors*nights);
        for(uint8_t age:g.children_ages){
            Money cr = age<=2  ? Money::zero(PLN)
                     : age<=6  ? rt.child36
                     : age<=12 ? rt.child712
                               : rt.adult;
            t=t+(cr*nights);
        }
        return t;
    }
private: std::map<MealPlan,Rates> r_;
};

class PricingEngine final {
public:
    struct Result { Money room_total,meals,grand_total; PricingContext ctx; };
    explicit PricingEngine(std::unique_ptr<MealPlanPricing> mp) : mp_{std::move(mp)} {
        mods_.emplace_back(std::make_unique<OccupancyModifier>());
        mods_.emplace_back(std::make_unique<LengthOfStayModifier>());
        mods_.emplace_back(std::make_unique<EarlyBookingModifier>());
        mods_.emplace_back(std::make_unique<LastMinuteModifier>());
        mods_.emplace_back(std::make_unique<GroupRateModifier>());
        mods_.emplace_back(std::make_unique<DayOfWeekModifier>());
    }
    [[nodiscard]] Result calculate(const PricingInput& in) const {
        Room::OccupancyCtx occ{in.occupancy,in.period,in.group};
        Money base=in.room.calculateBaseRate(in.period,occ)*in.period.nightsCount();
        PricingContext ctx{base};
        for(const auto& m:mods_) m->apply(ctx,in);
        Money meals=mp_->calculate(in.meal,in.gcnt,in.period.nightsCount());
        return {ctx.currentPrice(),meals,ctx.currentPrice()+meals,std::move(ctx)};
    }
    void addModifier(std::unique_ptr<IPriceModifier> m) { mods_.emplace_back(std::move(m)); }
private:
    std::vector<std::unique_ptr<IPriceModifier>> mods_;
    std::unique_ptr<MealPlanPricing> mp_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §7  DISCOUNT SYSTEM
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

struct Discount {
    enum class Combine:uint8_t{Exclusive,Additive,Stackable};
    std::string type,code; uint8_t pct; Money saving; Combine combine; std::string why;
};

struct DiscountCtx {
    const GuestProfile& guest; DateRange period; GuestGroupType group;
    int group_rooms; Money base; std::optional<std::string> promo;
};

class DiscountRule {
public:
    virtual ~DiscountRule()=default;
    [[nodiscard]] virtual std::optional<Discount> evaluate(const DiscountCtx&) const=0;
    [[nodiscard]] virtual std::string_view name() const noexcept=0;
};

class LoyaltyDiscount final : public DiscountRule {
public:
    [[nodiscard]] std::optional<Discount> evaluate(const DiscountCtx& c) const override {
        uint8_t p=loyaltyPct(c.guest.tier());
        if(!p) return std::nullopt;
        return Discount{"Loyalty","",p,c.base.applyDiscount(p),Discount::Combine::Stackable,"Loyalty tier"};
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "LoyaltyDiscount"; }
};

class GroupDiscount final : public DiscountRule {
public:
    [[nodiscard]] std::optional<Discount> evaluate(const DiscountCtx& c) const override {
        if(c.group!=GuestGroupType::Group&&c.group!=GuestGroupType::Corporate) return std::nullopt;
        uint8_t p=0; std::string r;
        if(c.group_rooms>=20){p=15;r="20+ rooms";}
        else if(c.group_rooms>=10){p=10;r="10+ rooms";}else return std::nullopt;
        return Discount{"GroupDiscount","",p,c.base.applyDiscount(p),Discount::Combine::Additive,r};
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "GroupDiscount"; }
};

class BirthdayDiscount final : public DiscountRule {
public:
    [[nodiscard]] std::optional<Discount> evaluate(const DiscountCtx& c) const override {
        if(!c.guest.hasBirthdayDuring(c.period)) return std::nullopt;
        return Discount{"Birthday","",15,c.base.applyDiscount(15),Discount::Combine::Additive,"Birthday during stay"};
    }
    [[nodiscard]] std::string_view name() const noexcept override { return "BirthdayDiscount"; }
};

struct AppliedDiscounts { std::vector<Discount> discounts; uint8_t total_pct; Money saving,final_price; };

class DiscountOrchestrator final {
public:
    explicit DiscountOrchestrator(std::vector<std::unique_ptr<DiscountRule>> rules)
        : rules_{std::move(rules)} {}

    [[nodiscard]] AppliedDiscounts apply(const DiscountCtx& c) const {
        std::vector<Discount> app;
        for(const auto& r:rules_){ auto d=r->evaluate(c); if(d) app.push_back(*d); }
        if(app.empty()) return {{},0,Money::zero(c.base.currency()),c.base};

        auto ex=std::find_if(app.begin(),app.end(),
            [](const Discount& d){ return d.combine==Discount::Combine::Exclusive; });

        std::vector<Discount> final_d;
        if(ex!=app.end()){
            final_d.push_back(*ex);
            for(const auto& d:app) if(d.type=="Loyalty") final_d.push_back(d);
        } else { final_d=std::move(app); }

        uint8_t addpct=0;
        for(const auto& d:final_d)
            if(d.combine!=Discount::Combine::Stackable) addpct+=d.pct;
        addpct=std::min(addpct,uint8_t{50});

        Money price=c.base.applyDiscount(addpct);
        for(const auto& d:final_d)
            if(d.combine==Discount::Combine::Stackable) price=price.applyDiscount(d.pct);

        return {std::move(final_d),addpct,c.base-price,price};
    }
private:
    std::vector<std::unique_ptr<DiscountRule>> rules_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §8  VALIDATION SERVICE
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

struct RoomNotAvailableError {
    RoomId room_id; std::vector<DateRange> nearest;
    std::string msg{"Wybrany pokój nie jest dostępny w tym terminie."};
};
struct CapacityExceededError {
    uint8_t cap,req; std::string msg;
    CapacityExceededError(uint8_t c,uint8_t r): cap{c},req{r},
        msg{"Maks. pojemność: "+std::to_string(c)+", żądano: "+std::to_string(r)+"."} {}
};
struct MinimumStayViolationError {
    uint8_t min,req; std::string msg;
    MinimumStayViolationError(uint8_t mn,uint8_t r): min{mn},req{r},
        msg{"Min. pobyt: "+std::to_string(mn)+" nocy."} {}
};
struct GuestBlacklistedError {
    GuestId gid; std::string internal_reason;
    std::string msg{"Brak dostępności dla wybranego terminu."};
};
struct PaymentDeclinedError {
    std::string code; std::string msg{"Płatność odrzucona."};
};
struct CancellationLimitError {
    uint8_t count; std::string msg{"Przekroczono limit anulowań."};
};
struct InvalidCheckinError   { std::string msg{"Data zameldowania w przeszłości."}; };
struct MinorWithoutGuardian  { std::string msg{"Nieletni muszą być z opiekunem."}; };
struct InsufficientRoomsError {
    int req,avail; std::string msg;
    InsufficientRoomsError(int r,int a): req{r},avail{a},
        msg{"Dostępnych: "+std::to_string(a)+"/"+std::to_string(r)+"."} {}
};

using ValidationError = std::variant<
    RoomNotAvailableError,CapacityExceededError,MinimumStayViolationError,
    GuestBlacklistedError,PaymentDeclinedError,CancellationLimitError,
    InvalidCheckinError,MinorWithoutGuardian,InsufficientRoomsError>;

struct ValidationReq {
    GuestId gid; DateRange period; RoomId rid;
    uint8_t total_guests,adults; std::vector<uint8_t> children;
    GuestGroupType group; int group_rooms{1};
    bool payment_ok{false}; uint8_t prev_cancellations{0};
};

class IGuestBlacklist {
public:
    virtual ~IGuestBlacklist()=default;
    [[nodiscard]] virtual bool        isBlacklisted(GuestId) const=0;
    [[nodiscard]] virtual std::string getReason(GuestId)     const=0;
};

class IRoomAvailabilityChecker {
public:
    virtual ~IRoomAvailabilityChecker()=default;
    [[nodiscard]] virtual bool isRoomAvailable(RoomId,const DateRange&)       const=0;
    [[nodiscard]] virtual std::vector<DateRange> nearestDates(RoomId,int=30)  const=0;
    [[nodiscard]] virtual uint8_t getCapacity(RoomId)                          const=0;
    [[nodiscard]] virtual int countAvailable(const DateRange&,RoomCategory)   const=0;
};

struct ValidationConfig {
    uint8_t max_cancel{3}; uint8_t wknd_min_nights{2}; bool guardian{true};
};

class ValidationService final {
public:
    using Config = ValidationConfig;

    ValidationService(const IRoomAvailabilityChecker& av,const IGuestBlacklist& bl,Config cfg=Config{})
        : av_{av},bl_{bl},cfg_{cfg} {}

    [[nodiscard]] std::expected<void,std::vector<ValidationError>>
    validate(const ValidationReq& r) const {
        std::vector<ValidationError> errs;
        if(bl_.isBlacklisted(r.gid)){
            errs.emplace_back(GuestBlacklistedError{r.gid,bl_.getReason(r.gid)});
            return std::unexpected(errs);
        }
        if(!av_.isRoomAvailable(r.rid,r.period))
            errs.emplace_back(RoomNotAvailableError{r.rid,av_.nearestDates(r.rid)});
        uint8_t cap=av_.getCapacity(r.rid);
        if(r.total_guests>cap) errs.emplace_back(CapacityExceededError{cap,r.total_guests});
        if(isWknd(r.period)&&r.period.nightsCount()<cfg_.wknd_min_nights)
            errs.emplace_back(MinimumStayViolationError{cfg_.wknd_min_nights,(uint8_t)r.period.nightsCount()});
        if(r.prev_cancellations>=cfg_.max_cancel)
            errs.emplace_back(CancellationLimitError{r.prev_cancellations});
        if(cfg_.guardian&&r.adults==0&&!r.children.empty())
            errs.emplace_back(MinorWithoutGuardian{});
        if(!errs.empty()) return std::unexpected(std::move(errs));
        return {};
    }

    [[nodiscard]] static std::vector<std::string> messages(const std::vector<ValidationError>& errs) {
        std::vector<std::string> out;
        for(const auto& e:errs) std::visit([&out](const auto& x){ out.push_back(x.msg); },e);
        return out;
    }

private:
    static bool isWknd(const DateRange& p) {
        auto wd=std::chrono::weekday{std::chrono::sys_days{p.checkIn()}};
        return wd==std::chrono::Friday||wd==std::chrono::Saturday;
    }
    const IRoomAvailabilityChecker& av_; const IGuestBlacklist& bl_; Config cfg_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §9  EVENT STORE
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

struct ConcurrencyException : std::runtime_error {
    ConcurrencyException(UUID agg,uint64_t exp,uint64_t act)
        : std::runtime_error("Concurrency conflict on "+agg.toString()+
            " expected v"+std::to_string(exp)+" got v"+std::to_string(act))
        , agg_{agg},exp_{exp},act_{act} {}
    [[nodiscard]] UUID     agg()      const noexcept { return agg_; }
    [[nodiscard]] uint64_t expected() const noexcept { return exp_; }
    [[nodiscard]] uint64_t actual()   const noexcept { return act_; }
private: UUID agg_; uint64_t exp_,act_;
};

class IEventStore {
public:
    virtual ~IEventStore()=default;
    virtual void append(UUID agg,uint64_t expected,
        std::vector<std::unique_ptr<DomainEvent>> evts)=0;
    [[nodiscard]] virtual std::vector<std::reference_wrapper<const DomainEvent>>
        loadEvents(UUID agg,uint64_t from=0) const=0;
    [[nodiscard]] virtual uint64_t currentVersion(UUID agg) const=0;
};

class InMemoryEventStore final : public IEventStore {
public:
    using Handler=std::function<void(const DomainEvent&)>;

    void append(UUID agg,uint64_t expected,
        std::vector<std::unique_ptr<DomainEvent>> evts) override
    {
        std::unique_lock lk{mx_};
        auto& s=streams_[agg.toString()];
        uint64_t cur=s.empty()?0:s.back()->aggregateVersion();
        if(cur!=expected) throw ConcurrencyException{agg,expected,cur};
        std::string_view type;
        for(auto& e:evts){ type=e->eventType(); s.emplace_back(std::move(e)); }
        notifyUnsafe(agg,type);
    }

    [[nodiscard]] std::vector<std::reference_wrapper<const DomainEvent>>
    loadEvents(UUID agg,uint64_t from=0) const override {
        std::shared_lock lk{smx_};
        auto it=streams_.find(agg.toString());
        if(it==streams_.end()) return {};
        std::vector<std::reference_wrapper<const DomainEvent>> out;
        for(const auto& e:it->second) if(e->aggregateVersion()>=from) out.emplace_back(*e);
        return out;
    }

    [[nodiscard]] uint64_t currentVersion(UUID agg) const override {
        std::shared_lock lk{smx_};
        auto it=streams_.find(agg.toString());
        if(it==streams_.end()) return 0;
        return it->second.empty()?0:it->second.back()->aggregateVersion();
    }

    void subscribe(std::string type,Handler h) {
        std::unique_lock lk{mx_}; handlers_[std::move(type)].push_back(std::move(h));
    }
    [[nodiscard]] size_t totalEventCount() const {
        std::shared_lock lk{smx_};
        size_t n=0; for(const auto& [_,s]:streams_) n+=s.size(); return n;
    }
    void clear(){ std::unique_lock lk{mx_}; streams_.clear(); }

private:
    void notifyUnsafe(UUID agg,std::string_view type){
        auto it=handlers_.find(std::string(type)); if(it==handlers_.end()) return;
        for(auto& h:it->second) h(*streams_[agg.toString()].back());
    }
    mutable std::shared_mutex smx_; std::mutex mx_;
    std::unordered_map<std::string,std::vector<std::unique_ptr<DomainEvent>>> streams_;
    std::unordered_map<std::string,std::vector<Handler>> handlers_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §10  COMMAND HANDLER
// ═══════════════════════════════════════════════════════════════════════════════
namespace hotel {

struct CreateReservationCmd {
    GuestId gid; DateRange period; RoomCategory rcat;
    std::optional<uint8_t> preferred_floor; std::optional<ViewType> preferred_view;
    GuestGroupType group; GuestCount gcnt; MealPlan meal;
    std::optional<std::string> promo; std::string channel{"Website"};
    int group_rooms{1}; UserId initiated_by; UUID correlation;
};

struct ReservationResult {
    ReservationId rid; RoomId allocated_room;
    Money total,saving; std::string confirmation;
};

struct AllocationFailed { std::string reason; };
struct PricingErr       { std::string reason; };
using CmdError = std::variant<std::vector<ValidationError>,AllocationFailed,PricingErr>;

class IRoomAllocator {
public:
    struct AllocResult { RoomId rid; uint8_t floor; RoomCategory cat; Money rate; };
    virtual ~IRoomAllocator()=default;
    [[nodiscard]] virtual std::optional<AllocResult> allocate(
        const DateRange&,RoomCategory,const GuestCount&,
        std::optional<uint8_t>,std::optional<ViewType>)=0;
    virtual void release(RoomId,const DateRange&)=0;
};

class INotifier {
public:
    virtual ~INotifier()=default;
    virtual void sendConfirmation(GuestId,ReservationId,const std::string&,const DateRange&,Money)=0;
};

class ConfirmationGen {
public:
    [[nodiscard]] std::string generate(ReservationId id) const {
        auto now=std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
        std::string s=id.value.toString(); s=s.substr(s.size()-6);
        std::transform(s.begin(),s.end(),s.begin(),::toupper);
        return "QT-"+std::to_string((int)now.year())+"-"+s;
    }
};

class CreateReservationHandler final {
public:
    CreateReservationHandler(ValidationService& vs,IRoomAllocator& ra,
        IEventStore& es,INotifier& ns,ConfirmationGen& cg)
        : vs_{vs},ra_{ra},es_{es},ns_{ns},cg_{cg} {}

    [[nodiscard]] std::expected<ReservationResult,CmdError>
    handle(const CreateReservationCmd& cmd) {
        ValidationReq vr{cmd.gid,cmd.period,RoomId{0},
            cmd.gcnt.total(),cmd.gcnt.adults,cmd.gcnt.children_ages,cmd.group,cmd.group_rooms};
        auto val=vs_.validate(vr);
        if(!val) return std::unexpected(CmdError{val.error()});

        auto alloc=ra_.allocate(cmd.period,cmd.rcat,cmd.gcnt,cmd.preferred_floor,cmd.preferred_view);
        if(!alloc) return std::unexpected(CmdError{AllocationFailed{"No rooms available"}});

        ReservationId rid{UUID::generate()};
        auto conf=cg_.generate(rid);
        Money total=alloc->rate*(int32_t)cmd.period.nightsCount();

        std::vector<std::unique_ptr<DomainEvent>> evts;
        evts.push_back(std::make_unique<ReservationCreated>(rid,1,cmd.initiated_by,cmd.correlation,
            ReservationCreated::Payload{cmd.gid,cmd.period,cmd.group,cmd.gcnt,cmd.meal,cmd.rcat,total,cmd.channel}));
        evts.push_back(std::make_unique<RoomAllocated>(rid,2,cmd.initiated_by,cmd.correlation,
            alloc->rid,alloc->cat,alloc->floor,alloc->rate));

        try { es_.append(rid.value,0,std::move(evts)); }
        catch(const ConcurrencyException&) {
            ra_.release(alloc->rid,cmd.period);
            return std::unexpected(CmdError{AllocationFailed{"Concurrency conflict — retry"}});
        }
        try { ns_.sendConfirmation(cmd.gid,rid,conf,cmd.period,total); } catch(...) {}
        return ReservationResult{rid,alloc->rid,total,Money::zero(total.currency()),conf};
    }
private:
    ValidationService& vs_; IRoomAllocator& ra_; IEventStore& es_; INotifier& ns_; ConfirmationGen& cg_;
};

} // namespace hotel

// ═══════════════════════════════════════════════════════════════════════════════
// §11  MINI TEST FRAMEWORK
// ═══════════════════════════════════════════════════════════════════════════════
namespace T {
    static int ok=0, fail=0;

    void pass(const char* s){ ++ok;  std::cout<<"  \033[32m✓\033[0m "<<s<<"\n"; }
    void fail_(const char* s,const char* f,int l){ ++fail; std::cout<<"  \033[31m✗\033[0m "<<s<<" ("<<f<<":"<<l<<")\n"; }

    template<typename A,typename B>
    void eq(A a,B b,const char* s,const char* f,int l){ if(a==(A)b) pass(s); else fail_(s,f,l); }
    void isTrue(bool v,const char* s,const char* f,int l){ if(v) pass(s); else fail_(s,f,l); }

    template<typename E,typename Fn>
    void throws(Fn fn,const char* s,const char* f,int l){
        try{ fn(); fail_(s,f,l); }
        catch(const E&){ pass(s); }
        catch(...){ fail_(s,f,l); }
    }

    void summary(){
        std::cout<<"\n";
        std::cout<<"  ─────────────────────────────────\n";
        std::cout<<"  \033[32m"<<ok<<" passed\033[0m  ";
        if(fail) std::cout<<"\033[31m"<<fail<<" failed\033[0m";
        else     std::cout<<"\033[32m0 failed\033[0m";
        std::cout<<"  |  "<<(ok+fail)<<" total\n";
        std::cout<<"  ─────────────────────────────────\n";
    }
}

#define EQ(a,b)    T::eq(a,b,#a" == "#b,__FILE__,__LINE__)
#define TRUE(v)    T::isTrue(v,#v,__FILE__,__LINE__)
#define NOTHROW(fn) try{ fn; T::pass(#fn" (no throw)"); }catch(...){ T::fail_(#fn" (unexpected throw)",__FILE__,__LINE__); }
#define THROW(E,fn) T::throws<E>([&]{ fn; },#E" from "#fn,__FILE__,__LINE__)

// ─── Test helpers ─────────────────────────────────────────────────────────────
static hotel::Date dFN(int n){
    auto t=std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    return std::chrono::year_month_day{t+std::chrono::days{n}};
}
static hotel::GuestProfile makeGuest(hotel::LoyaltyTier tier=hotel::LoyaltyTier::None){
    return hotel::GuestProfile{
        hotel::GuestId{hotel::UUID::generate()},"Jan","Kowalski",
        std::chrono::year_month_day{std::chrono::year{1985},std::chrono::month{6},std::chrono::day{15}},
        "jan@test.com","+48123456789",tier};
}
static hotel::Room makeRoom(hotel::Money rate){
    return hotel::Room{hotel::RoomId{101},"101",hotel::RoomCategory::Standard,
        hotel::BedConfiguration::Double,2,1,25.0,{hotel::ViewType::Garden},rate};
}

// ═══════════════════════════════════════════════════════════════════════════════
// §12  TESTS
// ═══════════════════════════════════════════════════════════════════════════════
void runTests(){
    using namespace hotel;

    std::cout<<"\n\033[1m═══════════════════════════════════════════\033[0m\n";
    std::cout<<"\033[1m  HOTEL SYSTEM — TEST SUITE\033[0m\n";
    std::cout<<"\033[1m═══════════════════════════════════════════\033[0m\n\n";

    // ── §Money ──────────────────────────────────────────────────────────────
    std::cout<<"\033[1m[ Money ]\033[0m\n";
    EQ(Money::fromGrosze(12345,PLN).grosze(), int64_t(12345));
    EQ(Money::fromGrosze(10050,PLN).toDecimal(), 100.50);
    EQ((Money::fromGrosze(1000,PLN)+Money::fromGrosze(500,PLN)).grosze(), int64_t(1500));
    EQ((Money::fromGrosze(2000,PLN)-Money::fromGrosze(500,PLN)).grosze(), int64_t(1500));
    THROW(CurrencyMismatchException, (void)(Money::fromGrosze(100,PLN)+Money::fromGrosze(100,EUR)));
    EQ((Money::fromGrosze(10000,PLN)*1.35).grosze(), int64_t(13500));
    EQ(Money::fromGrosze(10000,PLN).applyDiscount(10).grosze(), int64_t(9000));
    THROW(std::invalid_argument, Money::fromGrosze(100,PLN).applyDiscount(101));
    TRUE(Money::fromGrosze(1000,PLN) < Money::fromGrosze(2000,PLN));
    TRUE(Money::zero(PLN).isZero());
    EQ(Money::fromGrosze(123456,PLN).str(), std::string("1 234,56 zł"));

    // ── §DateRange ───────────────────────────────────────────────────────────
    std::cout<<"\n\033[1m[ DateRange ]\033[0m\n";
    { auto dr=DateRange{dFN(5),dFN(10)}; EQ(dr.nightsCount(), 5); }
    { auto a=DateRange{dFN(5),dFN(10)},b=DateRange{dFN(8),dFN(15)}; TRUE(a.overlaps(b)); }
    { auto a=DateRange{dFN(5),dFN(10)},b=DateRange{dFN(11),dFN(15)}; TRUE(!a.overlaps(b)); }
    { auto a=DateRange{dFN(5),dFN(10)},b=DateRange{dFN(10),dFN(15)}; TRUE(!a.overlaps(b)); }
    THROW(InvalidDateRangeException, DateRange(dFN(-1),dFN(5)));
    THROW(InvalidDateRangeException, DateRange(dFN(10),dFN(5)));
    { auto dr=DateRange{dFN(1),dFN(5)}; TRUE(dr.isWithinDays(2)); }
    { auto dr=DateRange{dFN(30),dFN(35)}; EQ(dr.daysUntilCheckin(), 30); }

    // ── §UUID ────────────────────────────────────────────────────────────────
    std::cout<<"\n\033[1m[ UUID ]\033[0m\n";
    TRUE(!(UUID::generate()==UUID::generate()));
    auto id=UUID::generate(); EQ(UUID::fromString(id.toString()),id);
    EQ(UUID::generate().toString().size(), size_t(36));
    THROW(std::invalid_argument, UUID::fromString("not-a-uuid"));

    // ── §GuestProfile ────────────────────────────────────────────────────────
    std::cout<<"\n\033[1m[ GuestProfile ]\033[0m\n";
    EQ(makeGuest().fullName(), std::string("Jan Kowalski"));
    EQ(loyaltyPct(LoyaltyTier::Gold),    uint8_t(8));
    EQ(loyaltyPct(LoyaltyTier::Diamond), uint8_t(18));
    EQ(loyaltyPct(LoyaltyTier::None),    uint8_t(0));
    {
        auto fn = [](){
            GuestProfile{GuestId{UUID::generate()},"A","B",
                std::chrono::year_month_day{std::chrono::year{1990},std::chrono::month{1},std::chrono::day{1}},
                "not-an-email","+48123456789"};
        };
        THROW(std::invalid_argument, fn());
    }
    {
        auto fn = [](){
            GuestProfile{GuestId{UUID::generate()},"A","B",
                std::chrono::year_month_day{std::chrono::year{1990},std::chrono::month{1},std::chrono::day{1}},
                "ok@test.com","12345"};
        };
        THROW(std::invalid_argument, fn());
    }

    // ── §Room ────────────────────────────────────────────────────────────────
    std::cout<<"\n\033[1m[ Room ]\033[0m\n";
    auto room=makeRoom(Money::fromGrosze(50000,PLN));
    TRUE(room.isAvailable());
    TRUE(room.canAccommodate(2));
    TRUE(!room.canAccommodate(3));
    EQ(room.maxCapacity(), uint8_t(2));
    {
        auto fn=[](){ Room{RoomId{1},"1",RoomCategory::Standard,BedConfiguration::Double,0,1,25.0,{},Money::zero(PLN)}; };
        THROW(std::invalid_argument, fn());
    }

    // ── §PricingEngine ───────────────────────────────────────────────────────
    std::cout<<"\n\033[1m[ PricingEngine ]\033[0m\n";
    {
        PricingContext ctx{Money::fromGrosze(10000,PLN)};
        ctx.apply("T",Money::fromGrosze(8000,PLN),"r");
        EQ((int)ctx.mods().size(), 1);
        EQ(ctx.totalDiscount().grosze(), int64_t(2000));
        EQ(ctx.currentPrice().grosze(),  int64_t(8000));
    }
    {
        // 7+ nights = 5% off
        auto base=Money::fromGrosze(70000,PLN);
        EQ(base.applyDiscount(5).grosze(), int64_t(66500));
    }
    {
        std::map<MealPlan,MealPlanPricing::Rates> rates;
        rates[MealPlan::RoomOnly]={Money::zero(PLN),Money::zero(PLN),Money::zero(PLN),Money::zero(PLN)};
        rates[MealPlan::BedAndBreakfast]={
            Money::fromGrosze(8000,PLN),Money::fromGrosze(4000,PLN),
            Money::fromGrosze(5600,PLN),Money::fromGrosze(7200,PLN)};
        PricingEngine eng{std::make_unique<MealPlanPricing>(std::move(rates))};
        auto r=makeRoom(Money::fromGrosze(50000,PLN));
        PricingInput in{r,DateRange{dFN(5),dFN(8)},GuestGroupType::Solo,{2,{},0},MealPlan::RoomOnly,0.65f};
        auto res=eng.calculate(in);
        TRUE(res.grand_total.grosze()>0);
    }

    // ── §Discounts ───────────────────────────────────────────────────────────
    std::cout<<"\n\033[1m[ Discounts ]\033[0m\n";
    {
        GroupDiscount rule;
        auto g=makeGuest();
        DiscountCtx ctx{g,DateRange{dFN(5),dFN(10)},GuestGroupType::Group,10,Money::fromGrosze(100000,PLN),{}};
        auto d=rule.evaluate(ctx);
        TRUE(d.has_value());
        EQ(d->pct, uint8_t(10));
    }
    {
        GroupDiscount rule;
        auto g=makeGuest();
        DiscountCtx ctx{g,DateRange{dFN(5),dFN(8)},GuestGroupType::Solo,1,Money::fromGrosze(30000,PLN),{}};
        TRUE(!rule.evaluate(ctx).has_value());
    }
    {
        LoyaltyDiscount rule;
        auto g=makeGuest(LoyaltyTier::Gold);
        DiscountCtx ctx{g,DateRange{dFN(5),dFN(10)},GuestGroupType::Solo,1,Money::fromGrosze(100000,PLN),{}};
        auto d=rule.evaluate(ctx);
        TRUE(d.has_value());
        EQ(d->pct, uint8_t(8));
    }
    {
        // Discount stacking cap at 50%
        EQ(Money::fromGrosze(10000,PLN).applyDiscount(50).grosze(), int64_t(5000));
    }

    // ── §EventStore ──────────────────────────────────────────────────────────
    std::cout<<"\n\033[1m[ EventStore ]\033[0m\n";
    {
        InMemoryEventStore store;
        auto agg=UUID::generate(); ReservationId rid{agg};
        UserId uid{UUID::generate()}; UUID corr=UUID::generate();
        std::vector<std::unique_ptr<DomainEvent>> evts;
        evts.push_back(std::make_unique<ReservationConfirmed>(rid,1,uid,corr,"PAY-001"));
        store.append(agg,0,std::move(evts));
        auto loaded=store.loadEvents(agg);
        EQ((int)loaded.size(), 1);
        EQ(loaded[0].get().eventType(), std::string_view("ReservationConfirmed"));
        EQ(store.totalEventCount(), size_t(1));
    }
    {
        // Optimistic concurrency: two writers at version 0 → second throws
        InMemoryEventStore store;
        auto agg=UUID::generate(); ReservationId rid{agg};
        UserId uid{UUID::generate()}; UUID corr=UUID::generate();
        std::vector<std::unique_ptr<DomainEvent>> e1,e2;
        e1.push_back(std::make_unique<ReservationConfirmed>(rid,1,uid,corr,"PAY-A"));
        e2.push_back(std::make_unique<ReservationConfirmed>(rid,1,uid,corr,"PAY-B"));
        store.append(agg,0,std::move(e1));
        THROW(ConcurrencyException, store.append(agg,0,std::move(e2)));
    }
    {
        // Load from version N filters correctly
        InMemoryEventStore store;
        auto agg=UUID::generate(); ReservationId rid{agg};
        UserId uid{UUID::generate()}; UUID corr=UUID::generate();
        for(int i=1;i<=3;++i){
            std::vector<std::unique_ptr<DomainEvent>> evts;
            evts.push_back(std::make_unique<ReservationConfirmed>(rid,(uint64_t)i,uid,corr,"P"));
            store.append(agg,(uint64_t)(i-1),std::move(evts));
        }
        EQ((int)store.loadEvents(agg,2).size(), 2);
        EQ((int)store.loadEvents(agg,0).size(), 3);
    }
    {
        // Realtime subscription
        InMemoryEventStore store;
        int fired=0;
        store.subscribe("ReservationConfirmed",[&fired](const DomainEvent&){ ++fired; });
        auto agg=UUID::generate(); ReservationId rid{agg};
        UserId uid{UUID::generate()}; UUID corr=UUID::generate();
        std::vector<std::unique_ptr<DomainEvent>> evts;
        evts.push_back(std::make_unique<ReservationConfirmed>(rid,1,uid,corr,"PAY"));
        store.append(agg,0,std::move(evts));
        EQ(fired, 1);
    }

    T::summary();
}

// ═══════════════════════════════════════════════════════════════════════════════
// §13  MAIN / LIVE DEMO
// ═══════════════════════════════════════════════════════════════════════════════
int main(){
    using namespace hotel;

    std::cout<<"\033[1m═══════════════════════════════════════════\033[0m\n";
    std::cout<<"\033[1m  Hotel Management System — C++23\033[0m\n";
    std::cout<<"\033[1m═══════════════════════════════════════════\033[0m\n";

    runTests();

    std::cout<<"\n\033[1m[ Live Demo ]\033[0m\n\n";

    // Money showcase
    auto price=Money::fromGrosze(150000,PLN);
    std::cout<<"Base price:     "<<price.str()<<"\n";
    std::cout<<"After 15% off:  "<<price.applyDiscount(15).str()<<"\n";
    std::cout<<"Saving:         "<<(price-price.applyDiscount(15)).str()<<"\n\n";

    // Confirmation number
    ReservationId rid{UUID::generate()};
    ConfirmationGen gen;
    std::cout<<"Confirmation:   "<<gen.generate(rid)<<"\n\n";

    // Full pricing flow — Family, 7 nights, B&B, 55% occupancy
    std::map<MealPlan,MealPlanPricing::Rates> rates;
    rates[MealPlan::BedAndBreakfast]={
        Money::fromGrosze(8000,PLN),Money::fromGrosze(4000,PLN),
        Money::fromGrosze(5600,PLN),Money::fromGrosze(7200,PLN)};
    rates[MealPlan::RoomOnly]={Money::zero(PLN),Money::zero(PLN),Money::zero(PLN),Money::zero(PLN)};

    PricingEngine engine{std::make_unique<MealPlanPricing>(std::move(rates))};
    auto room=makeRoom(Money::fromGrosze(50000,PLN));
    PricingInput input{room, DateRange{dFN(10),dFN(17)},
        GuestGroupType::Family,{2,{8,5},0},MealPlan::BedAndBreakfast,0.55f};

    auto result=engine.calculate(input);
    std::cout<<"Pricing — Family, 7 nights, B&B, 55% occupancy:\n";
    std::cout<<"  Room total:  "<<result.room_total.str()<<"\n";
    std::cout<<"  Meals:       "<<result.meals.str()<<"\n";
    std::cout<<"  Grand total: "<<result.grand_total.str()<<"\n";
    if(!result.ctx.mods().empty()){
        std::cout<<"  Modifiers:\n";
        for(const auto& m:result.ctx.mods())
            std::cout<<"    "<<m.name<<": "<<m.reason<<"\n";
    }

    // Discount orchestrator demo
    std::cout<<"\nDiscount demo — Gold loyalty + Group 12 rooms:\n";
    std::vector<std::unique_ptr<DiscountRule>> rules;
    rules.emplace_back(std::make_unique<LoyaltyDiscount>());
    rules.emplace_back(std::make_unique<GroupDiscount>());
    rules.emplace_back(std::make_unique<BirthdayDiscount>());
    DiscountOrchestrator orch{std::move(rules)};

    auto guest=makeGuest(LoyaltyTier::Gold);
    DiscountCtx ctx{guest,DateRange{dFN(10),dFN(17)},
        GuestGroupType::Group,12,Money::fromGrosze(500000,PLN),{}};
    auto applied=orch.apply(ctx);
    std::cout<<"  Discounts applied: "<<(int)applied.discounts.size()<<"\n";
    for(const auto& d:applied.discounts)
        std::cout<<"    "<<d.type<<": "<<(int)d.pct<<"%\n";
    std::cout<<"  Total saved:  "<<applied.saving.str()<<"\n";
    std::cout<<"  Final price:  "<<applied.final_price.str()<<"\n";

    std::cout<<"\n\033[1m═══════════════════════════════════════════\033[0m\n";
    return T::fail>0?1:0;
}
