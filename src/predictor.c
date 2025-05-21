//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
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
// gshare
uint32_t gshareGlobalHistoryRegister;
uint32_t gshareMask;
uint8_t *gshareBHT;


//
//TODO: Add your own Branch Predictor data structures here
//


void gshare_init_predictor(){
  uint32_t numBHTEntries = 1 << ghistoryBits; // power(2, ghistoryBits)

  gshareGlobalHistoryRegister = 0;
  gshareBHT = (uint8_t *)malloc(numBHTEntries * sizeof(uint8_t));
  memset(gshareBHT, WN, numBHTEntries * sizeof(uint8_t));
  gshareMask = numBHTEntries - 1; // a mask with 00...11..., the lower ghistoryBits bits is 1
}

uint8_t gshare_make_prediction(uint32_t pc){
  uint32_t BHTindex = (pc ^ gshareGlobalHistoryRegister) & gshareMask;
  uint8_t prediction = gshareBHT[BHTindex];

  // if (prediction == WT || prediction == ST){
  //   return TAKEN;
  // }
  // else{
  //   return NOTTAKEN;
  // }
  return (prediction == WT || prediction == ST) ? TAKEN : NOTTAKEN;
}

void gshare_train_predictor(uint32_t pc, uint8_t outcome){
  uint32_t BHTindex = (pc ^ gshareGlobalHistoryRegister) & gshareMask;

  // SN: 0, WN: 1, WT: 2, ST: 3
  if (outcome == TAKEN) {
    if (gshareBHT[BHTindex] != ST) {
      ++gshareBHT[BHTindex];
    }
  } 
  else if (outcome == NOTTAKEN) {
    if (gshareBHT[BHTindex] != SN) {
      --gshareBHT[BHTindex];
    }
  }
}

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      gshare_init_predictor();
      break;
    case TOURNAMENT:
      break;
    case CUSTOM:
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
      return gshare_make_prediction(pc);
    case TOURNAMENT:
    case CUSTOM:
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
      gshare_train_predictor(pc, outcome);
      break;
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
}
