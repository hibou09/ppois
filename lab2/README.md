# Лабораторная работа №2 — «Океанариум»

Объектно-ориентированная модель океанариума на C++17: аквариумы, обитатели, системы жизнеобеспечения, корм и медицина, посетители и билеты, персонал, бюджет и обслуживание.

## Сборка и запуск

```bash
cmake -S . -B build
cmake --build build
./build/oceanarium          # демонстрация
./build/oceanarium_tests    # модульные тесты (UnitTest++)
```

## Структура

| Файл | Содержимое |
|---|---|
| `include/Common.h` | дата, интервал времени, расположение |
| `include/Water.h` | параметры воды, датчики, фильтрация, нагрев, освещение |
| `include/Food.h` | корм и склад |
| `include/Health.h` | медкарта и лекарства |
| `include/Animals.h` | виды и обитатели, кораллы |
| `include/Feeding.h` | расписание кормлений, поставщики |
| `include/Tanks.h` | аквариумы, экспозиции, мониторинг |
| `include/Visitors.h` | посетители, билеты, абонементы, магазин, экскурсии, шоу |
| `include/Staff.h` | сотрудники, смены |
| `include/Management.h` | обслуживание, бюджет, зарплаты, класс Oceanarium |
| `include/Exceptions.h` | собственные исключения |

Тесты находятся в `tests/` (по одному файлу на модуль, плюс `test_edge_cases.cpp` с граничными случаями).

## Классы

«Методы» — методы с логикой (без конструкторов и простых геттеров/сеттеров). «Ассоциации» — другие классы модели, которые входят в класс как поле или используются в параметрах и возвращаемых значениях методов; наследование указано отдельным столбцом.

