#include "dectree.h"

/**
 * This function takes in the name of the binary file containing the data and
 * loads it into memory. The binary file format consists of the following:
 *
 *     -   4 bytes : `N`: Number of images / labels in the file
 *     -   1 byte  : Image 1 label
 *     - 784 bytes : Image 1 data (28x28)
 *          ...
 *     -   1 byte  : Image N label
 *     - 784 bytes : Image N data (28x28)
 *
 * You can simply set the `sx` and `sy` values for all the images to 28, we
 * will not test this with different image sizes.
 */
Dataset *load_dataset(const char *filename) {
  // TODO: Allocate data, read image data / labels, return
  
  //not testing different images sizes so can set sx and sy to 28
  int sx = 28;
  int sy = 28;
  //open file in binary format
  FILE* f = fopen(filename, "rb");
  if(f == NULL){
    fprintf(stderr, "File not found\n");
    return NULL;
  }
  //read first four bytes to see how many images there are
  int numImages;
  fread(&numImages, sizeof(int), 1, f);
  //begin allocating memory for dataset
  Dataset* d = calloc(sizeof(Dataset), 1);
  d->num_items = numImages;
  d->labels = calloc(sizeof(unsigned char), numImages);
  d->images = calloc(sizeof(Image), numImages);
  if(d->labels == NULL || d->images == NULL){
    fprintf(stderr, "Not enough memory\n");
    return NULL;
  }
  //go through every image and allocate memeory for them
  for(int i = 0; i < numImages; i++){
    d->images[i].sx = sx;
    d->images[i].sy = sy;
    //allocate memory for data
    d->images[i].data = calloc(sizeof(unsigned char), d->images[i].sx * d->images[i].sy);
    if(d->images[i].data == NULL){
      fprintf(stderr, "Not enough memory for data\n");
      return NULL;
    }
    //read first byte indicating label
    //note will be in unsigned char format will need to convert to proper label after
    if (0 == fread(d->labels+i, 1, 1, f)){
      fprintf(stderr, "error reading binary file to label\n");
    }
    if (0 == fread((d->images+i)->data, sizeof(unsigned char), d->images[i].sx * d->images[i].sy, f)){
      fprintf(stderr, "error reading binary file to images\n");
    }
  }
  fclose(f);
  return d;
}

/**
 * This function computes and returns the Gini impurity of the M images
 * (with their indices stored in the array) if they were to be split
 * based on the given pixel.
 * 
 * DO NOT CHANGE THIS FUNCTION; It is already implemented for you.
 */
double split_gini_impurity(Dataset *data, int M, int indices[M], int pixel) {
  int a_freq[10] = {0}, a_count = 0;
  int b_freq[10] = {0}, b_count = 0;
  for (int i = 0; i < M; i++) {
    int img_idx = indices[i];
    // The pixels are always either 0 or 255, but using < 128 for generality.
    if (data->images[img_idx].data[pixel] < 128)
      a_freq[data->labels[img_idx]]++, a_count++;
    else
      b_freq[data->labels[img_idx]]++, b_count++;
  }

  double a_gini = 0, b_gini = 0;
  for (int i = 0; i < 10; i++) {
    double a_i = ((double)a_freq[i]) / ((double)a_count);
    double b_i = ((double)b_freq[i]) / ((double)b_count);
    if (a_count) a_gini += a_i * (1 - a_i);
    if (b_count) b_gini += b_i * (1 - b_i);
  }
  // Weighted average of gini impurity of children
  return (a_gini * a_count + b_gini * b_count) / M;
}

/**
 * Given a subset of M images and their corresponding indices, find and return
 * the most frequent label and it's frequency. The last 2 parameters are
 * only for outputs.
 *
 * - The most frequent label should be stored in `*label`
 * - The frequency of this label should be stored in `*freq`
 * 
 * If multiple labels have the same maximal frequency, return the smallest one.
 */
void get_most_frequent(Dataset *data, int M, int indices[M], 
                       int *label, int *freq) {
  // TODO: Set the correct values and return
  //note: labels are in between 0 and 9
  //array where each index corresponds to its label
  int tempLabel = 0;
  int tempFreq = 0;
  int arr[10];
  //go through each index and get its label adding it to the respective label's total
  for(int i = 0; i < 10; i++){
    arr[i] = 0;
  }
  for(int i = 0; i < M; i++){
    arr[data->labels[indices[i]]]++;
  }

  for(int i = 0; i < 10; i++){
    if(arr[i] > tempFreq){
       tempLabel = i;
       tempFreq = arr[i];
    }
  }
  *label = tempLabel;
  *freq = tempFreq;
  return;
}

/**
 * Given a subset of M images and their corresponding indices, find and return
 * the best pixel to split the data based on. The best pixel is the one which
 * has the minimum Gini impurity as computed by `split_gini_impurity()`.
 * 
 * The return value should be a number between 0-783 (inclusive), representing
 *  the pixel the M images should be split based on.
 * 
 * If multiple pixels have the same minimal Gini impurity, return the smallest.
 */
