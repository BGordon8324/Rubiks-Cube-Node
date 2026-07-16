Rubik's Cube Node
Version 1.0

------------------------------------------------------------------------------------------------------------------------------------------------

Author: Ben Gordon
GitHub: https://github.com/BGordon8324
Website: https://www.bg3d.art/
Email: BenGordon8324@gmail.com

------------------------------------------------------------------------------------------------------------------------------------------------

A full breakdown of the rig design and node architecture can be found on my website at https://www.bg3d.art/rubikscube

------------------------------------------------------------------------------------------------------------------------------------------------

#Installation

Drag the install.py file into the maya view port. This will install and load the plugin, create a shelf and button for the Save/Load tool, and add a pre built 3x3x3 Rubik's Cube rig to the projects folder.

------------------------------------------------------------------------------------------------------------------------------------------------

# Animating

The rig behaves like a physical Rubik's Cube. At any given time, only one control family is active:

* Rows
* Columns
* Slices

Only controls belonging to the active family will affect the cube. Rotating a control from an inactive family will have no effect. Once all controls in the active family return to a multiple of 90°, the family releases ownership of the cubelets. Any rotated controls from another family immediately become the new active family, and the appropriate cubelets snap into alignment with those controls.


## Save / Load Tool

Unlike most Maya rig, this rig is implemented as a state machine rather than a hierarchy of transformations.

A traditional rig's output can always be determined directly from its current control values. A Rubik's Cube cannot. To evaluate correctly, the node must know both the current control rotations and the last committed cube state.

Because of this, timeline jumps are not deterministic. For example, if you animate from frame 0 to frame 100 and then jump back to frame 0, the controls return to their frame 0 values, but the cube's committed state is still the one reached at frame 100. The resulting cube state is therefore different from the original frame 0 state.

To solve this, the included Save / Load Tool lets you store and restore cube states.

### Usage

1. Select the cube's root control.
2. Click *Save* to store the current cube state.
3. Click *Load* whenever you want to restore a previously saved state.

Saving important poses (such as the starting position) allows you to reliably restore the cube regardless of timeline navigation.

------------------------------------------------------------------------------------------------------------------------------------------------

# Rigging

## Control Setup

Set the cube size, then create *size* number of controls for each control family: *Rows*, *Columns*, and *Slices*.

Connect a single rotation channel from each control to the corresponding row, column, or slice rotation input.

* *Columns* rotate about the local *X* axis.
* *Rows* rotate about the local *Y* axis.
* *Slices* rotate about the local *Z* axis.

Within each family, *control[0]* corresponds to the layer at the negative end of its axis, with the remaining controls progressing toward the positive direction.

The node only reads the connected rotation channel, so values on the control's other transform channels do not affect its operation. The connected rotation channel should be chosen to avoid gimbal lock. While gimbal lock does not affect the node's calculations, it can cause undesirable Euler interpolation in animation curves.

Control hierarchy is also independent of the node's evaluation. However, for a clean and intuitive rig, it is recommended that the controls be parented with the cubelets so their transforms visually match the layers they manipulate.

## Cubelet Setup

A cube requires *size³ − (size − 2)³* cubelets.

The node outputs an array, so the number of output connections is not fixed. Connect one output to each cubelet's *offsetParentMatrix*.

If more outputs exist than cubelets, the extra outputs are simply ignored. Likewise, the node internally tracks cubelets even if not all relevant outputs are connected.

Each cubelet should have zero translation and zero rotation, with all movement driven through its **Offset Parent Matrix**.

## Root Control

Connect the root control's *message* attribute to the Rubik's Cube node's *rigRoot* message attribute.

This connection is not required for the node itself, but it allows the Save / Load Tool to automatically locate the corresponding Rubik's Cube node.

It is also recommended that the root control be the parent of both the cubelets and all layer controls, providing a single transform for the entire rig.

------------------------------------------------------------------------------------------------------------------------------------------------

# License

Rubik's Cube Node License

Copyright (c) 2026 Benjamin Gordon

Permission is granted to any person obtaining a copy of this software to use,
copy, and modify it for personal and educational purposes.

Commercial use, redistribution, sublicensing, or sale of this software or
modified versions of this software is not permitted without prior written
permission from the copyright holder.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
