function [sonucR] = uygula(cem, ncember, R)
[m,n,l] = size(R);
% n = n;	% n = satirdaki piksel sayisi (RGB için x3)
sonucR = zeros(m,n,3);
for i = 1 : ncember 
	sol = round(cem(i,2) - cem(i,3));	% sol = merkez - r
	sag = round(cem(i,2) + cem(i,3));
	ust = round(cem(i,1) - cem(i,3));
	alt = round(cem(i,1) + cem(i,3));
	for x = ust : alt
		for y = sol : sag
			if ((x-cem(i,1))^2 + (y-cem(i,2)^2) <= (cem(i,3))^2)
                if (x > m) | (x < 1) | (y > n) | (y < 1)
                    continue;
                end;
                sonucR(x,y,1) = R(x,y,1);
                sonucR(x,y,2) = R(x,y,2);
                sonucR(x,y,3) = R(x,y,3);
%                xx = 3*x - 2;
%                yy = 3*y - 2;
%                if (xx > m) | (yy > n) | (1 > xx) | (1 > yy)
%                    continue;
%                end;
%                sonucR(xx,  yy) = R(xx,  yy);
%                sonucR(xx+1,yy+1) = R(xx+1,yy+1);
%                sonucR(xx+2,yy+2) = R(xx+2,yy+2);
            end;
        end;
    end;
end;

return;