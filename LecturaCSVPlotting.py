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
scatter(dff['mc-x'],dff['mc-y'])

#Graficamos con respecto a cada
for i in range(0,numObjs*numFrames,20):
    dffTemp = dff.iloc[i:i+(20-1)]
    print(dffTemp)
    scatter(dffTemp['mc-x'],dffTemp['mc-y'])
    

    
    




