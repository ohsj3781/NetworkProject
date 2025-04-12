# import matplotlib.pyplot as plt

# #Load the data from two data fiels manually
# with open('./flow1.dat','r') as file:
#     data1=file.readlines()
# flow1_time=[]
# flow1_value=[]
# for line in data1:
#     parts=line.split()
#     flow1_time.append(float(parts[0]))
#     flow1_value.append(float(parts[1]))

# with open('./flow2.dat','r') as file:
#     data2=file.readlines()
# flow2_time=[]
# flow2_value=[]
# for line in data2:
#     parts=line.split()
#     flow2_time.append(float(parts[0]))
#     flow2_value.append(float(parts[1]))

# plt.figure(figsize=(20,10))
# plt.plot(flow1_time,flow1_value,label='Flow1',marker='o',linestyle='-')
# plt.plot(flow2_time,flow2_value,label='Flow2',marker='o',linestyle='-')

# plt.title('2020314916')
# plt.xlabel('Time')
# plt.ylabel('Value')

# plt.legend()
# output_file='./week5_star.png'
# plt.savefig(output_file)
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


#Load the data from two data fiels manually
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

with open('./flow3.dat','r') as file:
    data3=file.readlines()
flow3_time=[]
flow3_value=[]
for line in data3:
    parts=line.split()
    flow3_time.append(float(parts[0]))
    flow3_value.append(float(parts[1]))

plt.figure(figsize=(20,10))
plt.plot(flow1_time, flow1_value, label='Flow1', marker='o', linestyle='-')
plt.plot(flow2_time, flow2_value, label='Flow2', marker='o', linestyle='-')
plt.plot(flow3_time,flow3_value, label='Flow3', marker='o', linestyle='-')

plt.title('2020314916')
plt.xlabel('Time')
plt.ylabel('Value')

# 1) Get the current Axes
ax = plt.gca()

# 2) Set the major locator to multiples of 0.1
# ax.xaxis.set_major_locator(ticker.MultipleLocator(0.1))

plt.legend()
output_file = './assignment01.png'
plt.savefig(output_file)
