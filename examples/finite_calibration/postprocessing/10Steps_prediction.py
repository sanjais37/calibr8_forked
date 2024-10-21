import pandas as pd

# Load the predicted data
predicted_data = pd.read_csv('matData_Predicted.csv', sep=',', header=0)

# Load the truth data
truth_data = pd.read_csv('matData_truth.csv', sep=',', header=0)

# Print the first few rows to inspect the data
print("Predicted Data Preview:")
print(predicted_data.head())

print("\nTruth Data Preview:")
print(truth_data.head())

# Check the data types
print("\nPredicted Data Types:")
print(predicted_data.dtypes)

print("\nTruth Data Types:")
print(truth_data.dtypes)

# Convert to numeric, coercing errors
predicted_data = predicted_data.apply(pd.to_numeric, errors='coerce')
truth_data = truth_data.apply(pd.to_numeric, errors='coerce')

# Calculate mean and standard deviation for predicted data
means_predicted = predicted_data.mean()
std_devs_predicted = predicted_data.std()

# Print the results
print("\nMean values for Predicted Data:")
print(means_predicted)
print("\nStandard Deviation values for Predicted Data:")
print(std_devs_predicted)

# Calculate mean values for truth data
means_truth = truth_data.mean()

# Compute mean percentage error
mean_percentage_error = ((means_predicted - means_truth) / means_truth) * 100

# Print the mean percentage error
print("\nMean Percentage Error:")
print(mean_percentage_error)


# Optional: Round percentage error to 2 decimal places
#percentage_error_rounded = percentage_error.round(2)

# Print rounded percentage error
#print("\nRounded Percentage Error:")
#print(percentage_error_rounded)

