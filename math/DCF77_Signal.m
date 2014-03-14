   %% Time specifications:
   Fs = 100000                  % samples per second
   dt = 1/Fs;                   % seconds per sample
   StopTime = 1.0 ;             % seconds
   t = (0:dt:StopTime-dt)';     % seconds

   %% Sine wave:
   Fc = 775                    % hertz
   x = 5*sin(2*pi*Fc*t);

   % Plot the signal versus time:
   figure;
   plot(t,x);
   xlabel('time (in seconds)');
   title('Signal versus Time');
   zoom xon;