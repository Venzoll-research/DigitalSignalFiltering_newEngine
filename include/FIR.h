#ifndef FIR_H
#define FIR_H

#include "FilterBase.h"

// Класс для FIR фильтра
class FIR : public FilterBase {
public:
    // Конструктор
    FIR(const std::string& name, const std::vector<double>& coefficients);

    // Переопределение функции для применения фильтра к сигналу
    Signal Filter(const Signal& inputSignal) override;

    // Переопределение функции для применения фильтра к одному значению
    double operator()(double x_t) override;

    // Метод clone
    FIR* clone() const override;

private:
    std::vector<double> coefficients;  // Коэффициенты фильтра
    std::vector<double> input_buffer;  // Буфер входных значений
};

#endif  // FIR_H