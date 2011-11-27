//
//  testprov.c
//  libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "provenance.h"

/* A simple example on using the provenance library
*/
int
main(int argc, char **argv)
{
    ProvPtr prov_ptr = create_provenance_object(1);
    print_provenance(prov_ptr);
    destroy_provenance_object(prov_ptr);
    return(0);
}