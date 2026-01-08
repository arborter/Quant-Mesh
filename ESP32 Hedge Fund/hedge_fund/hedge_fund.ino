/*
 * Project Name: Hedge Fund Simulator
 * Date: 09/25/2025
 * Raphael
 *   
  Objective:
    A hedge fund who uses machine learning to make trades and build a portfolio.

  Lessons Learned:
  
*/


#include "math.h"
#include "stdio.h"

// The uniform random number is a value within the range
// of 0 and 1 with equal possibility of being selected.
// This function simulates that randomness. It can be thought
// of as a wildcard whose sole purpose is a single possibility
// without knowledge of which. Here, this possibility is a
// value, and this value is between 0 and 1.
// It is called a uniform distribution because 
// each possibility is equally possible
float uniform(){
  return (double)rand() /RAND_MAX;
}

// The uniform random number distribution is flat.We can give shape to
// uniform random distribution if we take two independent uniform
// randomly distributed numbers and set them on the Box-Muller
// formula, we can geometricaly see them on a graph
// where trigonometric operations are applied to the uniformly distributed
// outcomes 

void dice_roll(int n_rolls){
  long sum = 0;
  for(int i = 0; i < n_rolls; i++){
    int roll = (rand() % 6) + 1;
    sum +=roll;
  }
  return (double) sum / n_rolls; 
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

}


/*
 * NOTES:
 * 
 * Box-Muller: a representation of flat, single-line outcomes in a circular space
 * 
 * The Box–Muller uses logarithm and cosine (or sine) to 
 * take two flat random numbers and spread them out in a 
 * way that matches the bell curve.
 * 
 * The log and square root stretch values so that big 
 * deviations (far tails of the bell) are rarer.
 * 
 * The cosine/sine spin the numbers around a circle, 
 * making the distribution symmetric about zero.
 * 
 * So the trigonometric part gives the “shape” 
 * (spread in all directions), and the log/sqrt 
 * part makes the tails decay like a true Gaussian.


*/
