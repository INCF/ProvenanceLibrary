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

ProvPtr create_provenance_object(int);
int destroy_provenance_object(ProvPtr);
void print_provenance(ProvPtr);

#endif
