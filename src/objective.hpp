#pragma once

#include <ROL_Objective.hpp>
#include <ROL_StdVector.hpp>
#include "arrays.hpp"
#include "defines.hpp"

namespace calibr8 {

class Primal;
class State;

using V = ROL::Vector<double>;
using SV = ROL::StdVector<double>;

class Objective : public ROL::Objective<double> {

  public:

    Objective(RCP<ParameterList> params);
    ~Objective();

    Array1D<double> model_params() const;
    Array1D<double> extract_active_params(
        Array1D<double> const& model_params) const;
    Array1D<double> transform_params(Array1D<double> const& params,
        bool scale_to_canonical);

    double value(ROL::Vector<double> const& p, double&) = 0;

#if 0
    Array1D<double> scale_params(Array1D<double> const& p);
    Array1D<double> unscale_params(Array1D<double> const& p);
    Array1D<double> scale_gradient(Array1D<double> const& g);
    Array1D<double> get_scaled_ig() { return scaled_ig; }
#endif

  protected:

    void setup_opt_params(ParameterList const& inverse_params);
    void set_params(ROL::Vector<double> const& p);
    Array1D<double> transform_gradient(Array1D<double> const& gradient);

    RCP<ParameterList> m_params;
    RCP<State> m_state;
    RCP<Primal> m_primal;

    Array1D<size_t> m_active_indices;
    Array1D<double> m_lower_bounds;
    Array1D<double> m_upper_bounds;
    size_t m_num_opt_params;

    ROL::Ptr<Array1D<double> const> getVector(V const& vec);
    ROL::Ptr<Array1D<double>> getVector(V& vec);

};

}