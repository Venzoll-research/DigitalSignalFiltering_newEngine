import sys
import numpy as np
from PyQt5.QtWidgets import *
from PyQt5.QtCore import Qt
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import signal_processor_new_prob as sp

class SignalPlotWidget(QWidget):
    def __init__(self, signal=None, parent=None):
        super().__init__(parent)
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.ax = self.figure.add_subplot(111)
        layout = QVBoxLayout()
        layout.addWidget(self.canvas)
        self.setLayout(layout)
        if signal:
            self.update_plot(signal)
    
    def update_plot(self, signal):
        self.ax.clear()
        self.ax.plot(signal.get_values())
        self.ax.grid(True)
        self.canvas.draw()

class SignalGeneratorTab(QWidget):
    def __init__(self):
        super().__init__()
        self.signals = {}
        self.init_ui()
    
    def init_ui(self):
        layout = QVBoxLayout()
        
        self.signal_type = QComboBox()
        self.signal_type.addItems(["Синусоида", "Квадратный", "Треугольный"])
        
        self.params = QFormLayout()
        self.amplitude = QLineEdit("1.0")
        self.frequency = QLineEdit("4.0")
        self.period = QLineEdit("1.0")
        self.num_points = QLineEdit("1000")
        self.noise_level = QLineEdit("0.01")
        
        self.params.addRow("Амплитуда:", self.amplitude)
        self.params.addRow("Частота (Гц):", self.frequency)
        self.params.addRow("Период (с):", self.period)
        self.params.addRow("Количество точек:", self.num_points)
        self.params.addRow("Уровень шума:", self.noise_level)
        
        self.generate_btn = QPushButton("Сгенерировать")
        self.generate_btn.clicked.connect(self.generate_signal)
        
        self.signal_list = QListWidget()
        self.signal_list.itemDoubleClicked.connect(self.plot_signal)
        
        btn_layout = QHBoxLayout()
        self.delete_btn = QPushButton("Удалить")
        self.delete_btn.clicked.connect(self.delete_signal)
        self.plot_btn = QPushButton("График")
        self.plot_btn.clicked.connect(self.plot_signal)
        btn_layout.addWidget(self.delete_btn)
        btn_layout.addWidget(self.plot_btn)
        
        layout.addWidget(QLabel("Тип сигнала:"))
        layout.addWidget(self.signal_type)
        layout.addLayout(self.params)
        layout.addWidget(self.generate_btn)
        layout.addWidget(QLabel("Список сигналов:"))
        layout.addWidget(self.signal_list)
        layout.addLayout(btn_layout)
        
        self.setLayout(layout)
    
    def generate_signal(self):
        try:
            sig_type = self.signal_type.currentText()
            amp = float(self.amplitude.text())
            freq = float(self.frequency.text())
            period = float(self.period.text())
            n = int(self.num_points.text())
            noise = float(self.noise_level.text())
            
            t = np.linspace(0, period, n)
            
            if sig_type == "Синусоида":
                values = amp * np.sin(2 * np.pi * freq * t)
            elif sig_type == "Квадратный":
                square_signal = np.sin(2 * np.pi * freq * t)
                values = amp * np.where(square_signal >= 0, 1, -1)
            else:  
                phase = np.mod(t, (1/freq)) * freq  
                values = amp * (2 * np.abs(phase - 0.5) - 0.5) * 2  
            
            values += noise * np.random.normal(size=n)
            
            signal = sp.Signal(values.tolist())
            name = f"{sig_type}_{len(self.signals)+1}"
            self.signals[name] = signal
            self.signal_list.addItem(name)
            
        except Exception as e:
            QMessageBox.critical(self, "Ошибка", f"Некорректные параметры: {str(e)}")
    
    def delete_signal(self):
        selected = self.signal_list.currentItem()
        if selected:
            name = selected.text()
            del self.signals[name]
            self.signal_list.takeItem(self.signal_list.row(selected))
    
    def plot_signal(self):
        selected = self.signal_list.currentItem()
        if selected:
            name = selected.text()
            signal = self.signals[name]
            self.plot_window = SignalPlotWidget(signal)
            self.plot_window.setWindowTitle(name)
            self.plot_window.show()

