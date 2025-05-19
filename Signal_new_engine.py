import signal_processor_new_prob as sp

# ==================== Создаем сигналы ====================
main_signal = sp.Signal([1.0, 2.0, 3.0, 4.0, 5.0])
noise_signal = sp.Signal([0.1, -0.1, 0.2, -0.2, 0.3])
control_signal = sp.Signal([0.5, 0.5, 0.5, 0.5, 0.5,0.5,0.5])

# ==================== Создаем фильтры ====================
fir_lowpass = sp.FIR("LowPass", [0.4, 0.3, 0.2, 0.1])
fir_highpass = sp.FIR("HighPass", [-0.2, 0.5, -0.2])
iir_filter = sp.IIR("IIR_Filter", [0.5, 0.3], [0.2])

simple_fir = sp.FIR("Simple_FIR", [0.5, 0.5])

# ==================== Создаем сумматоры ====================
sum_noise_reduction = sp.Summator("NoiseReducer", 1.0, -1.0)
sum_main_mix = sp.Summator("MainMixer", 0.7, 0.3)
sum_final = sp.Summator("FinalMix", 0.5, 0.5)

# ==================== Комплексный процессор ====================
complex_proc = sp.SignalProcessor()

# Добавляем все компоненты
complex_proc.add_block("LowPass", fir_lowpass)
complex_proc.add_block("HighPass", fir_highpass)
complex_proc.add_block("IIR_Filter", iir_filter)
complex_proc.add_block("NoiseReducer", sum_noise_reduction)
complex_proc.add_block("MainMixer", sum_main_mix)
complex_proc.add_block("FinalMix", sum_final)

# Устанавливаем входные сигналы
complex_proc.set_input("LowPass", control_signal)
complex_proc.set_input("HighPass", main_signal)
complex_proc.set_input("IIR_Filter", noise_signal)

# Соединяем компоненты
complex_proc.connect("LowPass", "MainMixer", 0)
complex_proc.connect("HighPass", "NoiseReducer", 0)
complex_proc.connect("IIR_Filter", "NoiseReducer", 1)
complex_proc.connect("NoiseReducer", "MainMixer", 1)
complex_proc.connect("MainMixer", "FinalMix", 0)
complex_proc.connect("NoiseReducer", "FinalMix", 1)  # Второй вход (добавлено)

# Указываем выход
complex_proc.set_output_block("FinalMix")

# Обработка
complex_result = complex_proc.process()
print("Комплексная обработка:", complex_result.get_values())

# ==================== Простой процессор ====================
simple_proc = sp.SignalProcessor()

# Минимальная конфигурация
simple_proc.add_block("Simple_FIR", simple_fir)
simple_proc.set_input("Simple_FIR", main_signal)
simple_proc.set_output_block("Simple_FIR")

# Обработка
simple_result = simple_proc.process()
print("Простая обработка:", simple_result.get_values())

# ==================== Дополнительные тесты ====================
# Тест операций с сигналами
s1 = sp.Signal([1.0, 2.0])
s2 = sp.Signal([3.0, 4.0])
print("Сложение сигналов:", (s1 + s2).get_values())
print("Умножение сигнала:", (s1 * 2.5).get_values())

# Тест фильтрации одного значения
test_fir = sp.FIR("TestFIR", [0.5, 0.5])
print("Фильтрация одного значения (тест):", test_fir(1.0))

# Тест сумматора
test_sum = sp.Summator("TestSum", 0.3, 0.7)
print("Сумматор (тест):", test_sum(1.5, 2.5))