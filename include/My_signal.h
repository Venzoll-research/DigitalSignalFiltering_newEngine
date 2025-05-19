#ifndef MY_SIGNAL_H
#define MY_SIGNAL_H

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>  //для вычисления суммы квадратов отклонений от среднего значения
#include <vector>

class Signal {
public:
    // Конструктор по умолчанию
    Signal();

    // Конструктор с параметром
    Signal(const std::vector<double>& values);

    ~Signal();

    // Перегрузка оператора + для сложения сигналов
    Signal operator+(const Signal& other) const;

    // Перегрузка оператора * для умножения сигнала на действительное число
    Signal operator*(double scalar) const;

    friend Signal operator*(double scalar, const Signal& M);

    // Метод для конкатенации сигналов
    Signal concatenate(const Signal& other) const;

    void input(std::istream& is);

    // Метод для вывода сигнала
    void output(std::ostream& os) const;

    // Метод для получения значений сигнала
    const std::vector<double>& getValues() const;

    // метод для сравнения сигналов
    void compareSignals(const Signal& original, const Signal& filtered) const;

private:
    std::vector<double> values;
};

#endif  // SIGNAL_H