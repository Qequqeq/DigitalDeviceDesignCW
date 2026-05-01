# Pomodoro Timer

*A full-cycle digital device development project — from concept and code to PCB design and 3D-printed enclosure.*

---

### Device Description

The “Pomodoro Timer” is a physical productivity tool that shows the current time and works as a Pomodoro timer. The device consists of the following components:

- **Two push buttons** – power button and timer start button.  
  ![Button](Images/fig2.png)
- **1.8” TFT LCD color display** (128×160) – information screen.  
  ![LCD](images/figure3.png)
- **Rotary encoder** – time adjustment knob.  
  ![Encoder](images/figure4.png)
- **Arduino Nano** – the main controller.  
  ![Arduino Nano](images/figure5.png)
- **RTC module (DS1302)** – real-time clock.  
  ![RTC module](images/figure6.png)

With the power button the device turns on. By default it displays the current time; pressing the additional button switches to Pomodoro timer mode. The user can press the encoder knob to enter the interval configuration mode, where turning the knob adjusts the work/rest durations to the user’s preference. The screen shows the current interval number, its type (work or rest), and the remaining time until the end of the interval.

The device is designed to **boost user productivity**. The Pomodoro Technique is effective because it breaks work into short, manageable intervals (25 minutes of work followed by 5 minutes of rest), which helps fight procrastination, improves focus, reduces mental fatigue, and prevents burnout by training the brain to concentrate and distribute energy rationally — ultimately increasing productivity and task satisfaction. This particular implementation stands out for its **extreme compactness**.

### Schematic

The schematic diagram of the device is shown in Figure 7.

![Schematic](Hardware/Schematic/SchematicDiagram.png)

### PCB Development

The designed PCB is a **two-layer board**. The top layer is shown in Figure 8, the bottom layer in Figure 9. The board has four mounting holes for attachment to the enclosure.

![Top layer (VCC)](images/figure8.png)
![Bottom layer (GND)](images/figure9.png)

The board can be exported as a 3D model to visualize how it will look with components. This model is useful for further enclosure development in CAD software (e.g., KOMPAS, Fusion 360, Blender). However, some components do not have 3D models due to their limited popularity and therefore do not appear on the board. The 3D model is shown in Figures 10 and 11.

![3D model – top](images/figure10.png)
![3D model – bottom](images/figure11.png)

### Enclosure Design

The enclosure was designed in **Blender**. It consists of two halves. The front half is shown in Figures 12 and 13. On the front side there is a rectangular cutout for the display, two rectangular cutouts for the buttons, and one round cutout for the encoder. On the rear side there is a cutout for the Arduino Nano power connector. Four mounting points for the PCB are provided, designed for **M1.6×8 mm screws** with a hole diameter of 0.8 mm. On the edges of the case there are four additional holes (6.5 mm deep) for screws of the same size, used to fasten the two halves together.

![Front half – front view](images/figure12.png)
![Front half – rear view](images/figure13.png)

The rear half is a cover that hides the board from the environment. It is shown in Figures 14 and 15. The thin wall thickness is 3 mm. The cover has four recesses 1.5 mm deep and 2 mm in diameter.

![Rear half – front view](images/figure14.png)
![Rear half – rear view](images/figure15.png)

The assembled device can be examined in detail in X-ray mode in Figures 16, 17, and 18.

![Assembly – fastening of two parts](images/figure16.png)
![Assembly – power connector](images/figure17.png)
![Assembly – front view](images/figure18.png)

### Functional Description

The device is a **Pomodoro timer with a clock**, designed to increase work efficiency using the Pomodoro Technique (25 minutes of work, 5 minutes of rest, and a long break after 4 work periods).

**Features:**
- Time setting by long-pressing the Start button
- Pomodoro timer with adjustable intervals
- Control via rotary encoder with button (rotate to change values / navigate menus, press to confirm / select)
- Pause the timer — because it’s not always possible to avoid interruptions during work
- Return to clock mode by long-pressing the encoder button

**Built-in modules:**
- Two buttons: Start (launch) and Pause (pause/resume)
- 128×160 color LCD display
- RTC module for automatic time retrieval after initialization on first power-on
- Rotary encoder with button for navigation

The device takes the Pomodoro timer experience to a new level. First, unlike the most popular format — a smartphone app — the device offers a **tactile experience**: turning a physical knob, pressing buttons, making interaction more engaging. Second, it **deliberately removes the ability to exit** work or rest modes. People often skip breaks; here, exiting rest mode is impossible. Exiting work mode is also impossible — once you start a task, you must see it through.

