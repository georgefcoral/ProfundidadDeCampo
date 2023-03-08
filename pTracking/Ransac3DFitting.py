import numpy as np
import matplotlib.pyplot as plt
import pyransac3d as pyrsc
import math

UMBRAL = 0;
def distance(p,eq):
    dist = np.abs(eq[2]*p[0][2]+eq[3]) / np.sqrt(eq[2]*eq[2])
    return dist
def plot3D(points,pf):

    fig = plt.figure()
    X = np.ones(len(points))
    Y = np.ones(len(points))
    Z = np.ones(len(points))
    for i in range(len(points)):
        X[i] = points[i][0];
        Y[i] = points[i][1];
        Z[i] = points[i][2];
    ax = plt.axes(projection ='3d')
    ax.scatter3D(X, Y, Z,color = 'b')
    ax.scatter3D(pf[0][0], pf[0][1], pf[0][0],color = 'r')
    ax.set_title('Plotting scatting')
    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')
    plt.show()


def XYZtoNumpyArray(xyz,units = 'm'):
    cont = 0
    arrayPoints = []
    for point in xyz:
        p = point.rstrip('\n')
        pointS = p.split(" ")
        cont =cont+1
        arrayPoints.append(pointS)
    points = np.zeros((cont, 3))

    c = 0
    if(units=='m'):
        for i in range(0,cont):
            arr = [float(arrayPoints[i][0]),float(arrayPoints[i][1]),float(arrayPoints[i][2])]
            points[i] = np.array(arr)
            c = c+1
    else:
        if(units=='mm'):
            for i in range(0,cont):
                arr = [float(arrayPoints[i][0])/1000,float(arrayPoints[i][1])/1000,float(arrayPoints[i][2])/1000]
                points[i] = np.array(arr)
                c = c+1
        else:
            for i in range(0,cont):
                arr = [float(arrayPoints[i][0])/100,float(arrayPoints[i][1])/100,float(arrayPoints[i][2])/100]
                points[i] = np.array(arr)
                c = c+1
    return points


#Print fuga points
pfFile = open('puntoFuga.xyz','r')
pF = XYZtoNumpyArray(pfFile)
pfFile.close()


#Leemos archivo XYZ para convertirlo a un numpy array.
xyz = open('archivo.xyz','r')
cont = 0
arrayPoints = []
for point in xyz:
    p = point.rstrip('\n')
    pointS = p.split(" ")
    #print(float(pointS[2])/1000)
    if(float(pointS[2])/1000>UMBRAL):
        cont =cont+1
        arrayPoints.append(pointS)


xyz.close()
points = np.zeros((cont, 3))


c = 0
for i in range(0,cont):
    arr = [float(arrayPoints[i][0])/1000,float(arrayPoints[i][1])/1000,float(arrayPoints[i][2])/1000]
    points[i] = np.array(arr)
    c = c+1

print("Total de puntos: ", len(points))

#Inicialize sphere obj for fitting
#plot3D(points,np.zeros((1,3)))

sphere = pyrsc.Sphere()
center, radius, inliers = sphere.fit(points, thresh=0.1,maxIteration=1000)

print("Grown truth radius for cloud points",radius*1000)

print("Center of sphere ", center)

data3d = points[inliers]
#plot3D(data3d,np.zeros((1,3)))

#Datos extraidos del experimento.
#Leemos archivo XYZ para convertirlo a un numpy array.

modelName = 'model.xyz'
#modelName = 'modelDetalled.xyz'
xyzM = open(modelName,'r')
cont = 0
arrayPointsM = []
for pointM in xyzM:
    #print(pointM)
    pM = pointM.rstrip('\n')
    pointSM = pM.split(" ")
    #print(pointSM)
    if(float(pointSM[2])>0 and float(pointSM[2])<.05 ):
        cont =cont+1
        arrayPointsM.append(pointSM)

xyzM.close()
pointsM = np.zeros((cont, 3))

for i in range(0,cont):
    arr = [float(arrayPointsM[i][0])*100,float(arrayPointsM[i][1])*100,float(arrayPointsM[i][2])*100]
    print(arr)
    pointsM[i] = np.array(arr)


sphereE = pyrsc.Sphere()

plot3D(pointsM,pF)

#filtering, index = np.where(np.logical_and(pointsM>.04,pointsM<=.5))
filtro = (pointsM[:,2]<.60) & (pointsM[:,2]>=.40)
# filtering, index = np.where(np.logical_and(pointsM>.3,pointsM<=.2))
#print(" Filtering >> ", filtering)
#print(" Index >> ", index)
ITERA = 100
dist = 0
d = 0
for i in range(ITERA):
    plane = pyrsc.Plane()
    equation, inliers = plane.fit(pointsM[filtro], thresh=0.4, minPoints=10, maxIteration=100)

    #Encontramos el plano que corta únicamente al eje Z

    print("pf: ",pF)
    # print(len(inliers))
    # print("Pts[inliers]",data3dE[inliers])
    # print("Pts[inliers]",len(data3dE[inliers]))
    d = abs(pF[0][2]*100-equation[3])
    dist =dist + d 
    print("Distancia: ",d)

print("Promedio de distancias >> ", (dist/ITERA))

