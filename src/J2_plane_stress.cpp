#include <Eigen/Dense>
#include "control.hpp"
#include "defines.hpp"
#include "fad.hpp"
#include "global_residual.hpp"
#include "J2_plane_stress.hpp"
#include "material_params.hpp"

namespace calibr8 {

static ParameterList get_valid_local_residual_params() {
  ParameterList p;
  p.set<std::string>("type", "J2_plane_stress");
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
J2_plane_stress<T>::J2_plane_stress(ParameterList const& inputs, int ndims) {

  this->m_params_list = inputs;
  this->m_params_list.validateParameters(get_valid_local_residual_params(), 0);

  int const num_residuals = 5;

  this->m_num_residuals = num_residuals;
  this->m_num_eqs.resize(num_residuals);
  this->m_var_types.resize(num_residuals);
  this->m_resid_names.resize(num_residuals);

  this->m_resid_names[0] = "zeta";
  this->m_var_types[0] = SYM_TENSOR;
  this->m_num_eqs[0] = get_num_eqs(SYM_TENSOR, ndims);

  this->m_resid_names[1] = "Ie";
  this->m_var_types[1] = SCALAR;
  this->m_num_eqs[1] = get_num_eqs(SCALAR, ndims);

  this->m_resid_names[2] = "alpha";
  this->m_var_types[2] = SCALAR;
  this->m_num_eqs[2] = get_num_eqs(SCALAR, ndims);

  this->m_resid_names[3] = "zeta_zz";
  this->m_var_types[3] = SCALAR;
  this->m_num_eqs[3] = get_num_eqs(SCALAR, ndims);

  this->m_resid_names[4] = "lambda_z";
  this->m_var_types[4] = SCALAR;
  this->m_num_eqs[4] = get_num_eqs(SCALAR, ndims);

  m_max_iters = inputs.get<int>("nonlinear max iters");
  m_abs_tol = inputs.get<double>("nonlinear absolute tol");
  m_rel_tol = inputs.get<double>("nonlinear relative tol");

}

template <typename T>
void J2_plane_stress<T>::init_params() {

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
J2_plane_stress<T>::~J2_plane_stress() {
}

template <typename T>
void J2_plane_stress<T>::init_variables_impl() {

  int const ndims = this->m_num_dims;
  int const zeta_idx = 0;
  int const Ie_idx = 1;
  int const alpha_idx = 2;
  int const zeta_zz_idx = 3;
  int const lambda_z_idx = 4;

  T const Ie = 1.0;
  T const alpha = 0.0;
  Tensor<T> const zeta = minitensor::zero<T>(ndims);
  T const zeta_zz = 0.0;
  T const lambda_z = 1.0;

  this->set_scalar_xi(Ie_idx, Ie);
  this->set_scalar_xi(alpha_idx, alpha);
  this->set_sym_tensor_xi(zeta_idx, zeta);
  this->set_scalar_xi(zeta_zz_idx, zeta_zz);
  this->set_scalar_xi(lambda_z_idx, lambda_z);

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

// TODO: change to plane stress
template <typename T>
void eval_be_bar_plane_stress(
    RCP<GlobalResidual<T>> global,
    Tensor<T> const& zeta_2D,
    T const& Ie,
    T const& zeta_zz,
    T const& lambda_z_prev,
    T const& lambda_z,
    T& J_2D,
    Tensor<T>& be_bar_2D,
    T& be_bar_zz) {
  Tensor<T> const I_2D = minitensor::eye<T>(2);
  Tensor<T> const I = minitensor::eye<T>(3);
  Tensor<T> const grad_u = global->grad_vector_x(0);
  Tensor<T> const grad_u_prev = global->grad_vector_x_prev(0);
  Tensor<T> const F_2D = grad_u + I_2D;
  J_2D = minitensor::det(F_2D);
  Tensor<T> const F_prev_2D = grad_u_prev + I_2D;
  Tensor<T> F_3D = insert_2D_tensor_into_3D(F_2D);
  Tensor<T> F_prev_3D = insert_2D_tensor_into_3D(F_prev_2D);
  F_3D(2, 2) = lambda_z;
  F_prev_3D(2, 2) = lambda_z_prev;
  /*
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << "F_3D(" << i << "," << j << ") = " << F_3D(i, j) << "\n";
    }
  }
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << "F_prev_3D(" << i << "," << j << ") = " << F_prev_3D(i, j) << "\n";
    }
  }
  */

  Tensor<T> const rF = F_3D * minitensor::inverse(F_prev_3D);
  T const det_rF = minitensor::det(rF);
  T const det_rF_13 = cbrt(det_rF);
  Tensor<T> const rF_bar = rF / det_rF_13;
  Tensor<T> const rF_barT = minitensor::transpose(rF_bar);
  Tensor<T> zeta_3D = insert_2D_tensor_into_3D(zeta_2D);
  zeta_3D(2, 2) = zeta_zz;
  Tensor<T> const be_bar_3D = rF_bar * (zeta_3D + Ie * I) * rF_barT;
  be_bar_2D = extract_2D_tensor_from_3D(be_bar_3D);
  be_bar_zz = be_bar_3D(2, 2);
}

template <typename T>
T norm_s_3D(Tensor<T> const& s_2D, T const& s_zz) {
  return sqrt(s_2D(0, 0) * s_2D(0, 0) + s_2D(1, 1) * s_2D(1, 1)
      + 2. * s_2D(0, 1) * s_2D(0, 1) + s_zz * s_zz);
}

template <typename T>
T det_be_bar_3D(Tensor<T> const& zeta_2D, T const& zeta_zz, T const& Ie) {
  return ((zeta_2D(0, 0) + Ie) * (zeta_2D(1, 1) + Ie)
      - zeta_2D(0, 1) * zeta_2D(0, 1)) * (zeta_zz + Ie);
}

template <>
int J2_plane_stress<double>::solve_nonlinear(RCP<GlobalResidual<double>>) {
  return 0;
}

template <>
int J2_plane_stress<FADT>::solve_nonlinear(RCP<GlobalResidual<FADT>> global) {

  int path;

  // pick an initial guess for the local variables
  {
    Tensor<FADT> const zeta_old = this->sym_tensor_xi_prev(0);
    FADT const Ie_old = this->scalar_xi_prev(1);
    FADT const alpha_old = this->scalar_xi_prev(2);
    FADT const zeta_zz_old = this->scalar_xi_prev(3);
    FADT const lambda_z_old = this->scalar_xi_prev(4);

    int const ndims = global->num_dims();
    FADT J_2D;
    Tensor<FADT> be_bar_2D_trial;
    FADT be_bar_zz_trial;
    FADT const lambda_z = this->scalar_xi(4);
    eval_be_bar_plane_stress(global, zeta_old, Ie_old, zeta_zz_old,
        lambda_z_old, lambda_z,
        J_2D, be_bar_2D_trial, be_bar_zz_trial);
    FADT const Ie_trial = (minitensor::trace(be_bar_2D_trial)
        + be_bar_zz_trial) / 3.;
    Tensor<FADT> const I = minitensor::eye<FADT>(ndims);
    Tensor<FADT> const zeta_trial = be_bar_2D_trial - Ie_trial * I;
    FADT const zeta_zz_trial = be_bar_zz_trial - Ie_trial;
    FADT const alpha_trial = alpha_old;
    double const E = val(this->m_params[0]);
    double const nu = val(this->m_params[1]);
    double const mu = compute_mu(E, nu);
    double const kappa = compute_kappa(E, nu);
    double const mat_factor = kappa / (2. * mu);
    this->set_sym_tensor_xi(0, zeta_trial);
    this->set_scalar_xi(1, Ie_trial);
    this->set_scalar_xi(2, alpha_trial);
    this->set_scalar_xi(3, zeta_zz_trial);
    path = ELASTIC;
  }

  // newton iteration until convergence

  int iter = 1;
  double R_norm_0 = 1.;
  bool converged = false;

  while ((iter <= m_max_iters) && (!converged)) {

    path = this->evaluate(global);

    double const R_norm = this->norm_residual();
    if (iter == 1) R_norm_0 = R_norm;
    double const R_norm_rel = R_norm / R_norm_0;
    if ((R_norm_rel < m_rel_tol) || (R_norm < m_abs_tol)) {
      converged = true;
      break;
    }

    EMatrix const J = this->eigen_jacobian(this->m_num_dofs);
    EVector const R = this->eigen_residual();
    EVector const dxi = J.fullPivLu().solve(-R);

    if (iter > 100) {
      print("\niter %d: R_norm = %e", iter, R_norm);
      std::cout << "  R = " << R << "\n";
      std::cout << "  J = " << J << "\n";
      std::cout << "  dxi = " << dxi << "\n";
      std::cout << "  det(J) = " << J.determinant() << "\n";
    }

    this->add_to_sym_tensor_xi(0, dxi);
    this->add_to_scalar_xi(1, dxi);
    this->add_to_scalar_xi(2, dxi);
    this->add_to_scalar_xi(3, dxi);
    this->add_to_scalar_xi(4, dxi);

# if 1
    {
      // backtracking line search parameters
      int const max_line_search_evals = 20;
      bool const do_print = true;
      double const beta = 1.0e-4;
      double const eta = 0.5;

      double const R_0 = R_norm;
      double const psi_0 = 0.5 * R_0 * R_0;
      double const psi_0_deriv = -2. * psi_0;

      this->evaluate(global, true, path);

      int j = 1;
      double alpha_prev = 1.;
      double alpha_j = 1.;
      double R_j = this->norm_residual();
      double psi_j = 0.5 * R_j * R_j;

      while (psi_j >= ((1. - 2. * beta * alpha_j) * psi_0)) {

        alpha_prev = alpha_j;
        alpha_j  = std::max(eta * alpha_j,
            -(std::pow(alpha_j, 2) * psi_0_deriv) /
             (2. * (psi_j - psi_0 - alpha_j * psi_0_deriv)));

        //if (do_print) {
        if (false) {
          print(" >> CE residual iter %d norm = %e, orig_norm = %e", j, R_j, R_0);
          print(" >> CE residual increase -- line search alpha_%d = %.2e",
              j, alpha_j);
        }

        if (j == max_line_search_evals) {
          break;
        }

        ++j;

        //double const alpha_diff = alpha_j - alpha_prev;
        //dxi *= alpha_diff;

        this->add_to_sym_tensor_xi(0, -alpha_prev * dxi);
        this->add_to_scalar_xi(1, -alpha_prev * dxi);
        this->add_to_scalar_xi(2, -alpha_prev * dxi);
        this->add_to_scalar_xi(3, -alpha_prev * dxi);
        this->add_to_scalar_xi(4, -alpha_prev * dxi);

        //if (do_print) {
        if (false) {
          path = this->evaluate(global, true, path);
          double const orig_norm = this->norm_residual();
          print(" >> orig_norm check = %e", orig_norm);
        }

        this->add_to_sym_tensor_xi(0, alpha_j * dxi);
        this->add_to_scalar_xi(1, alpha_j * dxi);
        this->add_to_scalar_xi(2, alpha_j * dxi);
        this->add_to_scalar_xi(3, alpha_j * dxi);
        this->add_to_scalar_xi(4, alpha_j * dxi);

        path = this->evaluate(global, true, path);

        R_j = this->norm_residual();
        psi_j = 0.5 * R_j * R_j;

      }
    }

#endif



    iter++;

  }

  // fail if convergence was not achieved
  if ((iter > m_max_iters) && (!converged)) {
    fail("J2_plane_stress:solve_nonlinear failed in %d iterations", m_max_iters);
  }

  return path;

}

template <typename T>
int J2_plane_stress<T>::evaluate(
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
  T const mu = compute_mu(E, nu);
  T const kappa  = compute_kappa(E, nu);

  Tensor<T> const zeta_old = this->sym_tensor_xi_prev(0);
  T const Ie_old = this->scalar_xi_prev(1);
  T const alpha_old = this->scalar_xi_prev(2);
  T const zeta_zz_old = this->scalar_xi_prev(3);
  T const lambda_z_old = this->scalar_xi_prev(4);

  Tensor<T> const zeta = this->sym_tensor_xi(0);
  T const Ie = this->scalar_xi(1);
  T const alpha = this->scalar_xi(2);
  T const zeta_zz = this->scalar_xi(3);
  T const lambda_z = this->scalar_xi(4);

  Tensor<T> const I = minitensor::eye<T>(ndims);
  Tensor<T> be_bar_trial_2D;
  T be_bar_trial_zz;
  T J_2D;
  eval_be_bar_plane_stress(global, zeta_old, Ie_old, zeta_zz_old,
      lambda_z_old, lambda_z, J_2D, be_bar_trial_2D, be_bar_trial_zz);
  T const Ie_trial = (minitensor::trace(be_bar_trial_2D)
      + be_bar_trial_zz) / 3.;
  Tensor<T> const zeta_trial = be_bar_trial_2D - Ie_trial * I;
  T const zeta_zz_trial = be_bar_trial_zz - Ie_trial;
  Tensor<T> const s_2D = mu * zeta;
  T const s_zz = mu * zeta_zz;
  T const s_mag = norm_s_3D(s_2D, s_zz);
  Tensor<T> const n_2D = s_2D / s_mag;
  T const n_zz = s_zz / s_mag;
  T const sigma_yield = Y + S * (1. - std::exp(-D * alpha));
  T const f = (s_mag - sqrt_23 * sigma_yield) / val(mu);

  Tensor<T> R_zeta;
  T R_Ie;
  T R_alpha;
  T R_zeta_zz;
  T R_lambda_z;

  T const mat_factor = kappa / (2. * mu);
  //R_lambda_z = std::pow(lambda_z, 2)
  //    - (1. + zeta_zz / mat_factor) / std::pow(J_2D, 2);

  R_lambda_z = lambda_z
      - std::sqrt((1. - zeta_zz / mat_factor) / std::pow(J_2D, 2));
  /*
  std::cout << "J_2D = " << J_2D << "\n";
  std::cout << "mat_factor = " << mat_factor << "\n";
  std::cout << "lambda_z**2 = " << std::pow(lambda_z, 2) << "\n";
  std::cout << "zeta_zz = " << zeta_zz << "\n";
  std::cout << "R_lambda_z = " << R_lambda_z << "\n";
  */

  if (!force_path) {
    // plastic step
    if (f > m_abs_tol || std::abs(f) < m_abs_tol) {
      T const dgam = sqrt_32 * (alpha - alpha_old);
      R_zeta = zeta - zeta_trial + 2. * dgam * Ie * n_2D;
      R_Ie = det_be_bar_3D(zeta, zeta_zz, Ie) - 1.;
      R_alpha = (s_mag - sqrt_23 * sigma_yield) / val(mu);
      R_zeta_zz = zeta_zz - zeta_zz_trial + 2. * dgam * Ie * n_zz;
      path = PLASTIC;
    }
    // elastic step
    else {
      R_zeta = zeta - zeta_trial;
      R_Ie = Ie - Ie_trial;
      R_alpha = alpha - alpha_old;
      R_zeta_zz = zeta_zz - zeta_zz_trial;
      path = ELASTIC;
    }
  }

  // force the path
  else {
    path = path_in;
    // plastic step
    if (path == PLASTIC) {
      T const dgam = sqrt_32 * (alpha - alpha_old);
      R_zeta = zeta - zeta_trial + 2. * dgam * Ie * n_2D;
      R_Ie = det_be_bar_3D(zeta, zeta_zz, Ie) - 1.;
      R_alpha = (s_mag - sqrt_23 * sigma_yield) / val(mu);
      R_zeta_zz = zeta_zz - zeta_zz_trial + 2. * dgam * Ie * n_zz;
    }
    // elastic step
    else {
      R_zeta = zeta - zeta_trial;
      R_Ie = Ie - Ie_trial;
      R_alpha = alpha - alpha_old;
      R_zeta_zz = zeta_zz - zeta_zz_trial;
    }
  }

  this->set_sym_tensor_R(0, R_zeta);
  this->set_scalar_R(1, R_Ie);
  this->set_scalar_R(2, R_alpha);
  this->set_scalar_R(3, R_zeta_zz);
  this->set_scalar_R(4, R_lambda_z);

  return path;

}

// returns actual cauchy
template <typename T>
Tensor<T> J2_plane_stress<T>::dev_cauchy(RCP<GlobalResidual<T>> global) {
  int const ndims = global->num_dims();
  T const E = this->m_params[0];
  T const nu = this->m_params[1];
  T const mu = compute_mu(E, nu);
  T const kappa = compute_kappa(E, nu);
  Tensor<T> const I = minitensor::eye<T>(ndims);
  Tensor<T> const grad_u = global->grad_vector_x(0);
  Tensor<T> const F = grad_u + I;
  T const lambda_z = this->scalar_xi(4);
  T const J = minitensor::det(F) * lambda_z;
  Tensor<T> const zeta = this->sym_tensor_xi(0);
  return mu * zeta / J + kappa / 2. * (J - 1. / J) * I;
}

// not used
template <typename T>
Tensor<T> J2_plane_stress<T>::cauchy(RCP<GlobalResidual<T>> global, T p) {
  int const ndims = global->num_dims();
  Tensor<T> const I = minitensor::eye<T>(ndims);
  Tensor<T> const dev_sigma = this->dev_cauchy(global);
  Tensor<T> const sigma = dev_sigma - p * I;
  return sigma;
}

template class J2_plane_stress<double>;
template class J2_plane_stress<FADT>;

}