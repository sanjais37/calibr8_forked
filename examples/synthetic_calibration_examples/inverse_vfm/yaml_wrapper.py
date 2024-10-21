import yaml
import subprocess

# Load the YAML file
with open('vfm_notch2D_Asym_small_J2_plane_stress.yaml', 'r') as file:
    yaml_data = yaml.safe_load(file)

# Define datasets
datasets = [
    {'E': 200.e3, 'nu': 0.3, 'Y': 360., 'S': 920., 'D': 2.2},
    {'E': 220.e3, 'nu': 0.35, 'Y': 380., 'S': 940., 'D': 2.3},
    # Add additional datasets as needed
]

# Iterate through datasets
for i, data in enumerate(datasets):
    yaml_data['inverse_notch2D_small_J2_plane_stress']['residuals']['local residual']['materials']['body'].update(data)
    with open(f'vfm_notch2D_Asym_small_J2_plane_stress_{i}.yaml', 'w') as file:
        yaml.dump(yaml_data, file)
    
    # Run the simulation
    subprocess.run(['inverse', f'vfm_notch2D_Asym_small_J2_plane_stress_{i}.yaml'])

