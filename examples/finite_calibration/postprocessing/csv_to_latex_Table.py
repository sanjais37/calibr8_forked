import pandas as pd

# Load the CSV file
data = pd.read_csv('matData_initial_predicted.csv')  # Replace with your file name

# Print the DataFrame to inspect it
print(data)

# Create LaTeX table from the DataFrame
latex_table = data.to_latex(index=False, float_format="%.2f")   # Set index=False to exclude the DataFrame index

# Print the LaTeX table
print(latex_table)

# Optionally, you can write it to a .tex file
with open('output_table.tex', 'w') as f:
    f.write(latex_table)

