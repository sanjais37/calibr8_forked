import numpy as np
import pandas as pd

def generate_data(n):
    # Define the bounds for each quantity
    bounds = {
        'E': [150.e3, 250.e3],
        'nu': [0.23, 0.35],
        'Y': [300., 370.],
        'S': [920., 1050.],
        'D': [2., 3.]
    }

    # Generate random data within the bounds
    data = {
        'E': np.random.uniform(bounds['E'][0], bounds['E'][1], n),
        'nu': np.random.uniform(bounds['nu'][0], bounds['nu'][1], n),
        'Y': np.random.uniform(bounds['Y'][0], bounds['Y'][1], n),
        'S': np.random.uniform(bounds['S'][0], bounds['S'][1], n),
        'D': np.random.uniform(bounds['D'][0], bounds['D'][1], n)
    }

    # Create a DataFrame
    df = pd.DataFrame(data)

    return df

def save_to_csv(df, filename):
    df.to_csv(filename, index=False)

# Parameters
n = 10  # Number of data sets
filename = 'matData_initial.csv'

# Generate data and save to CSV
df = generate_data(n)
save_to_csv(df, filename)

print(f"Data saved to {filename}")

