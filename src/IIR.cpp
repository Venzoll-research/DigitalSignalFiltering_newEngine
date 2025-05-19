#include "../include/IIR.h"

// Реализация конструктора
IIR::IIR(const std::string& name, const std::vector<double>& b_coefficients, const std::vector<double>& a_coefficients)
    : FilterBase(name), b_coefficients(b_coefficients), a_coefficients(a_coefficients), input_buffer(b_coefficients.size(), 0.0), output_buffer(a_coefficients.size(), 0.0) {}

// Реализация функции для клонирования объекта
IIR* IIR::clone() const {
    return new IIR(*this);
}

// Реализация функции для применения фильтра к сигналу
Signal IIR::Filter(const Signal& inputSignal) {
    std::vector<double> inputValues = inputSignal.getValues();
    std::vector<double> outputValues(inputValues.size(), 0.0);

    for (size_t t = 0; t < inputValues.size(); ++t) {
        for (size_t i = 1; i < input_buffer.size(); ++i) {
            input_buffer[i - 1] = input_buffer[i];
        }
        input_buffer.back() = inputValues[t];

        double y_t = 0.0;
        for (size_t i = 0; i < b_coefficients.size(); ++i) {
            y_t += b_coefficients[i] * input_buffer[input_buffer.size() - i - 1];
        }
        for (size_t j = 0; j < a_coefficients.size(); ++j) {
            y_t += a_coefficients[j] * output_buffer[output_buffer.size() - j - 1];
        }

        for (size_t j = 1; j < output_buffer.size(); ++j) {
            output_buffer[j - 1] = output_buffer[j];
        }
        output_buffer.back() = y_t;

        outputValues[t] = y_t;
    }

    return Signal(outputValues);
}

// Реализация функции для применения фильтра к одному значению
double IIR::operator()(double x_t) {
    for (size_t i = 1; i < input_buffer.size(); ++i) {
        input_buffer[i - 1] = input_buffer[i];
    }
    input_buffer.back() = x_t;

    double y_t = 0.0;
    for (size_t i = 0; i < b_coefficients.size(); ++i) {
        y_t += b_coefficients[i] * input_buffer[input_buffer.size() - i - 1];
    }
    for (size_t j = 0; j < a_coefficients.size(); ++j) {
        y_t += a_coefficients[j] * output_buffer[output_buffer.size() - j - 1];
    }

    for (size_t j = 1; j < output_buffer.size(); ++j) {
        output_buffer[j - 1] = output_buffer[j];
    }
    output_buffer.back() = y_t;

    return y_t;
}
