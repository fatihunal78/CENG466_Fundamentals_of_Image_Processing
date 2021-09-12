function [sonuc] = uy2(cem, ncem, R)
[m,n,l] = size(R);
sonuc = zeros(m,n,l);

for x = 1 : m
    for y = 1 : n
        for k = 1 : ncem
            if ( ((x-cem(k,1))^2) + ((y-cem(k,2))^2) < ((cem(k,3))^2) )
                sonuc(x,y,:) = R(x,y,:);
                break;
            end;
        end;
    end;
end;

return;