from enum import IntEnum

class ControlFamily(IntEnum):
    NONE = 0
    ROW = 1
    COLUMN = 2
    SLICE = 3

class ControlMembership:

    def __init__(
            self,
            family,
            controlIndex):

        self.family = family
        self.controlIndex = controlIndex
        
class MembershipCache:

    def __init__(
            self,
            size,
            cubeletCount):

        self.row = [
            [] for _ in range(size)]

        self.column = [
            [] for _ in range(size)]

        self.slice = [
            [] for _ in range(size)]

        self.cubelet = [
            [] for _ in range(cubeletCount)]
        
class TransientCache:

    def __init__(self):
        self.size = 0
        self.family = ControlFamily.NONE
        self.membership = None
        self.offsets = []
