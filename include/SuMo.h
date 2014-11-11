#ifndef SUMO_H
#define SUMO_H

#include <stdlib.h>
#include <stdio.h>
#include "stdUSB.h"

#define cc_buffersize 31
#define ac_buffersize 8100
#define all_ac_buffersize 24400
#define AC_CHANNELS   30
#define psec_buffersize 1536
#define info_buffersize 16
#define USB_READ_OFFSET 13

#define WRAP_CONSTANT 90

class SuMo{

 public:
  SuMo();
  ~SuMo();

  void sync_usb(bool SYNC);
  void software_trigger(unsigned int SOFT_TRIG_MASK);
  void reset_dll();
  void reset_self_trigger();
  void align_lvds();
  void set_self_trigger(bool ENABLE_TRIG, bool SYS_TRIG_OPTION, bool RATE_ONLY, bool TRIG_SIGN);
  void set_self_trigger_mask(int mask);
  void set_pedestal_value(unsigned int PED_VALUE);
  void set_dll_vdd(unsigned int VALUE);
  void set_trig_threshold(unsigned int TRIG_VALUE);
  void set_ro_target_count(unsigned int TARGET_RO_COUNT);
  void set_usb_read_mode(unsigned int READ_MODE);
  void manage_cc_fifo(bool VALUE);
  void toggle_LED(bool EN);
  void toggle_CAL(bool EN);
  
  int check_usb();
  int read_CC(bool SHOW_CC_STATUS,bool SHOW_AC_STATUS);
  int read_AC(bool ENABLE_FILESAVE, unsigned int trig_mode, int AC_adr);
  int read_ACS(bool ENABLE_FILESAVE);

  int dump_data();
  int get_AC_info(bool PRINT);
  int generate_ped(bool ENABLE_FILESAVE);
  int make_count_to_voltage(void);
  int load_lut();
  int load_ped();
  int scope_AC(int trig_mode, bool output_mode, int AC_adr);
  int log_data(const char* log_filename, unsigned int NUM_READS, int trig_mode, int acq_rate);
  int log_data_hd5(const char* log_filename, unsigned int NUM_READS, int trig_mode, 
		   int acq_rate, int boards);

  int check_active_boards(void);

  stdUSB usb;
 
 private:
  unsigned int LAST_AC_INSTRUCT;
  unsigned int LAST_LAST_AC_INSTRUCT;
  unsigned int EVENT_COUNT;
  unsigned long long int TIMESTAMP;
  float VBIAS[6];
  float RO_CNT[6];
  float RO_TARGET_CNT[6];
  float RO_DAC_VALUE[6];
  int LAST_SAMPLING_BIN[6];
  float TRIGGER_THRESHOLD[6];
  bool SELF_TRIG_MODE[6];
  bool TRIG_SIGN[6];
  bool SAMPLING_SPEED[6];

  //metadata from CC
  unsigned int LAST_CC_INSTRUCT;
  unsigned long long CC_TIMESTAMP;
  unsigned int CC_EVENT_COUNT;
  unsigned int CC_BIN_COUNT;
  unsigned int CC_EVENT_NO;

  static int compare ( const void * a, const void * b){
    return(*(unsigned short*)a - *(unsigned short*)b);
  }

  int unwrap(int ASIC);
  void unwrap_baseline(int *baseline, int ASIC);
  
  bool DC_ACTIVE[4];
  unsigned short AC_RAW_DATA[5][psec_buffersize];
  unsigned short AC_INFO[5][info_buffersize];
  unsigned int   CC_INFO[cc_buffersize];
  unsigned int PED_DATA[4][AC_CHANNELS][256];
  float PED_STDEV[4][AC_CHANNELS][256];
  float LUT[6][4096][AC_CHANNELS];
  float oldLUT[6][4096][AC_CHANNELS];

};       

#endif