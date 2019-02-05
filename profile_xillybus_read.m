devfile='';
maxlen=128;

% rawread
stream=XillyReadStream(devfile);
buff=uint8(zeros(1, maxlen));
ptr=libpointer('voidPtr', buff);
amount_rawread=0;
while 1 % Change this to stop at some point
    ret=stream.rawread(ptr, maxlen);
    if ret<0
        % error
    else
        amount_rawread=amount_rawread+ret;
    end
end

% read
amount_read=0;
while 1
    dat=stream.read('uint8', maxlen);
    amount_read=amount_read+length(dat);
end
    
