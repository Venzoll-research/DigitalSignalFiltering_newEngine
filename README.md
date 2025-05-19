# DigitalSignalFiltering_newEngine

# Signal Processor Framework

Проект представляет собой фреймворк для обработки дискретных сигналов с использованием фильтров (КИХ, БИХ) и сумматоров. Реализован на C++ с Python-биндингами через pybind11.

## Основные возможности
- **Сигналы**: сложение, умножение на скаляр, конкатенация.
- **Фильтры**:
  - **КИХ (FIR)**: линейная комбинация текущего и предыдущих входных значений.
  - **БИХ (IIR)**: линейная комбинация входных и выходных значений.
- **Сумматор**: комбинирует два сигнала с коэффициентами.
- **SignalProcessor**: 
  - Создание цепочек обработки из фильтров и сумматоров.
  - Соединение блоков через входные порты.
  - Вычисление выходного сигнала для заданной конфигурации.

# Python Usage

## Basic Example

```python
import signal_processor_new_prob as sp

# Create components
main_signal = sp.Signal([1.0, 2.0, 3.0])
fir = sp.FIR("LowPass", [0.5, 0.5])
summator = sp.Summator("Mixer", 0.7, 0.3)

# Configure pipeline
proc = sp.SignalProcessor()
proc.add_block("FIR", fir)
proc.add_block("Sum", summator)
proc.set_input("FIR", main_signal)
proc.connect("FIR", "Sum", 0)  # Sum's input 0 <- FIR
proc.set_input("Sum", sp.Signal([0.1, 0.2, 0.3]))  # Sum's input 1
proc.set_output_block("Sum")

# Process
result = proc.process()
print(f"Output: {result.get_values()}")
# Output: [0.7*0.5 + 0.3*0.1, 0.7*(1.0+0.5) + 0.3*0.2, ...]
```
## Advanced Pipeline
```python
# Create processing chain:
# INPUT -> [FIR1] -> [IIR1] -> [SUM]
#                   ↗
#         [NOISE] ─┘

proc = sp.SignalProcessor()

# Add blocks
proc.add_block("FIR1", sp.FIR("Cleaner", [0.4, 0.3, 0.2]))
proc.add_block("IIR1", sp.IIR("Smoother", [0.5], [0.2]))
proc.add_block("SUM", sp.Summator("NoiseCancel", 1.0, -1.0))

# Configure connections
proc.set_input("FIR1", main_signal)
proc.set_input("IIR1", noise_signal)
proc.connect("FIR1", "SUM", 0)
proc.connect("IIR1", "SUM", 1)
proc.set_output_block("SUM")

# Execute
output = proc.process()
```

Полный пример в файле Signal_new_engine.py
