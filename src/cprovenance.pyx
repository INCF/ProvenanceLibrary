cimport cprovenance

cdef class Prov:
    cdef cprovenance.Provenance *p_prov
    cdef cprovenance.RecordPtr p_record
    cdef cprovenance.IDREF act_id, id, agent_id
    cdef public char *prov_id
    def __cinit__(self, char* id):
        self.p_prov = cprovenance.newProvenanceFactory(id)
        self.prov_id = id
        if self.p_prov is NULL:
            raise MemoryError()
    def __dealloc__(self):
        if self.p_prov is not NULL:
            delProvenanceFactory(self.p_prov)
    cpdef addNamespace(self, char* href, char* prefix):
        """ append a namespace uri and prefix
        """
        cprovenance.addNamespace(self.p_prov, href, prefix)
    cpdef print_provenance(self, char* filename):
        """ print out the current provenance record to a file
        """
        cprovenance.print_provenance(self.p_prov, filename)
    cpdef delProvenanceFactory(self):
        """ delete the provenance instance
        """
        cprovenance.delProvenanceFactory(self.p_prov)
    cpdef newRecord(self):
        """ create a new record element
        """
        self.p_record = cprovenance.newRecord(self.p_prov)
    # these are all IDREF
    cpdef newEntity(self):
        """ create a new Entity element
        """
        cprovenance.newEntity(self.p_record)
    cpdef char newActivity(self, char* recipeLink, char* startTime, char* endTime):
        """ create a new Activity element
        """
        self.act_id = cprovenance.newActivity(self.p_record, recipeLink, startTime, endTime)
        return <char>self.act_id
    cpdef newAgent(self):
        """ create a new Agent element
        """
        self.agent_id = cprovenance.newAgent(self.p_record)
    cpdef newNote(self):
        """ create a new Note element
        """
        cprovenance.newNote(self.p_record)
    cpdef newUsedRecord(self, char* time):
        """ create a new UsedRecord element
        """
        cprovenance.newUsedRecord(self.p_record, self.act_id, self.id, time)
    cpdef newGeneratedByRecord(self, char* time):
        """ create a new GeneratedByRecord element
        """
        cprovenance.newGeneratedByRecord(self.p_record, self.id, self.act_id, time)
    cpdef newControlledByRecord(self, char* startTime, char* endTime):
        """ create a new ControlledByRecord element
        """
        cprovenance.newControlledByRecord(self.p_record, self.act_id, self.agent_id, startTime, endTime)
#    def newDerivedFromRecord(self, char* entity_effect, char* entity_cause):
#        cprovenance.newDerivedFromRecord(self.p_record, sel)
#    def newComplementOfRecord(RecordPtr p_rec, char* entity_effect, char* entity_cause)
    cpdef newAssociatedWithRecord(self, char* startTime, char* endTime):
        """ create a new AssociatedWithRecord element
        """
        cprovenance.newAssociatedWithRecord(self.p_record, self.act_id, self.agent_id, startTime, endTime)
#    def newInformedByRecord(RecordPtr p_rec, char* activity_effect, char* activity_cause, char* time)
#    def newHasAnnotationRecord(RecordPtr p_rec, char* thing, char* note)

    #these are both int
    cpdef add_child_element(self, char* key, char* value):
        """ add a child element to the current Entity element
        """
        cprovenance.add_child_element(self.p_record, self.id, key, value )
    cpdef add_attribute(self, char* name, char* value):
        """ add an attribute to the current Entity element
        """
        cprovenance.add_attribute(self.p_record, self.id, name, value)

# Not implemented - need a way to create these (i.e., newEntityEffect, newThing, etc.)
# entity_effect, entity_cause, activity_effect, activity_cause, thing, note