int find_best_split(Dataset *data, int M, int indices[M]) {
  //error in this function
  double smallest = -1;
  int bestPixel = 0;
  double temp;
  //recall all images are 28 * 28 pixels
  int pixelRange = 28*28;
    for(int j = 0; j < pixelRange; j++){
      temp = split_gini_impurity(data, M, indices, j);
      if(temp < smallest || smallest == -1){
        smallest = temp;
        bestPixel = j;
      }
    }
  // TODO: Return the correct pixel 
  return bestPixel;
}

//Helper function to split indicies and such based on pixel
void split_array(Dataset *data, int M, int indices[M], int pixel, int* rightM, int rightIndices[M], int* leftM, int leftIndices[M]){
  int rM = 0;
  int lM = 0;
  for(int i = 0; i < M; i++){
    //if pixel is written in
    if(data->images[indices[i]].data[pixel] == 255){
      rightIndices[rM] = indices[i];
      rM++;
    }
    else if(data->images[indices[i]].data[pixel] == 0){
      leftIndices[lM] = indices[i];
      lM++;
    }
    else{
      fprintf(stderr, "debugging error: pixel colour is not 0 or 255 (within split array) \n");
      return;
    }
  }
  *rightM = rM;
  *leftM = lM;
  return;
}



/**
 * This is the recursive function that creates the Decision tree. In each
 * recursive call, we only want to consider some subset of the entire dataset
 * corresponding to the node. To represent this, we pass in an array of indices
 * of these images in the dataset, along with it's length M. Be careful to
 * allocate this on the array for any recursive calls made, and free it when
 * you're done with creating the tree. Here, you need to:
 *
 *    - Compute ratio of most frequent image in current call, terminate if >95%
 *    - Find the best pixel to split on using `split_gini_impurity()`
 *    - Split the data, allocate corresponding arrays, and recurse
 *    - In either case, allocate a new node, set the correct values and return
 *       - If it's a child, set `classification`, and both children = NULL.
 *       - If it's not a child, set `pixel` and `left`/`right` (recursively). 
 */
DTNode *make_helper(Dataset *data, int M, int indices[M]) {
  // TODO: COnstruct and return tree
  int freq;
  int freqLabel;
  DTNode* new = calloc(sizeof(DTNode), 1);
  get_most_frequent(data, M, indices, &freqLabel, &freq);
  //if frequency is more than 95%
  if(((double)freq / (double)M) >= TERMINATE_RATIO ){
    //make leaf node
    new->right = NULL;
    new->left = NULL;
    new->classification = freqLabel;
    new->pixel = -1;
  }
  else{
    //find best pixel to split by
    int splitPixel = find_best_split(data,M,indices);
    new->classification = -1;
    new->pixel = splitPixel;
    int* rightIndices = calloc(sizeof(int), M);
    int rightM = 0;
    int* leftIndices = calloc(sizeof(int), M);
    int leftM = 0;
    //split indicies based on pixel and call for childs based on split
    split_array(data, M, indices, splitPixel, &rightM, rightIndices, &leftM, leftIndices);
    //assigned right and left based on split pixel
    new->right = make_helper(data, rightM, rightIndices);
    free(rightIndices);
    new->left = make_helper(data, leftM, leftIndices);
    free(leftIndices);
  }
  return new;
}

/**
 * This is the function exposed to the user. All you should do here is set
 * up the `indices` array correctly for the entire dataset and call 
 * `make_helper()` with the correct parameters.
 */
DTNode *make_dec_tree(Dataset *data) {
  // TODO: Set up `indices` array, call `make_helper` and return tree.
  //     Make sure you free any data that is not needed anymore (hint)
  int* indices = calloc(sizeof(int), data->num_items);
  for(int i = 0; i < data->num_items; i++){
    indices[i] = i;
  } 
  DTNode* root = make_helper(data,data->num_items, indices);
  free(indices);
  return root;
}

/**
 * Given a decision tree and an image to classify, return the predicted label.
 */
int dec_tree_classify(DTNode *root, Image *img) {
  // TODO: Return the correct label
  //if root is null then something went wrong
  if(root == NULL){
    fprintf(stderr, "error: null tree\n");
    return -1;
  }
  //if leaf node (only occasion where classification is not null)
  if(root->classification != -1){
    return root->classification;
  }
  //if pixel is drawn in (condition set by right)
  if(img->data[root->pixel] == 255){
    return dec_tree_classify(root->right, img);
  }
  //if pixel is empty (condition set by left)
  else if(img->data[root->pixel] == 0){
    return dec_tree_classify(root->left, img);
  }
  else{
    fprintf(stderr, "debugging error: pixel colour is not 0 or 255 (within dec_tree_classify)\n");
    return -1;
  }
  return -1;
}

/**
 * This function frees the Decision tree.
 */
void free_dec_tree(DTNode *node) {
  // TODO: Free the decision tree
  if(node == NULL){
    return;
  }
  //free left subtree
  free_dec_tree(node->left);
  //free right subtree
  free_dec_tree(node->right);
  //free node
  free(node);
  return;
}

/**
 * Free all the allocated memory for the dataset
 */
void free_dataset(Dataset *data) {
  for(int i = 0; i < data->num_items; i ++){
    free(data->images[i].data);
  }
  free(data->images);
  free(data->labels);
  free(data);
  // TODO: Free dataset (Same as A1)
  return;
}