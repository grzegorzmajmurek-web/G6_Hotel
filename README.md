# 🏨 Hotel Management System

> Object-oriented hotel reservation system written in **C++23**, built on Domain-Driven Design principles with full class hierarchy, amenity management, and dynamic pricing.

---

## 📐 Architecture Overview

The system is structured around **5 core classes** connected by inheritance and composition relationships.

```
Room  ──aggregation──▶  Reservation  ──inheritance──▶  Solo
                                     ──inheritance──▶  Family
                                     ──inheritance──▶  Group
                                     ──inheritance──▶  Couple

Hotel ──composition──▶  Amenity
Hotel ──manages──────▶  Reservation*
Hotel ──manages──────▶  Room
```

---

## 🗂️ Class Reference

### `Room`
Base entity representing a physical hotel room.

| Visibility | Member | Type | Description |
|---|---|---|---|
| `−` | `number` | `int` | Unique room identifier |
| `−` | `capacity` | `int` | Max guest count |
| `−` | `type` | `RoomCategory` | Standard / Deluxe / Suite / Penthouse |
| `−` | `status` | `RoomStatus` | Available / Occupied / Maintenance |
| `+` | `Room(n,c,type)` | constructor | Initialise with number, capacity, type |
| `+` | `getNumber()` | `int` | Returns room number |
| `+` | `getCapacity()` | `int` | Returns max capacity |
| `+` | `getType()` | `string` | Returns category label |
| `+` | `getStatus()` | `string` | Returns current status |

---

### `Reservation`
Abstract base class for all reservation types. Uses `protected` members so derived classes can read and override data.

| Visibility | Member | Type | Description |
|---|---|---|---|
| `#` | `name` | `string` | Guest full name |
| `#` | `room` | `int` | Assigned room number |
| `#` | `date` | `string` | Check-in date |
| `#` | `discount` | `double` | Applied discount (0.0 – 1.0) |
| `#` | `groupType` | `GuestGroupType` | Solo / Couple / Family / Group / Corporate |
| `#` | `mealPlan` | `MealPlan` | RoomOnly / BB / HalfBoard / FullBoard / AI |
| `+` | `reservationType()` | `string` | **Pure virtual** — overridden by each subclass |
| `+` | `calculatePrice()` | `double` | Base price × nights × meal plan modifier |
| `+` | `addDiscount(d)` | `void` | Apply percentage discount |
| `+` | `display()` | `void` | Print full reservation summary |

---

### Reservation Subtypes

#### `Solo`
Single traveller. Unlocks business packages and late check-out perks.

```
- age: int
+ reservationType(): string   → returns "Solo"
+ businessPackage(): void     → WiFi + workspace upgrade
```

#### `Family`
Two or more adults with children. Triggers cot availability and safety checks.

```
- adults: int
- children: int
+ reservationType(): string   → returns "Family"
+ familyPackage(): void       → kids meal + adjoining rooms
+ addCrib(): void             → request baby cot
```

#### `Group`
10+ people. Activates group rate pricing and organiser contact.

```
- numberOfPersons: int
- organiser: string
+ reservationType(): string   → returns "Group"
+ groupRate(): void           → apply group discount tier
+ getOrganiser(): string
```

#### `Couple`
Two adults. Unlocks romantic packages and surprise add-ons.

```
- romantic: bool
+ reservationType(): string   → returns "Couple"
+ romanticPackage(): void     → flowers + champagne + spa
+ addSurprise(): void         → hotel-arranged surprise
```

---

### `Hotel`
Central controller. Owns all rooms and amenities, manages reservations.

| Method | Signature | Description |
|---|---|---|
| `isAvailable` | `(room:int, date:str) → bool` | Check if room is free on a date |
| `checkAvailability` | `() → void` | Print full availability grid |
| `addReservation` | `(r: Reservation*) → void` | Register a new reservation |
| `refuseReservation` | `(reason: string) → void` | Reject with logged reason |
| `addDiscount` | `(resId:int, d:double) → void` | Apply discount to existing booking |
| `showReservations` | `() → void` | List all active reservations |
| `checkIn` | `(resId: int) → void` | Mark guest as arrived |
| `checkOut` | `(resId: int) → void` | Finalise stay, update room status |
| `addAmenity` | `(a: Amenity) → void` | Register spa / pool / gym etc. |
| `calculateTotalPrice` | `(resId:int) → double` | Room + meals + amenities |
| `generateReport` | `() → void` | Occupancy and revenue summary |
| `findReservation` | `(n:str) → Reservation*` | Search by guest name |

---

### `Amenity`
Represents a bookable hotel service. Owned by `Hotel` via **composition** — amenities cannot exist independently.

| Visibility | Member | Type | Description |
|---|---|---|---|
| `−` | `name` | `string` | Display name (e.g. "Rooftop Spa") |
| `−` | `type` | `AmenityType` | Spa / Pool / Gym / Parking / Transfer |
| `−` | `price` | `double` | Base price in PLN |
| `−` | `available` | `bool` | Booking open flag |
| `−` | `pricingModel` | `PricingModel` | Free / PerVisit / PerDay / PerPerson |
| `+` | `Amenity(n,t,p)` | constructor | — |
| `+` | `getName()` | `string` | — |
| `+` | `getPrice()` | `double` | — |
| `+` | `isAvailable()` | `bool` | — |
| `+` | `setAvailable(a)` | `void` | Open or close bookings |

---

## 🏷️ Enumeration Types

```cpp
enum class RoomCategory   { Standard, Deluxe, Suite, Penthouse };
enum class RoomStatus     { Available, Occupied, Maintenance };
enum class GuestGroupType { Solo, Couple, Family, Group, Corporate };
enum class MealPlan       { RoomOnly, BB, HalfBoard, FullBoard, AllInclusive };
enum class AmenityType    { Spa, Pool, Gym, Parking, Transfer };
enum class PricingModel   { Free, PerVisit, PerDay, PerPerson };
```

---

## 🔗 Relationship Legend

| Symbol | Meaning |
|---|---|
| `△` | Inheritance — derived class extends base |
| `◇` | Composition — child cannot exist without parent |
| `→` | Aggregation — loose association |
| `#` | Protected — accessible by derived classes |
| `−` | Private — accessible within class only |
| `+` | Public — accessible everywhere |

---

## ⚙️ Build & Run

```bash
# Requires GCC 13+ or Clang 18+ with C++23 support
g++ -std=c++23 -Wall -Wextra -o hotel main.cpp
./hotel
```

---

## 📁 File Structure

```
hotel-system/
├── src/
│   ├── domain/
│   │   ├── Room.hpp
│   │   ├── Reservation.hpp
│   │   ├── Solo.hpp
│   │   ├── Family.hpp
│   │   ├── Group.hpp
│   │   ├── Couple.hpp
│   │   └── Amenity.hpp
│   ├── Hotel.hpp
│   ├── Hotel.cpp
│   └── main.cpp
├── tests/
│   └── hotel_tests.cpp
├── CMakeLists.txt
└── README.md
```

---

## 📜 License

MIT — free to use, modify and distribute.
