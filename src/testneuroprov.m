loadlibrary('libneuroprov.so','neuroprovenance.h');

p_prov = calllib('libneuroprov','newProvenanceObject','BET');

ProcessType = 'ni:brain_extraction';
StartTime = datestr(now);
pause(1)
EndTime = datestr(now);
p_proc = calllib('libneuroprov','newProcess',p_prov,StartTime,EndTime,ProcessType);

InputType = 'NIFTI Image #1';
InputName = 'FileName';
InputValue = '???';
input_id = calllib('libneuroprov','newProcessInput',p_prov,p_proc,InputName,InputValue,InputType);

InputType = 'NIFTI Image #2';
InputName = 'FileName';
InputValue = '???';
input_id = calllib('libneuroprov','newProcessInput',p_prov,p_proc,InputName,InputValue,InputType);

calllib('libneuroprov','freeProcess',p_proc);

calllib('libneuroprov','printProvenance',p_prov, 'MATLABtestneuroprov.xml');
calllib('libneuroprov','delProvenanceObject',p_prov);