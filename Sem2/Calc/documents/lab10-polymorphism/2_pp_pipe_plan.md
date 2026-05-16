# План: PP-задача — интеграция в calc*

ТЗ: [2_pp_pipe_tz.md](2_pp_pipe_tz.md).

**Принцип:** переписать `solve_pp()` в уже существующих калькуляторах (`pipe_calculator_t`, `pump_calculator_t`, `pump_station_calculator_t`, `local_resistance_calculator_t`). **Не создавать** отдельные модули (`pp_solver.h`, `pp_residual_equation_t`, общие обёртки солвера). Полиморфная «обвязка» — позже; сейчас только встраивание в текущие `.cpp`/`.h`.

## Текущее состояние

- Все `solve_pp()` — `throw std::runtime_error("Код пока не реализован")`.
- Готовая математика уже в классах: `get_volume_flow_from_head_at_current_frequency()`, `get_volume_flow_from_head_nominal()`, формула в комментарии `local_resistance.cpp`, цикл `solve_pq()` у трубы.

## CMake и сторонние библиотеки

**Пути к зависимостям — только через [CMakePresets.json](../../CMakePresets.json), не в `CMakeLists.txt`.**

### [CMakeLists.txt](../../CMakeLists.txt)

- `find_package(fixed_solvers QUIET CONFIG)` + `target_link_libraries(Tasks … fixed_solvers::fixed_solvers)` (и Tests, как сейчас).
- **Без** `list(APPEND CMAKE_PREFIX_PATH …)` и без абсолютных путей в репозитории.
- FetchContent (GitHub) — только fallback, если пакет не найден при конфигурации без пресета/установки.

### [CMakePresets.json](../../CMakePresets.json)

Скрытый пресет `base-fixed-solvers`:

```json
{
  "name": "base-fixed-solvers",
  "hidden": true,
  "cacheVariables": {
    "CMAKE_PREFIX_PATH": "C:/install/fixed_solvers"
  }
}
```

(установленная библиотека по пути из ТЗ: **`C:\install\fixed_solvers`**.)

- `base-debug` / `base-release`: `"inherits": ["base-fixed-solvers"]` (или цепочка через них).
- `windows-msvc-*`: достаточно наследования от `base-debug` → подтянется префикс.
- `windows-gcc-*` / `windows-clang-*`: в `CMAKE_PREFIX_PATH` явно `C:/msys64/ucrt64;C:/install/fixed_solvers` — иначе дочерний пресет перезапишет значение из `base-fixed-solvers`.

Сборка: `cmake --preset windows-msvc-debug` (или свой рабочий пресет).

### Код

- `#include <fixed/fixed.h>`, `<fixed/fixed_bisection.h>` — **только** в [pipe_oil.cpp](../../src/pipe_oil.cpp).

## Интеграция по классам

### `pipe_calculator_t` — [pipe_oil.cpp](../../src/pipe_oil.cpp)

Внутри `solve_pp()`:

1. Проверки входов (`pressure_start`, `pressure_end`, параметры).
2. **Начальное приближение Q₀** — из лекции 5, с. 13–15 («PP в сосредоточенной постановке, квадратичное трение»; для рассредоточенной трубы — та же сосредоточенная оценка перед численным методом, с. 15):

   Полные напоры на концах:
   \[
   H_1 = \frac{p_\text{н}}{\rho g} + z_0,\quad H_2 = \frac{p_\text{к}}{\rho g} + z_{n-1},\quad \Delta H = H_1 - H_2
   \]

   λ по **Шифринсону** (константа при квадратичном трении, с. 7, формула (19)):
   \[
   \lambda = 0{,}11 \left(\frac{\Delta}{d}\right)^{0{,}25}
   \]
   (в коде: `0.11 * pow(get_relative_roughness(), 0.25)` или ветка `get_hydraulic_resistance_coefficient()` при ещё неизвестной скорости).

   Скорость и объёмный расход (с. 14):
   \[
   v(\Delta H) = \mathrm{sign}(\Delta H)\sqrt{\frac{2gd}{\lambda L}\,|\Delta H|},\qquad Q_0 = v \cdot S
   \]
   где \(L\) — **суммарная** длина трассы (для профиля: \(x_{n-1}-x_0\) или сумма `profile.get_length(i)`), \(S\) — `get_pipe_area()`.

   Связь с уже имеющимся кодом: `get_velocity_by_solve_pp()` реализует ту же зависимость \(v(\Delta H)\), но сейчас берёт `profile.get_length(0)` (один участок). Для Q₀ в `solve_pp()` либо поправить длину на полную \(L\), либо один раз вычислить по формуле лекции **в теле `solve_pp()`** (без нового файла/метода).

