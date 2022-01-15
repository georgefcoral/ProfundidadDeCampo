lin=-10;
lisup=10;
m=1.5;
b=3;
PercErr = .25;

[X,Y]=Noisy_Line(150,lin,lisup, 1, PercErr, m, b);
pause
Oline=plot ([lin:lisup],m.*[lin:lisup]+b,'g');
set(Oline,'LineWidth',2);
hold on
plot(X,Y,'.b');
pause

[sol,inl,outl,sx,sy,tstd]=ransac1_demo(X,Y,1,PercErr);
sol
pause
plot(X(inl),Y(inl),'r.');
pause
plot(X(outl),Y(outl),'g.');
Sline=plot ([lin:lisup],sol(1).*[lin:lisup]+sol(2),'r');
set(Sline,'LineWidth',2);
pause
plot(tstd(:,1),tstd(:,2),'.');
pause
[x,y]=size(tstd);
for i=1:2:x
	plot(tstd(i:i+1,1),tstd(i:i+1,2),'-.');
end
plot(sx,sy,'k*');
pause
Uline=plot(sx,sy,'y');
set(Uline,'LineWidth',2);
pause

hold off
