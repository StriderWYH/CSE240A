//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
// Parameters

int mask_perceptron = 0;

// Perceptron Data
int **weights;
int *history;
float y;

// Utility
int power(int base, int exp) {
  return exp == 0 ? 1 : base * power(base, exp - 1);
}

int transferHistory() {
  int ghr = 0;
  for (int i = 0; i < ghistoryBits; i++) {
    ghr = (ghr << 1) | (history[i] == 1 ? 1 : 0);
  }
  return ghr;
}

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      // Initialize the global history register
      break;
    case TOURNAMENT:
      // Initialize the local history register
      break;
    case CUSTOM:
      // Initialize custom predictor data structures
      init_custom_predictor();
      break;
    default:
      break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
    case TOURNAMENT:
    case CUSTOM:
      // Call the custom prediction function for perceptron-based predictors
      return make_custom_prediction(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case STATIC:
        break;
    case GSHARE:
        break;
    case TOURNAMENT:
        break;
    case CUSTOM:
        // Call the custom training function for perceptron-based predictors
        train_custom_predictor(pc, outcome);
        break;
    default:
        break;
  }
}

// Custom predictor initialization
void init_custom_predictor()
{
  // Initialize perceptron parameters
  ghistoryBits = 12;
  pcIndexBits = 10;
  for (int i = 0; i < pcIndexBits; i++) mask_perceptron = (mask_perceptron << 1) | 1;
  int size = power(2, pcIndexBits);
  weights = (int **)malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++) {
    weights[i] = (int *)calloc(ghistoryBits + 1, sizeof(int));
  }

  history = (int *)malloc(ghistoryBits * sizeof(int));
  for (int i = 0; i < ghistoryBits; i++) history[i] = -1;
}

// Make a custom prediction for conditional branch instruction at PC 'pc'
uint8_t make_custom_prediction(uint32_t pc)
{
  int ghr = transferHistory();
  int index = (pc & mask_perceptron) ^ (ghr & mask_perceptron);
  y = weights[index][ghistoryBits]; // bias
  for (int i = 0; i < ghistoryBits; i++) {
    y += weights[index][i] * history[i];
  }
  return y >= 0 ? TAKEN : NOTTAKEN;
}


void train_custom_predictor(uint32_t pc, uint8_t outcome)
{
  int result = outcome == TAKEN ? 1 : -1;
  int ghr = transferHistory();
  int index = (pc & mask_perceptron) ^ (ghr & mask_perceptron);
  int prediction = y >= 0 ? TAKEN : NOTTAKEN;

  int threshold = 1.93 * ghistoryBits + 14;
  if (prediction != outcome || (y > -threshold && y < threshold)) {
    weights[index][ghistoryBits] += result; // bias
    for (int i = 0; i < ghistoryBits; i++) {
      weights[index][i] += result * history[i];
    }
  }

  for (int i = ghistoryBits - 1; i > 0; i--) {
    history[i] = history[i - 1];
  }
  history[0] = result;
}
