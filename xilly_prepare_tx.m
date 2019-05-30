function xilly_prepare_tx(tx_samples)

ctrlfile = '/dev/xillybus_mem_8';
num_samples = length(tx_samples);
mem_end_addr = num_samples/128*16;

xilly_memwrite(ctrlfile, 1, bitand(mem_end_addr,255));
xilly_memwrite(ctrlfile, 2, bitand(bitshift(mem_end_addr,-8),255));

% Assume all samples are real for now, will change in the future
fh = fopen('/dev/xillybus_mem_128', 'a');
fseek(fh, 0, -1);
fwrite(fh, tx_samples, 'int8');
fclose(fh);

end