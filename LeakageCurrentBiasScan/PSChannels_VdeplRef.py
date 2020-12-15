import csv
from ROOT import TCanvas, TH1F, gStyle, TLegend

# dictionnary to link PS channels and detids
cr = csv.reader(open("Data/PSUtoDETID_wADD.csv","rb"))
# PS_CHANNEL, DETIDS

PS_DETIDS = {}
for row in cr:
    PS_DETIDS[row[0]] = row[1]


# Get VdeplRef
subdet='TIB'
filename='/afs/cern.ch/work/j/jlagram/public/SiStripRadMonitoring/VdeplRef/depletion_'+subdet+'.txt';
# OBJECT_ID       OBJECT  TYPE_DESCRIPTION        SENSOR  NUMBER_IN_CONTAINER     V_DEPL_V        CENTER  DET_ID  DCUID   STATUS
cr2 = csv.reader(open(filename, 'r'), delimiter='\t')

DETIDS_VDEPL = {}
for row in cr2:
    #print row
    DETIDS_VDEPL[row[7]] = row[5]


h = TH1F('h', 'Vdepl diff per PS channel', 10, 0, 100)
hlayers = []
for l in range(6):
    hlayers.append(TH1F('hL'+str(l), 'Vdepl diff per PS channel', 10, 0, 100))

for channel in PS_DETIDS.keys():
    #print channel
    layer = int(channel.split('_')[1])
    #print subdet, 'L', layer
    modules = PS_DETIDS[channel].split(',')
    vdepl_list=[]
    ref=0
    max_diff = 0
    for mod in modules:
        if DETIDS_VDEPL.has_key(mod):
            #print mod , DETIDS_VDEPL[mod]
            vdepl_list.append(DETIDS_VDEPL[mod])
        vdepl_list.sort()
        nmod = len(vdepl_list)
        if nmod==1:
          ref=mod
        if nmod>1:
            diff = float(vdepl_list[nmod-1]) - float(vdepl_list[0])
            if diff==0: print vdepl_list[nmod-1], vdepl_list[0]
            if diff > max_diff: max_diff = diff
    if nmod: print ref, max_diff
    if nmod: 
        h.Fill(max_diff)
        hlayers[layer-1].Fill(max_diff)

c = TCanvas()
gStyle.SetOptStat(0)
leg = TLegend(0.75, 0.6, 0.85, 0.85)
leg.SetBorderSize(0)
#h.Draw()
colors = [1, 2, 4, 8, 6]
for l in range(4):
    hlayers[l].SetLineWidth(2)
    hlayers[l].SetLineColor(colors[l])
    if l==0: 
        hlayers[l].Draw()
        hlayers[l].GetXaxis().SetTitle('V')
    else: hlayers[l].Draw('same')
    leg.AddEntry(hlayers[l], 'L'+str(l+1))
leg.Draw()
c.Print('Vdepl_diff_TIB.pdf')
