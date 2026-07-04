#include "RubiksCubeNode.h"

#include <numbers>
#include <unordered_map>
#include <vector>

#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MAngle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MPoint.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include <maya/MPlug.h>
#include <maya/MDataBlock.h>

#include <maya/MGlobal.h>

MTypeId RubiksCubeNode::id(0x00142440);

// Inputs

MObject RubiksCubeNode::aRigRoot;

MObject RubiksCubeNode::aCacheValid;

MObject RubiksCubeNode::aSize;

MObject RubiksCubeNode::aRowRotation;
MObject RubiksCubeNode::aColumnRotation;
MObject RubiksCubeNode::aSliceRotation;


// Outputs

MObject RubiksCubeNode::aOutOffsetParentMatrix;

// Cache

MObject RubiksCubeNode::aCachedColumnRotation;
MObject RubiksCubeNode::aCachedRowRotation;
MObject RubiksCubeNode::aCachedSliceRotation;

MObject RubiksCubeNode::aInitialRowRotation;
MObject RubiksCubeNode::aInitialColumnRotation;
MObject RubiksCubeNode::aInitialSliceRotation;

MObject RubiksCubeNode::aActiveFamily;

void* RubiksCubeNode::creator()
{
    return new RubiksCubeNode();
}

MStatus RubiksCubeNode::initialize()
{
    MStatus status;

    MFnNumericAttribute nAttr;
    MFnMatrixAttribute  mAttr;
    MFnUnitAttribute    uAttr;
    MFnEnumAttribute    eAttr;
    MFnMessageAttribute msgAttr;


    aRigRoot =
        msgAttr.create(
            "rigRoot",
            "root");

    msgAttr.setHidden(false);
    msgAttr.setReadable(false);
    msgAttr.setWritable(true);
    msgAttr.setStorable(true);
    msgAttr.setConnectable(true);
    msgAttr.setKeyable(false);
    msgAttr.setChannelBox(false);


    aCacheValid = nAttr.create(
        "cacheValid",
        "cache",
        MFnNumericData::kBoolean,
        false);

    nAttr.setHidden(true);
    nAttr.setStorable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setConnectable(false);
    nAttr.setKeyable(false);
    nAttr.setChannelBox(false);

    aSize = nAttr.create(
        "size",
        "size",
        MFnNumericData::kInt,
        1,
        &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    nAttr.setMin(1);

    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setConnectable(false);

    //=====================================================================
    // Row Controls
    //=====================================================================

    aRowRotation =
        uAttr.create(
            "rowRotation",
            "rowRot",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setReadable(false);
    uAttr.setWritable(true);
    uAttr.setConnectable(true);
    uAttr.setStorable(false);



    //=====================================================================
    // Column Controls
    //=====================================================================

    aColumnRotation =
        uAttr.create(
            "columnRotation",
            "colRot",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setReadable(false);
    uAttr.setWritable(true);
    uAttr.setConnectable(true);
    uAttr.setStorable(false);


    //=====================================================================
    // Slice Controls
    //=====================================================================

    aSliceRotation =
        uAttr.create(
            "sliceRotation",
            "sliceRot",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setReadable(false);
    uAttr.setWritable(true);
    uAttr.setConnectable(true);
    uAttr.setStorable(false);


    //=====================================================================
    // Outputs / Persistent Cache
    //=====================================================================

    aOutOffsetParentMatrix = mAttr.create(
        "outOffsetParentMatrix",
        "opm",
        MFnMatrixAttribute::kDouble,
        &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    mAttr.setArray(true);
    mAttr.setUsesArrayDataBuilder(true);

    mAttr.setReadable(true);
    mAttr.setWritable(true);
    mAttr.setStorable(true);

    aCachedRowRotation =
        uAttr.create(
            "cachedRowRotation",
            "rowCache",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setHidden(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setKeyable(false);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setConnectable(false);

    aCachedColumnRotation =
        uAttr.create(
            "cachedColumnRotation",
            "colCache",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setHidden(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setKeyable(false);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setConnectable(false);


    aCachedSliceRotation =
        uAttr.create(
            "cachedSliceRotation",
            "sliceCache",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setHidden(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setKeyable(false);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setConnectable(false);

    aInitialRowRotation =
        uAttr.create(
            "initialRowRotation",
            "rowInit",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setHidden(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setKeyable(false);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setConnectable(false);


    aInitialColumnRotation =
        uAttr.create(
            "initialColumnRotation",
            "colInit",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setHidden(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setKeyable(false);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setConnectable(false);


    aInitialSliceRotation =
        uAttr.create(
            "initialSliceRotation",
            "sliceInit",
            MFnUnitAttribute::kAngle,
            0.0);

    uAttr.setArray(true);
    uAttr.setHidden(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setKeyable(false);
    uAttr.setUsesArrayDataBuilder(true);
    uAttr.setConnectable(false);

    aActiveFamily =
        eAttr.create(
            "activeFamily",
            "activeFamily",
            static_cast<short>(ControlFamily::None),
            &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    eAttr.addField("None", static_cast<short>(ControlFamily::None));
    eAttr.addField("Row", static_cast<short>(ControlFamily::Row));
    eAttr.addField("Column", static_cast<short>(ControlFamily::Column));
    eAttr.addField("Slice", static_cast<short>(ControlFamily::Slice));

    eAttr.setReadable(true);
    eAttr.setWritable(true);
    eAttr.setStorable(true);
    eAttr.setKeyable(false);
    eAttr.setHidden(true);


    //=====================================================================
    // Add Attributes
    //=====================================================================

    addAttribute(aRigRoot);

    addAttribute(aCacheValid);

    addAttribute(aSize);

    addAttribute(aActiveFamily);

    addAttribute(aRowRotation);
    addAttribute(aColumnRotation);
    addAttribute(aSliceRotation);

    addAttribute(aOutOffsetParentMatrix);

    addAttribute(aCachedRowRotation);
    addAttribute(aCachedColumnRotation);
    addAttribute(aCachedSliceRotation);

    addAttribute(aInitialRowRotation);
    addAttribute(aInitialColumnRotation);
    addAttribute(aInitialSliceRotation);

    //=====================================================================
    // Attribute Affects
    //=====================================================================

    attributeAffects(aSize, aOutOffsetParentMatrix);

    attributeAffects(aRowRotation, aOutOffsetParentMatrix);
    attributeAffects(aColumnRotation, aOutOffsetParentMatrix);
    attributeAffects(aSliceRotation, aOutOffsetParentMatrix);

    return MS::kSuccess;
}

MStatus RubiksCubeNode::compute(
    const MPlug& plug,
    MDataBlock& dataBlock)
{
    const unsigned int size =
        static_cast<unsigned int>(
            dataBlock.inputValue(
                aSize).asInt());

    bool cacheValid = dataBlock.outputValue(aCacheValid).asBool();

    if (!cacheValid)
    {
        CHECK_MSTATUS(
            loadTransientCache(
                dataBlock));

        dataBlock.outputValue(aCacheValid).setBool(true);
    }

    if (m_cache.size != size)
    {
        CHECK_MSTATUS(
            onSizeChange(
                dataBlock));
    }

    std::vector<double> activeCacheRotation;
    std::vector<double> activeLiveRotation;

    std::vector<double> liveRowRotation;
    std::vector<double> liveColRotation;
    std::vector<double> liveSliceRotation;

    getLiveControlRotations(
        liveRowRotation,
        liveColRotation,
        liveSliceRotation,
        dataBlock);

    if (m_cache.activeFamily == ControlFamily::None)
    {

        m_cache.activeFamily = acquireActiveFamily(
            m_cache.rowCachedRotations,
            m_cache.columnCachedRotations,
            m_cache.sliceCachedRotations,
            dataBlock);

        if (m_cache.activeFamily == ControlFamily::None)
        {
            return MS::kSuccess;
        }

        determineCachedRotation(
            m_cache.activeFamily,
            activeCacheRotation,
            m_cache.rowCachedRotations,
            m_cache.columnCachedRotations,
            m_cache.sliceCachedRotations);

        CHECK_MSTATUS(
            buildOffsetCache(
                m_cache.opm,
                m_cache.activeFamily,
                activeCacheRotation));
    }
    else
    {
        determineCachedRotation(
            m_cache.activeFamily,
            activeCacheRotation,
            m_cache.rowCachedRotations,
            m_cache.columnCachedRotations,
            m_cache.sliceCachedRotations);
    }

    activeLiveRotation = getLiveControlRotations(m_cache.activeFamily, dataBlock);

    CHECK_MSTATUS(
        updateCubeletOPMs(
            m_cache.opm,
            activeLiveRotation,
            m_cache.activeFamily));

    CHECK_MSTATUS(
        buildActiveControlsCache(
            m_cache.size,
            dataBlock));

    updateActiveFamily(m_cache.activeFamily);


    if (m_cache.activeFamily != ControlFamily::None)
    {
        setActiveFamily(m_cache.activeFamily, dataBlock);

        setCachedControlRotations(
            m_cache.rowCachedRotations,
            m_cache.columnCachedRotations,
            m_cache.sliceCachedRotations,
            dataBlock);
        
        setCubeletOPMs(m_cache.opm, dataBlock);

        setInitialControlRotations(
            liveRowRotation,
            liveColRotation,
            liveSliceRotation,
            dataBlock);

        return MS::kSuccess;
    }

    updateCachedControlRotations(
        m_cache.rowCachedRotations,
        m_cache.columnCachedRotations,
        m_cache.sliceCachedRotations,
        dataBlock);

    CHECK_MSTATUS(
        buildMembershipCache(
            m_cache.opm,
            m_cache.size));
    
    m_cache.activeFamily = acquireActiveFamily(
        m_cache.rowCachedRotations,
        m_cache.columnCachedRotations,
        m_cache.sliceCachedRotations,
        dataBlock);


    if (m_cache.activeFamily != ControlFamily::None)
    {

        determineCachedRotation(
            m_cache.activeFamily,
            activeCacheRotation,
            m_cache.rowCachedRotations,
            m_cache.columnCachedRotations,
            m_cache.sliceCachedRotations);

        buildOffsetCache(
            m_cache.opm,
            m_cache.activeFamily,
            activeCacheRotation);

        activeLiveRotation = getLiveControlRotations(m_cache.activeFamily, dataBlock);

        CHECK_MSTATUS(
            updateCubeletOPMs(
                m_cache.opm,
                activeLiveRotation,
                m_cache.activeFamily));
    }

    setCachedControlRotations(
        m_cache.rowCachedRotations,
        m_cache.columnCachedRotations,
        m_cache.sliceCachedRotations,
        dataBlock);

    setActiveFamily(m_cache.activeFamily, dataBlock);

    setCubeletOPMs(m_cache.opm, dataBlock);

    setInitialControlRotations(
        liveRowRotation,
        liveColRotation,
        liveSliceRotation,
        dataBlock);

    return MS::kSuccess;
}



MMatrix RubiksCubeNode::getCubeletOPM(
    unsigned int opmIndex,
    MDataBlock& dataBlock) const
{
    MArrayDataHandle array =
        dataBlock.outputArrayValue(
            aOutOffsetParentMatrix);

    if (array.jumpToArrayElement(
        opmIndex) != MS::kSuccess)
    {
        return MMatrix::identity;
    }

    return array.outputValue()
        .asMatrix();
}

MStatus RubiksCubeNode::setCubeletOPM(
    unsigned int opmIndex,
    const MMatrix& matrix,
    MDataBlock& dataBlock)
{
    MArrayDataHandle array =
        dataBlock.outputArrayValue(
            aOutOffsetParentMatrix);

    if (array.jumpToArrayElement(
        opmIndex) != MS::kSuccess)
    {
        return MS::kFailure;
    }

    array.outputValue()
        .setMMatrix(matrix);

    return MS::kSuccess;
}

ControlFamily RubiksCubeNode::getActiveFamily(
    MDataBlock& dataBlock) const
{
    return static_cast<ControlFamily>(
        dataBlock.inputValue(
            aActiveFamily).asShort());
}

MStatus RubiksCubeNode::setActiveFamily(
    ControlFamily family,
    MDataBlock& dataBlock)
{
    MDataHandle handle =
        dataBlock.outputValue(
            aActiveFamily);

    handle.setShort(
        static_cast<short>(family));

    return MS::kSuccess;
}

MStatus RubiksCubeNode::setCachedControlRotation(
    ControlFamily family,
    unsigned int controlIndex,
    double rotation,
    MDataBlock& dataBlock)
{
    const MObject attribute =
        getCacheRotAttr(family);

    if (attribute.isNull())
    {
        return MS::kFailure;
    }

    MArrayDataHandle rotationArray =
        dataBlock.outputArrayValue(attribute);

    if (rotationArray.jumpToArrayElement(controlIndex)
        != MS::kSuccess)
    {
        MArrayDataBuilder builder =
            rotationArray.builder();

        builder.addElement(controlIndex);

        rotationArray.set(builder);

        if (rotationArray.jumpToArrayElement(controlIndex)
            != MS::kSuccess)
        {
            return MS::kFailure;
        }
    }

    rotationArray.outputValue().setMAngle(
        MAngle(
            rotation,
            MAngle::kRadians));

    return MS::kSuccess;
}

double RubiksCubeNode::getControlRotation(
    ControlFamily family,
    unsigned int controlIndex,
    MDataBlock& dataBlock,
    bool useCachedRotation) const
{
    MObject attribute;

    if (useCachedRotation)
    {
        attribute = getCacheRotAttr(family);
    }
    else
    {
        switch (family)
        {
        case ControlFamily::Row:
            attribute = aRowRotation;
            break;

        case ControlFamily::Column:
            attribute = aColumnRotation;
            break;

        case ControlFamily::Slice:
            attribute = aSliceRotation;
            break;

        default:
            attribute = MObject::kNullObj;
        }
    }

    if (attribute.isNull())
    {
        return 0.0;
    }

    MArrayDataHandle arrayHandle =
        dataBlock.inputArrayValue(attribute);

    if (arrayHandle.jumpToArrayElement(controlIndex)
        != MS::kSuccess)
    {
        return 0.0;
    }

    return arrayHandle.inputValue()
        .asAngle()
        .asRadians();
}

MObject RubiksCubeNode::getCacheRotAttr(ControlFamily family) const
{
    switch (family)
    {
    case ControlFamily::Row:
        return aCachedRowRotation;

    case ControlFamily::Column:
        return aCachedColumnRotation;

    case ControlFamily::Slice:
        return aCachedSliceRotation;

    default:
        return MObject::kNullObj;
    }
}



double RubiksCubeNode::coordinateFromIndex(
    int index,
    int size)
{
    if (size % 2)
    {
        return static_cast<double>(index)
            - static_cast<double>(size - 1) / 2.0;
    }

    return static_cast<double>(index)
        + 0.5
        - static_cast<double>(size) / 2.0;
}



unsigned int RubiksCubeNode::getCubeletCount(unsigned int size)
{
    return (size * size * size - (size - 2) * (size - 2) * (size - 2));
}

void RubiksCubeNode::resizeMembershipCache(unsigned int size)
{
    const unsigned int cubeletCount = getCubeletCount(size);

    m_cache.membership.row.assign(size, {});
    m_cache.membership.column.assign(size, {});
    m_cache.membership.slice.assign(size, {});

    m_cache.membership.cubelet.assign(cubeletCount, {});
}

void RubiksCubeNode::resizeOffsetCache(
    unsigned int size)
{
    m_cache.cubeletOffsets.assign(
        getCubeletCount(size),
        MMatrix::identity);
}


void RubiksCubeNode::resizeActiveControls(
    unsigned int size)
{
    m_cache.activeControls.row.assign(
        size,
        false);

    m_cache.activeControls.column.assign(
        size,
        false);

    m_cache.activeControls.slice.assign(
        size,
        false);
}

void RubiksCubeNode::clearMembershipCache()
{
    for (auto& row : m_cache.membership.row)
        row.clear();

    for (auto& column : m_cache.membership.column)
        column.clear();

    for (auto& slice : m_cache.membership.slice)
        slice.clear();

    for (auto& cubelet : m_cache.membership.cubelet)
        cubelet.clear();
}

void RubiksCubeNode::clearOffsetCache()
{
    std::fill(
        m_cache.cubeletOffsets.begin(),
        m_cache.cubeletOffsets.end(),
        MMatrix::identity);
}

MStatus RubiksCubeNode::loadTransientCache(
    MDataBlock& dataBlock)
{

    m_cache.size = static_cast<unsigned int>(
        dataBlock.inputValue(
            aSize).asInt());

    m_cache.activeFamily = getActiveFamily(dataBlock);

    m_cache.rowCachedRotations = getCachedControlRotations(ControlFamily::Row, dataBlock);
    m_cache.columnCachedRotations = getCachedControlRotations(ControlFamily::Column, dataBlock);
    m_cache.sliceCachedRotations = getCachedControlRotations(ControlFamily::Slice, dataBlock);

    m_cache.opm = getCubeletMatrices(dataBlock);

    resizeMembershipCache(m_cache.size);
    resizeActiveControls(m_cache.size);
    resizeOffsetCache(m_cache.size);


    CHECK_MSTATUS(
        buildMembershipCache(
            m_cache.opm,
            m_cache.size));

    std::vector<double> initialRot = getInitialControlRotations(
        m_cache.activeFamily,
        dataBlock);

    CHECK_MSTATUS(
        buildOffsetCache(
            m_cache.opm,
            m_cache.activeFamily,
            initialRot));

    CHECK_MSTATUS(
        buildActiveControlsCache(
            m_cache.size,
            dataBlock));

    return MS::kSuccess;
}

MStatus RubiksCubeNode::buildMembershipCache(
    const std::vector<MMatrix>& cubeletMatrices,
    unsigned int size)
{
    clearMembershipCache();

    const unsigned int cubeletCount = static_cast<unsigned int>(cubeletMatrices.size());

    for (unsigned int cubeletIndex = 0;
        cubeletIndex < cubeletCount;
        ++cubeletIndex)
    {
        const MMatrix& cubeletMatrix =
            cubeletMatrices[cubeletIndex];

        const int rowIndex =
            getControlIndex(
                cubeletMatrix[3][1],
                size);

        const int columnIndex =
            getControlIndex(
                cubeletMatrix[3][0],
                size);

        const int sliceIndex =
            getControlIndex(
                cubeletMatrix[3][2],
                size);

        if (rowIndex != -1)
        {
            m_cache.membership.row[rowIndex]
                .push_back(cubeletIndex);

            m_cache.membership.cubelet[cubeletIndex]
                .push_back(
                    {
                        ControlFamily::Row,
                        static_cast<unsigned int>(rowIndex)
                    });
        }

        if (columnIndex != -1)
        {
            m_cache.membership.column[columnIndex]
                .push_back(cubeletIndex);

            m_cache.membership.cubelet[cubeletIndex]
                .push_back(
                    {
                        ControlFamily::Column,
                        static_cast<unsigned int>(columnIndex)
                    });
        }

        if (sliceIndex != -1)
        {
            m_cache.membership.slice[sliceIndex]
                .push_back(cubeletIndex);

            m_cache.membership.cubelet[cubeletIndex]
                .push_back(
                    {
                        ControlFamily::Slice,
                        static_cast<unsigned int>(sliceIndex)
                    });
        }
    }

    return MS::kSuccess;
}

int RubiksCubeNode::getControlIndex(
    double coordinate,
    int size)
{
    constexpr double tolerance = 0.001;

    for (int index = 0;
        index < size;
        ++index)
    {
        const double validCoordinate =
            coordinateFromIndex(
                index,
                size);

        if (std::abs(
            coordinate - validCoordinate)
            <= tolerance)
        {
            return index;
        }
    }

    return -1;
}



void RubiksCubeNode::calculateOffset(
    unsigned int cubeletIndex,
    const MMatrix& cubeletMatrix,
    const MMatrix& controlMatrix)
{
    m_cache.cubeletOffsets[cubeletIndex] =
        cubeletMatrix *
        controlMatrix.inverse();
}

MStatus RubiksCubeNode::buildOffsetCache(
    const std::vector<MMatrix>& cubeletMatrices,
    ControlFamily family,
    const std::vector<double>& controlRotations)
{
    const std::vector<std::vector<unsigned int>>* memberships =
        nullptr;

    switch (family)
    {
    case ControlFamily::Row:
        memberships =
            &m_cache.membership.row;
        break;

    case ControlFamily::Column:
        memberships =
            &m_cache.membership.column;
        break;

    case ControlFamily::Slice:
        memberships =
            &m_cache.membership.slice;
        break;

    default:
        return MS::kSuccess;
    }

    for (unsigned int controlIndex = 0;
        controlIndex < memberships->size();
        ++controlIndex)
    {
        const MMatrix controlMatrix =
            controlRotationMatrix(
                family,
                MAngle(
                    controlRotations[controlIndex],
                    MAngle::kRadians));

        for (unsigned int cubeletIndex
            : (*memberships)[controlIndex])
        {
            calculateOffset(
                cubeletIndex,
                cubeletMatrices[cubeletIndex],
                controlMatrix);
        }
    }

    return MS::kSuccess;
}


MMatrix RubiksCubeNode::controlRotationMatrix(
    ControlFamily family,
    MAngle rotation) const
{
    MTransformationMatrix transform;

    switch (family)
    {
    case ControlFamily::Row:

        transform.rotateBy(
            MEulerRotation(
                0.0,
                rotation.asRadians(),
                0.0,
                MEulerRotation::kXYZ),
            MSpace::kTransform);

        break;

    case ControlFamily::Column:

        transform.rotateBy(
            MEulerRotation(
                rotation.asRadians(),
                0.0,
                0.0,
                MEulerRotation::kXYZ),
            MSpace::kTransform);

        break;

    case ControlFamily::Slice:

        transform.rotateBy(
            MEulerRotation(
                0.0,
                0.0,
                rotation.asRadians(),
                MEulerRotation::kXYZ),
            MSpace::kTransform);

        break;

    default:
        return MMatrix::identity;
    }

    return transform.asMatrix();
}

bool RubiksCubeNode::isTurning(
    ControlFamily family,
    unsigned int controlIndex,
    MDataBlock& dataBlock) const
{
    constexpr double quarterTurn =
        M_PI * 0.5;

    constexpr double tolerance =
        0.001;

    const double rotation =
        getControlRotation(
            family,
            controlIndex,
            dataBlock,
            false);

    const double nearestQuarterTurn =
        std::round(
            rotation /
            quarterTurn);

    return std::abs(
        rotation -
        nearestQuarterTurn *
        quarterTurn)
        > tolerance;
}

MStatus RubiksCubeNode::buildActiveControlsCache(
    unsigned int size,
    MDataBlock& dataBlock)
{
    for (unsigned int controlIndex = 0;
        controlIndex < size;
        ++controlIndex)
    {
        m_cache.activeControls.row[controlIndex] =
            isTurning(
                ControlFamily::Row,
                controlIndex,
                dataBlock);

        m_cache.activeControls.column[controlIndex] =
            isTurning(
                ControlFamily::Column,
                controlIndex,
                dataBlock);

        m_cache.activeControls.slice[controlIndex] =
            isTurning(
                ControlFamily::Slice,
                controlIndex,
                dataBlock);
    }

    return MS::kSuccess;
}

std::vector<MMatrix> RubiksCubeNode::getCubeletMatrices(
    MDataBlock& dataBlock)
{
    unsigned int size = m_cache.size;

    unsigned int cubeletCount = getCubeletCount(size);

    std::vector<MMatrix> cubeletMatrices(cubeletCount);

    for (unsigned int cubeletIndex = 0;
        cubeletIndex < cubeletCount;
        ++cubeletIndex)
    {
        cubeletMatrices[cubeletIndex] =
            getCubeletOPM(
                cubeletIndex,
                dataBlock);
    }

    return cubeletMatrices;
}

MStatus RubiksCubeNode::ensureArraySizes(
    MDataBlock& dataBlock,
    unsigned int size)
{
    const unsigned int cubeletCount = getCubeletCount(size);

    {
        MArrayDataHandle array =
            dataBlock.inputArrayValue(
                aRowRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    {
        MArrayDataHandle array =
            dataBlock.inputArrayValue(
                aColumnRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    {
        MArrayDataHandle array =
            dataBlock.inputArrayValue(
                aSliceRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    {
        MArrayDataHandle array =
            dataBlock.outputArrayValue(
                aCachedRowRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    {
        MArrayDataHandle array =
            dataBlock.outputArrayValue(
                aCachedColumnRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    {
        MArrayDataHandle array =
            dataBlock.outputArrayValue(
                aCachedSliceRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    {
        MArrayDataHandle array =
            dataBlock.outputArrayValue(
                aOutOffsetParentMatrix);

        ensureMatrixArraySize(
            array,
            cubeletCount);
    }

    {
        MArrayDataHandle array =
            dataBlock.outputArrayValue(
                aInitialRowRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    {
        MArrayDataHandle array =
            dataBlock.outputArrayValue(
                aInitialColumnRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    {
        MArrayDataHandle array =
            dataBlock.outputArrayValue(
                aInitialSliceRotation);

        ensureAngleArraySize(
            array,
            size);
    }

    return MS::kSuccess;
}

MStatus RubiksCubeNode::ensureMatrixArraySize(
    MArrayDataHandle& array,
    unsigned int requiredSize)
{
    MArrayDataBuilder builder =
        array.builder();

    for (unsigned int index = 0;
        index < requiredSize;
        ++index)
    {
        if (array.jumpToArrayElement(index) == MS::kSuccess)
        {
            continue;
        }

        MDataHandle handle =
            builder.addElement(index);

        handle.setMMatrix(
            MMatrix::identity);
    }

    array.set(builder);

    return MS::kSuccess;
}

MStatus RubiksCubeNode::ensureAngleArraySize(
    MArrayDataHandle& array,
    unsigned int requiredSize)
{
    MArrayDataBuilder builder =
        array.builder();

    for (unsigned int index = 0;
        index < requiredSize;
        ++index)
    {
        if (array.jumpToArrayElement(index) == MS::kSuccess)
        {
            continue;
        }

        MDataHandle handle =
            builder.addElement(index);

        handle.setMAngle(
            MAngle(0.0));
    }

    array.set(builder);

    return MS::kSuccess;
}


MStatus RubiksCubeNode::initializeSolvedState(
    MDataBlock& dataBlock)
{
    const unsigned int size =
        m_cache.size;

    const std::vector<MPoint> solvedPositions =
        getSolvedCubeletPositions(size);

    m_cache.opm = buildSolvedCubeletMatrices(solvedPositions);

    CHECK_MSTATUS(
        buildMembershipCache(
            m_cache.opm,
            size));

    std::vector<double> cacheControlRotations = getCachedControlRotations(m_cache.activeFamily, dataBlock);

    CHECK_MSTATUS(
        buildOffsetCache(
            m_cache.opm,
            m_cache.activeFamily,
            cacheControlRotations));

    if (m_cache.activeFamily == ControlFamily::None)
    {
        for (unsigned int cubeletIndex = 0;
            cubeletIndex < m_cache.opm.size();
            ++cubeletIndex)
        {
            CHECK_MSTATUS(
                setCubeletOPM(
                    cubeletIndex,
                    m_cache.opm[cubeletIndex],
                    dataBlock));
        }
    }
    else
    {
        const std::vector<std::vector<unsigned int>>* memberships =
            nullptr;

        switch (m_cache.activeFamily)
        {
        case ControlFamily::Row:
            memberships = &m_cache.membership.row;
            break;

        case ControlFamily::Column:
            memberships = &m_cache.membership.column;
            break;

        case ControlFamily::Slice:
            memberships = &m_cache.membership.slice;
            break;

        default:
            break;
        }

        if (memberships)
        {
            for (unsigned int controlIndex = 0;
                controlIndex < memberships->size();
                ++controlIndex)
            {
                const MMatrix controlMatrix =
                    controlRotationMatrix(
                        m_cache.activeFamily,
                        MAngle(
                            getControlRotation(
                                m_cache.activeFamily,
                                controlIndex,
                                dataBlock,
                                false),
                            MAngle::kRadians));

                for (unsigned int cubeletIndex
                    : (*memberships)[controlIndex])
                {
                    CHECK_MSTATUS(
                        setCubeletOPM(
                            cubeletIndex,
                            m_cache.cubeletOffsets[cubeletIndex] *
                            controlMatrix,
                            dataBlock));
                }
            }
        }
    }

    return MS::kSuccess;
}

MStatus RubiksCubeNode::onSizeChange(
    MDataBlock& dataBlock)
{
    const unsigned int size =
        static_cast<unsigned int>(
            dataBlock.inputValue(
                aSize).asInt());

    m_cache.size = size;

    resizeMembershipCache(size);
    resizeActiveControls(size);
    resizeOffsetCache(size);
    resizeCachedRotations(size);
    resizeOPMCache(size);


    CHECK_MSTATUS(
        ensureArraySizes(
            dataBlock,
            size));


    CHECK_MSTATUS(
        buildActiveControlsCache(
            size,
            dataBlock));


    CHECK_MSTATUS(
        initializeSolvedState(
            dataBlock));

    return MS::kSuccess;
}


ControlFamily RubiksCubeNode::acquireActiveFamily(
    const std::vector<double>& rowRotations,
    const std::vector<double>& columnRotations,
    const std::vector<double>& sliceRotations,
    MDataBlock& dataBlock) const
{
    constexpr double tolerance = 0.001;

    for (unsigned int controlIndex = 0;
        controlIndex < m_cache.size;
        ++controlIndex)
    {
        if (std::abs(
            getControlRotation(
                ControlFamily::Row,
                controlIndex,
                dataBlock,
                false)
            - rowRotations[controlIndex])
        > tolerance)
        {
            return ControlFamily::Row;
        }
    }

    for (unsigned int controlIndex = 0;
        controlIndex < m_cache.size;
        ++controlIndex)
    {
        if (std::abs(
            getControlRotation(
                ControlFamily::Column,
                controlIndex,
                dataBlock,
                false)
            - columnRotations[controlIndex])
        > tolerance)
        {
            return ControlFamily::Column;
        }
    }

    for (unsigned int controlIndex = 0;
        controlIndex < m_cache.size;
        ++controlIndex)
    {
        if (std::abs(
            getControlRotation(
                ControlFamily::Slice,
                controlIndex,
                dataBlock,
                false)
            - sliceRotations[controlIndex])
        > tolerance)
        {
            return ControlFamily::Slice;
        }
    }

    return ControlFamily::None;
}



MStatus RubiksCubeNode::updateCubeletOPMs(
    std::vector<MMatrix>& cubeletMatrices,
    const std::vector<double>& controlRotations,
    ControlFamily activeFamily) const
{
    const std::vector<std::vector<unsigned int>>* memberships =
        nullptr;

    switch (activeFamily)
    {
    case ControlFamily::Row:
        memberships = &m_cache.membership.row;
        break;

    case ControlFamily::Column:
        memberships = &m_cache.membership.column;
        break;

    case ControlFamily::Slice:
        memberships = &m_cache.membership.slice;
        break;

    default:
        return MS::kSuccess;
    }

    for (unsigned int controlIndex = 0;
        controlIndex < memberships->size();
        ++controlIndex)
    {
        const MMatrix controlMatrix =
            controlRotationMatrix(
                activeFamily,
                MAngle(
                    controlRotations[controlIndex],
                    MAngle::kRadians));

        for (unsigned int cubeletIndex
            : (*memberships)[controlIndex])
        {
            cubeletMatrices[cubeletIndex] =
                m_cache.cubeletOffsets[cubeletIndex] *
                controlMatrix;
        }
    }

    return MS::kSuccess;
}


void RubiksCubeNode::updateActiveFamily(
    ControlFamily& activeFamily) const
{
    const std::vector<bool>* activeControls =
        nullptr;

    switch (activeFamily)
    {
    case ControlFamily::Row:
        activeControls =
            &m_cache.activeControls.row;
        break;

    case ControlFamily::Column:
        activeControls =
            &m_cache.activeControls.column;
        break;

    case ControlFamily::Slice:
        activeControls =
            &m_cache.activeControls.slice;
        break;

    default:
        return;
    }

    for (bool active : *activeControls)
    {
        if (active)
        {
            return;
        }
    }

    activeFamily =
        ControlFamily::None;
}

MStatus RubiksCubeNode::setCachedControlRotations(
    const std::vector<double>& rowCache,
    const std::vector<double>& columnCache,
    const std::vector<double>& sliceCache,
    MDataBlock& dataBlock)
{
    for (unsigned int controlIndex = 0;
        controlIndex < m_cache.size;
        ++controlIndex)
    {
        CHECK_MSTATUS(
            setCachedControlRotation(
                ControlFamily::Row,
                controlIndex,
                rowCache[controlIndex],
                dataBlock));

        CHECK_MSTATUS(
            setCachedControlRotation(
                ControlFamily::Column,
                controlIndex,
                columnCache[controlIndex],
                dataBlock));

        CHECK_MSTATUS(
            setCachedControlRotation(
                ControlFamily::Slice,
                controlIndex,
                sliceCache[controlIndex],
                dataBlock));
    }

    return MS::kSuccess;
}

std::vector<MPoint> RubiksCubeNode::getSolvedCubeletPositions(
    unsigned int size)
{
    std::vector<MPoint> positions;

    positions.reserve(
        getCubeletCount(size));

    for (unsigned int x = 0;
        x < size;
        ++x)
    {
        for (unsigned int y = 0;
            y < size;
            ++y)
        {
            for (unsigned int z = 0;
                z < size;
                ++z)
            {
                const bool onSurface =
                    x == 0 ||
                    x == size - 1 ||
                    y == 0 ||
                    y == size - 1 ||
                    z == 0 ||
                    z == size - 1;

                if (!onSurface)
                {
                    continue;
                }

                positions.emplace_back(
                    coordinateFromIndex(
                        static_cast<int>(x),
                        static_cast<int>(size)),
                    coordinateFromIndex(
                        static_cast<int>(y),
                        static_cast<int>(size)),
                    coordinateFromIndex(
                        static_cast<int>(z),
                        static_cast<int>(size)));
            }
        }
    }

    return positions;
}

std::vector<MMatrix> RubiksCubeNode::buildSolvedCubeletMatrices(
    const std::vector<MPoint>& solvedPositions) const
{
    std::vector<MMatrix> matrices;

    matrices.reserve(
        solvedPositions.size());

    for (const MPoint& position : solvedPositions)
    {
        MTransformationMatrix transform;

        transform.setTranslation(
            MVector(position),
            MSpace::kTransform);

        matrices.push_back(
            transform.asMatrix());
    }

    return matrices;
}

std::vector<double> RubiksCubeNode::getCachedControlRotations(
    ControlFamily family,
    MDataBlock& dataBlock) const
{
    std::vector<double> rotations;
    rotations.reserve(
        m_cache.size);

    for (unsigned int controlIndex = 0;
        controlIndex < m_cache.size;
        ++controlIndex)
    {
        rotations.push_back(
            getControlRotation(
                family,
                controlIndex,
                dataBlock,
                true));
    }

    return rotations;
}

std::vector<double> RubiksCubeNode::getLiveControlRotations(
    ControlFamily family,
    MDataBlock& dataBlock) const
{
    std::vector<double> rotations;
    rotations.reserve(
        m_cache.size);

    for (unsigned int controlIndex = 0;
        controlIndex < m_cache.size;
        ++controlIndex)
    {
        rotations.push_back(
            getControlRotation(
                family,
                controlIndex,
                dataBlock,
                false));
    }

    return rotations;
}

MStatus RubiksCubeNode::setCubeletOPMs(
    const std::vector<MMatrix>& cubeletMatrices,
    MDataBlock& dataBlock)
{
    for (unsigned int cubeletIndex = 0;
        cubeletIndex < cubeletMatrices.size();
        ++cubeletIndex)
    {
        CHECK_MSTATUS(
            setCubeletOPM(
                cubeletIndex,
                cubeletMatrices[cubeletIndex],
                dataBlock));
    }

    return MS::kSuccess;
}


void RubiksCubeNode::updateCachedControlRotations(
    std::vector<double>& rowCache,
    std::vector<double>& columnCache,
    std::vector<double>& sliceCache,
    MDataBlock& dataBlock) const
{
    for (unsigned int controlIndex = 0;
        controlIndex < m_cache.size;
        ++controlIndex)
    {
        if (!m_cache.activeControls.row[controlIndex])
        {
            rowCache[controlIndex] =
                getControlRotation(
                    ControlFamily::Row,
                    controlIndex,
                    dataBlock,
                    false);
        }

        if (!m_cache.activeControls.column[controlIndex])
        {
            columnCache[controlIndex] =
                getControlRotation(
                    ControlFamily::Column,
                    controlIndex,
                    dataBlock,
                    false);
        }

        if (!m_cache.activeControls.slice[controlIndex])
        {
            sliceCache[controlIndex] =
                getControlRotation(
                    ControlFamily::Slice,
                    controlIndex,
                    dataBlock,
                    false);
        }
    }
}

void RubiksCubeNode::determineCachedRotation(
    ControlFamily family,
    std::vector<double>& activeCache,
    const std::vector<double>& rowCache,
    const std::vector<double>& colCache,
    const std::vector<double>& sliceCache)
{
    switch (family)
    {
    case ControlFamily::Row:
        activeCache = rowCache;
        break;

    case ControlFamily::Column:
        activeCache = colCache;
        break;

    case ControlFamily::Slice:
        activeCache = sliceCache;
        break;

    default:
        return;
    }
}

MObject RubiksCubeNode::getInitialRotAttr(
    ControlFamily family) const
{
    switch (family)
    {
    case ControlFamily::Row:
        return aInitialRowRotation;

    case ControlFamily::Column:
        return aInitialColumnRotation;

    case ControlFamily::Slice:
        return aInitialSliceRotation;

    default:
        return MObject::kNullObj;
    }
}

std::vector<double> RubiksCubeNode::getInitialControlRotations(
    ControlFamily family,
    MDataBlock& dataBlock) const
{
    const MObject attribute =
        getInitialRotAttr(family);

    if (attribute.isNull())
    {
        return {};
    }

    MArrayDataHandle arrayHandle =
        dataBlock.inputArrayValue(attribute);

    std::vector<double> rotations(
        m_cache.size,
        0.0);

    for (unsigned int controlIndex = 0;
        controlIndex < m_cache.size;
        ++controlIndex)
    {
        if (arrayHandle.jumpToArrayElement(controlIndex)
            == MS::kSuccess)
        {
            rotations[controlIndex] =
                arrayHandle.inputValue()
                .asAngle()
                .asRadians();
        }
    }

    return rotations;
}

MStatus RubiksCubeNode::setInitialControlRotations(
    const std::vector<double>& rotations,
    ControlFamily family,
    MDataBlock& dataBlock)
{
    const MObject attribute =
        getInitialRotAttr(family);

    if (attribute.isNull())
    {
        return MS::kFailure;
    }

    MArrayDataHandle arrayHandle =
        dataBlock.outputArrayValue(attribute);

    MArrayDataBuilder builder =
        arrayHandle.builder();

    for (unsigned int controlIndex = 0;
        controlIndex < rotations.size();
        ++controlIndex)
    {
        MDataHandle handle =
            builder.addElement(controlIndex);

        handle.setMAngle(
            MAngle(
                rotations[controlIndex],
                MAngle::kRadians));
    }

    arrayHandle.set(builder);

    return MS::kSuccess;
}

MStatus RubiksCubeNode::getLiveControlRotations(
    std::vector<double>& rowRotations,
    std::vector<double>& columnRotations,
    std::vector<double>& sliceRotations,
    MDataBlock& dataBlock) const
{
    rowRotations =
        getLiveControlRotations(
            ControlFamily::Row,
            dataBlock);

    columnRotations =
        getLiveControlRotations(
            ControlFamily::Column,
            dataBlock);

    sliceRotations =
        getLiveControlRotations(
            ControlFamily::Slice,
            dataBlock);

    return MS::kSuccess;
}

MStatus RubiksCubeNode::setInitialControlRotations(
    const std::vector<double>& rowRotations,
    const std::vector<double>& columnRotations,
    const std::vector<double>& sliceRotations,
    MDataBlock& dataBlock)
{
    CHECK_MSTATUS(
        setInitialControlRotations(
            rowRotations,
            ControlFamily::Row,
            dataBlock));

    CHECK_MSTATUS(
        setInitialControlRotations(
            columnRotations,
            ControlFamily::Column,
            dataBlock));

    CHECK_MSTATUS(
        setInitialControlRotations(
            sliceRotations,
            ControlFamily::Slice,
            dataBlock));

    return MS::kSuccess;
}

void RubiksCubeNode::resizeCachedRotations(unsigned int size)
{
    m_cache.rowCachedRotations.resize(size, 0.0);
    m_cache.columnCachedRotations.resize(size, 0.0);
    m_cache.sliceCachedRotations.resize(size, 0.0);
}

void RubiksCubeNode::resizeOPMCache(unsigned int size)
{
    m_cache.opm.resize(getCubeletCount(size), MMatrix::identity);
}