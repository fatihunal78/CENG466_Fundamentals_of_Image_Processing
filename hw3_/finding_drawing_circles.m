R = imread('hw3p2.jpg');                % R => girdi dosyasý
[m, n] = size(R);			% m,n => resmin buyuklugu
n = n/3;
tanteta = zeros(m, n);                  % her 'edge' için tanteta deðeri tutar.
GR = RGB2GRAY(R);                       % GR => resmin gri hali
KR = edge(GR, 'sobel');                 % KR => Kenarlarýn bulunduðu resim (edge map)
Dx = [1, 2, 1; 0, 0, 0; -1, -2, -1];    % noktanýn x'e göre türevini hesaplayan maske.
Dy = [1, 0, -1; 2, 0, -2; 1, 0, -1];    % noktanýn y'ye göre türevin, hesaplayan makse.
fprintf('tanteta bulunuyor..\n');
tanteta = tantetabul(KR, GR, Dx, Dy, tanteta);    % tanteta tablosunu doldurur.
toplayici = zeros(m, n, 80);                % toplayýcý hücreler (accumulator cells)
%yaricaplar = zeros(m, n);               % her toplayýcý hücrenin ait olduðu çemberin yarýçapý.
fprintf('toplayici hesaplaniyor..\n');
[toplayici] = tdoldur(toplayici, tanteta, KR);     
% toplayici hücrelerin deðerlerini bulur.

T = 18;	% yerel tepe deðerini bulmak için eþik deðeri. 
fprintf('cemberler bulunuyor..\n');
[cemberler,ncember] = cemberleribul(toplayici, T);

%fprintf('cemberler basiliyor..\n');
%merkez = cemberleribas(cemberler, ncember,GR);
%imshow(merkez);

fprintf('resmin son hali hazirlaniyor\n');
Rs = uy2(cemberler, ncember, R);
Rson = uint8(Rs);

imshow(Rson);