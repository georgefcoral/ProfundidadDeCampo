function []=plotting3D(R,fg,pF)
figure(fg)
%open file identifier
fid=fopen('model.xyz','w');
fod = fopen('modelDetalled.xyz','w');
fud = fopen('puntoFuga.xyz','w')
ptF = cstrcat(num2str(pF(1)),' ',num2str (pF(2)),' ',num2str (pF(3)),'\n');
fprintf(fud,ptF);
hold on
s1=length(R);
for i = 1:s1% de objeto
  if(length(R{i})!=0)

    % Z = R{i}(2:end,5);
    % X = R{i}(2:end,6);
    % Y = R{i}(2:end,7);

    Z = median(R{i}(2:end,5));
    X = median(R{i}(2:end,6));
    Y = median(R{i}(2:end,7));
    ZE = R{i}(2:end,5);
    XE = R{i}(2:end,6);
    YE = R{i}(2:end,7);
    %['33.126', '-73', '6.339']
    ptStr = cstrcat(num2str(X),' ',num2str (Y),' ',num2str (Z),'\n');
    for j = 1:length(XE)
      ptStr2 = cstrcat(num2str(XE(j)),' ',num2str (YE(j)),' ',num2str (ZE(j)),'\n');
      fprintf(fod,ptStr2);
    endfor   
    fprintf(fid,ptStr);
   %Xt = median(R{i}(2:end,8));
   %Yt = median(R{i}(2:end,9));
    plot3(XE,YE,ZE,'.k');%, Xt, Yt, Z, '.r');
  endif
endfor
plot3(pF(1),pF(2),pF(3),'*r');%, Xt, Yt, Z, '.r');
xlabel('X');
ylabel('Y');
zlabel('Z');

fclose(fod)
fclose(fid)
fclose(fud)
