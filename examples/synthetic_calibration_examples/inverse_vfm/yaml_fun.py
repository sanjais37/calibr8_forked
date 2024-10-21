#from ruamel.yaml import YAML
import os
import csv
import subprocess

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

# Read parameters from CSV and generate YAML files
input_csv = 'matData_initial.csv'
yaml_files = [] 

with open(input_csv, 'r') as csvfile:
    csvreader = csv.DictReader(csvfile)
    
    for i, row in enumerate(csvreader):
        # Extract parameters from CSV row
        E = float(row['E'])
        nu = float(row['nu'])
        Y = float(row['Y'])
        S = float(row['S'])
        D = float(row['D'])
        
        # Generate YAML content
        yaml_data = generate_yaml(E, nu, Y, S, D)
        
        # Save the YAML content to a file
        output_filename = f'generated_config_{i+1}.yaml'
        yaml_files.append(output_filename)
        with open(output_filename, 'w') as file:
            file.write(yaml_data)
            
        # Run the YAML file with the `primal` command
        command = ['time','inverse', output_filename]
        try:
            subprocess.run(command, check=True)
            print(f"Successfully ran: {output_filename}")
        except subprocess.CalledProcessError as e:
            print(f"Error running {output_filename}: {e}")
            
# Delete all generated YAML files
for yaml_file in yaml_files:
    try:
        os.remove(yaml_file)
        print(f"Deleted: {yaml_file}")
    except FileNotFoundError:
        print(f"File not found: {yaml_file}")
    except PermissionError:
        print(f"Permission denied: {yaml_file}")
    except Exception as e:
        print(f"Error deleting {yaml_file}: {e}")
