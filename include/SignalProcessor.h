#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <unordered_set>
#include "FilterBase.h"
#include "Summator.h"

class SignalProcessor {
public:
    SignalProcessor();
    
    ~SignalProcessor() = default;

    // Копирующий конструктор
    SignalProcessor(const SignalProcessor& other);

    // Оператор присваивания
    SignalProcessor& operator=(const SignalProcessor& other);

    // Перемещающий конструктор
    SignalProcessor(SignalProcessor&& other) noexcept;

    // Перемещающий оператор присваивания
    SignalProcessor& operator=(SignalProcessor&& other) noexcept;

    // Добавление блоков обработки
    void addBlock(const std::string& name, std::unique_ptr<FilterBase> block);
    
    // Установка входного сигнала для блока
    void setInput(const std::string& blockName, const Signal& signal);
    
    // Соединение выходного порта одного блока с входным портом другого
    void connect(const std::string& sourceBlock, const std::string& destBlock, int destInputPort = 0);
    
    // Установка выходного блока
    void setOutputBlock(const std::string& outputBlockName);
    
    // Выполнение обработки
    Signal process();

private:
    struct Node {
        std::unique_ptr<FilterBase> block;
        std::map<int, std::string> inputConnections; // port -> sourceBlock
        Signal externalInput;
        Signal cachedOutput;
        bool isProcessing = false; // Для обнаружения циклов
    };

    std::map<std::string, Node> processingGraph;
    std::string outputBlockName;

    Signal processNode(const std::string& nodeName);
    void validateGraph() const;
    void clearCache();
    void checkForCycles() const;
};

#endif // SIGNALPROCESSOR_H