
1. Build the solution file with Visual Studio 2013
2. Pictures can be given as Inputs with command line arguments
3. Use Mouse pad to rotate 
4. Program uses SPACE to toggle between normal draw, wireframe draw and point draw modes
5. z makes multiple (10) screnshots
6. x makes a single screenshot
7. s toggles gradient or textured display (texture must be given as second argument on the command line)
8. There is another pair of shader files in openggl-started code\tex that do the textured drawing.
9. The program uses glDrawElements in fill and wireframe mode, no need for indices in points mode as primitives share no vertices.