3. Лямбда-невязка **в теле метода** (захват `this`):
   - для пробного `Q`: `volume_flow = Q`, вызов `solve_pq()` → `r(Q) = pressure_profile.back() - pressure_end`;
   - скобка для биссекции: вокруг `Q_0`, например `[Q_0/10,\, Q_0 \cdot 10]` с проверкой смены знака; при необходимости расширить.
4. `fixed_scalar_wrapper_t` + `fixed_bisectional<1>::solve`; стартовая точка / центр скобки — `Q_0` из п. 2.
5. Записать найденный `Q`, затем финальный `solve_pq()` для профилей.

При \(|\Delta H| \approx 0\) — `Q = 0` без солвера.

### Источник формул (все объекты)

Один файл лекций: **`2026 ОАиП Лекции. Блок 2.pdf`** (тот же, что для трубы).

| Объект | Раздел PDF | PP-формула |
|--------|------------|------------|
| **Труба** | Лекция 5, с. 13–15 | \(v=\mathrm{sign}(\Delta H)\sqrt{2gd/(\lambda L)\|\Delta H\|}\), \(Q=vS\); численно — невязка по профилю |
| **Локальное сопротивление** | Лекция 7, с. 29–30 | \(Q=\mathrm{sign}(p_\text{н}-p_\text{к})\sqrt{2S^2/(\rho\xi\|p_\text{н}-p_\text{к}\|)}\) |
| **Насос** | Лекция 7, с. 33–35 | \(\Delta H_f(Q,f)=a(f/f_0)^2-bQ\|Q\|\); PP: \(Q=Q\bigl(\frac{p_\text{к}-p_\text{н}}{\rho g},f\bigr)\) → \(Q=\mathrm{sign}(a(f/f_0)^2-\Delta H)\sqrt{\|a(f/f_0)^2-\Delta H\|/b}\) |
| **Насосная станция** | Лекция 7, с. 36–37 | Последовательное соединение: \(a'= \sum a_i\), \(b'= \sum b_i\); та же \(Q(\Delta H)\) при номинальной частоте (или через уже суммированные коэффициенты в коде) |

### `pump_calculator_t` — [pump.cpp](../../src/pump.cpp)

`solve_pp()` — формула лекции (двучленная QH, с. 35): через уже реализованный  
`get_volume_flow_from_head_at_current_frequency()` (\(\Delta H=(p_\text{к}-p_\text{н})/(\rho g)\)).

```cpp
pump_task_result.volume_flow = get_volume_flow_from_head_at_current_frequency();
volume_flow = pump_task_result.volume_flow;
```

Кубическая характеристика — по лекции с. 34 (Кардано) — пока `throw`, как сейчас.

### `pump_station_calculator_t` — [pump.cpp](../../src/pump.cpp)

Эквивалентная QH станции (с. 37): \(\Delta H_\text{станц}(Q)=a'-b'Q|Q|\). PP — та же обратная формула, что у одного насоса:

```cpp
pump_station_result.volume_flow = get_volume_flow_from_head_nominal();
```

(суммирование \(a,b\) уже в `get_polynomial_coefficients()`).

### `local_resistance_calculator_t` — [local_resistance.cpp](../../src/local_resistance.cpp)

