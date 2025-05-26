//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <stdbool.h>
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
uint8_t* BHT_local;
uint8_t* BHT_global;
uint8_t* chooser;
uint32_t* PHT;
int GHR;


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
      init_predictor_TOURNAMENT();
      break;
    case CUSTOM:
      // Initialize custom predictor data structures
      break;
    default:
      break;
  }
}

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
      return make_prediction_TOURNAMENT(pc);
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Make a prediction for the Tournament predictor
// This function will use the global history and local history
// to make a prediction
uint8_t
make_prediction_TOURNAMENT(uint32_t pc)
{
  uint32_t mask_pc = (1 << pcIndexBits) - 1;
  uint32_t mask_PHT = (1 << lhistoryBits) - 1;

  // 1. obtain the history register
  uint32_t index_PHT = pc & mask_pc;            
  uint32_t value_PHT = PHT[index_PHT] & mask_PHT;
  uint8_t prediction_local = BHT_local[value_PHT]; 
  uint8_t prediction_global = BHT_global[GHR];

  // 2. get the chooser
  uint8_t choose = chooser[GHR];

  // Use the global history register if choose is > 1
  if (choose <= 1) {
      return prediction_local <= 1 ? NOTTAKEN : TAKEN;
  } else {
      return prediction_global <= 1 ? NOTTAKEN : TAKEN;
  }

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
        train_predictor_TOURNAMENT(pc, outcome);
        break;
    case CUSTOM:
        break;
    default:
        break;
  }
}

void 
train_predictor_TOURNAMENT(uint32_t pc, uint8_t outcome){
   // record the original prediction
    // Generate bitmasks for indexing
  uint32_t mask_pc  = (1 << pcIndexBits) - 1;
  uint32_t mask_PHT = (1 << lhistoryBits) - 1;
  uint32_t mask_GHR = (1 << ghistoryBits) - 1;

  // Extract indices and predictions
  int index_PHT        = pc & mask_pc;
  int local_history    = PHT[index_PHT];
  int prediction_local = BHT_local[local_history];
  int prediction_global= BHT_global[GHR];
  int chooser_value    = chooser[GHR];

  // Determine if local and global predictors disagree
  bool disagree = (prediction_local <= 1 && prediction_global >= 2) || 
                  (prediction_local >= 2 && prediction_global <= 1);

  // Update chooser based on correct predictor
  if (disagree) {
      if (outcome == NOTTAKEN) {
          if (prediction_local <= 1 && chooser_value > 0) {
              chooser[GHR]--;
          } else if (prediction_global <= 1 && chooser_value < 3) {
              chooser[GHR]++;
          }
      } else { // outcome == TAKEN
          if (prediction_local >= 2 && chooser_value > 0) {
              chooser[GHR]--;
          } else if (prediction_global >= 2 && chooser_value < 3) {
              chooser[GHR]++;
          }
      }
  }

  // Update local predictor
  if (outcome == NOTTAKEN) {
      if (prediction_local > 0) BHT_local[local_history]--;
  } else {
      if (prediction_local < 3) BHT_local[local_history]++;
  }

  // Update global predictor
  if (outcome == NOTTAKEN) {
      if (prediction_global > 0) BHT_global[GHR]--;
  } else {
      if (prediction_global < 3) BHT_global[GHR]++;
  }

  // Update history registers
  PHT[index_PHT] = ((local_history << 1) | (outcome == TAKEN ? 1 : 0)) & mask_PHT;
  GHR = ((GHR << 1) | (outcome == TAKEN ? 1 : 0)) & mask_GHR;
}