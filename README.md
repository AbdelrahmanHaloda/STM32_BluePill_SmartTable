# STM32F103 Reaction Trainer (Blue Pill + 20×4 I²C LCD + 12 illuminated limit switches)

A reaction/skills game for the **STM32F103C8T6 (Blue Pill)**.  
Features a 20×4 I²C LCD UI (menu + results), **3-second “Get ready” countdown**, and gameplay where one of **12 LEDs** lights up at a fixed cadence (Easy/Medium/Hard). Press the **matching limit switch** while its LED is ON to score. All inputs are **debounced**, and the app tracks **hits / trials**.

---

## ✨ Features
- 20×4 HD44780 LCD via I²C (PCF8574 backpack; **addr 0x27** → 0x4E 8-bit)
- Menu (Easy=5 s, Medium=3 s, Hard=1 s) → **READY** (3 s) → RUNNING → SUMMARY
- All LEDs blink during the READY countdown
- Random LED selection (no immediate repeats), fixed cadence per level
- Debounced inputs (limit switches + UP/DOWN/OK buttons)
- Clean modular code: `io`, `debounce`, `ui_lcd`, `game`, `i2c-lcd`

---

## 🧰 Hardware

- **MCU:** STM32F103C8T6 “Blue Pill”
- **LCD:** 20×4 HD44780 with I²C backpack (PCF8574)
- **Switches:** 12 *illuminated* limit switches (each has an LED + NO/NC/COM)
- **Nav Buttons:** 3× momentary push buttons (UP/DOWN/OK)
- **Programmer:** ST-LINK/V2 (SWD)

> **Power scheme:**  
> - LED **anodes** (+) and **switch commons** (COM) share a **+5 V bus**.  
> - Each LED **cathode** (−) goes to an **MCU open-drain output** (sinks current).  
> - Each switch **NO** goes to an **MCU input** (reads **HIGH when pressed**).  
> - MCU runs at **3.3 V**. All grounds must be **common**.

---

## 🔌 Pin Mapping

### LCD (I²C1)
| Function | Pin |
|---|---|
| SCL | **PB6** |
| SDA | **PB7** |
| Address | 7-bit **0x27** (8-bit write 0x4E) |

### 12 LED sinks (open-drain outputs; **LOW = ON**, HIGH = OFF)
| LED | Pin | LED | Pin |
|---|---|---|---|
| LED1 | **PA1** | LED7  | **PA7**  |
| LED2 | **PA2** | LED8  | **PA8**  |
| LED3 | **PA3** | LED9  | **PA9**  |
| LED4 | **PA4** | LED10 | **PA10** |
| LED5 | **PA5** | LED11 | **PA11** |
| LED6 | **PA6** | LED12 | **PA12** |

> Configure as **Output, Open-Drain, No-pull, Low speed**, default level **HIGH** (OFF at reset).

### 12 limit-switch inputs (NO → pin; **Pull-Down**; 5-V tolerant group)
| SW# | Pin | SW# | Pin |
|---|---|---|---|
| SW1 | **PB0** | SW7  | **PB9**  |
| SW2 | **PB1** | SW8  | **PB10** |
| SW3 | **PA0** | SW9  | **PB11** |
| SW4 | **PB4** | SW10 | **PB12** |
| SW5 | **PB5** | SW11 | **PB13** |
| SW6 | **PB8** | SW12 | **PB3**  |

> **Important:** Use only **5-V-tolerant** GPIO for these inputs. Do **not** put 5 V on PC13–PC15.

### Navigation buttons (inputs, Pull-Down)
- **UP:**  **PB14**  
- **DOWN:** **PB15**  
- **OK:**   **PA15**

---

## 🗂️ Project Structure

```
Inc/
deb.h # debounce API (12 switches + 3 buttons)
game.h # game state machine API
io.h # pin maps + simple LED/SW/BTN helpers
i2c-lcd.h # LCD driver (4-bit via PCF8574)
ui_lcd.h # LCD UI helpers (menu, ready, game, summary)

Src/
deb.c # debounce integrator (1 ms poll)
game.c # READY → RUNNING → SUMMARY logic + PRNG
io.c # LED on/off/toggle, raw reads for switches & buttons
i2c-lcd.c # lcd_init/send_cmd/send_data/put_cur/create_char
ui_lcd.c # render functions (20-chars-per-line safe writes)
main.c # app glue + state machine
```

---

## 🛠️ Building & Flashing (STM32CubeIDE)

1. **Clone** the repo.
2. **Open** in *STM32CubeIDE* (`File → Open Projects from File System…` or `Import → Existing Projects`).
3. Verify Cube settings:
   - `System → SYS → Debug = Serial Wire` (frees PB3/PB4)
   - `I2C1` on **PB6/PB7**
   - GPIO as per the tables above
4. **Build** and **Debug/Run** with **ST-LINK/V2** (SWD).  
   If you see “No device found”, try ST-LINK **Under Reset** and a lower SWD freq.

---

## ▶️ How to Use

1. On boot you’ll see the **menu**:
   - **UP/DOWN** to select **Easy (5 s), Medium (3 s), Hard (1 s)**
   - **OK** to start
2. **READY** screen (3 s): “Get ready… Starting in Xs”  
   All LEDs **blink together**; **OK** skips the countdown.
3. **RUNNING**: every interval one random LED turns ON.  
   Press the **matching limit switch** while it’s lit to score.
4. **SUMMARY**: shows `hits / trials`. **OK** returns to menu.

---

## ⚙️ Configuration

Edit these in `game.c` / `deb.h`:
```c
// game.c
// per level
EASY = 5000, MEDIUM = 3000, HARD = 1000  // ms per step
g->max_trials = 20;                      // set to e.g. 300 for ~5 minutes at 1s

// deb.h
#define DEB_THRESHOLD_MS 20              // button/switch debounce window
```

---

### ✅ LCD Notes (20×4)

20×4 DDRAM base addresses: row0 0x80, row1 0xC0, row2 0x94, row3 0xD4.

Always write exactly 20 chars per row to avoid wrap-around.
ui_lcd.c uses a helper that pads/truncates to 20 chars safely.

Backpack address is 0x27 (7-bit). The driver uses 0x4E for 8-bit writes.

---

### 🧪 Troubleshooting

No LCD text / garbage: check contrast, I²C wiring (PB6/PB7), and address (0x27).

Some menu lines vanish: ensure lines are written ≤ 20 chars and cursor set per row (fixed in ui_lcd.c).

Inputs never read HIGH: confirm common ground, pull-downs enabled, and pins are those listed above.

LEDs don’t light: LEDs must have current-limit resistors inside the switch module; MCU pins are open-drain (LOW=ON).

ST-LINK connect errors: try “Under Reset” mode and lower SWD frequency; ensure NRST wiring or power cycle the board.

### 🧩 Optional: Custom LCD Glyphs

The LCD driver exposes lcd_create_char(location, charmap) to load up to 8 custom characters (e.g., a smiley). See comments in i2c-lcd.c.

---

### 📜 License

MIT — see LICENSE.

---
