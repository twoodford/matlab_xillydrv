function read_data = xilly_read(streamfile, ctrlfile, num_samples)

% Set the number of samples
xilly_memwrite(ctl_devfile, 1, bitand(num_ofxfer,255));
xilly_memwrite(ctl_devfile, 2, bitand(bitshift(num_ofxfer,-8),255));
xilly_memwrite(ctl_devfile, 3, bitand(bitshift(num_ofxfer,-16),255));
xilly_memwrite(ctl_devfile, 4, bitand(bitshift(num_ofxfer,-24),255));

% Start streaming
xilly_memwrite(ctl_devfile, 0, 1);

% Grab samples
read_data = xilly_fiforead(streamfile, num_samples)

end
