import pandas as pd
import matplotlib.pyplot as plt
import os

# Check if the files exist
files = ['miss_displacement_forward.csv', 'miss_displacement_vfm.csv', 'miss_displacement_FEMU.csv']
for filename in files:
    if not os.path.isfile(filename):
        print(f"File not found: {filename}")
        exit()

# Load the data from the CSV files
try:
    df1 = pd.read_csv('miss_displacement_forward.csv', delimiter=',')  # Adjust delimiter if necessary
    df2 = pd.read_csv('miss_displacement_vfm.csv', delimiter=',')
    df3 = pd.read_csv('miss_displacement_FEMU.csv', delimiter=',')
except Exception as e:
    print(f"Error reading files: {e}")
    exit()

# Debugging: Print column names and first few rows
print("File 1 Columns:", df1.columns)
print("File 1 Head:\n", df1.head())

# Strip whitespace from column names
df1.columns = df1.columns.str.strip()
df2.columns = df2.columns.str.strip()
df3.columns = df3.columns.str.strip()

# Extract Points_1 and u_Magnitude for each DataFrame
try:
    points_1 = df1['Points_1']
    u_magnitude_1 = df1['u_Magnitude']
    u_magnitude_2 = df2['u_Magnitude']
    u_magnitude_3 = df3['u_Magnitude']
except KeyError as e:
    print(f"Column not found: {e}")
    exit()

# Compute the differences in u_Magnitude
u_magnitude_diff_1_2 = u_magnitude_1 - u_magnitude_2
u_magnitude_diff_1_3 = u_magnitude_1 - u_magnitude_3

# Plotting differences
plt.figure(figsize=(10, 6))

# Plot the difference between file 1 and file 2
plt.plot(points_1, u_magnitude_diff_1_2, marker='o', linestyle='-', color='b', label='Difference (Error VFM)')

# Plot the difference between file 1 and file 3
plt.plot(points_1, u_magnitude_diff_1_3, marker='x', linestyle='-', color='r', label='Difference (Error FEMU)')

# Add labels and title for differences plot
#plt.title('Difference in u_Magnitude vs. Points_1')
plt.xlabel('Points_1')
plt.ylabel('Difference in u_Magnitude')
plt.legend()
plt.grid()
plt.show()

# Plotting Points_1 vs u_Magnitude for all three files
plt.figure(figsize=(10, 6))

# Plot for file 1
plt.plot(points_1, u_magnitude_1, marker='o', linestyle='-', color='b', label='Forward')

# Plot for file 2
plt.plot(df2['Points_1'], u_magnitude_2, marker='x', linestyle='-', color='g', label='VFM')

# Plot for file 3
plt.plot(df3['Points_1'], u_magnitude_3, marker='s', linestyle='-', color='r', label='FEMU')

# Add labels and title for Points_1 vs u_Magnitude plot
#plt.title('Points_1 vs. u_Magnitude for All Files')
plt.xlabel('Height (section at x = 0.42 mm)')
plt.ylabel('u_Magnitude')
plt.legend()
plt.grid()
plt.show()

