function rx_dat = xilly_rxtx(tx_data)
assert(all(abs(tx_data)<=1));

scaled_data=32000*reshape([real(tx_data); imag(tx_data)], 2*length(tx_data), 1);
xilly_prepare_tx(scaled_data);

data = xilly_read('/dev/xillybus_read_128', '/dev/xillybus_mem_8', 2*length(tx_data));
data = reshape(data, 2, length(data)/2);
data1 = data(1,:); data2=data(2,:);
reordered_data1=reshape(flipud(reshape(data1, 8, length(data1)/8)), 1, length(data1));
reordered_data2=reshape(flipud(reshape(data2, 8, length(data2)/8)), 1, length(data2));

rx_dat=(single(reordered_data1) + 1j*single(reordered_data2))/128;