%% Initialize program and USB port
if(exist('M2USB'))
  fclose(M2USB);
 else
%  fclose(instrfindall);
 delete(instrfindall);
end



M2USB = serial('COM16','Baudrate', 9600); 
fopen(M2USB);
flushinput(M2USB);
cmplot=[0,0];


figure(1);
clf;
% rawdata = [];
% i = 0;
axis([0 1024 0 768])
hold on
while (1)
      m2_buffer = fgetl(M2USB);
    

      [x1, remain1] = strtok(m2_buffer);
      [y1, remain2] = strtok(remain1);
      [x2, remain3] = strtok(remain2);
      [y2, remain4] = strtok(remain3);      
      [x3, remain5] = strtok(remain4);
      [y3, remain6] = strtok(remain5);
      [x4, remain7] = strtok(remain6);
      [y4, remain8] = strtok(remain7);   
%       [xb, remain3] = strtok(remain2);
%       [yb, remain4] = strtok(remain3);
%       [xc, remain5] = strtok(remain4);
%       [yc, remain6] = strtok(remain5);
%       [xd, remain7] = strtok(remain6);
%       [yd,remain8] = strtok(remain7);
%       [xcm, remain9] = strtok(remain8);
%       [ycm, remain10] = strtok(remain9);
 
     
%     
%       xa=str2double(xa);
%       ya=str2double(ya);
%       xb=str2double(xb);
%       yb=str2double(yb);
%       xc=str2double(xc);
%       yc=str2double(yc);
%       xd=str2double(xd);
%       yd=str2double(yd);
      x1=str2double(x1);
      y1=str2double(y1);
      x2=str2double(x2);
      y2=str2double(y2);
      x3=str2double(x3);
      y3=str2double(y3);
      x4=str2double(x4);
      y4=str2double(y4);
      
      x = [x1, x2, x3, x4];
      y = [y1, y2, y3, y4];
      plot(x, y, '*');

%     
%      plot(xa,ya,'*r');
%       hold on;
%      plot(xb,yb,'*y');
%       
%      plot(xc,yc,'*b');
%       
%       plot(xd,yd,'*g');
%     
%       plot(459,4,'*k');
      
%       plot(xcm,ycm,'*k');
%       hold off;
%       axis([0 1024 0 768]);
      
%       (xcm-478)^2+(ycm-337)^2
%       rawdata = [rawdata; x1, x2, x3, x4, y1, y2, y3, y4];
      pause(.0001);
%     i = i+1;
end
% savefile = 'A.mat';
% save(savefile,'rawdata');