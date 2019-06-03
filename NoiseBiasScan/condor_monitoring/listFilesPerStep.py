import sys

# possibility to filter on a run number
run_to_keep = 0
if len(sys.argv)>1:
  print 'RUN ', sys.argv[1]
  run_to_keep = int(sys.argv[1])


# read steps definition
steps={}
f = open('noisebias_20180618_200_to_300V.txt', 'r')
for line in f:
  words = line.split(" ")
  if len(words)>4 and str.isdigit(words[0]):
	#print words
	index = words[0]
	timestamp = words[1]
	run = int(words[2])
	evt = int(words[3])
	step = int(words[4].replace('\n',''))
	if run_to_keep!=0 and run!=run_to_keep: continue
	if step not in steps.keys():
	  steps[step]=[]
	steps[step].append(evt)
f.close()

print steps


# read events numbers found in files
# ordered as in file list
eventsInFile=[]
fileList=[]
f = open('FileListWithEventsAndNoise.txt', 'r')
for line in f:
  words = line.split(" ")
  if len(words)>14:
    filename = words[0]
    n_evt = int(words[2])
    run = int(words[4])
    evt_range = words[6]
    evt_range = evt_range[1:-1]
    pair = evt_range.split(':')
    evt_min = int(pair[0])
    evt_max = int(pair[1])
    if run_to_keep!=0 and run!=run_to_keep: continue
    eventsInFile.append([evt_min, evt_max])
    fileList.append(filename)
f.close()


stepList = steps.keys()
stepList.sort()
# for each step produce a file list
for step in stepList:

  f = open('Jun2018_VRRandom0_files_part2_step'+str(step)+'.txt', 'w')

  evt_min = steps[step][0]
  evt_max = steps[step][1]

  print 'For step', step, 'keeping files:'
  for i in range(0, len(eventsInFile)):
    # events in file
    file_evt_min = eventsInFile[i][0]
    file_evt_max = eventsInFile[i][1]
    if (file_evt_min>=evt_min and file_evt_min<=evt_max) or (file_evt_max>=evt_min and file_evt_max<=evt_max):
      print fileList[i]
      f.write(fileList[i].replace('/eos/cms', '')+'\n')
  f.close()
