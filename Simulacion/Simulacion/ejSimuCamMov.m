
K = [600, 0, 320; 0, 600, 240; 0, 0,1];

R = eye(3);

T=[0,0,1]';

Botella;
Idx =  simCamMov(F006, K, R, T);
corrIdx = zeros(length(Idx),length(Idx(:,1)))';

%Hallamos indices de correspondencias.
for i  = 1 : length(Idx(:,1))-1
    corrIdx(i,:) = Idx(i,:).*Idx(i+1,:);
endfor

#Mapeamos correspondencias a las coordenadas XYZ
X = zeros(length(F006(:,1)),4);
XP = zeros(length(F006(:,1)),4);
stackPoints = zeros(length(Idx(:,1)));
for i = 1 : length(Idx(:,1))-1
    %Contienen Coordenadas en el espacio no homogeneas de un corte actual(X) y el siguiente(XP).
    for j = 1:2048
      X(j,:) = [F006(j,1)*corrIdx(i,j),F006(j,2)*corrIdx(i,j),F006(j,3)*corrIdx(i,j), 1];
      XP(j,:) = [F006(j,1)*corrIdx(i+1,j), F006(j,2)*corrIdx(i+1,j), F006(j,3)*corrIdx(i+1,j),1];
    endfor
    x = K * [R, T] * X';
    xp = K * [R, T] * XP'; 
    h = homography2d(x,xp);
    h
endfor


