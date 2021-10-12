#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jun 11 20:26:37 2021

@author: kanade
"""
import pandas as pd
import numpy as np
import math
from matplotlib.pyplot import scatter

numObjs = 20
numFrames = 11


#Lectura del archivo CSV a partir de un TXT
df = pd.read_csv('tracking.txt', header=None)
df.head()
# Se agregan las cabeceras de las columnas#
df.columns = ['mc-x','mc-y']
df = df.astype({"mc-x":int,"mc-y":int})
#Eliminamos valores desconocidos (NA)
df = df.dropna()
#Dimensiones del DF
rows,cols= df.shape
#Filtramos valores negativos.
dff = df.loc[(df['mc-x']>=0)&(df['mc-y']>=0)]
#Ploteamos todos vs todos sin considerar cada cuadro.
#scatter(dff['mc-x'],dff['mc-y'])
 
#Graficamos con respecto a cada
for i in range(0,numObjs*numFrames,20):
    dffTemp = dff.iloc[i:i+(20-1)]
   # print(dffTemp)
    #scatter(dffTemp['mc-x'],dffTemp['mc-y'])
    

dfobj1 = df.iloc[range(0,20*20,20)]
dfobj2 = df.iloc[range(1,20*20,20)]
dfobj3 = df.iloc[range(2,20*20,20)]
dfobj4 = df.iloc[range(3,20*20,20)]
dfobj5 = df.iloc[range(4,20*20,20)]
dfobj6 = df.iloc[range(5,20*20,20)]
dfobj7 = df.iloc[range(6,20*20,20)]
dfobj8 = df.iloc[range(7,20*20,20)]
dfobj9 = df.iloc[range(8,20*20,20)]
dfobj10 = df.iloc[range(9,20*20,20)]
dfobj11 = df.iloc[range(10,20*20,20)]
dfobj12 = df.iloc[range(11,20*20,20)]
dfobj13 = df.iloc[range(12,20*20,20)]
dfobj14 = df.iloc[range(13,20*20,20)]
dfobj15 = df.iloc[range(14,20*20,20)]
dfobj16 = df.iloc[range(15,20*20,20)]
dfobj17 = df.iloc[range(16,20*20,20)]
dfobj18 = df.iloc[range(17,20*20,20)]

scatter(dfobj1['mc-x'],dfobj1['mc-y'])
scatter(dfobj2['mc-x'],dfobj2['mc-y'])
scatter(dfobj3['mc-x'],dfobj3['mc-y'])
scatter(dfobj4['mc-x'],dfobj4['mc-y'])
    




