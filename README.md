# 🏨 System Zarządzania Hotelem

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OOP](https://img.shields.io/badge/Wzorzec-OOP-5b8dee?style=for-the-badge)
![DDD](https://img.shields.io/badge/Design-DDD-4ecdc4?style=for-the-badge)
![License](https://img.shields.io/badge/Licencja-MIT-f7b731?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Aktywny-26de81?style=for-the-badge)

**Obiektowy system rezerwacji hotelowych napisany w C++23.**  
Pełna hierarchia klas · Dynamiczne ceny · Zarządzanie udogodnieniami · Polimorficzne rezerwacje.

</div>

---

## 📐 Architektura systemu

```
Pokoj  ──agregacja──▷  Rezerwacja  ──dziedziczenie──▷  Solo
                                   ├──dziedziczenie──▷  Rodzina
                                   ├──dziedziczenie──▷  Grupa
                                   └──dziedziczenie──▷  Para

Hotel  ──kompozycja──◇  Udogodnienie
Hotel  ──zarządza────▷  vector<Rezerwacja*>
Hotel  ──zarządza────▷  vector<Pokoj>
```

---

## 🗂️ Opis klas

### `Pokoj`

> Podstawowa encja reprezentująca fizyczny pokój hotelowy.

| Widoczność | Składowa | Typ | Opis |
|:---:|---|---|---|
| `−` | `numer` | `int` | Unikalny identyfikator pokoju |
| `−` | `pojemnosc` | `int` | Maksymalna liczba gości |
| `−` | `typ` | `RoomCategory` | Standard / Deluxe / Suite / Penthouse |
| `−` | `status` | `RoomStatus` | Dostępny / Zajęty / Konserwacja |
| `+` | `Pokoj(n, p, typ)` | konstruktor | Inicjalizacja z numerem, pojemnością i kategorią |
| `+` | `getNumer()` | `int` | Zwraca numer pokoju |
| `+` | `getPojemnosc()` | `int` | Zwraca maksymalną pojemność |
| `+` | `getTyp()` | `string` | Zwraca nazwę kategorii |
| `+` | `getStatus()` | `string` | Zwraca aktualny status pokoju |

---

### `Rezerwacja` *(abstrakcyjna)*

> Klasa bazowa dla wszystkich typów rezerwacji. Używa składowych `protected`, aby klasy pochodne mogły odczytywać i nadpisywać dane.

| Widoczność | Składowa | Typ | Opis |
|:---:|---|---|---|
| `#` | `nazwa` | `string` | Imię i nazwisko gościa |
| `#` | `pokoj` | `int` | Numer przydzielonego pokoju |
| `#` | `data` | `string` | Data zameldowania (ISO 8601) |
| `#` | `znizka` | `double` | Zastosowana zniżka `0.0 – 1.0` |
| `#` | `typGrupowy` | `GuestGroupType` | Solo / Para / Rodzina / Grupa / Korporacja |
| `#` | `planWyzywienia` | `MealPlan` | BezWyzywienia / BB / Polpensja / Pelna / AI |
| `+` | `Rezerwacja(n, p, d)` | konstruktor | — |
| `+` | `typRezerwacji()` | `string` | **Czysto wirtualna** — nadpisywana przez każdą podklasę |
| `+` | `obliczCene()` | `double` | Cena bazowa × liczba nocy × modyfikator wyżywienia |
| `+` | `dodajZnizke(z)` | `void` | Zastosuj procentową zniżkę do rezerwacji |
| `+` | `pokaz()` | `void` | Wydrukuj pełne podsumowanie rezerwacji |

---

### Podtypy rezerwacji

#### 👤 `Solo`
Jeden dorosły. Odblokowuje pakiety biznesowe i przywileje późnego wymeldowania.

```
− wiek: int
+ typRezerwacji()   →  "Solo"
+ pakietBiznes()    →  WiFi + ulepszenie miejsca pracy
```

#### 👨‍👩‍👧‍👦 `Rodzina`
Dwoje lub więcej dorosłych z dziećmi. Uruchamia dostępność łóżeczek, kontrole bezpieczeństwa i ceny posiłków dla dzieci.

```
− dorośli: int
− dzieci: int
+ typRezerwacji()   →  "Rodzina"
+ pakietRodzinny()  →  posiłki dla dzieci + pokoje przylegające
+ dodajLozeczko()   →  zamów łóżeczko dziecięce
```

#### 👥 `Grupa`
Dziesięć lub więcej osób. Uruchamia cennik grupowy i przechowuje dane kontaktowe organizatora.

```
− liczbaOsob: int
− organizator: string
+ typRezerwacji()   →  "Grupa"
+ stawkaGrupowa()   →  zastosuj poziom zniżki grupowej
+ getOrganizator()  →  zwraca nazwę organizatora
```

#### 💑 `Para`
Dwoje dorosłych. Odblokowuje pakiety romantyczne, szampana na powitanie i niespodzianki organizowane przez hotel.

```
− romantyczny: bool
+ typRezerwacji()   →  "Para"
+ pakietRomantyczny()  →  kwiaty + szampan + spa
+ dodajNiespodziankę() →  niespodzianka zorganizowana przez hotel
```

---

### `Hotel` *(kontroler)*

> Główna klasa. Posiada wszystkie pokoje i udogodnienia, zarządza cyklem życia rezerwacji.

| Widoczność | Metoda | Zwraca | Opis |
|:---:|---|---|---|
| `−` | `rezerwacje` | `vector<Rezerwacja*>` | Polimorfyczna kolekcja wszystkich rezerwacji |
| `−` | `pokoje` | `vector<Pokoj>` | Wszystkie zarejestrowane pokoje |
| `−` | `udogodnienia` | `vector<Udogodnienie>` | Spa, basen, siłownia, parking… |
| `−` | `cennik` | `map<MealPlan, double>` | Dzienny modyfikator ceny dla każdego planu |
| `+` | `czyDostepny(pokoj, data)` | `bool` | Sprawdź czy pokój jest wolny w danym dniu |
| `+` | `sprawdzDostepnosc()` | `void` | Wydrukuj siatkę dostępności pokoi |
| `+` | `dodajRezerwacje(r)` | `void` | Zarejestruj nowy wskaźnik rezerwacji |
| `+` | `odmowRezerwacje(powod)` | `void` | Odrzuć z zalogowanym powodem |
| `+` | `dodajZnizke(idRez, z)` | `void` | Zastosuj zniżkę do istniejącej rezerwacji |
| `+` | `pokazRezerwacje()` | `void` | Wylistuj wszystkie aktywne rezerwacje |
| `+` | `zamelduj(idRez)` | `void` | Oznacz gościa jako przybyłego, zaktualizuj status pokoju |
| `+` | `wymelduj(idRez)` | `void` | Zakończ pobyt, zwolnij pokój, wygeneruj fakturę |
| `+` | `dodajUdogodnienie(u)` | `void` | Zarejestruj usługę spa / basen / siłownię |
| `+` | `obliczCalkowitaCene(idRez)` | `double` | Pokój + wyżywienie + udogodnienia |
| `+` | `generujRaport()` | `void` | Podsumowanie obłożenia i przychodów |
| `+` | `wyszukajRezerwacje(nazwa)` | `Rezerwacja*` | Wyszukaj po nazwisku gościa |

---

### `Udogodnienie` *(kompozycja — należy do Hotel)*

> Reprezentuje płatną usługę hotelową. Nie może istnieć niezależnie od klasy `Hotel`.

| Widoczność | Składowa | Typ | Opis |
|:---:|---|---|---|
| `−` | `nazwa` | `string` | Nazwa wyświetlana, np. `"Spa na dachu"` |
| `−` | `typ` | `AmenityType` | Spa / Basen / Siłownia / Parking / Transfer |
| `−` | `cena` | `double` | Cena bazowa w walucie lokalnej |
| `−` | `dostepnosc` | `bool` | Czy rezerwacje są aktualnie otwarte |
| `−` | `modelCenowy` | `PricingModel` | Bezpłatny / ZaWizytę / ZaDzień / ZaOsobę |
| `+` | `Udogodnienie(n, t, c)` | konstruktor | — |
| `+` | `getNazwa()` | `string` | — |
| `+` | `getCena()` | `double` | — |
| `+` | `isDostepne()` | `bool` | — |
| `+` | `setDostepnosc(d)` | `void` | Otwórz lub zamknij rezerwacje dla tej usługi |

---

## 🏷️ Typy wyliczeniowe

```cpp
enum class RoomCategory   { Standard, Deluxe, Suite, Penthouse                };
enum class RoomStatus     { Dostepny, Zajety, Konserwacja                      };
enum class GuestGroupType { Solo, Para, Rodzina, Grupa, Korporacja             };
enum class MealPlan       { BezWyzywienia, BB, Polpensja, PelnaObsluga, AI    };
enum class AmenityType    { Spa, Basen, Silownia, Parking, Transfer            };
enum class PricingModel   { Bezplatny, ZaWizyte, ZaDzien, ZaOsobe             };
```

---

## 🔗 Legenda symboli

| Symbol | Nazwa | Znaczenie |
|:---:|---|---|
| `△` | Dziedziczenie | Klasa pochodna rozszerza bazową. Dziecko dziedziczy wszystkie składowe `public` i `protected`. |
| `◇` | Kompozycja | Dziecko nie może istnieć bez rodzica. `Udogodnienie` jest niszczone razem z `Hotel`. |
| `→` | Agregacja | Słabe powiązanie. `Pokoj` może istnieć niezależnie od `Rezerwacji`. |
| `−` | Prywatna | Dostępna tylko wewnątrz klasy deklarującej. |
| `#` | Chroniona | Dostępna w klasie i wszystkich klasach pochodnych. |
| `+` | Publiczna | Dostępna z dowolnego miejsca w programie. |

---

## ⚙️ Kompilacja i uruchomienie

**Wymagania:** GCC 13+ lub Clang 18+ z obsługą C++23.

```bash
# Bezpośrednia kompilacja
g++ -std=c++23 -Wall -Wextra -o hotel main.cpp
./hotel

# Z użyciem CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/hotel
```

---

## 📁 Struktura plików

```
hotel-system/
├── src/
│   ├── domain/
│   │   ├── Pokoj.hpp
│   │   ├── Rezerwacja.hpp
│   │   ├── Solo.hpp
│   │   ├── Rodzina.hpp
│   │   ├── Grupa.hpp
│   │   ├── Para.hpp
│   │   └── Udogodnienie.hpp
│   ├── Hotel.hpp
│   ├── Hotel.cpp
│   └── main.cpp
├── tests/
│   └── hotel_testy.cpp
├── CMakeLists.txt
└── README.md
```

---

## 📜 Licencja

Ten projekt jest objęty licencją **MIT** — możesz go swobodnie używać, modyfikować i rozpowszechniać.
