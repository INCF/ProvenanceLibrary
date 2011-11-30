//
//  provenance.h
//  libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef libprov_provenance_h
#define libprov_provenance_h

typedef struct {
  int id;
  void* private;
}  Provenance;

typedef Provenance Prov, *ProvPtr;
typedef const char *IDREF;

/*Initialization and Cleanup routines*/
ProvPtr create_provenance_object(int);
int destroy_provenance_object(ProvPtr);

/*IO routines*/
void print_provenance(ProvPtr, const char*);

/* Record creation routines */
IDREF add_entity(ProvPtr);
IDREF add_activity(ProvPtr, const char* recipeLink, const char* startTime, const char* endTime);
IDREF add_agent(ProvPtr p_prov, IDREF);
IDREF add_note(ProvPtr p_prov, IDREF);

/* Record manipulation routines */
int add_attribute(ProvPtr, IDREF, const char* key, const char* value);
int set_property(ProvPtr, IDREF, const char* name, const char* value);

#endif
