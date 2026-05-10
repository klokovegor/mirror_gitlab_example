# Установка

## Первичная настройка

### Windows x64
- Установите Visual Studio 17 2022 (Desktop development with C++).
- Установите MSYS2 в `C:/msys64` (скачивание: https://www.msys2.org/) и добавьте компоненты UCRT64:

```powershell
C:\msys64\usr\bin\bash -lc "pacman -Syu --noconfirm"
C:\msys64\usr\bin\bash -lc "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-clang mingw-w64-ucrt-x86_64-gdb mingw-w64-ucrt-x86_64-lldb mingw-w64-ucrt-x86_64-lldb-dap mingw-w64-ucrt-x86_64-ninja"
C:\msys64\usr\bin\bash -lc "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gtest"
```

- Установите vcpkg в `C:/vcpkg`:

```powershell
git clone https://github.com/microsoft/vcpkg C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg integrate install
```

### Linux
- Установите инструменты: `git`, `cmake`, `ninja`, `gcc`, `clang`, `gdb`, `lldb`.
- Установите vcpkg в `${HOME}/vcpkg`:

```bash
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
"$HOME/vcpkg/bootstrap-vcpkg.sh"
```

### macOS
- Установите Xcode Command Line Tools (`clang`, `lldb`, `git`).
- Установите инструменты: `cmake`, `ninja` (например, через Homebrew).
- Установите vcpkg в `${HOME}/vcpkg`:

```bash
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
"$HOME/vcpkg/bootstrap-vcpkg.sh"
```

## Поддерживаемые компиляторы

- Windows: MSVC, GCC, Clang
- Linux: GCC, Clang
- macOS: Clang

## Зависимости

Проект использует GTest.

Установка для Windows (x64, только MSVC):

```powershell
C:\vcpkg\vcpkg install gtest:x64-windows
```

Для Windows GCC/Clang зависимости берутся из MSYS2 UCRT64 (`C:/msys64/ucrt64`) через пресеты CMake.

Установка для Linux:

```bash
$HOME/vcpkg/vcpkg install gtest:x64-linux
```

Установка для macOS:

```bash
$HOME/vcpkg/vcpkg install gtest:x64-osx
```

## Структура установки

Каждый пресет устанавливает артефакты в отдельный путь по компилятору и конфигурации:
- Windows: `C:/install/klokov_egor7/<compiler>/<Debug|Release>`
- Linux/macOS: `$HOME/install/klokov_egor7/<compiler>/<Debug|Release>`

## Конфигурация / сборка / тесты

Используйте один из пресетов:
- `windows-msvc-debug`
- `windows-gcc-debug`
- `windows-clang-debug`
- `windows-msvc-release`
- `windows-gcc-release`
- `windows-clang-release`
- `linux-gcc-debug`
- `linux-clang-debug`
- `linux-gcc-release`
- `linux-clang-release`
- `macos-clang-debug`
- `macos-clang-release`

Примеры:

```powershell
cmake --preset windows-gcc-debug
cmake --build --preset windows-gcc-debug
ctest --preset windows-gcc-debug --output-on-failure
```

```bash
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug --output-on-failure
```

```powershell
cmake --preset windows-msvc-release
cmake --build --preset windows-msvc-release
cmake --install --preset windows-msvc-release
```

## Отладка

- VS Code: используйте конфигурации запуска из `.vscode/launch.json`.
- Для Clang настроены `lldb-dap` профили на Windows/Linux/macOS.

## Pretty-printers Eigen (папка install)

Все отладчики для Clang берут принтеры из общей папки установки:
- Windows: `C:/install/eigen-pretty-printers`
- Linux/macOS: `$HOME/install/eigen-pretty-printers`

Требуемая структура:
- `${EIGEN_PRINTERS_ROOT}/gdb/printers.py`
- `${EIGEN_PRINTERS_ROOT}/lldb/eigen_printers.py`
- `${EIGEN_PRINTERS_ROOT}/natvis/Eigen.natvis`
