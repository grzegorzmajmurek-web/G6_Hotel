-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Maj 09, 2026 at 10:08 AM
-- Wersja serwera: 10.4.28-MariaDB
-- Wersja PHP: 8.2.4

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `hotel`
--

-- --------------------------------------------------------

--
-- Struktura tabeli dla tabeli `klienci`
--

CREATE TABLE `klienci` (
  `id` int(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `lastname` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `klienci`
--

INSERT INTO `klienci` (`id`, `name`, `lastname`) VALUES
(1, 'TEST', 'TEST'),
(2, 'TEST', 'TEST'),
(3, 'TEST', 'TEST'),
(4, 'TEST', 'TEST'),
(5, 'TEST', 'TEST'),
(6, 'TEST', 'TEST'),
(7, 'TEST', 'TEST'),
(8, 'TEST', 'TEST'),
(9, 'TEST', 'TEST'),
(10, 'TEST', 'TEST'),
(11, 'TEST', 'TEST'),
(12, 'TEST', 'TEST'),
(13, 'TEST', 'TEST'),
(14, 'TEST', 'TEST'),
(15, 'TEST', 'TEST'),
(16, 'TEST', 'TEST'),
(17, 'TEST', 'TEST'),
(18, 'TEST', 'TEST');

-- --------------------------------------------------------

--
-- Struktura tabeli dla tabeli `pokoje`
--

CREATE TABLE `pokoje` (
  `id` int(255) NOT NULL,
  `room_number` int(255) DEFAULT NULL,
  `type` varchar(255) DEFAULT NULL,
  `capacity` int(255) DEFAULT NULL,
  `price` int(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `pokoje`
--

INSERT INTO `pokoje` (`id`, `room_number`, `type`, `capacity`, `price`) VALUES
(1, 101, 'Standard', 2, 200),
(2, 102, 'Standard', 2, 200),
(3, 201, 'Deluxe', 3, 350),
(4, 202, 'Deluxe', 3, 350),
(5, 301, 'Suite', 4, 600);

-- --------------------------------------------------------

--
-- Struktura tabeli dla tabeli `rezerwacje`
--

CREATE TABLE `rezerwacje` (
  `id` int(255) NOT NULL,
  `klient_id` int(255) DEFAULT NULL,
  `room_id` int(255) DEFAULT NULL,
  `room_number` int(255) DEFAULT NULL,
  `reservation_price` int(255) DEFAULT NULL,
  `visiting_start` date DEFAULT NULL,
  `visiting_end` date NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `rezerwacje`
--

INSERT INTO `rezerwacje` (`id`, `klient_id`, `room_id`, `room_number`, `reservation_price`, `visiting_start`, `visiting_end`) VALUES
(1, 1, 1, 301, 0, '2026-06-20', '2026-07-20');

--
-- Indeksy dla zrzutów tabel
--

--
-- Indeksy dla tabeli `klienci`
--
ALTER TABLE `klienci`
  ADD PRIMARY KEY (`id`);

--
-- Indeksy dla tabeli `pokoje`
--
ALTER TABLE `pokoje`
  ADD PRIMARY KEY (`id`);

--
-- Indeksy dla tabeli `rezerwacje`
--
ALTER TABLE `rezerwacje`
  ADD PRIMARY KEY (`id`),
  ADD KEY `fk_rezerwacje_klient` (`klient_id`),
  ADD KEY `fk_pokoje_rezerwacje` (`room_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `klienci`
--
ALTER TABLE `klienci`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=19;

--
-- AUTO_INCREMENT for table `pokoje`
--
ALTER TABLE `pokoje`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT for table `rezerwacje`
--
ALTER TABLE `rezerwacje`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `rezerwacje`
--
ALTER TABLE `rezerwacje`
  ADD CONSTRAINT `fk_pokoje_rezerwacje` FOREIGN KEY (`room_id`) REFERENCES `pokoje` (`id`),
  ADD CONSTRAINT `fk_rezerwacje_klient` FOREIGN KEY (`klient_id`) REFERENCES `klienci` (`id`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
