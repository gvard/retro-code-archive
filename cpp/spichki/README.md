# 🪵 Spichki (Matches) Game

[English](#english) | [Русский](#русский)

---

## English

### 📜 History & Context
This program is a classic educational logic game originally developed in **C++ Builder 5** as a student project back in **2001–2002**. As part of the broader **retro-code-archive** project, the source code was rescued, stabilized, and refactored for modern systems in **August–September 2026** using C++ Builder 10.4.

### ✨ Features
* **Classic Logic Rules:** A two-player game where a random number of matches (`TShape` controls between 25 and 30) is generated. Players take turns removing 1 or 2 matches. The player who takes the last match wins.
* **Dual Control System:** Fully playable both using a mouse (via `TRadioButton` controls and a submission button) and natively with a keyboard (pressing `'1'` or `'2'` keys).
* **Dynamic Graphic Canvas:** Implemented runtime instantiation of VCL objects (`new TShape(0)`). The array of matches recalculates its physical alignment on the screen automatically based on the total randomized count.
* **Turn Management & Logging:** Turn-based control system utilizing active state switching for radio buttons. Player moves are tracked and logged in real-time inside separate `TMemo` panels.
* **Safe Memory Management:** Clean heap deallocation loops (`delete MyShape[i]`) preventing memory leaks upon object deletion and game soft-resets.

### 🛠️ Key Controls
* **Keyboard Inputs:** `1` or `2` — Take 1 or 2 matches instantly.
* **UI Controls:** `RadioButton1` / `RadioButton2` (Player 1 moves), `RadioButton3` / `RadioButton4` (Player 2 moves).

---

## Русский

### 📜 История и контекст
Эта программа представляет собой классическую обучающую логическую игру, изначально разработанную в **C++ Builder 5** в качестве учебного проекта в **2001–2002 годах**. В рамках общего проекта **retro-code-archive** исходный код был извлечен из архивов, стабилизирован и доведен до актуального состояния в **августе — сентябре 2026 года** с использованием среды C++ Builder 10.4.

### ✨ Возможности программы
* **Классическая логическая игра:** Игра для двух участников, где на старте случайно генерируется кучка спичек (компоненты `TShape` в количестве от 25 до 30 штук). Игроки по очереди берут 1 или 2 спички. Забравший последнюю спичку объявляется победителем.
* **Двойная система управления:** Программа полностью поддерживает как классическое управление мышью (через переключатели `TRadioButton` и кнопку подтверждения ходов), так и быстрое клавиатурное управление (нажатием клавиш `'1'` или `'2'`).
* **Динамический холст:** Реализовано динамическое создание VCL-объектов в памяти во время работы приложения (`new TShape(0)`). Координаты отрисовки массива спичек автоматически рассчитываются на экране в зависимости от выпавшего случайного количества.
* **Учет ходов и логирование:** Автоматическое переключение доступности элементов управления в зависимости от текущего хода. История ходов каждого игрока фиксируется в реальном времени в персональных текстовых панелях `TMemo`.
* **Безопасная очистка памяти:** Корректные циклы освобождения кучи (`delete MyShape[i]`) со сбросом указателей в `nullptr`, предотвращающие утечки памяти при уничтожении объектов и автоматическом перезапуске игрового сессионного цикла.

### 🛠️ Ключевые элементы управления
* **Клавиатура:** Клавиши `1` или `2` — мгновенно забрать 1 или 2 спички.
* **Интерфейс UI:** `RadioButton1` / `RadioButton2` (Ход 1-го игрока), `RadioButton3` / `RadioButton4` (Ход 2-го игрока).
