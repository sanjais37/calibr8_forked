import csv
import subprocess
import os

def generate_yaml(E, nu, Y, S, D):
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
      nonlinear max iters: 300
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

  quantity of interest:
    type: 'calibration'
    coordinate index: 1
    coordinate value: 1.0
    load input file: "../forward/load.dat"
    objective out file: "objective.dat"
    reaction force component: 1
    displacement weights: [1e8, 1e8]
    balance factor: 0.1e0

  inverse:
    objective type: "adjoint"
    check gradient: false
    iteration limit: 50
    gradient tolerance: 1e-8
    max line search evals: 50
    materials:
      body:
        E: [150.e3, 250.e3]
        nu: [0.22, 0.35]
        Y: [300., 370.]
        S: [920., 1050.]
        D: [2., 3.]

  linear algebra:
    Linear Solver Type: "Belos"
    Preconditioner Type: "Teko"
    Linear Solver Types:
      Belos:
        Solver Type: "Block GMRES"
        Solver Types:
          Block GMRES:
            Convergence Tolerance: 1.e-12
            Output Frequency: 10
            Output Style: 1
            Verbosity: 33
            Maximum Iterations: 200
            Block Size: 1
            Num Blocks: 200
            Flexibile Gmres: false
        VerboseObject:
          Output File: "none"
          Verbosity Level: "none"
    Preconditioner Types:
      Teko:
        Inverse Type: "BGS2x2"
        Write Block Operator: false
        Test Block Operator: false
        Inverse Factory Library:
          BGS2x2:
            Type: "Block Gauss-Seidel"
            Use Upper Triangle: false
            Inverse Type 1: "AMG2"
          AMG2:
            Type: "MueLu"
            number of equations: 2
            verbosity: "none"
            'problem: symmetric': false
          GS:
            Type: "Ifpack2"
            Overlap: 1
            Ifpack2 Settings:
              'relaxation: type': "Gauss-Seidel"
    """  # Ensure proper formatting

    return yaml_content.strip() + '\n'  # Ensure newline at end

# Read parameters from CSV and generate YAML files
input_csv = 'matData_initial.csv'

yaml_files = []  # List to keep track of generated YAML files

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
        command = ['time', 'inverse', output_filename]
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

