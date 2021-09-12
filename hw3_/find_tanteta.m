function [sonuc] = tantetabul (kenar, gri, dx, dy, sonuc)
[m,n] = size(gri);
% sonuc=[];
satir=[];

Dx = convn(gri, dx, 'same');
Dy = convn(gri, dy, 'same');
sonuc = Dy ./ Dx;

%for i = 1:m
%    	for j = 1:n
%        	if (kenar(i,j)==1)
%			x = convn(gri, dx, 'same');
%			y = convn(gri, dy, 'same');
%			t = y / x;
%			fprintf('tantetabul: t=%d\n',t);
%			sonuc(i,j) = t;
	%		satir = [satir,t];
	%    	else 
	%		satir = [satir,55];
%		end;
%   	end;
    	%sonuc = [sonuc; satir];
%  	satir = [];
%end;

return;
