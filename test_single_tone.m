clear all; close all

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Set up the usrp experiment
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Fs = 1000e6;             % 1000 Msps sample rate

% Params:
%USE_USRP_TXRX           = 1;            % Enable WARPLab-in-the-loop (otherwise sim-only)
TX_SCALE = 0.1;

txLength = 5e5;



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Signal processing to generate transmit signal
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


Ts = 1/Fs;

t = [0:Ts:((txLength-1))*Ts].'; % Create time vector(Sample Frequency is Ts (Hz))

payload = exp(t*1i*1*pi*Fs/3);%+exp(t*1i*pi*10e6); %5 and 10 MHz sinusoid as our "payload"

txData = [payload]; %,payload*exp(1i*0.5*pi)
txData = TX_SCALE .* txData ./ max(abs(txData));

% TX_NUM_SAMPS = length(txData);
% 
% transmit_matrix = zeros(1, TX_NUM_SAMPS);
transmit_matrix = txData.';


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Transmit and receive signal using WARPLab
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

while true
    %===================================
    % Matlab interface
    rx_IQ = xilly_rxtx(transmit_matrix);
    %===================================
    
    
    
    figure(1)
    subplot(2,2,1)
    plot(real(rx_IQ))
    ylim([-1 1]);
    xlim([0 txLength]);
    xlabel('I Samples');
    ylabel('Amplitude');
    subplot(2,2,3)
    plot(imag(rx_IQ))
    ylim([-1 1]);
    xlim([0 txLength]);
    xlabel('Q Samples');
    ylabel('Amplitude');
    subplot(2,2,2)
    fft_a = fft(real(rx_IQ(1:txLength)));
    x = linspace(-20.48,20.48,length(fft_a));
    plot(x,fftshift(20*log10(abs(fft_a))))
    xlabel('I Frequency (MHz)');
    ylabel('Magnitude');
    subplot(2,2,4)
    fft_b = fft(imag(rx_IQ(1:txLength)));
    x = linspace(-20.48,20.48,length(fft_b));
    plot(x,fftshift(20*log10(abs(fft_b))))
    xlabel('Q Frequency (MHz)');
    ylabel('Magnitude');

%     figure(2)
%     plot(real(rx_IQ))
%     hold on   
%     plot(imag(rx_IQ))
%     hold off
%     ylim([-0.1, 0.1])

    pause(0.1)
 end
