#include "LeptonInjector/distributions/primary/vertex/RangeFunction.h"

#include <typeindex>  // for type_index
#include <typeinfo>   // for type_info

namespace LI {
namespace distributions {

//---------------
// class RangeFunction
//---------------

RangeFunction::RangeFunction() {}

double RangeFunction::operator()(LI::dataclasses::ParticleType const & primary_type, double energy) const {
    return 0.0;
}

bool RangeFunction::operator==(RangeFunction const & distribution) const {
    if(this == &distribution)
        return true;
    else
        return this->equal(distribution);
}

bool RangeFunction::operator<(RangeFunction const & distribution) const {
    if(typeid(this) == typeid(&distribution))
        return this->less(distribution);
    else
        return std::type_index(typeid(this)) < std::type_index(typeid(&distribution));
}

} // namespace distributions
} // namespace LeptonInjector
