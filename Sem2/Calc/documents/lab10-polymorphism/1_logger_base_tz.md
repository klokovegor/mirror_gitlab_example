Перед реализацией гидравлических моделей нужно прогнать под отладчиком учебный пример из
лекции:
базовый класс logger_base с виртуальным методом;
минимум два наследника с разным поведением;
вызов через ссылку/указатель на базовый тип.
Проверить руками:
что полиморфный вызов действительно выбирает реализацию по реальному типу объекта;
что будет при передаче по значению (срезка объекта);
что будет, если убрать virtual ;
что будет, если убрать override .
Кратко зафиксировать выводы в отдельной заметке по этапу.

В данном ТЗ необходимо создать отдельный файл с тестом в котором не будет проверок. Будет чистый вызов, того файла, который должен быть в src и содержать реализацию полиморфиза. Названия для файлов logger_base.h logger_base.cpp Коды бери из тексат представленого ниже. Заметки как указано в задании не делай это моя задача.

Лекция: Полиморфизм и виртуальные
методы
Цель: показать, как ООП (инкапсуляция + полиморфизм) позволяет использовать
newton_solver и bisection_solver для разных уравнений (equation_t) без
изменения кода алгоритма.
1) Вступление
Проблема: один и тот же алгоритм решения, но много разных уравнений.
Идея: отделяем алгоритм от математической модели.
Ключевая идея: один и тот же код алгоритма работает с разными реализациями, не
зная о них ничего конкретного.
Зачем это нужно на практике? Рассмотрим на примере задачи: есть система
логирования. Иногда нужно писать в файл, иногда — отправлять сообщение в
мессенджер, иногда — в стандартный вывод. Очевидное решение — разветвление
через switch или if по типу логгера.
Что здесь не так: - При добавлении нового вида логгера нужно менять функцию
log_error, которая, казалось бы, уже готова. - Если таких функций много, менять
придётся каждую. - Условные конструкции разбросаны по всей кодовой базе и
дублируют друг друга.
Полиморфизм позволяет заменить все эти switch-конструкции единственным
вызовом виртуального метода.
Но у него есть принципиальный недостаток: при добавлении нового вида логгера
придётся менять уже существующий код алгоритма. Это нарушает принцип
открытости/закрытости (OCP): программные сущности должны быть открыты для
расширения, но закрыты для модификации.
enum class logger_type { file, max_messenger };
void log_error(logger_type type) {
if (type == logger_type::file) {
// писать в файл
} else if (type == logger_type::max_messenger) {
// отправить сообщение
}
// добавили новый тип? придём сюда и будем менять этот код
}
Полиморфизм решает эту проблему: новые реализации добавляются без изменения
существующего кода.
2) Минимальный пример virtual + архитектура
Пример в каком-то смысле продолжает полиморфизм std::ostream&. Можно
написать функцию, которая принимает ссылку на std::ostream&. При этом в
функцию можно передавать как std::ofstream, так и std::stringstream.
Рассмотрим, как это реализуется под капотом: как должны быть написаны ostream,
ofstream, stringstream.
Изложение будем вести на примере кода для логирования кода ошибок.
Мини-пример семантики виртуального метода:
База: logger_base с виртуальным write_message(const std::string& msg).
Два наследника: file_logger и max_messenger_logger, каждый по-разному
переопределяет write_message.
Функция log_error(logger_base& logger) вызывает
logger.write_message("disk is almost full").
При передаче file_logger/max_messenger_logger в log_error вызывается
нужная реализация во время выполнения.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
void print_status(std::ostream& out, const std::string& msg) {
out << "[status] " << msg << "\n";
}
int main() {
std::ofstream file("log.txt");
std::stringstream memory;
print_status(file, "written to file");
print_status(memory, "written to memory");
std::cout << memory.str() << "\n";
return 0;
}
#include <iostream>
#include <string>
class logger_base {
public:
virtual void write_message(const std::string& msg) {
std::cout << "[base] " << msg << std::endl;
}
virtual ~logger_base() = default;
Как это работает в задумке: - функция log_error принимает ссылку на базовый тип
logger_base&; - фактическая реализация write_message выбирается в runtime по
реальному типу объекта; - код log_error не меняется при добавлении новых типов
логгеров.
Как выстрелить себе в ногу и как это не делать
1. Отсутсвие virtual
если убрать virtual у logger_base::write_message, то при вызове через
logger_base& будет вызываться базовая реализация, и полиморфное поведение в
log_error пропадет.
};
class file_logger : public logger_base {
public:
void write_message(const std::string& msg) override {
std::cout << "[file] " << msg << std::endl;
}
};
class max_messenger_logger : public logger_base {
public:
void write_message(const std::string& msg) override {
std::cout << "[max] " << msg << std::endl;
}
};
void log_error(logger_base& logger) {
logger.write_message("disk is almost full");
}
int main() {
file_logger file_log;
max_messenger_logger max_log;
log_error(file_log);
log_error(max_log);
return 0;
}
// --- БЕЗ virtual ---
class logger_base_static {
public:
void write_message(const std::string& msg) { // не виртуальный
std::cout << "[base] " << msg << std::endl;
}
};
2. Отсутсвие override
если убрать override, код может остаться рабочим, но исчезает проверка
компилятора на точное переопределение; при ошибке в сигнатуре метод не
переопределится и полиморфизм сломается.
типичные «тихие» ошибки без override: опечатка в имени метода,
пропущенный const, другой тип параметра.
с override такие ошибки превращаются в ошибку компиляции (раннее
обнаружение проблемы вместо runtime-сюрприза).
это хороший пример fail fast на этапе компиляции: ошибку ловим до запуска
программы.
Без override легко сделать незаметную ошибку:
Компилятор промолчит: новый метод вполне допустим. Но полиморфизм сломан —
вызов через logger_base& пойдёт в базовую реализацию, а не в нашу.
class file_logger_static : public logger_base_static {
public:
void write_message(const std::string& msg) { // переопределяем?
нет — скрываем
std::cout << "[file] " << msg << std::endl;
}
};
void log_error_static(logger_base_static& logger) {
logger.write_message("disk is almost full");
}
int main() {
file_logger_static sl;
log_error_static(sl); // [base] disk is almost full ← вызвался
базовый!
}
class logger_base {
public:
virtual void write_message(const std::string& msg) {
std::cout << "[base] " << msg << std::endl;
}
virtual ~logger_base() = default;
};
class file_logger : public logger_base {
public:
// опечатка: лишний параметр — это уже ДРУГОЙ метод, не переопределение!
void write_message(const std::string& msg, int level) {
std::cout << "[file] " << msg << std::endl;
}
};
Спецификатор override говорит компилятору: «эта функция должна
переопределять виртуальный метод базового класса». Если это не так — ошибка
компиляции:
3. Срезка объекта (object slicing)
Если убрать ссылку и передавать logger_base по значению, произойдет object
slicing: потеряется реальный производный тип, и вызов пойдет в
logger_base::write_message, а не в переопределенный метод:
При копировании file_log в параметр logger копируется только базовая часть
объекта. Полиморфизм сломан, и компилятор не предупредит.
Абстрактные классы
Иногда базовый класс не может и не должен предоставлять реализацию метода по
умолчанию. Например, нет никакой «базовой» реализации write_message — у
каждого логгера она своя, и осмысленного «общего» поведения не существует.
В таких случаях метод объявляется чисто виртуальным:
Синтаксис = 0 означает: «метод не имеет реализации в этом классе, наследники
обязаны её предоставить».
Класс, содержащий хотя бы одну чисто виртуальную функцию, называется
абстрактным. Создать объект абстрактного класса нельзя:
class file_logger : public logger_base {
public:
void write_message(const std::string& msg, int level) override { //
ошибка компиляции!
std::cout << "[file] " << msg << std::endl;
}
};
void log_error(logger_base logger) { // передача по значению — ошибка!
logger.write_message("disk is almost full");
}
int main() {
file_logger file_log;
log_error(file_log); // копируется только часть logger_base,
write_message всегда базовая
}
class logger_base {
public:
virtual void write_message(const std::string& msg) = 0;
virtual ~logger_base() = default;
};
logger_base log; // ошибка компиляции: cannot instantiate abstract class
Это правильное поведение — «просто логгер» не имеет смысла без конкретной
реализации.
3) Предметный пример - реализация решателей
equation_t (базовый абстрактный класс):
виртуальный метод residuals(x) (остаток/невязка);
виртуальный метод derivative(x) для метода Ньютона.
newton_solver и bisection_solver (конкретные решатели):
работают через интерфейс equation_t, не зная конкретный тип уравнения.
Несколько наследников equation_t: линейное, квадратное, кубическое,
нелинейное и т.д.
#include <iostream>
#include <cmath>
constexpr double eps = 0.000001;
class equation_t {
public:
virtual double residuals(double x) const = 0;
virtual double derivative(double x) const = 0;
virtual ~equation_t() = default;
};
class quadratic_equation_t : public equation_t {
public:
// x^2 - 17 = 0
double residuals(double x) const override {
return x * x - 17.0;
}
double derivative(double x) const override {
return 2.0 * x;
}
};
class cubic_equation_t : public equation_t {
public:
// x^3 - x - 2 = 0
double residuals(double x) const override {
return x * x * x - x - 2.0;
}
double derivative(double x) const override {
return 3.0 * x * x - 1.0;
}
};
double newton_solver(const equation_t& equation, double x0) {
double x1 = x0 - equation.residuals(x0) / equation.derivative(x0);
На что обратить внимание при разборе:
newton_solver зависит только от контракта equation_t, а не от конкретного
уравнения;
в строках вызова residuals/derivative проявляется полиморфизм: выбирается
реализация quadratic_equation_t или cubic_equation_t;
при переходе на новое уравнение меняется только объект equation_t, а
алгоритм newton_solver остается тем же.
4) Контракт интерфейса equation_t и связь с
LSP/OCP/DIP
Общее определение: контракт - это формально или неформально
зафиксированное соглашение о том, что принимает компонент, что возвращает
и какие гарантии поведения дает при корректном использовании.
Контракт в этом примере:
equation_t задает обязательный интерфейс residuals(x) и derivative(x);
newton_solver и bisection_solver ожидают предсказуемое поведение этих
методов;
residuals(x) корректно вычисляется для допустимой области x;
derivative(x) согласована с residuals(x) и не ломает шаг метода Ньютона;
наследник не меняет смысл входов/выходов и не добавляет неожиданных
побочных эффектов (антипример: для одного и того же x метод
residuals(x) в разные моменты возвращает разные значения из-за скрытого
состояния объекта).
Типичные нарушения в этом контексте:
неожиданные исключения для допустимых x;
некорректные значения residuals/derivative;
скрытое изменение состояния, влияющее на сходимость (в том числе когда
residuals(x) недетерминирован для фиксированного x).
Связь с принципом подстановки Лисков (LSP):
общее определение: объекты подтипа должны быть взаимозаменяемы с
объектами базового типа без нарушения корректности программы;
while (std::fabs(x1 - x0) > eps) {
x0 = x1;
const double d = equation.derivative(x0);
x1 = x0 - equation.residuals(x0) / d;
}
return x1;
}
int main() {
quadratic_equation_t quadratic;
cubic_equation_t cubic;
std::cout << "quadratic root: " << newton_solver(quadratic, 4.0) << "\n";
std::cout << "cubic root: " << newton_solver(cubic, 1.5) << "\n";
return 0;
}
любой наследник equation_t можно подставить в код решателя без
изменения корректности;
если контракт нарушен, нарушается и LSP.
Связь с Open/Closed Principle (OCP):
общее определение: программные сущности должны быть открыты для
расширения, но закрыты для модификации;
контракт equation_t позволяет добавлять новые уравнения через
наследование;
код решателя не нужно менять при расширении набора уравнений.
Связь с Dependency Inversion Principle (DIP):
общее определение: высокоуровневые модули не должны зависеть от
низкоуровневых; оба должны зависеть от абстракций;
в этом примере высокоуровневые модули: newton_solver и
bisection_solver (алгоритмы поиска корня);
низкоуровневые модули: конкретные уравнения quadratic_equation_t,
cubic_equation_t и другие наследники equation_t;
решатели зависят от абстракции equation_t, а не от конкретных классов
уравнений;
замена реализаций уравнений не требует изменения кода решателей.
Подробнее про LSP принцип: Liskov substitution principle
5) Завершение
Вывод: виртуальные методы + контракт + полиморфизм = расширяемая
архитектура численных методов.
Домашнее задание: добавить новый наследник equation_t без модификации
newton_solver и bisection_solver.
*6) Механизм реализации полиморфизма в C++ -
таблица виртуальных функций (vtable)
Как компилятор реализует позднее связывание? Через механизм vtable (таблица
виртуальных функций).
Для каждого класса, имеющего виртуальные методы, компилятор создаёт таблицу
vtable — массив указателей на функции. Каждый виртуальный метод соответствует
одному слоту в этой таблице.
Каждый объект такого класса хранит скрытый указатель vptr, который указывает
на vtable своего класса. Этот указатель занимает место — обычно 8 байт на 64-
битной системе.
Схема в памяти:
объект file_logger:
[ vptr ] ──────────────────→ vtable file_logger:
[ file_ (std::ofstream) ] [ &file_logger::write_message ]
[ &file_logger::~file_logger ]
объект max_messenger_logger:
[ vptr ] ──────────────────→ vtable max_messenger_logger:
[ &max_messenger_logger::write_message ]
[
&max_messenger_logger::~max_messenger_logger ]
Когда вызывается logger.write_message(msg) через ссылку или указатель на
базовый класс: 1. Из объекта читается vptr. 2. По vptr находится vtable. 3. Из vtable
читается адрес нужной функции. 4. По этому адресу происходит вызов.
Это один дополнительный уровень косвенности (indirection) по сравнению с
обычным вызовом функции. На современных процессорах накладные расходы
минимальны, но они есть

План действий сохраняй в lab10-polymorphism\1_logger_base_plan.md