class ProcessorTab(QWidget):
    def __init__(self, signal_tab):
        super().__init__()
        self.signal_tab = signal_tab
        self.processor = sp.SignalProcessor()  
        self.blocks = {}  
        self.connections = []
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()
        
        self.block_type = QComboBox()
        self.block_type.addItems(["FIR", "IIR", "Summator"])
        self.add_block_btn = QPushButton("Добавить блок")
        self.add_block_btn.clicked.connect(self.add_block)
        
        self.block_list = QListWidget()
        
        self.set_input_btn = QPushButton("Установить вход")
        self.set_input_btn.clicked.connect(self.set_input)
        self.output_block = QLineEdit()
        self.set_output_btn = QPushButton("Установить выход")
        self.set_output_btn.clicked.connect(self.set_output_block)
        
        self.process_btn = QPushButton("Обработать")
        self.process_btn.clicked.connect(self.process)

        self.manage_connections_btn = QPushButton("Управление соединениями")
        self.manage_connections_btn.clicked.connect(self.manage_connections)
        layout.addWidget(self.manage_connections_btn)

        layout.addWidget(QLabel("Тип блока:"))
        layout.addWidget(self.block_type)
        layout.addWidget(self.add_block_btn)
        layout.addWidget(QLabel("Список блоков:"))
        layout.addWidget(self.block_list)
        layout.addWidget(self.set_input_btn)
        layout.addWidget(QLabel("Выходной блок:"))
        layout.addWidget(self.output_block)
        layout.addWidget(self.set_output_btn)
        layout.addWidget(self.process_btn)
        
        self.setLayout(layout)

    def manage_connections(self):
        dialog = QDialog(self)
        dialog.setWindowTitle("Управление соединениями")
        layout = QVBoxLayout()

        # Выбор источника
        src_label = QLabel("Источник:")
        self.src_combo = QComboBox()
        self.src_combo.addItems(self.blocks.keys())

        # Выбор приемника
        dest_label = QLabel("Приемник:")
        self.dest_combo = QComboBox()
        self.dest_combo.addItems(self.blocks.keys())

        # Выбор порта
        port_label = QLabel("Порт приемника:")
        self.port_spin = QSpinBox()
        self.port_spin.setRange(0, 1)

        # Кнопки
        btn_add = QPushButton("Добавить соединение")
        btn_add.clicked.connect(lambda: self.add_connection(
            self.src_combo.currentText(),
            self.dest_combo.currentText(),
            self.port_spin.value()
        ))
        btn_remove = QPushButton("Удалить соединение")
        btn_remove.clicked.connect(self.remove_connection)

        # Список соединений
        self.connections_list = QListWidget()

        # Компоновка
        layout.addWidget(src_label)
        layout.addWidget(self.src_combo)
        layout.addWidget(dest_label)
        layout.addWidget(self.dest_combo)
        layout.addWidget(port_label)
        layout.addWidget(self.port_spin)
        layout.addWidget(btn_add)
        layout.addWidget(btn_remove)
        layout.addWidget(self.connections_list)

        dialog.setLayout(layout)
        dialog.exec_()
    
    
    def add_connection(self, source, dest, port):
        try:
            if source not in self.blocks or dest not in self.blocks:
                raise ValueError("Несуществующие блоки")

            self.processor.connect(source, dest, port)
            
            connection_str = f"{source} → {dest} (порт {port})"
            self.connections.append((source, dest, port))
            self.connections_list.addItem(connection_str)

            QMessageBox.information(self, "Успех", "Соединение добавлено")

        except Exception as e:
            QMessageBox.critical(self, "Ошибка", f"Ошибка соединения: {str(e)}")

    def remove_connection(self):
        try:
            selected = self.connections_list.currentRow()
            if selected >= 0:
                source, dest, port = self.connections[selected]

                self.connections.pop(selected)
                self.connections_list.takeItem(selected)

        except Exception as e:
            QMessageBox.critical(self, "Ошибка", f"Ошибка удаления: {str(e)}")
    
    def add_block(self):
        try:
            block_type = self.block_type.currentText()
            name, ok = QInputDialog.getText(self, "Имя блока", "Введите уникальное имя:")
            if not ok or not name:
                return

            if block_type == "FIR":
                coeffs, ok = QInputDialog.getText(self, "Коэффициенты FIR", "Введите коэффициенты через запятую:")
                if ok:
                    coeffs = [float(c.strip()) for c in coeffs.split(",")]
                    self.blocks[name] = sp.FIR(name, coeffs)
                    self.processor.add_block(name, self.blocks[name]) 

            elif block_type == "IIR":
                b_coeffs, ok = QInputDialog.getText(self, "Коэффициенты IIR (b)", "Введите через запятую:")
                a_coeffs, ok = QInputDialog.getText(self, "Коэффициенты IIR (a)", "Введите через запятую:")
                if ok:
                    b = [float(c.strip()) for c in b_coeffs.split(",")]
                    a = [float(c.strip()) for c in a_coeffs.split(",")]
                    self.blocks[name] = sp.IIR(name, b, a)
                    self.processor.add_block(name, self.blocks[name])

            elif block_type == "Summator":
                u, ok = QInputDialog.getDouble(self, "Коэффициент u", "Введите u:", 1.0)
                v, ok = QInputDialog.getDouble(self, "Коэффициент v", "Введите v:", 1.0)
                if ok:
                    self.blocks[name] = sp.Summator(name, u, v)
                    self.processor.add_block(name, self.blocks[name])

            self.block_list.addItem(name)

        except Exception as e:
            QMessageBox.critical(self, "Ошибка", f"Ошибка создания блока: {str(e)}")

    def set_input(self):
        try:
            block_name = self.block_list.currentItem().text()
            
            signal_names = list(self.signal_tab.signals.keys())
            signal_name, ok = QInputDialog.getItem(
                self, "Выбор сигнала", "Выберите вход:", signal_names, 0, False
            )
            
            if ok and signal_name:
                self.processor.set_input(block_name, self.signal_tab.signals[signal_name])
                QMessageBox.information(self, "Успех", f"Вход {signal_name} → {block_name}")

        except Exception as e:
            QMessageBox.critical(self, "Ошибка", f"Ошибка установки входа: {str(e)}")

    def set_output_block(self):
        try:
            block_name = self.block_list.currentItem().text()
            self.output_block.setText(block_name)
            self.processor.set_output_block(block_name)
        except:
            QMessageBox.critical(self, "Ошибка", "Выберите блок из списка!")

    def process(self):
        try:
            result = self.processor.process()
            
            self.plot_window = SignalPlotWidget(result)
            self.plot_window.setWindowTitle("Результат обработки")
            self.plot_window.show()

        except Exception as e:
            QMessageBox.critical(self, "Ошибка обработки", str(e))

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Signal Processor GUI")
        self.setGeometry(100, 100, 800, 600)
        
        self.tabs = QTabWidget()
        self.signal_tab = SignalGeneratorTab()
        self.processor_tab = ProcessorTab(self.signal_tab)
        
        self.tabs.addTab(self.signal_tab, "Сигналы")
        self.tabs.addTab(self.processor_tab, "Обработка")
        
        self.setCentralWidget(self.tabs)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())