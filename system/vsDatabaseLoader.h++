// File vsDatabaseLoader.h++

#ifndef VS_DATABASE_LOADER_HPP
#define VS_DATABASE_LOADER_HPP

#include <Performer/pf/pfNode.h>
#include <Performer/pfdb/pfflt.h>
#include "vsNode.h++"
#include "vsGrowableArray.h++"

enum vsDatabaseType
{
    VS_DATABASE_TYPE_DEFAULT,
    VS_DATABASE_TYPE_FLT
};

enum vsDatabaseUnits
{
    VS_DATABASE_UNITS_METERS,
    VS_DATABASE_UNITS_FEET,
    VS_DATABASE_UNITS_KILOMETERS
};

struct vsdbMatrixBlock
{
    char        magicString[4];
    pfMatrix    aboveMatrix;
    pfMatrix    belowMatrix;
};

class vsDatabaseLoader
{
private:

    vsGrowableArray    extensions;
    int                extensionCount;
    
    vsGrowableArray    nodeNames;
    int                nodeNameCount;
    
    int                unitMode;

    int                inittedFlag;
    
    int                classifyExtension(char *name);

    void               fixPerformerFltDOF(pfNode *node);
    void               fixGeodes(pfNode *targetGraph);
    void               replaceBillboards(pfNode *targetGraph);

VS_INTERNAL:

    void           init();
    
    static void    fltLoaderCallback(pfNode *node, int mgOp, int *cbs,
                                     COMMENTcb *comment, void *userData);

    int            checkName(const char *possibleName);

public:

                vsDatabaseLoader(char *fileExtension);
                ~vsDatabaseLoader();

    void        addExtension(char *fileExtension);
    
    void        addImportantNodeName(char *newName);
    void        clearNames();
    
    void        setUnits(int databaseUnit);
    
    void        addPath(char *filePath);
    
    vsNode      *loadDatabase(char *databaseFilename);
};

#endif
