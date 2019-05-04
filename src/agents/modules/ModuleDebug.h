//
//  ModuleDebug.h
//  SUNA
//
//  Created by RHVT on 23/3/19.
//  Copyright © 2019 RHVT. All rights reserved.
//

#ifndef ModuleDebug_hpp
#define ModuleDebug_hpp

#include <stdio.h>
#include "Module.h"
#include "../../parameters.h"

class ModuleDebug
{
private:
    ModuleDebug();
    ~ModuleDebug();
public:
    static void printInformationFlowGraph(Module *mod, const char *filename);
    static void printGraph(Module *mod, const char *filename);
    static void printVars(Module *mod);
    static void printDNA(Module *mod);
    static void printDNA(Module *mod, const char *filename);
    static void printInternalStates(Module *mod);
    static void printFiredStates(Module *mod);
};

#endif /* ModuleDebug_hpp */
