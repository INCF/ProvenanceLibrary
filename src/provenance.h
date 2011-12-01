//
//  provenance.h
//  libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 TankThink Labs LLC. All rights reserved.
//

#ifndef libprov_provenance_h
#define libprov_provenance_h

typedef struct {
  char* id;
  void* private;
}  Provenance;

typedef Provenance Prov, *ProvPtr;
typedef const char *IDREF;

/*Initialization and Cleanup routines*/
ProvPtr create_provenance_object(const char*);
int destroy_provenance_object(ProvPtr);

/*IO routines*/
void print_provenance(ProvPtr, const char*);

/* Record creation routines */
IDREF add_entity(ProvPtr);
IDREF add_activity(ProvPtr, const char* recipeLink, const char* startTime, const char* endTime);
IDREF add_agent(ProvPtr);
IDREF add_note(ProvPtr);

/* Dependency routines */
IDREF add_usedRecord(ProvPtr p_prov, IDREF activity, IDREF entity, const char* time);
IDREF add_generatedByRecord(ProvPtr p_prov, IDREF entity, IDREF activity, const char* time);
IDREF add_controlledByRecord(ProvPtr p_prov, IDREF activity, IDREF agent, const char* startTime, const char* endTime);
IDREF add_DerivedFromRecord(ProvPtr p_prov, IDREF entity_effect, IDREF entity_cause);
IDREF add_informedByRecord(ProvPtr p_prov, IDREF activity_effect, IDREF activity_cause, const char* time);
IDREF add_hasAnnotationRecord(ProvPtr p_prov, IDREF thing, IDREF note);

/* Record manipulation routines */
int add_attribute(ProvPtr, IDREF, const char* key, const char* value);
int set_property(ProvPtr, IDREF, const char* name, const char* value);

#endif
