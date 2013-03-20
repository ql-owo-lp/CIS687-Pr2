///////////////////////////////////////////////////////////////
// This file is a designed test file.  It may not pass the compile. 
// I try to include every possible situation here, to test the correctness of
// my actions and rules
// The codes are formatted in strange way on purpose
// - Kevin Wang  2013 Spring

#include "B.h"
#include "C.h"

void B::doNothing () {
}

template <V,E>
void B::doNothing() {
}

template <V,E> B::~B() {
}

#endif