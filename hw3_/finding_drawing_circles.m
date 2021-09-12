R = imread('hw3p2.jpg');                % R => girdi dosyas�
[m, n] = size(R);			% m,n => resmin buyuklugu
n = n/3;
tanteta = zeros(m, n);                  % her 'edge' i�in tanteta de�eri tutar.
GR = RGB2GRAY(R);                       % GR => resmin gri hali
KR = edge(GR, 'sobel');                 % KR => Kenarlar�n bulundu�u resim (edge map)
Dx = [1, 2, 1; 0, 0, 0; -1, -2, -1];    % noktan�n x'e g�re t�revini hesaplayan maske.
Dy = [1, 0, -1; 2, 0, -2; 1, 0, -1];    % noktan�n y'ye g�re t�revin, hesaplayan makse.
fprintf('tanteta bulunuyor..\n');
tanteta = tantetabul(KR, GR, Dx, Dy, tanteta);    % tanteta tablosunu doldurur.
toplayici = zeros(m, n, 80);                % toplay�c� h�creler (accumulator cells)
%yaricaplar = zeros(m, n);               % her toplay�c� h�crenin ait oldu�u �emberin yar��ap�.
fprintf('toplayici hesaplaniyor..\n');
[toplayici] = tdoldur(toplayici, tanteta, KR);     
% toplayici h�crelerin de�erlerini bulur.

T = 18;	% yerel tepe de�erini bulmak i�in e�ik de�eri. 
fprintf('cemberler bulunuyor..\n');
[cemberler,ncember] = cemberleribul(toplayici, T);

%fprintf('cemberler basiliyor..\n');
%merkez = cemberleribas(cemberler, ncember,GR);
%imshow(merkez);

fprintf('resmin son hali hazirlaniyor\n');
Rs = uy2(cemberler, ncember, R);
Rson = uint8(Rs);

imshow(Rson);