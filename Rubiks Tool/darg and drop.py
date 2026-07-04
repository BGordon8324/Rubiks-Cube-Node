import sys
import inspect
import textwrap
from pathlib import Path

import maya.cmds as cmds
import maya.mel as mel


# -- Button commands ----------------------------------------------------------

_TOOL_DIR = str(Path(__file__).parent)


def _rubiks_cube_tool():

    import sys
    import importlib

    toolPath = {tool_dir}

    if toolPath not in sys.path:
        sys.path.insert(0, toolPath)

    import RubiksCubeData
    import RubiksCubeIO
    import RubiksCubeIOUI

    importlib.reload(RubiksCubeData)
    importlib.reload(RubiksCubeIO)
    importlib.reload(RubiksCubeIOUI)

    RubiksCubeIOUI.RubiksCubeIOUI().show()

# -- Helpers ------------------------------------------------------------------

def _fn_to_cmd(fn):

    src = textwrap.dedent(
        inspect.getsource(fn))

    body = textwrap.dedent(
        "\n".join(src.splitlines()[1:]))

    return body.format(
        tool_dir=repr(_TOOL_DIR))

def _add_button(shelf, btn):

    cmds.shelfButton(
        parent=shelf,
        sourceType="python",
        style="iconAndTextVertical",
        **btn)


# -- Button definitions -------------------------------------------------------

_ICON = str(Path(__file__).parent / "cube icon")

BUTTONS = [
    {
        "label": "Rubiks Cube",
        "annotation": "Open Rubiks Cube Save/Load Tool",
        "image": _ICON,
        "command": _fn_to_cmd(_rubiks_cube_tool),
    },
]


# -- Installer ----------------------------------------------------------------

def onMayaDroppedPythonFile(*args):


    if _TOOL_DIR not in sys.path:
        sys.path.insert(0, _TOOL_DIR)

    shelf_name = "RubiksCube"

    if not cmds.shelfLayout(
            shelf_name,
            exists=True):

        mel.eval(
            'global string $gShelfTopLevel; '
            'shelfLayout -manage 1 -p $gShelfTopLevel "{}"'.format(
                shelf_name))

    existing = cmds.shelfLayout(
        shelf_name,
        q=True,
        childArray=True) or []

    for child in existing:
        cmds.deleteUI(child)

    for btn in BUTTONS:
        _add_button(
            shelf_name,
            btn)

    mel.eval(
        'global string $gShelfTopLevel; '
        'shelfTabLayout -e -selectTab "{}" $gShelfTopLevel;'.format(
            shelf_name))