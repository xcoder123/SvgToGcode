SvgToGcode
==========

Takes SVG and converts the elements in gcode instructions.

Last year in course of a month I wrote a simple application which takes SVG xml file and converts it to gcode. 
However, due to lack of time I wasn't able to finish it.

As of now the application has the following features:

- Nearly all primitves of SVG can be drawn
- Transformations like scale, rotate, translate, matrix have been implemented
- GUI for rearrenging the object on the platform
- Recursive best effort longest path searching algorithm



There are still many features I would like to implement and I belive are quite mandatory for succesful project:

- Filling
- Support line width, could be viewed as filling algorithm
- Automatic line invisibility test (detect where lines shouldn't be visible
- Scaling of the image
- Improved longest path algorithm, for example use djekstra algorithm to find the longest path
- Construct a new algorithm to detect the shortest path between multiple elements
- Support of multiple pictures in one workspace


Maybe the code for svg parsing will be useful for someone.

Original article: http://morf.lv/modules.php?name=projects&lasit=16
