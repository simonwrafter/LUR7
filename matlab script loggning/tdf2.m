function [ den, num ] = tdf2( w0, zeta, sampling_time )
% Secind order filter
    G = tf(w0^2, [1 2*zeta*w0 w0^2]);
    H = c2d(G, sampling_time, 'tustin');
    [den,num] = tfdata(H, 'v');
end

