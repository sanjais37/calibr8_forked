#include <Eigen/Dense>
#include "control.hpp"
#include "defines.hpp"
#include "fad.hpp"
#include "global_residual.hpp"
#include "J2_hypo_plane_stress.hpp"
#include "material_params.hpp"

namespace calibr8 {

using minitensor::dev;
using minitensor::inverse;
using minitensor::trace;
using minitensor::transpose;

static ParameterList get_valid_local_residual_params() {
  ParameterList p;
  p.set<std::string>("type", "J2_hypo_plane_stress");
  p.set<int>("nonlinear max iters", 0);
  p.set<double>("nonlinear absolute tol", 0.);
  p.set<double>("nonlinear relative tol", 0.);
  p.sublist("materials");
  return p;
}

static ParameterList get_valid_material_params() {
  ParameterList p;
  p.set<double>("E", 0.);
  p.set<double>("nu", 0.);
  p.set<double>("Y", 0.);
  p.set<double>("S", 0.);
  p.set<double>("D", 0.);
  return p;
}

template <typename T>
J2HypoPlaneStress<T>::J2HypoPlaneStress(ParameterList const& inputs, int ndims) {

  this->m_params_list = inputs;
  this->m_params_list.validateParameters(get_valid_local_residual_params(), 0);

  int const num_residuals = 3;

  this->m_num_residuals = num_residuals;
  this->m_num_eqs.resize(num_residuals);
  this->m_var_types.resize(num_residuals);
  this->m_resid_names.resize(num_residuals);

  // unrotated Cauchy stress
  this->m_resid_names[0] = "TC";
  this->m_var_types[0] = SYM_TENSOR;
  this->m_num_eqs[0] = get_num_eqs(SYM_TENSOR, ndims);

  // isotropic hardening variable
  this->m_resid_names[1] = "alpha";
  this->m_var_types[1] = SCALAR;
  this->m_num_eqs[1] = get_num_eqs(SCALAR, ndims);

  // out-of-plane stretch
  this->m_resid_names[2] = "lambda_z";
  this->m_var_types[2] = SCALAR;
  this->m_num_eqs[2] = get_num_eqs(SCALAR, ndims);

  m_max_iters = inputs.get<int>("nonlinear max iters");
  m_abs_tol = inputs.get<double>("nonlinear absolute tol");
  m_rel_tol = inputs.get<double>("nonlinear relative tol");

}

template <typename T>
J2HypoPlaneStress<T>::~J2HypoPlaneStress() {
}

template <typename T>
void J2HypoPlaneStress<T>::init_params() {

  int const num_params = 5;
  this->m_params.resize(num_params);
  this->m_param_names.resize(num_params);

  this->m_param_names.resize(num_params);
  this->m_param_names[0] = "E";
  this->m_param_names[1] = "nu";
  this->m_param_names[2] = "Y";
  this->m_param_names[3] = "S";
  this->m_param_names[4] = "D";

  int const num_elem_sets = this->m_elem_set_names.size();
  resize(this->m_param_values, num_elem_sets, num_params);

  ParameterList& all_material_params =
      this->m_params_list.sublist("materials", true);

  for (int es = 0; es < num_elem_sets; ++es) {
    std::string const& elem_set_name = this->m_elem_set_names[es];
    ParameterList& material_params =
        all_material_params.sublist(elem_set_name, true);
    material_params.validateParameters(get_valid_material_params(), 0);
    this->m_param_values[es][0] = material_params.get<double>("E");
    this->m_param_values[es][1] = material_params.get<double>("nu");
    this->m_param_values[es][2] = material_params.get<double>("Y");
    this->m_param_values[es][3] = material_params.get<double>("S");
    this->m_param_values[es][4] = material_params.get<double>("D");
  }

  this->m_active_indices.resize(1);
  this->m_active_indices[0].resize(1);
  this->m_active_indices[0][0] = 0;
}

template <typename T>
void J2HypoPlaneStress<T>::init_variables_impl() {

  int const ndims = this->m_num_dims;
  int const TC_idx = 0;
  int const alpha_idx = 1;
  int const lambda_z_idx = 2;

  Tensor<T> const TC = minitensor::zero<T>(ndims);
  T const alpha = 0.;
  T const lambda_z = 1.;

  this->set_sym_tensor_xi(TC_idx, TC);
  this->set_scalar_xi(alpha_idx, alpha);
  this->set_scalar_xi(lambda_z_idx, lambda_z);

}

template <typename T>
Tensor<T> eval_d(RCP<GlobalResidual<T>> global) {
  int const ndims = global->num_dims();
  Tensor<T> const I = minitensor::eye<T>(ndims);
  Tensor<T> const grad_u = global->grad_vector_x(0);
  Tensor<T> const grad_u_prev = global->grad_vector_x_prev(0);
  Tensor<T> const F = grad_u + I;
  Tensor<T> const F_prev = grad_u_prev + I;
  Tensor<T> const Finv = inverse(F);
  Tensor<T> const R = minitensor::polar_rotation(F);
  Tensor<T> const L = (F - F_prev) * Finv;
  Tensor<T> const D = 0.5 * (L + transpose(L));
  Tensor<T> const d = transpose(R) * D * R;
  return d;
}

template <typename T>
Tensor<T> extract_2D_tensor_from_3D(Tensor<T> const& t_3D) {
  Tensor<T> t_2D = minitensor::zero<T>(2);
  for (int i = 0; i < 2 ; ++i) {
    for (int j = 0; j < 2; ++j) {
      t_2D(i, j) = t_3D(i, j);
    }
  }
  return t_2D;
}

template <typename T>
Tensor<T> insert_2D_tensor_into_3D(Tensor<T> const& t_2D) {
  Tensor<T> t_3D = minitensor::zero<T>(3);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      t_3D(i, j) = t_2D(i, j);
    }
  }
  return t_3D;
}

