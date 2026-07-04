import maya.api.OpenMaya as om
import maya.cmds as cmds

from RubiksCubeData import (
    ControlFamily,
    ControlMembership,
    TransientCache,
    MembershipCache
)

class RubiksCubeIO:

    def __init__(self, node):
        self.node = node
        self.cache = TransientCache()

    # -------------------------------------------------------------------------
    # Public
    # -------------------------------------------------------------------------

    def get_permutation(self):
    
        size = (
            self.get_size())
    
        family = (
            self.get_active_family())
    
        cubeletMatrices = (
            self.get_cubelet_opms())
            
    
        if family != ControlFamily.NONE:
            
            self.build_membership(
                cubeletMatrices,
                size)
    
            liveRotations = (
                self.get_live_rotations())
    
            cachedRotations = (
                self.get_cached_rotations())
    
            cubeletMatrices = (
                self.undo_live_rotation(
                    cubeletMatrices,
                    family,
                    liveRotations,
                    cachedRotations))
    
        return (size, cubeletMatrices)

    def set_permutation(
            self,
            size,
            cubeletMatrices):

        family = (
            self.get_active_family())
            
        cube_size = cmds.getAttr(f'{self.node}.size')
        if cube_size != size:
            return cmds.warning('Cannot load permutation. Cube size and permutation size differ')
    
        if family != ControlFamily.NONE:
    
            self.build_membership(
                cubeletMatrices,
                size)
                
                
            cachedRotations = (
                self.get_cached_rotations())
                
    
            self.build_offsets(
                cubeletMatrices,
                family,
                cachedRotations)        

            liveRotations = (
                self.get_live_rotations())                
            
            cubeletMatrices = (
                self.apply_live_rotation(
                    cubeletMatrices,
                    family,
                    liveRotations))
                    
        self.set_cubelet_opms(
            cubeletMatrices)
    
        self.set_cache_valid(
            False)


    # -------------------------------------------------------------------------
    # Maya I/O
    # -------------------------------------------------------------------------


    def get_cubelet_opms(self):
        count = cmds.getAttr(
            f"{self.node}.outOffsetParentMatrix",
            size=True)

        matrices = []

        for index in range(count):
            values = cmds.getAttr(
                f"{self.node}.outOffsetParentMatrix[{index}]")
            matrices.append(
                om.MMatrix(values))

        return matrices

    def set_cubelet_opms(self, matrices):
        for index, matrix in enumerate(matrices):
            cmds.setAttr(
                f"{self.node}.outOffsetParentMatrix[{index}]",
                *matrix,
                type="matrix")

    def get_active_family(self):
        return cmds.getAttr(
            f"{self.node}.activeFamily")

    def get_live_rotations(self):
        return {
            "row": cmds.getAttr(
                f"{self.node}.rowRotation")[0],

            "column": cmds.getAttr(
                f"{self.node}.columnRotation")[0],

            "slice": cmds.getAttr(
                f"{self.node}.sliceRotation")[0]
        }

    def get_cached_rotations(self):
        return {
            "row": cmds.getAttr(
                f"{self.node}.cachedRowRotation")[0],

            "column": cmds.getAttr(
                f"{self.node}.cachedColumnRotation")[0],

            "slice": cmds.getAttr(
                f"{self.node}.cachedSliceRotation")[0]
        }

    def get_size(self):
        return cmds.getAttr(
            f"{self.node}.size")

    def set_size(self, size):
        cmds.setAttr(
            f"{self.node}.size",
            size)

    def set_cache_valid(self, valid):
        cmds.setAttr(
            f"{self.node}.cacheValid",
            valid)

    # -------------------------------------------------------------------------
    # Cube Algorithms
    # -------------------------------------------------------------------------

    @staticmethod
    def coordinate_from_index(
            index,
            size):
    
        if size % 2:
    
            return (
                float(index)
                - float(size - 1) / 2.0)
    
        return (
            float(index)
            + 0.5
            - float(size) / 2.0)
            

    def build_membership(
            self,
            cubeletMatrices,
            size):
    
        cubeletCount = len(
            cubeletMatrices)
    
        self.cache.membership = MembershipCache(
            size,
            cubeletCount)
    
        for cubeletIndex, cubeletMatrix in enumerate(
                cubeletMatrices):
    
            rowIndex = (
                RubiksCubeIO.get_control_index(
                    cubeletMatrix[13],
                    size))
    
            columnIndex = (
                RubiksCubeIO.get_control_index(
                    cubeletMatrix[12],
                    size))
    
            sliceIndex = (
                RubiksCubeIO.get_control_index(
                    cubeletMatrix[14],
                    size))
    
            if rowIndex != -1:
    
                self.cache.membership.row[
                    rowIndex].append(
                        cubeletIndex)
    
                self.cache.membership.cubelet[
                    cubeletIndex].append(
                        ControlMembership(
                            ControlFamily.ROW,
                            rowIndex))
    
            if columnIndex != -1:
    
                self.cache.membership.column[
                    columnIndex].append(
                        cubeletIndex)
    
                self.cache.membership.cubelet[
                    cubeletIndex].append(
                        ControlMembership(
                            ControlFamily.COLUMN,
                            columnIndex))
    
            if sliceIndex != -1:
    
                self.cache.membership.slice[
                    sliceIndex].append(
                        cubeletIndex)
    
                self.cache.membership.cubelet[
                    cubeletIndex].append(
                        ControlMembership(
                            ControlFamily.SLICE,
                            sliceIndex))

    def build_offsets(
            self,
            cubeletMatrices,
            family,
            rotations):
                
    
        if family == ControlFamily.ROW:
    
            memberships = (
                self.cache.membership.row)
                
            rotations = rotations['row']
    
        elif family == ControlFamily.COLUMN:
    
            memberships = (
                self.cache.membership.column)
                
            rotations = rotations['column']
    
        elif family == ControlFamily.SLICE:
    
            memberships = (
                self.cache.membership.slice)
                
            rotations = rotations['slice']
    
        else:
    
            return
            
    
        self.cache.offsets = [
            om.MMatrix()
            for _ in cubeletMatrices]
            
    
        for controlIndex, cubeletIndices in enumerate(
                memberships):

            controlMatrix = (
                self.control_rotation_matrix(
                    family,
                    rotations[controlIndex]))
                    
    
            for cubeletIndex in cubeletIndices:
                self.cache.offsets[
                    cubeletIndex] = (
                        self.calculate_offset(
                            cubeletMatrices[
                                cubeletIndex],
                            controlMatrix))


    def undo_live_rotation(
            self,
            cubeletMatrices,
            family,
            liveRotations,
            cachedRotations):
    
        inactiveMatrices = list(
            cubeletMatrices)
    
        if family == ControlFamily.ROW:
    
            memberships = (
                self.cache.membership.row)
    
            liveRotations = (
                liveRotations["row"])
    
            cachedRotations = (
                cachedRotations["row"])
    
        elif family == ControlFamily.COLUMN:
    
            memberships = (
                self.cache.membership.column)
    
            liveRotations = (
                liveRotations["column"])
    
            cachedRotations = (
                cachedRotations["column"])
    
        elif family == ControlFamily.SLICE:
    
            memberships = (
                self.cache.membership.slice)
    
            liveRotations = (
                liveRotations["slice"])
    
            cachedRotations = (
                cachedRotations["slice"])
    
        else:
    
            return inactiveMatrices
    
        for controlIndex, cubeletIndices in enumerate(
                memberships):
    
            deltaRotation = (
                liveRotations[controlIndex] -
                cachedRotations[controlIndex])
    
            inverseDelta = (
                self.control_rotation_matrix(
                    family,
                    deltaRotation).inverse())
    
            for cubeletIndex in cubeletIndices:
    
                inactiveMatrices[
                    cubeletIndex] = (
                        inactiveMatrices[
                            cubeletIndex] *
                        inverseDelta)
    
        return inactiveMatrices

    def apply_live_rotation(
            self,
            cubeletMatrices,
            family,
            liveRotations):
    
        outputMatrices = [
            om.MMatrix(matrix)
            for matrix in cubeletMatrices]
            
    
        if family == ControlFamily.ROW:
    
            memberships = (
                self.cache.membership.row)
    
            liveRotations = (
                liveRotations["row"])
    
        elif family == ControlFamily.COLUMN:
    
            memberships = (
                self.cache.membership.column)
    
            liveRotations = (
                liveRotations["column"])
    
        elif family == ControlFamily.SLICE:
    
            memberships = (
                self.cache.membership.slice)
    
            liveRotations = (
                liveRotations["slice"])
    
        else:
    
            return outputMatrices
    
        for controlIndex, cubeletIndices in enumerate(
                memberships):
    
            controlMatrix = (
                self.control_rotation_matrix(
                    family,
                    liveRotations[controlIndex]))
    
            for cubeletIndex in cubeletIndices:
    
                outputMatrices[
                    cubeletIndex] = (
                        om.MMatrix(
                            self.cache.offsets[
                                cubeletIndex] *
                            controlMatrix))
    
        return outputMatrices

    # -------------------------------------------------------------------------
    # Math Helpers
    # -------------------------------------------------------------------------

    def control_rotation_matrix(
            self,
            family,
            rotation):

        euler = om.MEulerRotation()
    
        if family == ControlFamily.ROW:
    
            euler.y = rotation
    
        elif family == ControlFamily.COLUMN:
    
            euler.x = rotation
    
        elif family == ControlFamily.SLICE:
    
            euler.z = rotation
    
        else:
    
            return om.MMatrix()
            

    
        euler.order = (
            om.MEulerRotation.kXYZ)
            
    
        return euler.asMatrix()

    @staticmethod
    def get_control_index(
            coordinate,
            size):
    
        tolerance = 0.001
    
        for index in range(size):
    
            validCoordinate = (
                RubiksCubeIO.coordinate_from_index(
                    index,
                    size))
    
            if abs(
                    coordinate -
                    validCoordinate) <= tolerance:
    
                return index
    
        return -1
        
    def calculate_offset(
            self,
            cubeletMatrix,
            controlMatrix):
                
        return (
            cubeletMatrix *
            controlMatrix.inverse())
            