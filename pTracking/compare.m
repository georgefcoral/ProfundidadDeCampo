load test.mat
load model.xyz

figure(1);

X = model(:,1);
Y = model(:,2);
Z = model(:,3);

XE = test(:,1);
YE = test(:,2);
ZE = test(:,3);

hold on

 plot3(X,Y,Z,'.k');
 plot3(XE/1000,YE/1000,ZE/1000,'.r');
 
xlabel('X');
ylabel('Y');
zlabel('Z');

