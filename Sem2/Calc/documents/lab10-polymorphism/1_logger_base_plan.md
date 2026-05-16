# План: этап 1 — logger_base (полиморфизм)

## Цель

Эталон в `src/`, демонстрация **всех особенностей** из ТЗ (стр. 6–10) в тесте. Без `EXPECT_*`; заметки — у пользователя.

## `src/logger_base.h` (header-only)

Корректный код лекции: `virtual`, `override`, `file_logger`, `max_messenger_logger`, `inline log_error(logger_base&)`. Без `.cpp`.

## `test/test_logger_base.cpp`

Без assertions. Четыре сценария — антипримеры **только в тесте**.

### 1. PolymorphicViaReference

Вызовы `log_error` из заголовка для `file_logger` и `max_messenger_logger`.

- Ожидание: `[file]`, `[max]`

### 2. ObjectSlicingByValue

`log_error_by_value(logger_base logger)` в тесте + `file_logger`.

- Ожидание: `[base]` (срезка)

### 3. WithoutVirtual

`logger_base_static`, `file_logger_static`, `log_error_static` (как в лекции, ТЗ 120–154).

- Ожидание: `[base]` (нет virtual)

### 4. WithoutOverrideWrongSignature

`file_logger_wrong_sig` с `write_message(string, int)` **без** `override` + `log_error(bad)`.

- Ожидание: `[base]` (метод не переопределён)

Не включать вариант с `override` и неверной сигнатурой — не компилируется.

## Таблица stdout

| TEST | Префикс |
|------|---------|
| PolymorphicViaReference | `[file]`, `[max]` |
| ObjectSlicingByValue | `[base]` |
| WithoutVirtual | `[base]` |
| WithoutOverrideWrongSignature | `[base]` |

Комментарии `// ожидание: ...` у вызовов.

## CMake

`logger_base.h` → Tasks; `test_logger_base.cpp` → Tests.

## Проверка

`ctest -R LoggerBaseDemo --verbose`, сверка с таблицей.

## Вне scope

`logger_base.cpp`, заметки, гидравлика, абстрактный базовый класс.
