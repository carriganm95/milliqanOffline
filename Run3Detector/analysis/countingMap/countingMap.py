import math
from ROOT import *
import numpy as np
chain = TChain("t")
chain.Add("MilliQan_Run469_default_v25.root")

#all 318 event map
#all entries
#for index, entry in enumerate(chain):
entry = 0


#heat map building for NeHa----------------
nx = 6
ny = 21
#month =[]
rowName = ["front pannel","1st row(layer one)","2nd row(layer one)","3rd row(layer one)", "4th row(layer one)","top pannel (layer1&2)","1st row(layer two)","2nd row(layer two)","3rd row(layer two)", "4th row(layer two)","empty","1st row(layer 3)","2nd row(layer 3)","3rd row(layer 3)", "4th row(layer 3)","top pannel (layer3&4)","1st row(layer 4)","2nd row(layer 4)","3rd row(layer 4)", "4th row(layer 4)","back pannel"]
columnName = ["left pannel","1st column","2nd column","3rd column", "4th column", "right pannel"]
c1 = TCanvas("c1","demo bin labels",10,10,600,600)
c1.SetGrid()
c1.SetLeftMargin(0.30)
c1.SetRightMargin(0.30)
c1.SetBottomMargin(0.30)
h = TH2F("h","event 318 heatmap",nx,-1,nx-1,ny,0,ny)
#-----------------------


while entry < chain.GetEntries():

#while entry < 1200:    
    chain.GetEvent(entry)
    #chain.GetEvent(306318)
    if chain.event == 318:
    #if chain.event < 1002:
    #if nx < 7: # used for loop over all events
        columnlist = chain.column
        rowlist = chain.row
        layerList = chain.layer

        chanList = chain.chan
        nPEList = chain.nPE
        durationList = chain.duration
        heightList = chain.height
        #print(chain.chan)
        areaList = chain.area
        
        #j = 1




        for chainIndex, chan in enumerate(chanList):
            if nPEList[chainIndex] > 40.0 and  heightList[chainIndex]>1200.0 and durationList[chainIndex]>200.0 and areaList[chainIndex]>200.0:
            #if j == 1: # take away filter condition
                row = rowlist[chainIndex]
                column = columnlist[chainIndex]
                layer = layerList[chainIndex]
                

                if layer == -1: #back pannel 
                    h.Fill(column,row,1)
                    #print(column,row,layer)
                if layer == 0: #layer one
                    h.Fill(column,row+1,1)
                    #print(column,row,layer)  
                if layer == 1: #layer two
                    h.Fill(column,row+6,1)
                    #print(column,row,layer)
                if layer == 2: #layer three
                    h.Fill(column,row+11,1)
                    #print(column,row,layer)
                if layer == 3: #layer four
                    h.Fill(column,row+16,1)
                    #print(column,row,layer)
                if layer == 4: #back pannel
                    h.Fill(column,row+20,1)
                    #print(column,row,layer)

        h.LabelsDeflate("X")
        h.LabelsDeflate("Y")

        #h.LabelsOption("v")
        #let the axixs label in the expected labels
        i = 1
        while i <= nx:
            h.GetXaxis().SetBinLabel(i,columnName[i-1])   
            i += 1

        k = 1
        while k <= ny:
            h.GetYaxis().SetBinLabel(k,rowName[k-1])   
            k += 1

            
    
    entry += 1

h.Draw("text")  #DISPLAY THE BIN number

c1.Draw()