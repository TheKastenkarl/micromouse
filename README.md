# Micromouse: Designing an Educational Racing - Robot from Scratch [TUM IN2106/IN4235]

A practical course at TU Munich to build a micromouse robot from scratch, led by [Dr. Ph.D. Alexander Lenz](https://www.ce.cit.tum.de/air/people/dr-alexander-lenz/). This repository serves as a collection of code and ressources during our group work of SS22. See the official [course description](https://campus.tum.de/tumonline/wbLv.wbShowLVDetail?pStpSpNr=950367380&pSpracheNr=2) for further information.

Content overview:
- `src/`: Source and header files for the programming of a [Microchip dsPIC33FJ64MC804](https://www.microchip.com/en-us/product/dsPIC33FJ64MC804) microcontroller using the [MPLAB X IDE](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide) and the [MPLAB XC16 Compiler](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers).
- `pcb/`: Schematics and board layout designed in [Autodesk Eagle](https://www.autodesk.com/products/eagle/overview?term=1-YEAR&tab=subscription).
- `cad/`: 3D printed CAD parts including sensor and motor mountings.

### PCB

List of used Eagle libraries:

| Name  | Internal?         |
|-------|-------------------|
| [microchip-dspic33fjxxmc](http://eagle.autodesk.com/eagle/libraries?utf8=%E2%9C%93&q%5Btitle_or_author_or_description_cont%5D=dsPIC33FJ64MC&button=) | No             |
| SparkFun Capacitors | Yes |
| SparkFun Connectors | Yes |
| SparkFun LEDs       | Yes |
| SparkFun Resistors  | Yes |
