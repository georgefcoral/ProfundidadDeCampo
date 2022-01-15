lin=-10;
lisup=10;
m=1.5;
b=3;

[X,Y]=Noisy_Line(150,lin,lisup, 1, 0.25, m, b);

plot ([lin:lisup],m.*[lin:lisup]+b,'g');
hold on
plot(X,Y,'.b');


[sol,inl,outl,sx,sy,tstd]=ransac1_demo(X,Y,1,0.25);
sol
length(inl)
length(outl)
plot(X(inl),Y(inl),'r.');
plot(X(outl),Y(outl),'g.');
plot ([lin:lisup],sol(1).*[lin:lisup]+sol(2),'r');
plot(tstd(:,1),tstd(:,2),'.');
plot(sx,sy,'y*');
hold off
