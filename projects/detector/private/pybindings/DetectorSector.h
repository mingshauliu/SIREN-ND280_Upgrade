#include <memory>
#include <sstream>
#include <iostream>

#include <pybind11/pybind11.h>

#include "../../public/LeptonInjector/detector/DetectorModel.h"

std::string to_str(LI::detector::DetectorSector const & sector) {
    std::stringstream ss;
    sector.Print(ss);
    return ss.str();
}

void register_DetectorSector(pybind11::module_ & m) {
    using namespace pybind11;
    using namespace LI::detector;

    class_<DetectorSector, std::shared_ptr<DetectorSector>>(m, "DetectorSector")
        .def(init<>())
        .def("__str__", &to_str)
        .def_readwrite("name",&DetectorSector::name)
        .def_readwrite("material_id",&DetectorSector::material_id)
        .def_readwrite("level", &DetectorSector::level)
        .def_readwrite("geo",&DetectorSector::geo)
        .def_readwrite("density",&DetectorSector::density);
}