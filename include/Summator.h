#ifndef SUMMATOR_H
#define SUMMATOR_H

#include "FilterBase.h"

// Класс для сумматора
class Summator : public FilterBase {
public:
    // Конструктор
    Summator(const std::string& name, double u, double v);

    // Переопределение функции для применения фильтра к сигналу
    Signal Filter(const Signal& inputSignal) override;
    double operator()(double x_t) override;
    // Переопределение функции для применения фильтра к двум значениям
    double operator()(double x1, double x2);
    // Переопределение функции для применения сумматора к двум сигналам
    Signal operator()(const Signal& signal1, const Signal& signal2);

    // Переопределение функции для клонирования объекта
    Summator* clone() const override;

private:
    double u;  // Коэффициент u
    double v;  // Коэффициент v
};

#endif  // SUMMATOR_H