import matplotlib.pyplot as plt


log_name = input("Enter log name : ")
data_names = input("Enter data names : ")
data_names = data_names.split()
datas = {}


# Load the data from two data files manually
for i in range(0, len(data_names)):
    data_name = './' + log_name + '-' + data_names[i] + '.data'
    with open(data_name, 'r') as file:
        data = file.readlines()
    flow_time = []
    flow_value = []
    for line in data:
        parts = line.split()
        flow_time.append(float(parts[0]))
        flow_value.append(float(parts[1]))
    datas[data_names[i]] = (flow_time, flow_value)

# Plot both flows
plt.figure(figsize=(100, 30))

for data_name in data_names:
    plt.plot(datas[data_name][0], datas[data_name][1], label=data_name, marker='o', linestyle='-')

# Adding title and labels
title = ''
for data_name in data_names:
    title += data_name + ' '
plt.title(title)
plt.xlabel('Time')
plt.ylabel('Value')

plt.legend() # Show legend
output_file = './' + log_name + '_plot.png' # Save the plot as a .png file
plt.savefig(output_file)