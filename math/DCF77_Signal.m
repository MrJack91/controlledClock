   %% Time specifications:
   Fs = 100000                  % samples per second
   dt = 1/Fs;                   % seconds per sample
   StopTime = 1.0 ;             % seconds
   t = (0:dt:StopTime-dt)';     % seconds

   %% Sine wave:
   Fc = 775                    % hertz
   x = 5*sin(2*pi*Fc*t);

   % Plot the signal versus time:
   %figure;
   %plot(t,x);
   %xlabel('time (in seconds)');
   %title('Signal versus Time');
   %zoom xon;
   
   
   %% Normales Signal
   
   tEnd = 4.0;
   t2 = [0:0.01:tEnd];
   x2 = 5 * sin(2*pi*t2);
   
   fig2 = figure(2);
   plot(t2,x2);
   hold on;
   plot([0,(tEnd + 0.2)],[0,0], 'k');
   set(gca,'XTickLabel',[],'YTickLabel',[])
   xlabel('Zeit');
   xlim([0,(tEnd +0.2)]);
   ylim([-8,8]);
   
   %title('Signal ohne Amplitudenmodulation');
   hgexport(fig2, 'SWP2_Analyse_DCF77_Signal_ohne.eps');
   
   %% Signal mit reduzierter Amplitude
   t31 = [0:0.01:1.0];
   x31 = 5 * sin(2*pi*t31);
   
   t32 = [1.0:0.01:3.5];
   x32 = 1.25 * sin(2*pi*t32);
   
   t33 = [3.5:0.01:4.0];
   x33 = 5 * sin(2*pi*t33);
   
   fig3 = figure(3);
   plot(t31,x31,'r');
   hold on;
   plot(t32,x32,'r');
   plot(t33,x33,'r');
   plot([0,(tEnd + 0.2)],[0,0], 'k');
   set(gca,'XTickLabel',[],'YTickLabel',[])
   xlabel('Zeit');
   xlim([0,(tEnd +0.2)]);
   ylim([-8,8]);
   
   %title('Signal mit Amplitudenmodulation');
   hgexport(fig3, 'SWP2_Analyse_DCF77_Signal_mit.eps');