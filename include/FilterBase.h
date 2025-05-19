#ifndef FILTERBASE_H
#define FILTERBASE_H

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "My_signal.h"

// Базовый класс для фильтров
class FilterBase {
public:
    // Конструктор
    FilterBase(const std::string& name) : name(name) {}

    // Виртуальный деструктор
    virtual ~FilterBase() = default;

    // Виртуальная функция для применения фильтра к сигналу
    virtual Signal Filter(const Signal& inputSignal) = 0;

    // Виртуальная функция для применения фильтра к одному значению
    virtual double operator()(double x_t) = 0;

    // Функция для получения имени фильтра
    std::string getName() const {
        return name;
    }
    
    // Виртуальная функция для клонирования объекта
    virtual FilterBase* clone() const = 0;

private:
    std::string name;  // Имя фильтра
};

#endif  // FILTERBASE_H