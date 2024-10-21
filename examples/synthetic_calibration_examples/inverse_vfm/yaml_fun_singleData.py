from ruamel.yaml import YAML

def generate_yaml(E, nu, Y, S, D):
    # Constructing the YAML structure as a formatted string
    yaml_content = f"""
inverse_notch2D_small_J2_plane_stress:
  problem:
    name: inverse_notch2D_Asym_small_J2_plane_stress

  discretization:
    geom file: '/home/sk63837/calibr8/source/calibr8/test/mesh/notch2D_Asym/notch2D_Asym.dmg'
    mesh file: '../forward/notch2D_Asym_small_J2_plane_stress_synthetic/'
    assoc file: '/home/sk63837/calibr8/source/calibr8/test/mesh/notch2D_Asym/notch2D_Asym.txt'
    num steps: 4
    step size: 1.

  residuals:
    global residual:
      type: 'mechanics_plane_stress'
      nonlinear max iters: 30
      nonlinear absolute tol: 1.e-12
      nonlinear relative tol: 1.e-12
      max line search evals: 50
      print convergence: false
    local residual:
      type: 'small_hill_plane_stress'
      nonlinear max iters: 500
      nonlinear absolute tol: 1.e-12
      nonlinear relative tol: 1.e-12
      materials:
        body:
          E: {E}
          nu: {nu}
          Y: {Y}
          S: {S}
          D: {D}
          R00: 1.
          R11: 1.
          R22: 1.
          R01: 1.

  dirichlet bcs:
    expression:
      bc 1: [0, 0, ymin, 0.0]
      bc 2: [0, 1, ymin, 0.0]
      bc 3: [0, 1, ymax, '0.01 * t']

  inverse:
    objective type: "FS_VFM"
    check gradient: false
    print vfm mismatch: false
    internal power scale factor: 1.
    objective scale factor: 1e2
    thickness: 1.
    load input file: "../forward/load.dat"
    gradient tolerance: 1e-8
    iteration limit: 300
    max line search evals: 50
    materials:
      body:
        E: [150.e3, 250.e3]
        nu: [0.22, 0.35]
        Y: [300., 370.]
        S: [920., 1050.]
        D: [2., 3.]

  virtual fields:
    w_x: 'y'
    w_y: 'y*(y+1.)/2.'
    """
    return yaml_content.strip() + '\n' 

# Example usage:
params = {'E': 200.e3, 'nu': 0.3, 'Y': 360., 'S': 920., 'D': 2.2}
yaml_data = generate_yaml(**params)

# Save the string to a file
output_filename = 'generated_config.yaml'
with open(output_filename, 'w') as file:
    file.write(yaml_data)
