import yaml
import subprocess

# File paths
data_file = 'data.txt'
yaml_template_file = 'notch2D_Asym_small_J2_plane_stress.yaml'
yaml_output_prefix = 'notch2D_Asym_small_J2_plane_stress_'

# Read data from the .txt file, skipping the first line (label)
data_sets = []
with open(data_file, 'r') as file:
    # Skip the header line
    next(file)
    for line in file:
        # Remove any extra spaces and trailing commas, then split by commas
        cleaned_line = line.strip().rstrip(',')
        if cleaned_line:  # Ensure the line is not empty
            values = cleaned_line.split(',')
            if len(values) == 5:
                try:
                    E, nu, Y, S, D = map(float, values)
                    data_sets.append({
                        'E': E,
                        'nu': nu,
                        'Y': Y,
                        'S': S,
                        'D': D
                    })
                except ValueError as e:
                    print(f"Error converting values to float: {e}")
            else:
                print(f"Invalid line (not 5 values): {line}")
        else:
            print(f"Empty or whitespace line skipped: {line}")

# Process each data set
for i, data in enumerate(data_sets):
    # Load the YAML template
    try:
        with open(yaml_template_file, 'r') as file:
            yaml_data = yaml.safe_load(file) or {}
    except FileNotFoundError:
        yaml_data = {}

    # Ensure the structure exists
    if 'residuals' not in yaml_data:
        yaml_data['residuals'] = {}
    if 'local residual' not in yaml_data.get('residuals', {}):
        yaml_data['residuals']['local residual'] = {'materials': {'body': {}}}
    if 'materials' not in yaml_data['residuals']['local residual']:
        yaml_data['residuals']['local residual']['materials'] = {}
    if 'body' not in yaml_data['residuals']['local residual']['materials']:
        yaml_data['residuals']['local residual']['materials']['body'] = {}

    # Update the material properties
    yaml_data['residuals']['local residual']['materials']['body'].update(data)

    # Write the updated data to a new YAML file
    yaml_output_file = f"{yaml_output_prefix}{i+1}.yaml"
    with open(yaml_output_file, 'w') as file:
        yaml.dump(yaml_data, file, default_flow_style=False)

    print(f"YAML file '{yaml_output_file}' has been updated.")

    # Run the command to process the YAML file
    try:
        subprocess.run(['primal', yaml_output_file], check=True)
        print(f"Command 'primal {yaml_output_file}' executed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error running 'primal {yaml_output_file}': {e}")

