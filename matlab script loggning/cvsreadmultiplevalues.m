function values = cvsreadmultiplevalues(file, row, read_vector )
%Read the values when the vector is 1
    values = [];
    for i = 1:length(read_vector)
       if read_vector(i) == 1
           read2 = funccsvread(file, row, i*2);
           read1 = funccsvread(file, row, i*2+1);
           value_add = read1 + read2*100;
           values = [values value_add];
       end
    end
end

