% run using octave --persist iso.m
a = 30;
b = 30;
ISO = [ cos(a) -cos(b) 0; sin(a) sin(b) 1; 0 0 0 ];
m = zeros(3,100);
pos=1;
for i = 1:10
 for j = 1:10
  m(1,pos) = i;
  m(2,pos) = j;
  pos = pos + 1;
 end
end
PROJ = ISO * m;
x = m(1,:);
y = m(2,:);
%scatter(x,y);
xp = PROJ(1,:);
yp = PROJ(2,:);
scatter(xp,yp);
