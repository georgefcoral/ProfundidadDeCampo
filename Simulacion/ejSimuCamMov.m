
K = [600, 0, 320; 0, 600, 240; 0, 0,1];
iK = inv(K);

R = eye(3);

T=[0,0,1]';

Botella;
% El incremento de movimiento de la camara (0.5mm)
incZ =  0.0005; 

[Idx, TT] =  simCamMov(F006, K, R, T, incZ);
corrIdx = zeros(length(Idx),length(Idx(:,1)))';

%Hallamos indices de correspondencias.
for i  = 1 : length(Idx(:,1))-1
    corrIdx(i,:) = Idx(i,:).*Idx(i+1,:);
endfor

N = length(F006(:,1));

%Mapeamos correspondencias a las coordenadas XYZ
M=length(corrIdx(:,1));
X = zeros(N, 4);
Z = zeros(M, N);
H = zeros(3,3,N-1);
for i = 1 : M
    %Contienen Coordenadas en el espacio no homogeneas de un corte actual(X) 
    Idx2 = find (corrIdx(i,:) ~= 0);
    display(sprintf('Numero de correspondencia en el par (%d, %d) = %d', i, i+1, length(Idx2)))
    j = 0;
    X  = zeros(length(Idx2), 4);
    for k = 1:length(Idx2)
      l = Idx2(k);
      if l < N
         j += 1;
         X(j,:) = [F006(l,1),F006(l,2),F006(l,3), 1];
      end
    end

    %Descartamos los que no entramos
    X =  X(1:j, :);
    if length(X(:,1)) < 4
      display (sprintf('No se pudo calcular la homografia en la iteración %d', i));
      continue
    end
   
   %Aqui proyectamos lo que ya conocemos, las coordenadas
   % de puntos en el espacio, para obtener su correspondiente
   % coordenadas en la imagen.
    display ('TTes')
    [TT(i,:)', TT(i+1,:)']
    x = K * [R, TT(i,:)'] * X';
    xp = K * [R, TT(i+1,:)'] * X';
 
%    plot(x(1,:),x(2,:), 'r*', xp(1,:),xp(2,:), 'b*');
%    pause

    H(:,:,i) = homography2d(iK*x, iK*xp);
    if H(3,3,i) ~= 0
       H(:,:,i) /= H(3,3,i);
   else
      display ('Ups!, hay un error');
   end
   for k = 1:length(Idx2)
      l = Idx2(k);
      q = mean(diag(H(1:2,1:2)));
      Z(i,l) = incZ/(q-1); 
   end
end


