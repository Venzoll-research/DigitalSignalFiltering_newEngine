#ifndef IIR_H
#define IIR_H

#include "FilterBase.h"

// Класс для IIR фильтра
class IIR : public FilterBase {
public:
    // Конструктор
    IIR(const std::string& name, const std::vector<double>& b_coefficients, const std::vector<double>& a_coefficients);

    // Переопределение функции для применения фильтра к сигналу
    Signal Filter(const Signal& inputSignal) override;
    
    // Переопределение функции для применения фильтра к одному значению
    double operator()(double x_t) override;

    // Переопределение функции для клонирования объекта
    IIR* clone() const override;

private:
    std::vector<double> b_coefficients;  // Коэффициенты b
    std::vector<double> a_coefficients;  // Коэффициенты a
    std::vector<double> input_buffer;    // Буфер входных значений
    std::vector<double> output_buffer;   // Буфер выходных значений
};

#endif  // IIR_H
