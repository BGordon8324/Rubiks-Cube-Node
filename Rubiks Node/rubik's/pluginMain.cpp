#include <maya/MFnPlugin.h>

#include "RubiksCubeNode.h"

MStatus initializePlugin(MObject obj)
{
    MFnPlugin plugin(
        obj,
        "Ben Gordon",
        "1.0",
        "Any"
    );

    MStatus status =
        plugin.registerNode(
            "rubiksCubeNode",
            RubiksCubeNode::id,
            RubiksCubeNode::creator,
            RubiksCubeNode::initialize
        );

    if (!status)
    {
        status.perror(
            "registerNode rubiksCubeNode"
        );
    }

    return status;
}


MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);

    MStatus status =
        plugin.deregisterNode(
            RubiksCubeNode::id
        );

    if (!status)
    {
        status.perror(
            "deregisterNode rubiksCubeNode"
        );
    }

    return status;
}