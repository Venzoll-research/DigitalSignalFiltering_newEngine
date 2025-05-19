cmake_minimum_required(VERSION 3.12)
project(signal_processor_new_prob)

set(Python_ROOT_DIR "C:/Users/37111/AppData/Local/Programs/Python/Python313")  # Путь к Python
set(pybind11_DIR "C:/Users/37111/AppData/Local/Programs/Python/Python313/Lib/site-packages/pybind11/share/cmake/pybind11")

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

set(SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/src/My_signal.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/FIR.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/IIR.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/Summator.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/SignalProcessor.cpp
${CMAKE_CURRENT_SOURCE_DIR}/pybind_wrapper.cpp  # Файл-обёртка для pybind11
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -fPIC -std=c++17 -shared")
pybind11_add_module(signal_processor_new_prob ${SOURCES})


target_include_directories( signal_processor_new_prob PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # Ваши .h файлы
    ${Python_INCLUDE_DIRS}         # Заголовки Python
    ${pybind11_INCLUDE_DIRS}       # Заголовки pybind11
)

target_link_libraries(signal_processor_new_prob PRIVATE
    pybind11::module      # pybind11
    Python::Python        # Python
)

 set_target_properties(signal_processor_new_prob PROPERTIES SUFFIX ".pyd")