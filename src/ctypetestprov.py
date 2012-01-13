import provenance as prov

p_prov = prov.newProvenanceFactory('1')

p_record = prov.RecordPtr(prov.newRecord(p_prov))

act_id = prov.IDREF(prov.newActivity(p_record,'', "11/30/11 00:13:20.650432 EST", "11/30/11 00:13:20.650550 EST"))

prov.add_child_element(p_record, act_id, "type", "program")
prov.add_child_element(p_record, act_id, "name", "freesurfer")
prov.add_child_element(p_record, act_id, "version", "5.1")
prov.add_child_element(p_record, act_id, "cmdline", "foo")

id = prov.IDREF(prov.newEntity(p_record))
prov.add_child_element(p_record, id, "type", "input")
prov.newUsedRecord(p_record, act_id, id, '')

id = prov.newEntity(p_record)
prov.add_child_element(p_record, id, "type", "environment")

id = prov.newEntity(p_record)
prov.add_child_element(p_record, id, "type", "runtime")

id = prov.newEntity(p_record)
prov.add_child_element(p_record, id, "type", "output:file")
prov.add_child_element(p_record, id, "warped_file", "/full/path/to/file")
prov.newGeneratedByRecord(p_record, id, act_id, '')

id = prov.newEntity(p_record)
prov.add_child_element(p_record, id, "type", "output:stat")
prov.add_child_element(p_record, id, "pearson_correlation_coefficient", ".234")
prov.newGeneratedByRecord(p_record, id, act_id, '')

prov.print_provenance(p_prov, "ctypetestprov.xml")
