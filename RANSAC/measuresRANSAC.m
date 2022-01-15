
lin=0;
lisup=7;
m=1.5;
b=3;
PercErr = .25;

ms1 = [15.42,15.346,14.184,13.630,13.094,12.022,11.948];
ms1  = sort(ms1);
index1 = [ 0:length(ms1)-1];
ms2 = [11.890,12.240,12.880,13.428,13.956,14.524,15.150,15.624];
index2 = [0:length(ms2)-1];


hold on
plot(index1,ms1,'*b');

[sol,inl,outl,sx,sy,tstd]=ransac1_demo(index1,ms1,0.5,0.25);


##
##sol
##length(inl)
##length(outl)
##plot(index1(inl),ms1(inl),'r.');
##plot(index1(outl),ms1(outl),'g.');
##
##
##plot ([lin:lisup],sol(1).*[lin:lisup]+sol(2),'r');
##
##
##plot(tstd(:,1),tstd(:,2),'.');
##plot(sx,sy,'y*');
##hold off




