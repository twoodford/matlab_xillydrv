function read_data = xilly_read(streamfile, ctrlfile, num_samples)

% 64-bit packets, 8-bit samples
num_ofxfer = num_samples/64*8;

% Set the number of samples
xilly_memwrite(ctrlfile, 1, bitand(num_ofxfer,255));
xilly_memwrite(ctrlfile, 2, bitand(bitshift(num_ofxfer,-8),255));
xilly_memwrite(ctrlfile, 3, bitand(bitshift(num_ofxfer,-16),255));
xilly_memwrite(ctrlfile, 4, bitand(bitshift(num_ofxfer,-24),255));

% Grab samples
read_data = xilly_fiforead(streamfile, num_samples, 2);

end
