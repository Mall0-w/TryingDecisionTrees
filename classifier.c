#include "dectree.h"

// Makefile included in starter:
//    To compile:               make
//    To decompress dataset:    make datasets
//
// Or, to compile manually, use:
//    gcc -o classifier dectree.c classifier.c -g -Wall -std=c99 -lm
//
// Running decision tree generation / validation:
//    ./classifier datasets/training_data.bin datasets/testing_data.bin

/*****************************************************************************/
/* Do not add anything outside the main function here. Any core logic other  */
/* than what is described below should go in `dectree.c`. You've been warned!*/
/*****************************************************************************/

/**
 * main() takes in 2 command line arguments:
 *    - training_data: A binary file containing training image / label data
 *    - testing_data: A binary file containing testing image / label data
 *
 * You need to do the following:
 *    - Parse the command line arguments, call `load_dataset()` appropriately.
 *    - Call `make_dec_tree()` to build the decision tree with training data
 *    - For each test image, call `dec_tree_classify()` and compare the real 
 *        label with the predicted label
 *    - Print out (only) one integer to stdout representing the number of 
 *        test images that were correctly classified.
 *    - Free all the data allocated and exit.
 *  (You should for the most part be able to re-use code from A1 for this)
 */

int main(int argc, char *argv[]) {
  // TODO: Handle command line arguments
  if(argc != 3){
    fprintf(stderr, "Invalid number of arguments\n");
    return 1;
  }

  char* training = argv[1];
  char* testing = argv[2];

  Dataset* trainData = load_dataset(training);
 
  Dataset* testData = load_dataset(testing);
  
  if(trainData == NULL || testData == NULL){
    fprintf(stderr, "error loading dataset\n");
    return 1;
  }
  //construct decision tree
  DTNode* decisionTree = make_dec_tree(trainData);
  free_dataset(trainData);
  // TODO: Compute the total number of correct predictions
  int total_correct = 0;

  for(int i = 0; i < testData->num_items; i++){
    if(dec_tree_classify(decisionTree, testData->images+i) == testData->labels[i]){
      total_correct++;
    }
  }
  free_dataset(testData);
  free_dec_tree(decisionTree);

  // Print out answer
  printf("%d\n", total_correct);
  return 0;
}