template <>
int J2HypoPlaneStress<double>::solve_nonlinear(RCP<GlobalResidual<double>>) {
  return 0;
}

template <>
int J2HypoPlaneStress<FADT>::solve_nonlinear(RCP<GlobalResidual<FADT>> global) {

  int path;

  // pick an initial guess for the local variables
  {
    double const E = val(this->m_params[0]);
    double const nu = val(this->m_params[1]);
    double const lambda = compute_lambda(E, nu);
    double const mu = compute_mu(E, nu);
    int const ndims = this->m_num_dims;
    Tensor<FADT> const I = minitensor::eye<FADT>(ndims);
    Tensor<FADT> const TC_old = this->sym_tensor_xi_prev(0);
    FADT const alpha_old = this->scalar_xi_prev(1);
    FADT const lambda_z_old = this->scalar_xi_prev(2);
    Tensor<FADT> const d = eval_d(global);
    FADT const d_zz = -lambda * (d(0, 0) + d(1, 1)) / (lambda +  2. * mu);
    Tensor<FADT> const TC = TC_old + lambda * (trace(d) + d_zz) * I + 2. * mu * d;
    FADT const alpha = alpha_old;
    FADT const lambda_z = lambda_z_old / (1. - d_zz);
    this->set_sym_tensor_xi(0, TC);
    this->set_scalar_xi(1, alpha);
    this->set_scalar_xi(2, lambda_z);
    path = ELASTIC;
  }

  // newton iteration until convergence

  int iter = 1;
  double R_norm_0 = 1.;
  bool converged = false;

  while ((iter <= m_max_iters) && (!converged)) {

    path = this->evaluate(global);

    double const R_norm = this->norm_residual();
    //print("C residual norm = %e", R_norm);
    if (iter == 1) R_norm_0 = R_norm;
    double const R_norm_rel = R_norm / R_norm_0;
    if ((R_norm_rel < m_rel_tol) || (R_norm < m_abs_tol)) {
      converged = true;
      break;
    }

    EMatrix const J = this->eigen_jacobian(this->m_num_dofs);
    EVector const R = this->eigen_residual();
    EVector const dxi = J.fullPivLu().solve(-R);

    this->add_to_sym_tensor_xi(0, dxi);
    this->add_to_scalar_xi(1, dxi);
    this->add_to_scalar_xi(2, dxi);

    iter++;

  }

  // fail if convergence was not achieved
  if ((iter > m_max_iters) && (!converged)) {
    fail("J2HypoPlaneStress:solve_nonlinear failed in %d iterations", m_max_iters);
  }

  return path;

}

