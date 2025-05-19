#include "../include/Summator.h"

// Реализация конструктора
Summator::Summator(const std::string& name, double u, double v)
    : FilterBase(name), u(u), v(v) {}

// Реализация функции для применения фильтра к сигналу
Signal Summator::Filter(const Signal& inputSignal) {
    // В данном случае, метод Filter не имеет смысла для сумматора,
    // так как сумматор работает с двумя входными значениями.
    // Можно оставить его пустым или выбросить исключение.
    throw std::runtime_error("Filter method is not applicable for Summator");
}

// Реализация функции для клонирования объекта
Summator* Summator::clone() const {
    return new Summator(*this);
}

double Summator::operator()(double x_t) {
    // Этот оператор не имеет смысла для сумматора,
    // так как сумматор работает с двумя входными значениями.
    throw std::runtime_error("operator()(double) is not applicable for Summator");
}

Signal Summator::operator()(const Signal& signal1, const Signal& signal2) {
    return u * signal1 + v * signal2;
}

// Реализация функции для применения фильтра к одному значению
double Summator::operator()(double x1, double x2) {
    return u * x1 + v * x2;
}