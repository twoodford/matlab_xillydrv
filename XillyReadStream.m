classdef XillyReadStream
    %XILLYREADSTREAM Xillybus PCIe stream reader (FPGA to PC)
    
    properties
        fhandle
    end
    
    methods
        function obj = XillyReadStream(devfile)
            %XILLYREADSTREAM Open a Xillybus stream
            if ~libisloaded('libc_fn')
                loadlibrary('/lib/x86_64-linux-gnu/libc-2.27.so', '/usr/include/fcntl.h','alias','libc_fn');
            end
            if ~libisloaded('libc_uni')
                loadlibrary('/lib/x86_64-linux-gnu/libc-2.27.so', '/usr/include/unistd.h','alias','libc_uni');
            end
            % Octal values
            % 00000  O_RDONLY
            % 04000  O_NONBLOCK
            obj.fhandle = calllib('libc_fn', 'open', devfile, ...
                int32(oct2dec(4000)), libpointer);
        end
        
        function amount = rawread(obj, buffer, buff_size)
            %RAWREAD Read data from stream
            amount=calllib('libc_uni', 'read', obj.fhandle, ...
                buffer, buff_size);
        end
        
        function data = read(obj, type, maxlen)
            %READ Read data from stream
            value=typecast(uint8(zeros(1, maxlen)),type);
            info=whos('value');
            width=info.bytes/prod(info.size);
            buffer = libpointer('voidPtr', value);
            amount=obj.rawread(buffer, info.bytes);
            if amount < 0
                error('Error during read() call');
            elseif amount==0
                data = [];
            else
                data = buffer.Value(1:amount/width);
            end
        end
        
        function close(obj)
            if calllib('libc_uni', 'close', obj.fhandle)~=0
                error('Couldn''t close() stream');
            end
        end
    end
end

