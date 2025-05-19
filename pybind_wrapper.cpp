#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../include/My_signal.h"
#include "../include/FilterBase.h"
#include "../include/FIR.h"
#include "../include/IIR.h"
#include "../include/Summator.h"
#include "../include/SignalProcessor.h"

namespace py = pybind11;

// Обёртка для виртуального класса FilterBase
class PyFilterBase : public FilterBase {
public:
    using FilterBase::FilterBase;

    Signal Filter(const Signal& inputSignal) override {
        PYBIND11_OVERRIDE_PURE(Signal, FilterBase, Filter, inputSignal);
    }

    double operator()(double x_t) override {
        PYBIND11_OVERRIDE_PURE(double, FilterBase, operator(), x_t);
    }

    FilterBase* clone() const override {
        PYBIND11_OVERRIDE_PURE(FilterBase*, FilterBase, clone, );
    }
};

PYBIND11_MODULE(signal_processor_new_prob, m) {
    // ==================== Signal ====================
    py::class_<Signal>(m, "Signal")
        .def(py::init<>())
        .def(py::init<const std::vector<double>&>())
        .def("get_values", &Signal::getValues)
        .def("input", &Signal::input)
        .def("output", &Signal::output)
        .def("__add__", &Signal::operator+)
        .def("__mul__", &Signal::operator*)
        .def("__rmul__", [](double scalar, const Signal& s) { return scalar * s; });

    // ==================== FilterBase ====================
    py::class_<FilterBase, PyFilterBase, std::shared_ptr<FilterBase>>(m, "FilterBase")
        .def(py::init<const std::string&>())
        .def("get_name", &FilterBase::getName)
        .def("filter", &FilterBase::Filter)
        .def("__call__", &FilterBase::operator());

    // ==================== FIR ====================
    py::class_<FIR, FilterBase, std::shared_ptr<FIR>>(m, "FIR")
        .def(py::init<const std::string&, const std::vector<double>&>())
        .def("clone", &FIR::clone);

    // ==================== IIR ====================
    py::class_<IIR, FilterBase, std::shared_ptr<IIR>>(m, "IIR")
        .def(py::init<const std::string&, const std::vector<double>&, const std::vector<double>&>())
        .def("clone", &IIR::clone);

    // ==================== Summator ====================
    py::class_<Summator, FilterBase, std::shared_ptr<Summator>>(m, "Summator")
        .def(py::init<const std::string&, double, double>())
        .def("__call__", py::overload_cast<double, double>(&Summator::operator()))
        .def("__call__", py::overload_cast<const Signal&, const Signal&>(&Summator::operator()))
        .def("clone", &Summator::clone);

    // ==================== SignalProcessor ====================
    py::class_<SignalProcessor>(m, "SignalProcessor")
        .def(py::init<>())
        .def("add_block", [](SignalProcessor& sp, const std::string& name, std::shared_ptr<FilterBase> block) {
            sp.addBlock(name, std::unique_ptr<FilterBase>(block->clone()));
        }, py::keep_alive<1, 3>())
        .def("set_input", &SignalProcessor::setInput)
        .def("connect", &SignalProcessor::connect)
        .def("set_output_block", &SignalProcessor::setOutputBlock)
        .def("process", &SignalProcessor::process);
}