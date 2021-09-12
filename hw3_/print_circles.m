function [merkezler] = cemberleribas (cem, k, R)
[m,n] = size(R);
%merkezler = R;
merkezler = zeros(m,n);
for i = 1:k
    merkezler(cem(i,1), cem(i,2)) = 255;
end;
return;