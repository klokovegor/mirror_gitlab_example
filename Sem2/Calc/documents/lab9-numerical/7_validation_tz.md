Численное сравнение вашего расчетчика и эталонного расчетчика
Сравнение реализации бисекции и Ньютона с fixed_solvers.
Нужно выполнить численное исследование (research). С точки зрения кода его можно организовать в виде
теста.
Пример использования fixed_solvers для поиска корней уравнения см. в
fixed_solvers\testing\newton_solver.cpp.
Верификация состоит в нахождении корня уравнения собственным расчетчиком и расчетчиком из
fixed_solvers. Найденные корни должны приблизительно совпадать.
Проведите такую верификацию для всех уравнений, которые использовались в предыдущих задачах
лабораторной (линейное, квадратное, гиперболическое, гармоническое и другие добавленные вами
тестовые случаи).
В дальнейшем рекомендуется использовать реализации численных методов из fixed_solvers

Проект fixed_solvers находиться по пути D:\Coding\Projects\fixed_solvers.

Тебе предстоит помимо реализации ТЗ настроить CMakeLists.txt так, чтобы он находил эту заинстоленную библиотеку по пути C:\Program Files (x86)\fixed_solvers.

План реализации 7_validation_tz.md

Цель

Сравнить корни, найденные текущей реализацией в проекте, с результатами fixed_solvers для набора уравнений из предыдущих задач, и зафиксировать это как воспроизводимый тестовый сценарий.

Что меняем





Добавить тест(ы) в [d:/Coding/Projects/VSProjects/klokov.egor/Sem2/Calc/test/test_numerical_methods.cpp](d:/Coding/Projects/VSProjects/klokov.egor/Sem2/Calc/test/test_numerical_methods.cpp), которые:





запускают текущие методы (bisection, newton) для каждого целевого уравнения;



запускают эквивалентные решатели из fixed_solvers;



сравнивают найденные корни с заданным допуском.



При необходимости расширить вспомогательные функции/обертки в [d:/Coding/Projects/VSProjects/klokov.egor/Sem2/Calc/src/numerical_methods.h](d:/Coding/Projects/VSProjects/klokov.egor/Sem2/Calc/src/numerical_methods.h) и [d:/Coding/Projects/VSProjects/klokov.egor/Sem2/Calc/src/numerical_methods.cpp](d:/Coding/Projects/VSProjects/klokov.egor/Sem2/Calc/src/numerical_methods.cpp) минимально, только если это нужно для единообразного вызова тестовых сценариев.



Обновить CMake-конфигурацию проекта Calc (целевые CMakeLists.txt в ветке Sem2/Calc), чтобы находилась установленная библиотека fixed_solvers из C:/Program Files (x86)/fixed_solvers и корректно линковалась с тестовой целью.



Зафиксировать результат в плане выполнения в [d:/Coding/Projects/VSProjects/klokov.egor/Sem2/Calc/documents/lab9-numerical/7_validation_plan.md](d:/Coding/Projects/VSProjects/klokov.egor/Sem2/Calc/documents/lab9-numerical/7_validation_plan.md): какие уравнения покрыты, какой допуск использован, как запускать проверку.

Подход к валидации





Использовать уже существующие тестовые кейсы уравнений (линейное, квадратное, гиперболическое, гармоническое и добавленные пользовательские случаи).



Для каждого кейса сравнивать:





our_root (текущая реализация);



reference_root (fixed_solvers);



критерий: abs(our_root - reference_root) <= tolerance.



Подобрать единый разумный tolerance с учетом различий реализаций и численных погрешностей, чтобы тесты были устойчивыми и информативными.

Проверка готовности





Сборка проходит с подключенной fixed_solvers.



Тесты верификации запускаются локально и проходят.



При расхождении корней тест печатает диагностически полезное сообщение (метод, кейс, оба значения, абсолютная ошибка).



Документ 7_validation_plan.md отражает фактическую реализацию и шаги запуска.