| № | Класс | Назначение | Родитель | Полей | Методов | Ассоциации |
|---|---|---|---|---|---|---|
| 1 | `Species` | Биологический вид | — | 6 | 2 | `WaterRequirements` |
| 2 | `Animal` | Абстрактный обитатель океанариума | — | 7 | 10 | `Date`, `FoodItem`, `HealthRecord`, `Species`, `WaterParameters` |
| 3 | `Fish` | Рыба | `Animal` | 2 | 3 | `Species` |
| 4 | `Shark` | Акула | `Fish` | 2 | 3 | `Animal`, `Species` |
| 5 | `Ray` | Скат | `Fish` | 3 | 2 | `Species` |
| 6 | `Turtle` | Морская черепаха | `Animal` | 2 | 5 | `Species` |
| 7 | `Octopus` | Осьминог | `Animal` | 3 | 5 | `Species` |
| 8 | `Jellyfish` | Медуза | `Animal` | 2 | 3 | `Species` |
| 9 | `Penguin` | Пингвин | `Animal` | 3 | 6 | `Species` |
| 10 | `Seal` | Тюлень — участник шоу | `Animal` | 3 | 4 | `Species` |
| 11 | `Coral` | Колония кораллов | — | 4 | 3 | `WaterParameters` |
| 12 | `Date` | Календарная дата (григорианский календарь) | — | 3 | 8 | — |
| 13 | `TimeSlot` | Интервал времени внутри суток, в минутах от полуночи: [start, end) | — | 2 | 4 | — |
| 14 | `Location` | Расположение объекта на территории океанариума | — | 3 | 3 | — |
| 15 | `FeedingEntry` | Запись расписания: какое животное, чем, сколько и когда кормить | — | 4 | 1 | — |
| 16 | `FeedingSchedule` | Суточное расписание кормлений | — | 2 | 5 | `Animal`, `Date`, `FeedingEntry`, `FoodStorage` |
| 17 | `Supplier` | Поставщик корма | — | 5 | 4 | `Date`, `FoodItem` |
| 18 | `FoodItem` | Партия корма определённого вида | — | 5 | 5 | `Date` |
| 19 | `FoodStorage` | Склад кормов с ограниченной вместимостью | — | 2 | 8 | `Date`, `FoodItem` |
| 20 | `MedicalEntry` | Одна запись в медицинской карте | — | 5 | 2 | `Date` |
| 21 | `HealthRecord` | Медицинская карта животного | — | 4 | 6 | `Date`, `MedicalEntry` |
| 22 | `Medication` | Лекарство со складским остатком | — | 4 | 4 | `Animal`, `Date` |
| 23 | `MaintenanceTask` | Задача технического обслуживания аквариума | — | 7 | 3 | `Date`, `Employee`, `Tank` |
| 24 | `MaintenanceLog` | Журнал задач обслуживания | — | 2 | 7 | `Date`, `Employee`, `MaintenanceTask`, `Tank` |
| 25 | `Budget` | Бюджет океанариума | — | 5 | 5 | — |
| 26 | `Payroll` | Расчёт и выплата зарплаты | — | 3 | 3 | `Budget`, `Employee` |
| 27 | `Oceanarium` | Океанариум — корневой объект модели | — | 12 | 16 | `Budget`, `Date`, `Employee`, `Exhibit`, `FeedingSchedule`, `FoodStorage`, `MaintenanceLog`, `Payroll`, `StaffSchedule`, `Tank`, `TicketOffice` |
| 28 | `Employee` | Базовый сотрудник | — | 5 | 4 | `Date` |
| 29 | `Aquarist` | Аквариумист — обслуживает закреплённые аквариумы | `Employee` | 3 | 6 | `Date`, `Tank` |
| 30 | `Veterinarian` | Ветеринар | `Employee` | 4 | 6 | `Animal`, `Date`, `Medication` |
| 31 | `Diver` | Водолаз — работает внутри больших аквариумов | `Employee` | 2 | 4 | `Date`, `OpenOceanTank` |
| 32 | `Trainer` | Дрессировщик морских животных | `Employee` | 2 | 2 | `Date`, `Seal`, `Show` |
| 33 | `Guide` | Экскурсовод | `Employee` | 2 | 4 | `Date`, `Tour` |
| 34 | `Cashier` | Кассир | `Employee` | 3 | 2 | `Date`, `Membership`, `PromoCode`, `Ticket`, `TicketOffice`, `Visitor` |
| 35 | `Shift` | Рабочая смена сотрудника | — | 3 | 2 | `Date`, `TimeSlot` |
| 36 | `StaffSchedule` | Расписание смен | — | 1 | 5 | `Date`, `Shift` |
| 37 | `Tank` | Базовый аквариум | — | 10 | 15 | `Animal`, `FiltrationSystem`, `HeatingSystem`, `Location`, `WaterParameters` |
| 38 | `CoralReefTank` | Рифовый аквариум с кораллами | `Tank` | 2 | 6 | `Animal`, `Coral`, `Location`, `WaterParameters` |
| 39 | `OpenOceanTank` | Большой аквариум «Открытый океан» с подводным тоннелем | `Tank` | 2 | 4 | `Location`, `WaterParameters` |
| 40 | `TouchPool` | Контактный бассейн, где посетители могут трогать животных | `Tank` | 2 | 4 | `Animal`, `Location`, `WaterParameters` |
| 41 | `QuarantineTank` | Карантинный аквариум для новых и больных животных | `Tank` | 2 | 5 | `Animal`, `Date`, `Location`, `WaterParameters` |
| 42 | `Exhibit` | Тематическая экспозиция из нескольких аквариумов | — | 4 | 6 | `Tank` |
| 43 | `EnvironmentMonitor` | Мониторинг среды: набор датчиков и журнал тревог | — | 3 | 5 | `Sensor`, `Tank` |
| 44 | `Visitor` | Посетитель океанариума | — | 5 | 5 | — |
| 45 | `Ticket` | Входной билет на конкретную дату | — | 6 | 5 | `Date` |
| 46 | `PromoCode` | Промокод на скидку | — | 4 | 3 | `Date` |
| 47 | `Membership` | Абонемент постоянного посетителя | — | 4 | 6 | `Date` |
| 48 | `TicketOffice` | Билетная касса | — | 6 | 5 | `Date`, `Membership`, `PromoCode`, `Ticket`, `Visitor` |
| 49 | `Product` | Товар сувенирного магазина | — | 5 | 3 | — |
| 50 | `GiftShop` | Сувенирный магазин | — | 4 | 6 | `Product`, `Visitor` |
| 51 | `Tour` | Экскурсия по аквариумам | — | 7 | 8 | `Guide`, `Tank`, `TimeSlot`, `Visitor` |
| 52 | `Show` | Шоу морских животных | — | 6 | 6 | `Seal`, `TimeSlot` |
| 53 | `WaterParameters` | Текущие параметры воды в аквариуме | — | 5 | 7 | — |
| 54 | `WaterRequirements` | Диапазоны параметров воды, допустимые для вида | — | 7 | 6 | `WaterParameters` |
| 55 | `Sensor` | Датчик параметра воды | — | 7 | 3 | `WaterParameters` |
| 56 | `FiltrationSystem` | Система фильтрации воды | — | 4 | 4 | `WaterParameters` |
| 57 | `HeatingSystem` | Нагреватель/охладитель воды | — | 3 | 3 | `WaterParameters` |
| 58 | `LightingSystem` | Освещение аквариума (свет включается в 08:00) | — | 4 | 4 | — |
| | **Итого** | | | **232** | **284** | **114** |

