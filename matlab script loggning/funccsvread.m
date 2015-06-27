function [ ret ] = funccsvread( file, row, col )
% Read an int from [row, col] in file 'file'
    ret = csvread(file, row, col, [row, col, row, col]);
end

