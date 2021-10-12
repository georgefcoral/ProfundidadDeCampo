clear;
figure(1);
clf;

K = [600, 0, 320; 0, 600, 240; 0, 0,1];

R = eye(3);
T=[0,0,1]';

Botella;

zMin = min(F006(:,3));
zMax = max(F006(:,3));

pCampo = (zMax-zMin)/25;

incZ =  0.0005; 
zTravel=0.6;
N = round(zTravel/incZ);

z=zMax;
nP = length(F006(:,3));
Idx = zeros(N, nP);
for i = 1:N
   idx=find(F006(:,3)<z & F006(:,3)>=(z-pCampo));
   Idx(i,idx) = 1;
   X = [F006(idx,:)';ones(1,length(idx))];
   x = K * [R, T] * X;
   for j=1:length(x)
      x(:,j) /= x(3,j);
   end
   plot(x(1,:), x(2,:), "*");
   axis equal;
   pause(0.01);
   z = z-incZ;
   T = T + [0,0,incZ]';
end


%for i=1:25
% k=1;
% for j = 1:152
% XL(:,j+152*i) = [xl(1,j,i);xl(2,j,i);i];
% end
% end



