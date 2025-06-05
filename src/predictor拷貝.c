//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "YiXin Huang";
const char *studentID   = "A69032401";
const char *email       = "yih102@ucsd.edu";

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
uint32_t gshareGlobalHistoryRegister;
uint32_t gshareMask;
uint8_t *gshareBHT;

uint8_t* BHT_local;
uint8_t* BHT_global;
uint8_t* chooser;
uint32_t* PHT;
int GHR;


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
// Initialize the Tournament predictor
// This function will initialize the local history register
// and the global history register
void  init_predictor_TOURNAMENT()
{
  // Initialize the global history register
  GHR = 0;

  // Initialize the local history register
  BHT_local = (uint8_t*) malloc(sizeof(uint8_t) * (1 << lhistoryBits));
  for (int i = 0; i < (1 << lhistoryBits); i++) {
    BHT_local[i] = WN;
  }

  // Initialize the global history table
  BHT_global = (uint8_t*) malloc(sizeof(uint8_t) * (1 << ghistoryBits));
  for (int i = 0; i < (1 << ghistoryBits); i++) {
    BHT_global[i] = WN;
  }

  // Initilize the pattern history table
  PHT = (uint32_t*) malloc(sizeof(uint32_t) * (1 << pcIndexBits));
  for (int i = 0; i < (1 << pcIndexBits); i++) {
    PHT[i] = 0;
  }

  // Initialize the chooser table
  chooser = (uint8_t*) malloc(sizeof(uint8_t) * (1 << ghistoryBits));
  for (int i = 0; i < (1 << ghistoryBits); i++) {
    chooser[i] = WG;
  }
}




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
      gshare_init_predictor();
      break;
    case TOURNAMENT:
      // Initialize the local history register
      init_predictor_TOURNAMENT();
      break;
    case CUSTOM:
      // Initialize custom predictor data structures
      init_custom_predictor();
      break;
    default:
      break;
  }
}

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
      return gshare_make_prediction(pc);
    case TOURNAMENT:
      return make_prediction_TOURNAMENT(pc);
    case CUSTOM:
      // Call the custom prediction function for perceptron-based predictors
      return make_custom_prediction(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}
