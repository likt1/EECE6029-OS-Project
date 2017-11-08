import random
import sys

# arg checking
if len(sys.argv) < 12:
    sys.exit('Error in form, must have: fileName numJobs minProcess# \n\
maxProcess# minATinc maxATinc minBT maxBT minPri maxPri \nconsistentProcBehavior:0 or 1 \
optionalConsistentBehaviorVariation')

# function defines
def getStr(min, max):
    return str(getNum(min, max))

def getNum(min, max):
    return random.randint(min, max)

# defaults
numJobs = 25            # Number of jobs to generate
pConsistent = False     # Bool to keep process behavior consistent
pJVar = 2               # The amount of variation for jobs when consistent

pNumRange = [0, 5]      # Range of process numbers to generate
aTimeIncRange = [0, 20] # Range of arrival time increments
bTimeRange = [1, 35]    # Range of burst times to generate
priRange = [0, 10]      # Range of priority to generate

# arg parsing
numJobs = int(sys.argv[2])
pConsistent = int(sys.argv[11]) > 0
if len(sys.argv) == 12:
    pJVar = int(sys.argv[12])

pNumRange = [int(sys.argv[3]), int(sys.argv[4])]
aTimeIncRange = [int(sys.argv[5]), int(sys.argv[6])]
bTimeRange = [int(sys.argv[7]), int(sys.argv[8])]
priRange = [int(sys.argv[9]), int(sys.argv[10])]

# arg double checking log
print('\nJobs to create: ' + str(numJobs));
print('Process # range: ' + str(pNumRange));
print('Arrival Time Increment range: ' + str(aTimeIncRange));
print('Burst Time Range: ' + str(bTimeRange));
print('Priority range: ' + str(priRange));
print('Consistent Process Behavior: ' + str(pConsistent) +
      ' with Variance: ' + str(pJVar));

# init
time = 0;
random.seed()
pBehavior = {}

# init consistency dict for processes
if pConsistent:
    i = pNumRange[1]
    while i > -1:
        pBehavior[i] = getNum(bTimeRange[0], bTimeRange[1])
        i -= 1

# write to file
print('Opening: ' + sys.argv[1] + '.csv')
with open(sys.argv[1] + '.csv', 'w') as f:
    f.write('/process,at,bt,priority\n')
        
    while numJobs > 0:
        procNum = getNum(pNumRange[0], pNumRange[1])
        jobBurst = getNum(bTimeRange[0], bTimeRange[1])

        if pConsistent:
            jobBurst = pBehavior[procNum] + getNum(-pJVar, pJVar)

        if jobBurst < 1:
            jobBurst = 1
        
        f.write(str(procNum) + ',' + str(time) + ',' + str(jobBurst))
        f.write(',' + getStr(priRange[0], priRange[1]) + '\n')

        time += random.randint(aTimeIncRange[0], aTimeIncRange[1])
        numJobs -= 1
        
f.closed
