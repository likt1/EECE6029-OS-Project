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

# Preparing data for plotting
data.fillna("Idle", inplace=True)
data["Length"] = data.End - data.Start

# Creating plot object
fig,ax = plt.subplots(figsize=(6,3))

# Cycling throughn data structure to create rectangles
labels = []
for i, process in enumerate(data.groupby("Process")):
    labels.append(process[0])
    for r in process[1].groupby("ID"):
        data = r[1][["Start", "Length"]]
        ax.broken_barh(data.values, (i-0.4,0.8))

# Plotting
ax.set_yticks(range(len(labels)))
ax.set_yticklabels(labels) 
ax.set_xlabel("Burst Units")
ax.set_ylabel("Process #")
plt.show()