Лекция 7, с. 29 (вывод из Вейсбаха через \(\Delta p\)), PP — с. 30:

```cpp
// Q = sign(p_n - p_k) * sqrt(2*S^2 / (rho*xi*|p_n - p_k|))
```

`sign_pressure_difference()`, `calc_local_resistance_coefficient()`, `get_pipe_area()` — без дублирования в отдельных файлах.

## Тесты

**Принцип:** тесты `solve_pp` проверяют **численный результат** (`EXPECT_NEAR`, `EXPECT_DOUBLE_EQ`, `ASSERT_NO_THROW`), а не `EXPECT_THROW` на заглушку. Удалить или переименовать тесты `*SolvePpIsNotImplementedYet*`.

### Заменить в тестах

| Было | Станет |
|------|--------|
| `PipeCalculator.SolvePpIsNotImplementedYet` | `SolvePpDoesNotThrow` + проверка `volume_flow` / `get_pipe_task_result()` |
| `PipeTaskCsv.WritesSolvePpScenarioProfilesToExeRelativeProfilesDir` | `ASSERT_NO_THROW(solve_pp)`; профили из результата расчёта, не заглушка |
| `PumpCalculator.SolvePpIsNotImplementedYet` | `SolvePpMatchesHeadFormula` — `volume_flow` ≈ `get_volume_flow_from_head_at_current_frequency()` |
| `PumpStationCalculator.SolvePpIsNotImplementedYet` | `SolvePpMatchesNominalHeadFormula` — ≈ `get_volume_flow_from_head_nominal()` |
| `LocalResistanceCalculator.SolvePpIsNotImplementedYet` | `SolvePpMatchesFormula` — явная формула ТЗ (аналог `SolvePqMatchesPressureDropFormula`) |

### Сценарии проверки значений

**Труба** (`test_pipe_oil.cpp`):

1. **Round-trip PQ↔PP:** задать `volume_flow` → `solve_pq` → граничные давления → сбросить расход → `solve_pp` → `EXPECT_NEAR(Q_pp, Q_ref, tol)`.
2. **Согласованность с PQ:** после `solve_pp` вызвать `solve_pq` с найденным Q → `pressure_out` ≈ заданному `pressure_end`.
3. **Нулевой перепад:** при равных давлениях и отметках — `Q ≈ 0`.
4. **Невязка:** опционально `|p_calc(Q*) - p_end| < ε` после сходимости.

**Насос / НС** (`test_pump.cpp`): `EXPECT_NEAR` к формулам лекции 7 (с. 35, таблица PP); эталон — те же выражения, что в `get_volume_flow_from_head_at_current_frequency()` / `get_volume_flow_from_head_nominal()`.

**Локальное сопротивление** (`test_local_resistance.cpp`): `EXPECT_NEAR` к формуле лекции 7, с. 30 (не только ТЗ).

Допуски: как в существующих PQ-тестах (`1e-6` … `1e-4` в зависимости от масштаба).

## Вне scope

- Новые базовые классы / `hydraulic_chain` (fall-through, копирование в `chain_task_result`).
- Вынос общих солверов из `numerical_methods`.
- Отдельные файлы под PP.

## Порядок

1. Пресеты: `CMAKE_PREFIX_PATH` для `fixed_solvers`; `CMakeLists.txt` — линковка Tasks, без путей.
2. `pipe_calculator_t::solve_pp` (интеграция + fixed_solvers).
3. `solve_pp` насос / НС / локальное сопротивление.
4. Тесты.

## Критерии готовности

- Нет заглушки «не реализовано» в четырёх `solve_pp`.
- Труба: невязка + `fixed_solvers` внутри `pipe_oil.cpp`.
- Остальные: формулы внутри своих `calc*`.
- В тестах нет `EXPECT_THROW(calc.solve_pp(), …)` на «не реализовано»; PP-тесты проверяют значения расхода/давлений.
- `ctest` по PP-сценариям зелёный.