In addition, the main screen — a minimalist, geek-style clock — serves as an aesthetic desktop accessory even when the Pomodoro functions are not in use.

### Realized Project

Figure 19 shows the functional prototype assembled on the PCB.

![Assembled device](images/figure19.png)

The buttons were replaced with identical ones that do not differ in properties or connection from those described above.

Figure 20 shows the main screen of the developed device. Pressing the Start button switches to Pomodoro timer mode with the last used settings. To change the Pomodoro intervals, press the encoder button and set the desired values (Figure 21).

![Main menu](images/figure20.png)
![Settings menu](images/figure21.png)

To start the timer, select “Start timer” and press the encoder button. The device then enters Pomodoro mode (Figure 22). The current time is displayed in the upper right corner, and the current Pomodoro session number is shown in the center.

![Pomodoro mode (work session)](images/figure22.png)

**Demonstration video:**  
[https://drive.google.com/file/d/1I5Sh2wMGCJ8CNwYgE7zXQAQE2WJvOOLk/view](https://drive.google.com/file/d/1I5Sh2wMGCJ8CNwYgE7zXQAQE2WJvOOLk/view)

### Conclusion

During this course work, the task of designing and implementing a digital device — the **Pomodoro Timer** — was successfully solved. The device is intended to increase personal productivity through the interval work method.

The hardware part was developed, including an Arduino Nano microcontroller, a DS1302 real-time clock module for accurate timekeeping, a color TFT display (ST7735) for information visualization, as well as an encoder and tactile buttons for a user-friendly interface. Based on the selected components, a principle schematic was designed ensuring correct interconnection of all modules with the controller, including necessary power and pull-up signal lines.

The next stage was the design of a **two-layer PCB** topology using CAD software. To protect internal components and ensure ergonomics, a **3D-printed enclosure** was designed and manufactured, providing convenient placement of the display and controls.

An important part of the work was the **firmware implementation**. The microcontroller code was written and debugged, implementing real-time clock logic, the Pomodoro timer algorithm with alternating work/rest modes, and a menu system for parameter configuration. As a result, a **working prototype** was created, which successfully passed testing and demonstrated full functionality in accordance with the technical specification.

Thus, the goal of the course work was achieved. The developed “Pomodoro Timer” device is a **complete digital product** ready for use. Completing this work allowed the reinforcement of practical skills in the **full cycle of electronic device design**: from schematic and PCB development to microcontroller programming and enclosure creation.

---

### Описание устройства

Устройство может показывать текущее время и работать в режиме Pomodoro-таймера. В его составе:

- Два кнопочных переключателя – кнопка включения и кнопка запуска таймера.  
  ![Кнопка](Images/fig2.png)
- Цветной дисплей TFT LCD 1,8" (128×160) – экран отображения информации.  
  ![LCD дисплей](images/figure3.png)
- Поворотный энкодер – тумблер управления временем.  
  ![Энкодер](images/figure4.png)
- Плата Arduino Nano – управляющий элемент.  
  ![Arduino Nano](images/figure5.png)
- RTC модуль (DS1302) – часы реального времени.  
  ![RTC модуль](images/figure6.png)

С помощью кнопки включения устройство включается. По умолчанию показывает время; при нажатии дополнительной кнопки переходит в режим Pomodoro-таймера. Пользователь нажатием на ручку энкодера попадает в режим настройки временных интервалов: вращением регулирует удобное время работы/отдыха. На экран выводится номер текущего интервала, его вид (отдых или работа), а также оставшееся время.

Устройство предназначено для **повышения продуктивности**. Метод Помодоро эффективен, так как разбивает работу на короткие контролируемые интервалы (25 минут работы и 5 минут отдыха), помогает бороться с прокрастинацией, улучшает концентрацию, снижает умственную усталость и предотвращает выгорание. Особенность данной реализации — **особая компактность**.

### Принципиальная схема

Принципиальная схема устройства представлена на рисунке 7.

![Принципиальная схема](images/figure7.png)

### Разработка печатной платы

Печатная плата — **двухслойная** (верхний слой — рисунок 8, нижний — рисунок 9). Предусмотрены 4 отверстия для крепления к корпусу.

![Верхний слой (VCC)](images/figure8.png)
![Нижний слой (GND)](images/figure9.png)

Плату можно конвертировать в 3D-модель (рисунки 10 и 11), что полезно при дальнейшем проектировании корпуса (КОМПАС, Fusion 360, Blender). Некоторые компоненты не имеют своих 3D-моделей, поэтому они не отображаются.

![3D-модель – верх](images/figure10.png)
![3D-модель – низ](images/figure11.png)

### Проектирование корпуса

Корпус спроектирован в **Blender** и состоит из двух половинок. Лицевая часть (рис. 12, 13) имеет прямоугольный вырез под дисплей, два под кнопки и круглый под энкодер. Сзади — вырез под разъём питания Arduino Nano. Для платы предусмотрены 4 крепления под винты **M1,6×8 мм**. По торцам — ещё 4 отверстия глубиной 6.5 мм для скрепления половинок.

![Лицевая часть – вид спереди](images/figure12.png)
![Лицевая часть – вид сзади](images/figure13.png)

Тыльная часть — крышка (рис. 14, 15). Толщина тонкой стенки 3 мм, есть 4 углубления (1,5 мм глубиной, ∅2 мм).

![Тыльная часть – вид спереди](images/figure14.png)
![Тыльная часть – вид сзади](images/figure15.png)

Собранное устройство в X-ray режиме (рис. 16–18):

![Крепление двух частей](images/figure16.png)
![Разъём питания](images/figure17.png)
![Лицевая часть](images/figure18.png)

### Описание функционала

Устройство представляет собой **Помодоро-таймер с часами** для повышения эффективности по методу Помодоро (25 мин работы / 5 мин отдыха, длинный перерыв после 4 циклов).

**Функции:**
- Установка времени долгим нажатием кнопки Start
- Помодоро-таймер с настраиваемыми интервалами
- Управление энкодером с кнопкой (поворот — смена значений/выбор в меню, нажатие — подтверждение/выбор)
- Возможность поставить таймер на паузу
- Выход в меню часов по долгому нажатию кнопки энкодера

**Модули:**
- Две кнопки: Start (запуск) и Pause (пауза/возобновление)
- Цветной LCD-дисплей 128×160
- Модуль часов реального времени (автоматическая загрузка времени при первом включении)
- Поворотный энкодер с кнопкой

Устройство выводит опыт использования помодоро-таймеров на новый уровень: во-первых, в отличие от приложений на телефон, оно даёт **тактильные ощущения** и более глубокое взаимодействие. Во-вторых, **невозможно выйти из режима работы или отдыха** — люди часто пренебрегают отдыхом, поэтому из отдыха выйти нельзя; из работы — тоже, ведь начатое дело нужно доводить до конца.

Главный экран — минималистичные часы в гик-стиле — служит эстетичным украшением рабочего места, даже если владелец не пользуется таймером.

### Реализованный проект

Функциональная модель, собранная на печатной плате — рисунок 19.

![Собранное устройство](images/figure19.png)

Кнопки заменены на аналогичные, не отличающиеся по свойствам.

На рисунке 20 — главный экран. По нажатию кнопки Start устройство переходит в режим помодоро-таймера с последними настройками. Для изменения интервалов нужно нажать кнопку энкодера и задать значения (рис. 21).

![Главное меню](images/figure20.png)
![Меню настройки](images/figure21.png)

Для запуска таймера выбирается пункт «Start timer» и нажимается кнопка энкодера. Устройство переходит в режим помодоро (рис. 22). В правом верхнем углу отображается текущее время, по центру — номер текущей «помидорки».

![Режим помодоро (рабочий интервал)](images/figure22.png)

**Демонстрационное видео:**  
[https://drive.google.com/file/d/1I5Sh2wMGCJ8CNwYgE7zXQAQE2WJvOOLk/view](https://drive.google.com/file/d/1I5Sh2wMGCJ8CNwYgE7zXQAQE2WJvOOLk/view)

### Заключение

В ходе выполнения курсовой работы была успешно решена задача проектирования и реализации цифрового устройства — «Pomodoro Timer». Разработана аппаратная часть на базе Arduino Nano, DS1302, ST7735, энкодера и тактовых кнопок; спроектирована принципиальная схема, двухслойная печатная плата, 3D-модель корпуса. Написан и отлажен код прошивки, реализующий логику часов, алгоритм таймера и систему меню. Создан действующий прототип, полностью соответствующий техническому заданию.

Цель курсовой работы достигнута. Устройство представляет собой законченный цифровой продукт, готовый к эксплуатации. Выполнение работы закрепило практические навыки полного цикла проектирования электронных устройств: от схемы и платы до программирования и создания корпусных элементов.

---

## 📌 Repository structure (рекомендуемая)

- `hardware/` — schematic, PCB files, BOM
- `enclosure/` — STL files for 3D printing, Blender sources
- `firmware/` — Arduino sketch (`.ino`) and required libraries
- `images/` — all images used in README
- `docs/` — full PDF reports (course work and practical assignments)

## 📄 License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.
