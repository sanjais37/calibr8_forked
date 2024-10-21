import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

# Load data from a CSV or TXT file
file_path = 'grad_comparison_VF.csv'  # Change as needed

try:
    if file_path.endswith('.csv'):
        data_vf = pd.read_csv(file_path, header=0).values  # Reads the CSV and considers the header
    else:
        data_vf = np.loadtxt(file_path, skiprows=1)  # Skips the first row (header) for TXT
except Exception as e:
    print(f"Error loading file: {e}")
    exit()

# Convert to float to ensure all data is numeric
try:
    data_vf = data_vf.astype(float)
except ValueError as e:
    print(f"Error converting data types: {e}")
    exit()

n = data_vf.shape[0]  # Get number of rows

# Extracting columns
x = data_vf[:, 0]  # Step Size
y1 = data_vf[:, 1] / 1e2  # Forward Sensitivity Gradients
y2 = data_vf[:, 2] / 1e2  # Adjoint Gradients


# Create log-log plot
#plt.figure(figsize=(10, 6))
#plt.loglog(x, y1, linestyle='-', marker='s', color='k', linewidth=1.2, markersize=14, label='Forward Sensitivity Gradients')
#plt.loglog(x, y2, linestyle='-', marker='^', color='k', linewidth=1.2, markersize=10, label='Adjoint Gradients')


# Create log-log plot
plt.figure(figsize=(10, 6))
plt.loglog(x, y1, linestyle='-', marker='s', markerfacecolor='none', markeredgecolor='k', color='k', linewidth=1.2, markersize=13, label='Forward Sensitivity Gradients')
plt.loglog(x, y2, linestyle='-', marker='^', markerfacecolor='none', markeredgecolor='k', color='k', linewidth=1.2, markersize=10, label='Adjoint Gradients')


# Set the axis limits
plt.xlim([1e-13, 10])
plt.ylim([1e-10, 10])

# Set equal aspect ratio
plt.gca().set_aspect('equal', adjustable='box')

# Customize axes
plt.gca().invert_xaxis()  # Reverse x-axis

# Set tick format for both x and y axes
plt.gca().xaxis.set_major_formatter(ScalarFormatter(useMathText=True))
#plt.gca().yaxis.set_major_formatter(ScalarFormatter(useMathText=True))

# Increase font size of x and y ticks
plt.xticks(fontsize=16)
plt.yticks(fontsize=16)

# Add labels and legend
plt.xlabel(r'Finite Difference Step Size ($\varepsilon_{FD}$)', fontsize=18, labelpad=10)
plt.ylabel(r'$E_{FD \, check}$', fontsize=18, labelpad=10)
#plt.legend(fontsize=12, frameon=False)
plt.legend(fontsize=14, frameon=True, framealpha=1, edgecolor='black')


# # Set custom x-ticks with LaTeX formatting
plt.xticks([1e-12, 1e-8, 1e-4, 1], [r'$10^{-12}$', r'$10^{-8}$', r'$10^{-4}$', r'$1$'])  # Custom labels for each tick

#plt.minorticks_on()  # Enable the minor ticks
plt.tick_params(axis='both', direction='in', length=6, pad=10)  # Adjust length as needed

# Add grid and box
plt.grid(False)
plt.box(on=True)

 

# Save the plot as a PDF file
plt.savefig('gradient_comparison.pdf', format='pdf', bbox_inches='tight')  # Adjust filename as needed

# Show the plot
plt.tight_layout()  # Adjust layout for better fit
plt.show()

