file = 'test2.txt';

%Time vectors
dta0time = [];
dta1time = [];
dta2time = [];
dta3time = [];
front2time = [];
mid0time = [];
mid1time = [];
rear2time = [];
rear3time = [];
rear4time = [];

% Parameter vectors
rpm = [];
water_temp = [];
kph_x10 = [];
oil_temp = [];
clutch_left = [];
clutch_right = [];
clutch_left_filtered = [];
clutch_right_filtered = [];
gear = [];
brake_pressure = [];
neutral_up_time = [];
neutral_down_time = [];
servo_left_dutycycle = [];
servo_right_dutycycle = [];


row = 0;
while (1)
    %Read id of CAN package
    try
        id = funccsvread(file, row, 0);
    catch
        end_of_file = 1
        break; 
    end
    % Handla the right CAN package
    switch id
        %DTA: rpm and water temp
        case 2000
            try
                time = funccsvread(file, row, 1);
                dta0time = [dta0time time];
            catch
                printerror = 'catching from time read: id = 2000'
                break;
            end

            try
                twovalues = cvsreadmultiplevalues(file, row, [1 0 1 0]);
                rpm = [rpm twovalues(1)];
                water_temp = [water_temp twovalues(2)];
            catch
                printerror = 'cathing from cvsreadmultiplevalues: rpm and water_temp'
                break;
            end
        % DTA: kph
        case 2001
            try
                time = funccsvread(file, row, 1);
                dta1time = [dta1time time];
            catch
                printerror = 'catching from time read: id = 2001'
                break;
            end
            
            try
                values = cvsreadmultiplevalues(file, row, [0 0 1 0]);
                kph_x10 = [kph_x10 values(1)];
            catch
                printerror = 'cathing from cvsreadmultiplevalues: kph_x10'
                break;
            end
        % DTA: Oil temp
        case 2002
            try
                time = funccsvread(file, row, 1);
                dta2time = [dta2time time];
            catch
                printerror = 'catching from time read: id = 2002'
                break;
            end
            
            try
                values = cvsreadmultiplevalues(file, row, [0 1 0 0]);
                oil_temp = [oil_temp values(1)];
            catch
                printerror = 'cathing from cvsreadmultiplevalues: oil_temp'
                break;
            end
            
        % DTA: Gear
        case 2003
            try
                time = funccsvread(file, row, 1);
                dta3time = [dta3time time];
            catch
                printerror = 'catching from time read: id = 2003'
                break;
            end
            
            try
                values = cvsreadmultiplevalues(file, row, [1 0 0 0]);
                gear = [gear values(1)];
            catch
                printerror = 'cathing from cvsreadmultiplevalues: gear'
                break;
            end
            
        % Brake pressure
        case 4002
            try
                time = funccsvread(file, row, 1);
                front2time = [front2time time];
            catch
                printerror = 'catching from time read: id = 4002'
                break;
            end

            try
                values = cvsreadmultiplevalues(file, row, [0 1]);
                brake_pressure = [brake_pressure values(1)];
            catch
                printerror = 'error reading values: brake pressure'
            end
        % Clutch
        case 1501
            try
                time = funccsvread(file, row, 1);
                mid1time = [mid1time time];
            catch
                printerror = 'catching from time read: id = 1501'
                break;
            end

            try
                values = cvsreadmultiplevalues(file, row, [1 1]);
                clutch_left = [clutch_left values(1)];
                clutch_right = [clutch_right values(2)];
            catch
                printerror = 'error reading values: clutch'
            end
            
        % Successful neutral time
        case 4502
            try
                time = funccsvread(file, row, 1);
                rear2time = [rear2time time];
            catch
                printerror = 'catching from time read: id = 4502'
                break;
            end

            try
                values = cvsreadmultiplevalues(file, row, [1 1]);
                neutral_up_time = [neutral_up_time values(1)];
                neutral_down_time = [neutral_down_time values(2)];
            catch
                printerror = 'error reading values: neatraul up or down'
            end
            
        % Filtered clutch values
        case 4503
            try
                time = funccsvread(file, row, 1);
                rear3time = [rear3time time];
            catch
                printerror = 'catching from time read: id = 4503'
                break;
            end

            try
                values = cvsreadmultiplevalues(file, row, [1 1]);
                clutch_left_filtered = [clutch_left_filtered values(1)];
                clutch_right_filtered = [clutch_right_filtered values(2)];
            catch
                printerror = 'error reading values: filtered clutch'
            end
            
        % Servo duty cycle
        case 4504
            try
                time = funccsvread(file, row, 1);
                rear4time = [rear4time time];
            catch
                printerror = 'catching from time read: id = 4504'
                break;
            end

            try
                values = cvsreadmultiplevalues(file, row, [1 1]);
                servo_left_dutycycle = [servo_left_dutycycle values(1)];
                servo_right_dutycycle = [servo_right_dutycycle values(2)];
            catch
                printerror = 'error reading values: servo dutycycle'
            end
            
    end
    row = row + 1;
end


% Filtering, think about the sampling time!
%[numclutch, denclutch] = tdf2(2*pi*1, 0.65, 1);
%clutch_left_filter = zeros(1, length(clutch_left));
%clutch_left_filter(1) = clutch_left(1); clutch_left_filter(2) = clutch_left(2);
%clutch_right_filter = zeros(1, length(clutch_right));
%clutch_right_filter(1) = clutch_right(1); clutch_right_filter(2) = clutch_right(2);

%for i = 3:length(clutch_left)
   %clutch_left_filter(i) = numclutch(1)*clutch_left(i)+numclutch(2)*clutch_left(i-1)+numclutch(3)*clutch_left(i-2)-denclutch(2)*clutch_left_filter(i-1)-denclutch(3)*clutch_left_filter(i-2);
   %clutch_right_filter(i) = numclutch(1)*clutch_right(i)+numclutch(2)*clutch_right(i-1)+numclutch(3)*clutch_right(i-2)-denclutch(2)*clutch_right_filter(i-1)-denclutch(3)*clutch_right_filter(i-2);
%end
