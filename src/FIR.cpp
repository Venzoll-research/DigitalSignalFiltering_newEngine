#include "../include/FIR.h"

// Реализация конструктора
FIR::FIR(const std::string& name, const std::vector<double>& coefficients)
    : FilterBase(name), coefficients(coefficients), input_buffer(coefficients.size(), 0.0) {}

// Реализация функции для клонирования объекта
FIR* FIR::clone() const {
    return new FIR(*this);
}

// Реализация функции для применения фильтра к сигналу
Signal FIR::Filter(const Signal& inputSignal) {
    std::vector<double> inputValues = inputSignal.getValues();
    std::vector<double> outputValues(inputValues.size(), 0.0);

    for (size_t t = 0; t < inputValues.size(); ++t) {
        for (size_t i = 1; i < input_buffer.size(); ++i) {
            input_buffer[i - 1] = input_buffer[i];
        }
        input_buffer.back() = inputValues[t];

        double y_t = 0.0;
        for (size_t i = 0; i < coefficients.size(); ++i) {
            y_t += coefficients[i] * input_buffer[input_buffer.size() - i - 1];
        }
        outputValues[t] = y_t;
    }

    return Signal(outputValues);
}

// Реализация функции для применения фильтра к одному значению
double FIR::operator()(double x_t) {
    for (size_t i = 1; i < coefficients.size(); ++i) {
        input_buffer[i - 1] = input_buffer[i];
    }
    input_buffer.back() = x_t;

    double y_t = 0.0;
    for (size_t i = 0; i < coefficients.size(); ++i) {
        y_t += coefficients[i] * input_buffer[input_buffer.size() - i - 1];
    }
    return y_t;
}
