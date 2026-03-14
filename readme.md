# Basic Boids

This is a simple boids (bird-objects) program that uses C++ and OpenGL. It makes up part of the "how to program shaders for generative art" course on my website naetharu.com.

## What is generative art?

In this context "generative" means art that arises from simple emergent properties based on simple rules. These could be physics rules used to push objects around. It could be pattern based such as in Conways Game of Life. Or any other set of programmatic rules that give rise to beautiful visuals. 

This should not be confused with AI - the art here is about writing simple elegant rules and seeing the beauty that arises from it. It has nothing to do with machine leanring or generative AI.

## What are Boids?

Boids are a fun concept in graphics programming, where points are given properties that allow them to move around in a manner that mimics a flock or swam. They look very beautiful and are my favorite thing to play with in generative art.

## To build the project

cmake -S . -B build
cmake --build build

## Controls

The Boids have 3 controls: 
Q/A will control their seperation in space
W/S will control their alignment with one another
E/D will control their coherence as a group

## A note on performance

This version of boids uses a naive compute shader set up where we are checking each boid against ALL other boids per cycle. This does mean that the program requires reasonable GPU power to run, even at just 5000 points.

This is intentional - the program is part of the course I am writing on how to make visual art with OpenGL. The next version will dig into how to optimize this by only checking against other boids in the local area. 

If you wish to run this version on a lower end system, you may wish to reduce the number of boids. This is reasoably simple to do, and just requires the hard-coded boid numbers to be lowered in both main() and the shaders.