// File vsDatabaseLoader.h++

#ifndef VS_DATABASE_LOADER_HPP
#define VS_DATABASE_LOADER_HPP

#include <string.h>
#include <Performer/pf/pfGeode.h>
#include <Performer/pf/pfBillboard.h>
#include <Performer/pf/pfDCS.h>
#include <Performer/pfdu.h>
#include <Performer/pfdb/pfflt.h>

#include "vsNode.h++"

#define VS_DBL_MAX_EXT_COUNT  5
#define VS_DBL_MAX_NAME_COUNT 200

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

    char        *extensions[VS_DBL_MAX_EXT_COUNT];
    int         extensionCount;
    
    char        *nodeNames[VS_DBL_MAX_NAME_COUNT];
    int         nodeNameCount;
    
    int         unitMode;

    int         inittedFlag;
    
    int         classifyExtension(char *name);

    void        fixPerformerFltDOF(pfNode *node);
    void        fixGeodes(pfNode *targetGraph);
    void        replaceBillboards(pfNode *targetGraph);

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
    
    void	addPath(char *filePath);
    
    vsNode      *loadDatabase(char *databaseFilename);
};

#endif
