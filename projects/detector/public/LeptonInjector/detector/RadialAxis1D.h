#pragma once
#ifndef LI_RadialAxis1D_H
#define LI_RadialAxis1D_H
#include <memory>
#include <string>
#include <exception>
#include <functional>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/base_class.hpp>

#include "LeptonInjector/math/Vector3D.h"

#include "LeptonInjector/detector/Axis1D.h"

namespace LI {
namespace detector {

class RadialAxis1D : public Axis1D {
friend cereal::access;
public:
    RadialAxis1D();
    RadialAxis1D(const math::Vector3D& fAxis, const math::Vector3D& fp0);
    RadialAxis1D(const math::Vector3D& fp0);
    ~RadialAxis1D() {};

    bool compare(const Axis1D& dens_distr) const override;

    Axis1D* clone() const override { return new RadialAxis1D(*this); };
    std::shared_ptr<const Axis1D> create() const override {
        return std::shared_ptr<const Axis1D>(new RadialAxis1D(*this));
    };

    double GetX(const math::Vector3D& xi) const override;
    double GetdX(const math::Vector3D& xi, const math::Vector3D& direction) const override;

    template<class Archive>
        void serialize(Archive & archive, std::uint32_t const version) {
            if(version == 0) {
                archive(cereal::virtual_base_class<Axis1D>(this));
            } else {
                throw std::runtime_error("RadialAxis1D only supports version <= 0");
            }
        };
};

} // namespace detector
} // namespace LI

CEREAL_CLASS_VERSION(LI::detector::RadialAxis1D, 0);
CEREAL_REGISTER_TYPE(LI::detector::RadialAxis1D);
CEREAL_REGISTER_POLYMORPHIC_RELATION(LI::detector::Axis1D, LI::detector::RadialAxis1D);

#endif // LI_RadialAxis1D_H
