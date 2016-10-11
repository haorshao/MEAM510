clc;clear all;
% calculate reference
P1 = [0, 14.5];
P2 = [-10.563, -2.483];
P3 = [11.655, -8.741];
P4 = [0, -14.5];

x = [P1(1), P2(1), P3(1), P4(1)];
y = [P1(2), P2(2), P3(2), P4(2)];


% case 1; not missing point 1 or point 4
d = [];
for i = 1 : length(x)-1
    for j = i+1 : length(x)
        dist = (x(i) - x(j))^2 + (y(i) - y(j))^2;
        d = [d; i, j, sqrt(dist)];
    end
end
[dmax,indmax] = max(d(:,3));
[dmin,indmin] = min(d(:,3));
% % coincide = point 4
% % chooce point 1 as reference point 
% % choose v(p4->p1) as reference vector
ref_p = P4;
ref_v = P1 - P4;
ref_v = ref_v / sqrt(ref_v*ref_v');


% case 2: miss point 4 (the one with two other start on its side)
% d = [];
% for i = 1 : length(x)-1
%     for j = i+1 : length(x)
%        if(i~=4 && j~=4) 
%             dist = (x(i) - x(j))^2 + (y(i) - y(j))^2;
%             d = [d; i, j, sqrt(dist)];
%        end
%     end
% end
% [dmax,indmax] = max(d(:,3));
% [dmin,indmin] = min(d(:,3));
% % coincide = point 1
% % chooce point 1 as reference point 
% % choose v(p1->p3) as reference vector
% ref_p = P1;
% ref_v = P3 - P1;
% ref_v = ref_v / sqrt(ref_v*ref_v');
% 
% %% case 3: miss point 1
% d = [];
% for i = 1 : length(x)-1
%     for j = i+1 : length(x)
%        if(i~=1 && j~=1) 
%             dist = (x(i) - x(j))^2 + (y(i) - y(j))^2;
%             d = [d; i, j, sqrt(dist)];
%        end
%     end
% end
% [dmax,indmax] = max(d(:,3));
% [dmin,indmin] = min(d(:,3));
% % coincide = point 3
% % chooce point 3 as reference point 
% % choose v(p3->p2) as reference vector
% ref_p = P3;
% ref_v = P2 - P3;
% ref_v = ref_v / sqrt(ref_v*ref_v');