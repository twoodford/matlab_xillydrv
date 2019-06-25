function rx_dat = xilly_rxtx(tx_data)
assert(all(abs(tx_data)<=1));

scaled_data=32000*reshape([real(tx_data); imag(tx_data)], 2*length(tx_data), 1);
xilly_prepare_tx(scaled_data);

data = xilly_read('/dev/xillybus_read_128', '/dev/xillybus_mem_8', 2*length(tx_data)+2048);
data = reshape(data, 2, length(data)/2);
data1 = data(1,:); data2=data(2,:);

rx_dat=(-double(data1) + 1j*double(data2))/2048;