#pragma once

#include <vector>

#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>

enum class ControlFamily
{
    None = 0,
    Row,
    Column,
    Slice
};

struct ControlMembership
{
    ControlFamily family;
    unsigned int controlIndex;
};


struct Membership
{
    // control[control index] -> opm cubelet index
    std::vector<std::vector<unsigned int>> row;
    std::vector<std::vector<unsigned int>> column;
    std::vector<std::vector<unsigned int>> slice;

    // cubelet index -> control family & index
    std::vector<std::vector<ControlMembership>> cubelet;
};


struct CubeletData
{
    MMatrix rowOffset;
    MMatrix columnOffset;
    MMatrix sliceOffset;
};


struct ActiveControls
{
    std::vector<bool> row;
    std::vector<bool> column;
    std::vector<bool> slice;
};

struct TransientCache
{
    unsigned int size = 0;

    ControlFamily activeFamily;

    Membership membership;
    ActiveControls activeControls;
    std::vector<MMatrix> cubeletOffsets;

    std::vector<double> rowCachedRotations;
    std::vector<double> columnCachedRotations;
    std::vector<double> sliceCachedRotations;

    std::vector<MMatrix> opm;

};


class RubiksCubeNode : public MPxNode
{
public:

    RubiksCubeNode() = default;
    ~RubiksCubeNode() override = default;

    static void* creator();
    static MStatus initialize();

    MStatus compute(
        const MPlug& plug,
        MDataBlock& dataBlock) override;

    static MTypeId id;

    //=========================================================================
    // Inputs
    //=========================================================================

    static MObject aRigRoot;

    static MObject aSize;

    static MObject aRowRotation;
    static MObject aColumnRotation;
    static MObject aSliceRotation;

    //=========================================================================
    // Outputs / Persistent Cache
    //=========================================================================

    static MObject aCacheValid;

    static MObject aOutOffsetParentMatrix;

    static MObject aCachedRowRotation;
    static MObject aCachedColumnRotation;
    static MObject aCachedSliceRotation;

    static MObject aInitialRowRotation;
    static MObject aInitialColumnRotation;
    static MObject aInitialSliceRotation;

    static MObject aActiveFamily;


private:


    //=========================================================================
    // Initialization
    //=========================================================================

    MStatus initializeSolvedState(
        MDataBlock& dataBlock);

    MStatus onSizeChange(
        MDataBlock& dataBlock);

    MStatus loadTransientCache(
        MDataBlock& dataBlock);

    MStatus ensureArraySizes(
        MDataBlock& dataBlock,
        unsigned int size);

    MStatus ensureAngleArraySize(
        MArrayDataHandle& array,
        unsigned int requiredSize);

    MStatus ensureMatrixArraySize(
        MArrayDataHandle& array,
        unsigned int requiredSize);


    //=========================================================================
    // Solved State
    //=========================================================================

    std::vector<MPoint> getSolvedCubeletPositions(
        unsigned int size);

    std::vector<MMatrix> buildSolvedCubeletMatrices(
        const std::vector<MPoint>& positions) const;


    //=========================================================================
    // Attribute Access
    //=========================================================================

    MObject getCacheRotAttr(
        ControlFamily family) const;

    MObject getInitialRotAttr(
        ControlFamily family) const;

    ControlFamily getActiveFamily(
        MDataBlock& dataBlock) const;

    MStatus setActiveFamily(
        ControlFamily family,
        MDataBlock& dataBlock);

    double getControlRotation(
        ControlFamily family,
        unsigned int controlIndex,
        MDataBlock& dataBlock,
        bool useCachedRotation) const;

    MStatus setCachedControlRotation(
        ControlFamily family,
        unsigned int controlIndex,
        double rotation,
        MDataBlock& dataBlock);

    std::vector<double> getCachedControlRotations(
        ControlFamily family,
        MDataBlock& dataBlock) const;

