# CAD Design

This directory contains the mechanical design assets for the **Arduino Self-Balancing Robot** project.

The CAD models represent the mechanical components used to construct the robot and provide a foundation for reviewing the design, understanding component geometry, and supporting future fabrication or modifications.

## Contents

| File | Description |
|---|---|
| [`cad-design.jpeg`](cad-design.jpeg) | Visual reference for the CAD design |
| [`plate.stl`](plate.stl) | STL mesh of the plate component |
| [`rods.stl`](rods.stl) | STL mesh of the rod components |

## Design Overview

The mechanical design supports the assembly of a two-wheeled self-balancing robot. The chassis and component mounting arrangement are important because the robot's mass distribution and centre of gravity influence balancing performance.

Key mechanical considerations include:

- Chassis rigidity and structural stability
- Motor and wheel alignment
- Mounting space for the Arduino and MPU6050
- Battery placement and centre of gravity
- Accessibility for wiring, maintenance, and component replacement

## File Formats

**STL (`.stl`)** files contain triangulated 3D mesh geometry and can be imported into compatible CAD software or prepared for 3D printing.

The design image provides a visual reference for understanding the mechanical model.

> Note: STL files generally do not preserve the original parametric feature history. Include the native CAD source file as well, if available.

## Future Improvements

- Include the editable native CAD source files.
- Add dimensioned engineering drawings.
- Document chassis dimensions and mounting-hole locations.
- Include an assembled robot model and exploded view.
- Document material selection and fabrication details.

## Related Project Resources

- [Main project documentation](../README.md)
- [Hardware components](../docs/hardware-components.md)
- [Circuit connections](../docs/circuit-connections.md)
- [Balancing test records](../tests/balancing-tests.md)

---

*Part of the Arduino Self-Balancing Robot project.*
