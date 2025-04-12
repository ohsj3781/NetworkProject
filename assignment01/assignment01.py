import matplotlib.pyplot as plt

#Load the data from three data fiels manually
with open('./flow1.dat','r') as file:
    data1=file.readlines()
flow1_time=[]
flow1_value=[]
for line in data1:
    parts=line.split()
    flow1_time.append(float(parts[0]))
    flow1_value.append(float(parts[1]))

with open('./flow2.dat','r') as file:
    data2=file.readlines()
flow2_time=[]
flow2_value=[]
for line in data2:
    parts=line.split()
    flow2_time.append(float(parts[0]))
    flow2_value.append(float(parts[1]))

with open('./flowBack.dat','r') as file:
    data3=file.readlines()
flowBack_time=[]
flowBack_value=[]
for line in data3:
    parts=line.split()
    flowBack_time.append(float(parts[0]))
    flowBack_value.append(float(parts[1]))

plt.figure(figsize=(20,10))
plt.plot(flow1_time, flow1_value, label='Flow1', marker='o', linestyle='-')
plt.plot(flow2_time, flow2_value, label='Flow2', marker='o', linestyle='-')
plt.plot(flowBack_time,flowBack_value, label='BackgroupndFlow', marker='o', linestyle='-')

plt.title('2020314916')
plt.xlabel('Time')
plt.ylabel('Value')

plt.legend()
output_file = './assignment01.png'
plt.savefig(output_file)