template <typename T>
int J2HypoPlaneStress<T>::evaluate(
    RCP<GlobalResidual<T>> global,
    bool force_path,
    int path_in) {

  int path = ELASTIC;
  int const ndims = this->m_num_dims;
  double const sqrt_23 = std::sqrt(2./3.);
  double const sqrt_32 = std::sqrt(3./2.);

  T const E = this->m_params[0];
  T const nu = this->m_params[1];
  T const Y = this->m_params[2];
  T const S = this->m_params[3];
  T const D = this->m_params[4];
  T const lambda = compute_lambda(E, nu);
  T const mu = compute_mu(E, nu);

  Tensor<T> const TC_old = this->sym_tensor_xi_prev(0);
  T const alpha_old = this->scalar_xi_prev(1);
  T const lambda_z_old = this->scalar_xi_prev(2);

  Tensor<T> const TC = this->sym_tensor_xi(0);
  T const alpha = this->scalar_xi(1);
  T const lambda_z = this->scalar_xi(2);

  // TODO: see if the 3D projected approach is equivalent

  /*
  Tensor<T> const TC_3D = insert_2D_tensor_into_3D(TC);
  Tensor<T> const s_3D = dev(TC_3D);
  T const s_3D_mag = minitensor::norm(s_3D);
  T const sigma_yield = Y + S * (1. - std::exp(-D * alpha));
  T const f = s_3D_mag - sqrt_23 * sigma_yield;
  */

  //T const phi = std::sqrt(0.5 * (std::pow(TC(0, 0) - TC(1, 1), 2)
  //    + std::pow(TC(0, 0), 2) + std::pow(TC(1, 1), 2))
  //    + 3. * std::pow(TC(0, 1), 2));

  T const phi = std::sqrt(std::pow(TC(0, 0), 2) + std::pow(TC(1, 1), 2)
      - TC(0, 0) * TC(1, 1)
      + 3. / 2. * std::pow(TC(0, 1), 2)
      + 3. / 2. * std::pow(TC(1, 0), 2));
  T const sigma_yield = Y + S * (1. - std::exp(-D * alpha));
  T const f = phi - sigma_yield;

  Tensor<T> R_TC;
  T R_alpha;
  T R_lambda_z;

  Tensor<T> const I = minitensor::eye<T>(ndims);
  Tensor<T> const d = eval_d(global);
  T const d_zz = -lambda * (d(0, 0) + d(1, 1)) / (lambda +  2. * mu);
  R_TC = TC - TC_old - lambda * (trace(d) + d_zz) * I - 2. * mu * d;

  if (!force_path) {
    // plastic step
    if (f > m_abs_tol || std::abs(f) < m_abs_tol) {
      // TODO: update 3D approach
      /*
      T const dgam = sqrt_32 * (alpha - alpha_old);
      Tensor<T> const n_3D = s_3D / s_3D_mag;
      Tensor<T> const n_2D = extract_2D_tensor_from_3D(n_3D);
      R_TC += 2. * mu * dgam * n_2D;
      R_alpha = f;
      T const dp_zz = 2. * mu * dgam * n_3D(2, 2) / (lambda + 2. * mu);
      R_lambda_z = lambda_z - lambda_z_old / (1. - (d_zz + dp_zz));
      */
      T const dgam = alpha - alpha_old;
      T const dp_xx = dgam * (2. * TC(0, 0) - TC(1, 1)) / (2. * phi);
      T const dp_yy = dgam * (2. * TC(1, 1) - TC(0, 0)) / (2. * phi);
      T const dp_xy = dgam * 3. * TC(0, 1) / (2. * phi);
      T const dp_zz = -(dp_xx + dp_yy);
      // correction from return map
      T const corr_dp_zz = 2. * mu * dp_zz / (2. * mu + lambda);
      R_TC(0, 0) += 2. * mu * dp_xx - lambda * corr_dp_zz;
      R_TC(1, 1) += 2. * mu * dp_yy - lambda * corr_dp_zz;
      R_TC(0, 1) += 2. * mu * dp_xy;
      R_TC(1, 0) += 2. * mu * dp_xy;
      R_alpha = f;
      R_lambda_z = lambda_z - lambda_z_old / (1. - (d_zz + corr_dp_zz));
      path = PLASTIC;
    }
    // elastic step
    else {
      R_alpha = alpha - alpha_old;
      R_lambda_z = lambda_z - lambda_z_old / (1. - d_zz);
      path = ELASTIC;
    }
  }

  // force the path
  else {
    path = path_in;
    // plastic step
    if (path == PLASTIC) {
      T const dgam = alpha - alpha_old;
      T const dp_xx = dgam * (2. * TC(0, 0) - TC(1, 1)) / (2. * phi);
      T const dp_yy = dgam * (2. * TC(1, 1) - TC(0, 0)) / (2. * phi);
      T const dp_xy = dgam * 3. * TC(0, 1) / (2. * phi);
      T const dp_zz = -(dp_xx + dp_yy);
      // correction from return map
      T const corr_dp_zz = 2. * mu * dp_zz / (2. * mu + lambda);
      R_TC(0, 0) += 2. * mu * dp_xx - lambda * corr_dp_zz;
      R_TC(1, 1) += 2. * mu * dp_yy - lambda * corr_dp_zz;
      R_TC(0, 1) += 2. * mu * dp_xy;
      R_TC(1, 0) += 2. * mu * dp_xy;
      R_alpha = f;
      R_lambda_z = lambda_z - lambda_z_old / (1. - (d_zz + corr_dp_zz));
    }
    // elastic step
    else {
      R_alpha = alpha - alpha_old;
      R_lambda_z = lambda_z - lambda_z_old / (1. - d_zz);
    }
  }

  this->set_sym_tensor_R(0, R_TC);
  this->set_scalar_R(1, R_alpha);
  this->set_scalar_R(2, R_lambda_z);

  return path;

}

// returns rotated Cauchy instead of dev
template <typename T>
Tensor<T> J2HypoPlaneStress<T>::dev_cauchy(RCP<GlobalResidual<T>> global) {
  int const ndims = this->m_num_dims;
  Tensor<T> const I = minitensor::eye<T>(ndims);
  Tensor<T> const grad_u = global->grad_vector_x(0);
  Tensor<T> const F = grad_u + I;
  Tensor<T> const TC = this->sym_tensor_xi(0);
  Tensor<T> const R = minitensor::polar_rotation(F);
  Tensor<T> const RC = R * TC * transpose(R);
  return RC;
}

template <typename T>
Tensor<T> J2HypoPlaneStress<T>::cauchy(RCP<GlobalResidual<T>> global, T p) {
  int const ndims = this->m_num_dims;
  Tensor<T> const I = minitensor::eye<T>(ndims);
  Tensor<T> const RC = this->dev_cauchy(global);
  Tensor<T> const sigma = RC - trace(RC) / 3. * I - p * I;
  return sigma;
}

template class J2HypoPlaneStress<double>;
template class J2HypoPlaneStress<FADT>;

}