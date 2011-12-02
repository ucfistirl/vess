#ifndef VS_COLLADA_DOCUMENT_HPP
#define VS_COLLADA_DOCUMENT_HPP

#include "atXMLDocument.h++"
#include "atList.h++"
#include "atMap.h++"
#include "atString.h++"
#include "atStringTokenizer.h++"
#include "vsComponent.h++"
#include "vsSkeleton.h++"
#include "vsSkeletonKinematics.h++"
#include "vsPathMotionManager.h++"
#include "vsList.h++"
#include "vsCharacter.h++"
#include "vsCOLLADAAnimation.h++"
#include "vsCOLLADADataSource.h++"
#include "vsCOLLADAEffectParameter.h++"
#include "vsCOLLADAFixedEffect.h++"
#include "vsCOLLADAController.h++"
#include "vsCOLLADAGeometry.h++"
#include "vsCOLLADANode.h++"
#include "vsCOLLADASkin.h++"

class VESS_SYM vsCOLLADADocument : public vsObject
{
protected:

    atList         *pathList;

    atString       documentPath;

    vsComponent    *sceneRoot;
    vsCharacter    *sceneCharacter;

    vsList         *skeletonList;
    vsList         *skinList;

    vsList         *animationList;

    double         unitScale;
    atMatrix       upAxisTransform;

    u_long         unnamedCount;

    atMap          *skeletonRoots;
    atMap          *skeletons;
    atMap          *animations;
    bool           parsingSkeleton;

    atMap          *animationLibrary;
    atMap          *animationClipLibrary;
    atMap          *cameraLibrary;
    atMap          *controllerLibrary;
    atMap          *effectLibrary;
    atMap          *geometryLibrary;
    atMap          *imageLibrary;
    atMap          *lightLibrary;
    atMap          *materialLibrary;
    atMap          *nodeLibrary;
    atMap          *visualSceneLibrary;


    atString               getUnnamedName();

    atString               findFile(const char *filename);

    vsCOLLADADataSource    *getDataSource(atString id);

    vsCOLLADAEffect        *getEffect(atString id);
    vsCOLLADAEffect        *getMaterial(atString id);
    vsCOLLADAController    *getController(atString id);
    vsCOLLADAGeometry      *getGeometry(atString id);
    vsCOLLADANode          *getVisualScene(atString id);

    vsCOLLADANode          *getSkeletonRoot(atString id);
    bool                   isSkeletonRoot(vsComponent *node);
    vsSkeleton             *getSkeleton(atString id);

    int                    getIntToken(atStringTokenizer *tokens);
    double                 getFloatToken(atStringTokenizer *tokens);
    atVector               parseVector(atXMLDocument *doc,
                                       atXMLDocumentNodePtr text, int size);

    void                   unrefDeleteMap(atMap *map);
    void                   unrefDeleteTreeMap(atMap *map);


    void           processAnimation(atXMLDocument *doc,
                                    atXMLDocumentNodePtr current);
    void           processLibraryAnimations(atXMLDocument *doc,
                                            atXMLDocumentNodePtr current);

    void           processLibraryAnimationClips(atXMLDocument *doc,
                                                atXMLDocumentNodePtr current);

    void           processLibraryCameras(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current);

    void           processController(atXMLDocument *doc,
                                     atXMLDocumentNodePtr current);
    void           processLibraryControllers(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current);

    void           processSampler2D(atXMLDocument *doc,
                                    atXMLDocumentNodePtr current,
                                    vsCOLLADAEffectParameter *param);

    vsCOLLADAEffectParameter *processNewParam(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current);

    void           processMaterialAttributes(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current,
                                             vsCOLLADAFixedEffect *effect);
    void           processCommonEffect(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current,
                                       atString *id);
    void           processGLSLEffect(atXMLDocument *doc,
                                     atXMLDocumentNodePtr current);
    void           processEffect(atXMLDocument *doc,
                                 atXMLDocumentNodePtr current);
    void           processLibraryEffects(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current);

    void           processLibraryGeometries(atXMLDocument *doc,
                                            atXMLDocumentNodePtr current);

    void           processImage(atXMLDocument *doc,
                                atXMLDocumentNodePtr current);
    void           processLibraryImages(atXMLDocument *doc,
                                        atXMLDocumentNodePtr current);

    void           processLight(atXMLDocument *doc,
                                atXMLDocumentNodePtr current);
    void           processLibraryLights(atXMLDocument *doc,
                                        atXMLDocumentNodePtr current);

    void           processSetParam(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current,
                                   vsCOLLADAEffect *effect);
    void           processMaterial(atXMLDocument *doc,
                                   atXMLDocumentNodePtr current);
    void           processLibraryMaterials(atXMLDocument *doc,
                                           atXMLDocumentNodePtr current);
    void           processInstanceMaterial(atXMLDocument *doc,
                                           atXMLDocumentNodePtr current,
                                           vsComponent *geomComponent);
    void           processBindMaterial(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current,
                                       vsComponent *geomComponent);

    vsSkeleton     *createSkeleton(vsCOLLADANode *root, vsCOLLADASkin *skin);
    void           processInstanceController(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current,
                                             vsCOLLADANode *parent);
    void           processInstanceGeometry(atXMLDocument *doc,
                                           atXMLDocumentNodePtr current,
                                           vsCOLLADANode *parent);
    void           processNodePass1(atXMLDocument *doc,
                                    atXMLDocumentNodePtr current,
                                    vsCOLLADANode *parent);
    void           processLOD(char *properties, vsCOLLADANode *node);
    void           processNodeFCTechnique(atXMLDocument *doc,
                                          atXMLDocumentNodePtr current,
                                          vsCOLLADANode *node);
    void           processNodeExtra(atXMLDocument *doc,
                                    atXMLDocumentNodePtr current,
                                    vsCOLLADANode *node);
    void           processNodePass2(atXMLDocument *doc,
                                    atXMLDocumentNodePtr current,
                                    vsCOLLADANode *parent,
                                    int thisIndex);
    void           processLibraryNodes(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current);

    void           processVisualScene(atXMLDocument *doc,
                                      atXMLDocumentNodePtr current);
    void           processLibraryVisualScenes(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current);

    void           addChannelsFromAnimation(vsList *list,
                                            vsCOLLADAAnimation *anim);
    void           buildAnimations(vsList *skeletonList,
                                   vsList *skelKinList);
    void           buildCharacter(vsCOLLADANode *sceneRootNode,
                                  atMatrix sceneMat);
    void           processScene(atXMLDocument *doc,
                                atXMLDocumentNodePtr current);

    void           processAsset(atXMLDocument *doc,
                                atXMLDocumentNodePtr current);

    void           parseDocument(atXMLDocument *doc);

public:

                          vsCOLLADADocument(atXMLDocument *doc,
                                            atList *paths);
    virtual               ~vsCOLLADADocument();

    virtual const char    *getClassName();

    vsComponent           *getScene();

    vsCharacter           *getCharacter();
};


#endif
