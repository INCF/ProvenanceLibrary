cdef extern from "provenance.h":
    enum: libprov_provenance_h

    ctypedef void *RecordPtr
    ctypedef struct Provenance:
        char* id
        RecordPtr p_record
        void* private
    ctypedef Provenance *ProvPtr
    ctypedef char *IDREF
    ProvPtr newProvenanceFactory(char*)
    int delProvenanceFactory(ProvPtr)
    int addNamespace(ProvPtr, char* href, char* prefix)
    void print_provenance(ProvPtr, char*)
    RecordPtr newRecord(ProvPtr)
    IDREF newEntity(RecordPtr)
    IDREF newActivity(RecordPtr, char* recipeLink, char* startTime, char* endTime)
    IDREF newAgent(RecordPtr)
    IDREF newNote(RecordPtr)
    IDREF newUsedRecord(RecordPtr p_rec, IDREF activity, IDREF entity, char* time)
    IDREF newGeneratedByRecord(RecordPtr p_rec, IDREF entity, IDREF activity, char* time)
    IDREF newControlledByRecord(RecordPtr p_rec, IDREF activity, IDREF agent, char* startTime, char* endTime)
    IDREF newDerivedFromRecord(RecordPtr p_rec, IDREF entity_effect, IDREF entity_cause)
    IDREF newComplementOfRecord(RecordPtr p_rec, IDREF entity_effect, IDREF entity_cause)
    IDREF newAssociatedWithRecord(RecordPtr p_record, IDREF activity, IDREF agent, char* startTime, char* endTime)
    IDREF newInformedByRecord(RecordPtr p_rec, IDREF activity_effect, IDREF activity_cause, char* time)
    IDREF newHasAnnotationRecord(RecordPtr p_rec, IDREF thing, IDREF note)
    int add_child_element(RecordPtr p_record, char* id, char* key, char* value)
    int add_attribute(RecordPtr p_record, IDREF id, char* name, char* value)