Перечисления: `Diet`, `ConservationStatus`, `TicketType`, `MembershipLevel`, `SensorType`.

## Исключения

Все исключения наследуются от `OceanariumException` (`std::runtime_error`).

| Исключение | Когда возникает |
|---|---|
| `OceanariumException` | Базовое исключение для всех ошибок океанариума |
| `InvalidDateException` | Некорректная календарная дата |
| `InvalidParameterException` | Некорректный аргумент (отрицательный вес, пустое имя и т.п.) |
| `InvalidWaterParametersException` | Параметры воды физически невозможны или не подходят обитателю |
| `TankCapacityExceededException` | В аквариуме нет места для нового животного |
| `IncompatibleSpeciesException` | Виды не могут жить вместе (хищник/жертва, разные требования к воде) |
| `AnimalNotFoundException` | Животное с указанным идентификатором не найдено |
| `TankNotFoundException` | Аквариум с указанным идентификатором не найден |
| `EmployeeNotFoundException` | Сотрудник с указанным идентификатором не найден |
| `InsufficientFoodException` | Недостаточно корма на складе |
| `ExpiredProductException` | Срок годности корма или лекарства истёк |
| `OutOfStockException` | Товар или лекарство закончились |
| `StorageFullException` | Склад переполнен |
| `InvalidTicketException` | Билет недействителен (использован, на другую дату, не тот тип) |
| `TicketSoldOutException` | Билеты на указанную дату распроданы |
| `InvalidPromoCodeException` | Промокод недействителен или исчерпан |
| `InsufficientFundsException` | Недостаточно средств у посетителя или на счёте океанариума |
| `BudgetExceededException` | Превышен лимит расходов бюджета |
| `ScheduleConflictException` | Пересечение смен, кормлений и т.п |
| `LicenseExpiredException` | Лицензия ветеринара просрочена |
| `EmployeeNotQualifiedException` | У сотрудника нет нужной квалификации или полномочий |
| `EquipmentFailureException` | Отказ оборудования (выключенный или забитый фильтр) |
| `SensorNotCalibratedException` | Датчик не откалиброван, показаниям нельзя доверять |
| `EventFullException` | Нет мест на экскурсию или шоу |
| `TaskNotFoundException` | Задача обслуживания не найдена |
| `QuarantineViolationException` | Попытка выпустить животное из карантина досрочно |

## Сводка требований

| Требование | Минимум | В проекте |
|---|---|---|
| Классов | 50 | 58 |
| Полей | 150 | 232 |
| Методов с логикой | 100 | 284 |
| Ассоциаций | 30 | 114 |
| Собственных исключений | 12 | 26 |