    std::vector<double> getInitialControlRotations(
        ControlFamily family,
        MDataBlock& dataBlock) const;

    MStatus setInitialControlRotations(
        const std::vector<double>& rotations,
        ControlFamily family,
        MDataBlock& dataBlock);

    std::vector<double> getLiveControlRotations(
        ControlFamily family,
        MDataBlock& dataBlock) const;

    MStatus getLiveControlRotations(
        std::vector<double>& rowRotations,
        std::vector<double>& columnRotations,
        std::vector<double>& sliceRotations,
        MDataBlock& dataBlock) const;

    MStatus setCachedControlRotations(
        const std::vector<double>& rowCache,
        const std::vector<double>& columnCache,
        const std::vector<double>& sliceCache,
        MDataBlock& dataBlock);

    MStatus setInitialControlRotations(
        const std::vector<double>& rowRotations,
        const std::vector<double>& columnRotations,
        const std::vector<double>& sliceRotations,
        MDataBlock& dataBlock);

    void updateCachedControlRotations(
        std::vector<double>& rowCache,
        std::vector<double>& columnCache,
        std::vector<double>& sliceCache,
        MDataBlock& dataBlock) const;

    MMatrix getCubeletOPM(
        unsigned int opmIndex,
        MDataBlock& dataBlock) const;

    MStatus setCubeletOPM(
        unsigned int opmIndex,
        const MMatrix& matrix,
        MDataBlock& dataBlock);

    std::vector<MMatrix> getCubeletMatrices(
        MDataBlock& dataBlock);

    MStatus setCubeletOPMs(
        const std::vector<MMatrix>& cubeletMatrices,
        MDataBlock& dataBlock);


    //=========================================================================
    // Cache Management
    //=========================================================================

    void resizeMembershipCache(
        unsigned int size);

    void resizeActiveControls(
        unsigned int size);

    void resizeOffsetCache(
        unsigned int size);

    void resizeCachedRotations(
        unsigned int size);

    void resizeOPMCache(
        unsigned int size);

    void clearMembershipCache();

    void clearOffsetCache();

    MStatus buildMembershipCache(
        const std::vector<MMatrix>& cubeletMatrices,
        unsigned int size);

    MStatus buildOffsetCache(
        const std::vector<MMatrix>& cubeletMatrices,
        ControlFamily family,
        const std::vector<double>& controlRotations);

    void calculateOffset(
        unsigned int cubeletIndex,
        const MMatrix& cubeletMatrix,
        const MMatrix& controlMatrix);

    MStatus buildActiveControlsCache(
        unsigned int size,
        MDataBlock& dataBlock);


    //=========================================================================
    // Evaluation
    //=========================================================================

    bool isTurning(
        ControlFamily family,
        unsigned int controlIndex,
        MDataBlock& dataBlock) const;

    ControlFamily acquireActiveFamily(
        const std::vector<double>& rowRotations,
        const std::vector<double>& columnRotations,
        const std::vector<double>& sliceRotations,
        MDataBlock& dataBlock) const;

    void updateActiveFamily(
        ControlFamily& activeFamily) const;

    void determineCachedRotation(
        ControlFamily family,
        std::vector<double>& activeCache,
        const std::vector<double>& rowCache,
        const std::vector<double>& colCache,
        const std::vector<double>& sliceCache);

    MStatus updateCubeletOPMs(
        std::vector<MMatrix>& cubeletMatrices,
        const std::vector<double>& controlRotations,
        ControlFamily activeFamily) const;


    //=========================================================================
    // Math / Utility
    //=========================================================================

    MMatrix controlRotationMatrix(
        ControlFamily family,
        MAngle rotation) const;

    static double coordinateFromIndex(
        int index,
        int size);

    static int getControlIndex(
        double coordinate,
        int size);

    unsigned int getCubeletCount(
        unsigned int size);

    TransientCache m_cache;
    bool m_loadedCache = false;
    ActiveControls m_activeControls;

};
