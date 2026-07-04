import maya.cmds as cmds
import json
import maya.api.OpenMaya as om

from pathlib import Path
from RubiksCubeIO import RubiksCubeIO


class RubiksCubeIOUI:

    PERMUTATIONS_DIRECTORY = Path(__file__).parent / 'Cube Permutations'

    WINDOW = "RubiksCubeSaveLoadTool"

    def __init__(self):

        self.ensure_permutations_directory()
        self.permutationMenu = None
        
        
    def ensure_permutations_directory(self):

        self.PERMUTATIONS_DIRECTORY.mkdir(
            parents=True,
            exist_ok=True)


    def show(self):
    
        if cmds.window(
                self.WINDOW,
                exists=True):
    
            cmds.deleteUI(
                self.WINDOW)
    
        cmds.window(
            self.WINDOW,
            title="Rubiks Cube Save/Load Tool",
            sizeable=True,
            widthHeight=(600, 120))
    
        cmds.columnLayout(
            adjustableColumn=True,
            rowSpacing=10)
    
        cmds.button(
            label="Save",
            command=self.on_save)
    
        cmds.text(
            label="Cube Permutations",
            align="left")
    
        self.permutationMenu = cmds.optionMenu()
    
        self.populate_permutation_menu()
    
        cmds.rowLayout(
            numberOfColumns=2,
            adjustableColumn=1)
    
        cmds.button(
            label="Load",
            command=self.on_load)
    
        cmds.button(
            label="Delete",
            command=self.on_delete)
    
        cmds.setParent("..")
    
        cmds.showWindow(
            self.WINDOW)
            
    def populate_permutation_menu(self):
    
        items = cmds.optionMenu(
            self.permutationMenu,
            query=True,
            itemListLong=True)
    
        for item in items or []:
            cmds.deleteUI(item)
    
        for filePath in sorted(
                self.PERMUTATIONS_DIRECTORY.glob("*.json")):
    
            cmds.menuItem(
                label=filePath.stem,
                parent=self.permutationMenu)
                
    def detect_cubes(self):
    
        selection = cmds.ls(
            selection=True,
            long=True)
    
        if not selection:
            cmds.warning(
                "No objects currently selected.")
            return []
    
        cubes = []
    
        for root in selection:
    
            connections = cmds.listConnections(
                f"{root}.message",
                source=False,
                destination=True,
                type="rubiksCubeNode")
    
            if not connections:
                continue
    
            cube = connections[0]
    
            if cube not in cubes:
                cubes.append(
                    cube)
    
        if not cubes:
            cmds.warning(
                "No Rubik's Cube nodes found.")
    
        return cubes
        
        
    def detect_cube(self):
    
        cubes = self.detect_cubes()
    
        if not cubes:
            return None
    
        return cubes[0]
                

    def on_save(self, *_):
    
        cube = self.detect_cube()
    
        if cube is None:
            return
    
        name = self.prompt_save_name()
    
        if not name:
            return
    
        filePath = (
            self.PERMUTATIONS_DIRECTORY /
            f"{name}.json")
    
        if filePath.exists():
            if not self.confirm_overwrite(
                    name):
                return
    
        size, matrices = RubiksCubeIO(cube).get_permutation()
    
        data = {
            "size": size,
            "matrices": [
                list(matrix)
                for matrix in matrices
            ]
        }
    
        filePath.write_text(
            json.dumps(
                data,
                indent=4))
    
        self.populate_permutation_menu()         
        
    def on_load(
            self,
            *_):
    
        cubes = self.detect_cubes()
    
        if not cubes:
            return
    
        name = cmds.optionMenu(
            self.permutationMenu,
            query=True,
            value=True)
    
        filePath = self.PERMUTATIONS_DIRECTORY / f"{name}.json"
    
        if not filePath.exists():
            cmds.warning(
                f'Permutation "{name}" does not exist.')
            self.populate_permutation_menu()
            return
    
        data = json.loads(filePath.read_text())
    
        size = data["size"]
    
        matrices = [
            om.MMatrix(matrix)
            for matrix in data["matrices"]
        ]
    
        for cube in cubes:
            RubiksCubeIO(cube).set_permutation(size, matrices)
            RubiksCubeIO(cube).set_permutation(size, matrices)
            #doesn't write properly to the opm the fist time, but does on the second
            #can't figure it out and at this point I don't care
            #maybe its a weird maya thing because it doesn't expect you to write to outputs
        
    def on_delete(self, *_):

        self.ensure_permutations_directory()
    
        name = cmds.optionMenu(
            self.permutationMenu,
            query=True,
            value=True)
    
        if not name:
            cmds.warning(
                "No permutation selected.")
            return
    
        filePath = self.PERMUTATIONS_DIRECTORY / f"{name}.json"
    
        if not filePath.exists():
            cmds.warning(
                f'Permutation "{name}" does not exist.')
            self.populate_permutation_menu()
            return
    
        result = cmds.confirmDialog(
            title="Delete Permutation",
            message=(
                f'Delete permutation "{name}"?'),
            button=[
                "Delete",
                "Cancel"],
            defaultButton="Delete",
            cancelButton="Cancel",
            dismissString="Cancel")
    
        if result != "Delete":
    
            return
    
        filePath.unlink()
    
        self.populate_permutation_menu()
        
    def prompt_save_name(self):
    
        result = cmds.promptDialog(
            title="Save Permutation",
            message="Permutation Name:",
            button=["Save", "Cancel"],
            defaultButton="Save",
            cancelButton="Cancel",
            dismissString="Cancel")
    
        if result != "Save":
    
            return None
    
        name = cmds.promptDialog(
            query=True,
            text=True).strip()

        if not name:

            cmds.warning(
                "Please enter a permutation name.")

            return None

        return name
            

    def confirm_overwrite(
            self,
            name):
    
        result = cmds.confirmDialog(
            title="Overwrite Permutation",
            message=(
                f'"{name}" already exists.\n\n'
                "Do you want to overwrite it?"),
            button=["Overwrite", "Cancel"],
            defaultButton="Overwrite",
            cancelButton="Cancel",
            dismissString="Cancel")
    
        return result == "Overwrite"