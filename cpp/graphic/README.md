# 📈 Graphic Charting Engine

[English](#english) | [Русский](#русский)

---

## English

### 📜 History & Context
This program was originally developed in **C++ Builder 5** as a student project back in **2001–2002**. After spending over two decades as a legacy artifact, the codebase was rescued, refactored, and fully modernized to its current adaptive state in **August–September 2026** using C++ Builder 10.4 (Clang-based Win32 compiler).

### ✨ Features
* **Robust Math Parser:** Supports operators (`+`, `-`, `*`, `/`, `^`), algebraic order of operations, and nested brackets without crashing on undefined regions or division-by-zero.
* **Absolute Value Parsing:** Built-in processing for vertical bars `|x|`.
* **Non-Lossy Constants:** High-precision, case-insensitive evaluation for mathematical constants `"pi"` and `"e"` tied directly to native hardware `<cmath>` definitions.
* **Multi-Graph Mode ("Do not erase"):** An overlay feature allowing multiple functions to be plotted over one another on the same coordinate grid for easy comparison.
* **Adaptive Fullscreen Engine (`F11` / `Esc`):** Seamless transition into a clean, maximized view with automatic DPI canvas stretching and contextual control translocation.
* **Keyboard-Only Control:** Fully navigable via `Tab` progression and instant graph plotting by pressing `Enter` inside any text input field (with silenced error beeps).
* **Smart Axis Formatting:** Adaptive tick steps to prevent CPU freezing on large scales, automatic collision avoidance with arrows, bidirectional Y-axis label shifts, and clean float truncation (stripping trailing zeros and removing long `E` notation strings).

### 🛠️ Built-in Presets
The engine includes a pre-defined library of mathematical functions accessible via the dropdown menu:
* `x^2+2*x+1`
* `x^3-x`
* `0.1*x^3-x^2+x`
* `x^4-4*x^2`
* `x*sin(1/x)`
* `|x|`
* `1/x`
* `1/x^2`
* `1/(1+x^2)`
* `1/(1+25*x^2)`
* `sin(x)`
* `cos(x)`
* `sin(x)*cos(x)`
* `tg(x)*ctg(x)`
* `sin(x^2)`
* `ln(x)`
* `x*ln(x)`
* `x^3/(x^2-0.5)`
* `e^(-x^2)*cos(2*pi*x)`
* `cos(pi*|x|)-|x|`

---

## Русский

### 📜 История и контекст
Эта программа изначально создавалась в **C++ Builder 5** в качестве учебного проекта в **2001–2002 годах**. Спустя более чем два десятка лет исходный код был извлечен из архивов, полностью переработан и доведен до актуального состояния в **августе — сентябре 2026 года** с использованием среды C++ Builder 10.4 (компилятор Clang Win32).

### ✨ Возможности программы
* **Отказоустойчивый парсер:** Поддерживает операторы (`+`, `-`, `*`, `/`, `^`), приоритет операций и вложенные скобки. Защищен от зависаний при делении на ноль или выходе за область определения функции.
* **Вычисление модуля:** Нативная поддержка вертикальных черт `|x|` в формулах.
* **Поддержка констант без потери точности:** Нечувствительный к регистру букв разбор констант `"pi"` и `"e"`, завязанный напрямую на аппаратные системные макросы библиотеки `<cmath>`.
* **Режим наложения (Чекбокс «Не стирать»):** Позволяет строить несколько графиков на одном холсте друг поверх друга для наглядного сравнения функций.
* **Адаптивный полноэкранный режим (`F11` / `Esc`):** Чистый разворот программы на весь экран с автоматическим пересчетом масштаба осей.
* **Управление без мыши:** Сквозная навигация с клавиатуры по клавише `Tab` и построение графика по нажатию `Enter` внутри текстовых полей (с подавлением системного писка ошибок).
* **Координатная сетка:** Адаптивный шаг засечек осей (защита от зависаний CPU на огромных диапазонах), исключение наложений текста на стрелки, двунаправленный сдвиг шкалы Y, и корректное форматирование чисел (скрытие длинной экспоненциальной нотации `E` и очистка лишних нулей вроде `.00`).

### 🛠️ Встроенные примеры для построения
В выпадающем списке программы содержатся следующие математические функции:
* `x^2+2*x+1` — Квадратичная парабола
* `x^3-x` — Кубическая парабола
* `0.1*x^3-x^2+x` — Кубический многочлен
* `x^4-4*x^2` — Симметричная функция высшего порядка
* `x*sin(1/x)` — Функция с топологическим синусом
* `|x|` — Простейший модуль
* `1/x` — Классическая гипербола
* `1/x^2` — Положительная гипербола
* `1/(1+x^2)` — Лоренциан (кривая распределения)
* `1/(1+25*x^2)` — Функция Рунге
* `sin(x)` — Синусоида
* `cos(x)` — Косинусоида
* `sin(x)*cos(x)` — Произведение тригонометрических функций
* `tg(x)*ctg(x)` — Тождественная единица (с учетом разрывов)
* `sin(x^2)` — Функция с переменной частотой колебаний
* `ln(x)` — Натуральный логарифм
* `x*ln(x)` — Функция логарифмического роста
* `x^3/(x^2-0.5)` — Рациональная дробь с вертикальными асимптотами
* `e^(-x^2)*cos(2*pi*x)` — Затухающие колебания (модуляция Гаусса)
* `cos(pi*|x|)-|x|` — Комбинированная тригонометрическая функция с модулем
