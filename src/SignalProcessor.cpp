#include "SignalProcessor.h"
#include <queue>
#include <algorithm>

SignalProcessor::SignalProcessor() = default;

// Копирующий конструктор
SignalProcessor::SignalProcessor(const SignalProcessor& other) {
    outputBlockName = other.outputBlockName;
    for (const auto& [name, node] : other.processingGraph) {
        Node newNode;
        newNode.block.reset(node.block->clone());
        newNode.inputConnections = node.inputConnections;
        newNode.externalInput = node.externalInput;
        newNode.cachedOutput = node.cachedOutput;
        newNode.isProcessing = false; //сброс состояния
        processingGraph[name] = std::move(newNode);
    }
}

// Оператор присваивания
SignalProcessor& SignalProcessor::operator=(const SignalProcessor& other) {
    if (this != &other) {
        processingGraph.clear();
        outputBlockName = other.outputBlockName;
        for (const auto& [name, node] : other.processingGraph) {
            Node newNode;
            newNode.block.reset(node.block->clone());
            newNode.inputConnections = node.inputConnections;
            newNode.externalInput = node.externalInput;
            newNode.cachedOutput = node.cachedOutput;
            newNode.isProcessing = false; //сброс состояния
            processingGraph[name] = std::move(newNode);
        }
    }
    return *this;
}

// Перемещающий конструктор
SignalProcessor::SignalProcessor(SignalProcessor&& other) noexcept
    : processingGraph(std::move(other.processingGraph)),
      outputBlockName(std::move(other.outputBlockName)) {}

// Перемещающий оператор присваивания
SignalProcessor& SignalProcessor::operator=(SignalProcessor&& other) noexcept {
    if (this != &other) {
        processingGraph = std::move(other.processingGraph);
        outputBlockName = std::move(other.outputBlockName);
    }
    return *this;
}

void SignalProcessor::addBlock(const std::string& name, std::unique_ptr<FilterBase> block) {
    if (processingGraph.find(name) != processingGraph.end()) {
        throw std::runtime_error("Block " + name + " already exists");
    }
    processingGraph[name] = {std::move(block), {}, Signal(), Signal(), false};
}

void SignalProcessor::setInput(const std::string& blockName, const Signal& signal) {
    auto it = processingGraph.find(blockName);
    if (it == processingGraph.end()) {
        throw std::runtime_error("Block " + blockName + " not found");
    }
    it->second.externalInput = signal;
}

void SignalProcessor::connect(const std::string& sourceBlock, const std::string& destBlock, int destInputPort) {
    if (processingGraph.find(sourceBlock) == processingGraph.end() ||
        processingGraph.find(destBlock) == processingGraph.end()) {
        throw std::runtime_error("Invalid block name in connection");
    }
    processingGraph[destBlock].inputConnections[destInputPort] = sourceBlock;
}

void SignalProcessor::setOutputBlock(const std::string& outputBlockName) {
    if (processingGraph.find(outputBlockName) == processingGraph.end()) {
        throw std::runtime_error("Output block " + outputBlockName + " not found");
    }
    this->outputBlockName = outputBlockName;
}

Signal SignalProcessor::process() {
    clearCache();
    checkForCycles();
    validateGraph();
    return processNode(outputBlockName);
}

Signal SignalProcessor::processNode(const std::string& nodeName) {
    Node& node = processingGraph[nodeName];
    
    if (!node.cachedOutput.getValues().empty()) {
        return node.cachedOutput;
    }
    
    if (node.isProcessing) {
        throw std::runtime_error("Cycle detected involving block " + nodeName);
    }
    
    node.isProcessing = true;
    
    // Сбор входных сигналов
    std::vector<Signal> inputs;
    if (!node.externalInput.getValues().empty()) {
        inputs.push_back(node.externalInput);
    } else {
        // Сортировка портов для сохранения порядка
        std::map<int, std::string> orderedPorts(node.inputConnections.begin(), node.inputConnections.end());
        for (const auto& [port, source] : orderedPorts) {
            inputs.push_back(processNode(source));
        }
    }
    
    // Обработка в зависимости от типа блока
    if (auto* summator = dynamic_cast<Summator*>(node.block.get())) {
        if (inputs.size() < 2) {
            throw std::runtime_error("Summator " + nodeName + " requires two inputs");
        }
        node.cachedOutput = (*summator)(inputs[0], inputs[1]);
    } else {
        if (inputs.empty()) {
            throw std::runtime_error("No input for filter " + nodeName);
        }
        node.cachedOutput = node.block->Filter(inputs[0]);
    }
    
    node.isProcessing = false;
    return node.cachedOutput;
}

void SignalProcessor::validateGraph() const {
    if (outputBlockName.empty()) {
        throw std::runtime_error("Output block not specified");
    }
    
    for (const auto& [name, node] : processingGraph) {
        if (dynamic_cast<Summator*>(node.block.get())) {
            if (node.inputConnections.size() + (node.externalInput.getValues().empty() ? 0 : 1) < 2) {
                throw std::runtime_error("Summator " + name + " must have two inputs");
            }
        }
    }
}

void SignalProcessor::clearCache() {
    for (auto& [name, node] : processingGraph) {
        node.cachedOutput = Signal();
        node.isProcessing = false;
    }
}

void SignalProcessor::checkForCycles() const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;

    const auto& dfs = [&](const std::string& nodeName, auto&& dfs) -> void {
        if (recursionStack.count(nodeName)) {
            throw std::runtime_error("Cycle detected involving block " + nodeName);
        }
        if (visited.count(nodeName)) return;
        
        visited.insert(nodeName);
        recursionStack.insert(nodeName);
        
        const auto& node = processingGraph.at(nodeName);
        for (const auto& [port, source] : node.inputConnections) {
            dfs(source, dfs);
        }
        
        recursionStack.erase(nodeName);
    };

    for (const auto& [name, node] : processingGraph) {
        if (!visited.count(name)) {
            dfs(name, dfs);
        }
    }
}