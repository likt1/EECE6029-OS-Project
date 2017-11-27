import sys
import pandas as ps
import matplotlib.pyplot as plt

# Checking and importing CSV
try:
	data = ps.read_csv(sys.argv[1])
except IndexError:
	print("Please include file location as argument to program call\n(e.g. python gantt.py ../output/gantt_rr.csv)\n")
	quit()
except IOError:
	print("{} not found!\n".format(sys.argv[1]))
	quit()

tatByID = data.pivot(index="ID", columns="Process", values="Turn-Around")
wtByID  = data.pivot(index="ID", columns="Process", values="Waiting")



fig, axes = plt.subplots(nrows=2, ncols=2, figsize=(12, 9))

plt.subplot(2,2,1)
tatByID.boxplot()
plt.title("Turn-Around Time: Process # v. Time Units")

plt.subplot(2,2,2)
wtByID.boxplot()
plt.title("Waiting Time Statistics:  Process # v. Time Units")

legend=[]
for name, group in data.groupby('Process'):
    group.plot(x='Completion', y='Turn-Around', ax=axes[1,0], legend=False, title="Completion vs. Turn-Around")
    legend.append(name)

legend=[]
for name, group in data.groupby('Process'):
    group.plot(x='Completion', y='Waiting', ax=axes[1,1], legend=False, title="Completion vs. Waiting")
    legend.append(name)

plt.show()