function [R] = find3DCoords(N0,l,Tv,iK,shift,pFPix,stepping,step)
    shift = stepping * step

    for i = 1:l
        [r,c] = size(N0{i});
        R{i}=zeros(r,7);
    end
    
    dTx = Tv(1) * shift / 1000;
    dTy = Tv(2) * shift / 1000;
    dTz = Tv(3) * shift / 1000;%Incremento de Desplazamiento.
    
    for i = 1 : l%Aquí tenemos el # del objeto que contiene su trayectoria.
        N0{i}(1,4) = 0;
        
        s=size(N0{i});
        for j = 2 : s(1)  %Aquí recorremos la trayectoria por cada objeto.
    
    
    
            Tx = dTx * N0{i}(j,3);
            Ty = dTy * N0{i}(j,3);
            Tz = dTz * N0{i}(j,3);%Desplazamiento absoluto con respecto al primer cuadro.
    
    
            X_ = [N0{i}(j-1,11), N0{i}(j-1,12),1];%Aquí la j-esima coordenada del i-esimo objeto anterior.
            X = [N0{i}(j,11), N0{i}(j,12),1];%Aquí la j-esima coordenada del i-esimo objeto.
            
    
            % Xw_ = Xw_ / Xw_(3);
            % Xw  = Xw  / Xw (3);
            if(X(2) - X_(2) != 0 & X(1) - X_(1) != 0)
    
                deltaX = X(1) - X_(1);%u
                deltaY = X(2) - X_(2);%v
    
                R{i}(j,1) = ((dTz)*(X(1) - pFPix(1))) / deltaX;  %Z;
                R{i}(j,2) = ((dTz)*(X(2) - pFPix(2))) / deltaY;  %Z;
                R{i}(j,3) = N0{i}(j,  7); 
                R{i}(j,4) = N0{i}(j, 10); 
                R{i}(j,5) =  R{i}(j,  1) + Tz;% Z component
    
                Xw  = iK * transpose(X);
                Xw = Xw / Xw(3);
                R{i}(j,6) = Xw(1) * R{i}(j, 5) + Tx;%Esta es X
                R{i}(j,7) = Xw(2) * R{i}(j, 5) + Ty;%Esta es Y
                %R{i}(j,8) = Xw(1) * R{i}(j, 1) + Tx;%Esta es X
                %R{i}(j,9) = Xw(2) * R{i}(j, 1) + Ty;%Esta es Y
    
            else
                
                display("Division por cero detectada.")
                N0{i}(j,4) = 0;
                N0{i}(j,7) = Tz;
            end
    
        endfor
    endfor