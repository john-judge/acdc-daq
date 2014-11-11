#include "SuMo.h"
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

int SuMo::generate_ped(bool ENABLE_FILESAVE){
  sync_usb(0);
  //set_usb_read_mode(0);
  unsigned int num_ped_reads = 50;
  unsigned int psec_cnt;
  float temp;
  
  unsigned short raw_ped_data_array[AC_CHANNELS][256][num_ped_reads];
  unsigned int raw_sum_array[AC_CHANNELS][256];
  
  for(int targetAC = 0; targetAC < 4; targetAC++){
    
    set_usb_read_mode(targetAC);

    for(int k=0; k<AC_CHANNELS; k++){
      for(int i=0; i<256; i++){
	raw_sum_array[k][i] = 0;
      }
    }

    if(DC_ACTIVE[targetAC] == false){
      printf("skipping pedestal generation for board #%d\n", targetAC);
      continue;
    } 

    for(int k=0; k<num_ped_reads; k++){
      psec_cnt = 0;
      usleep(1000);
      software_trigger(1 << targetAC);
      usleep(1000);
      read_AC(false, 1, targetAC);
      usleep(1000);
      manage_cc_fifo(1);
    
      for(int i=0; i<AC_CHANNELS; i++){
	if(i>0 && i % 6 == 0)
	  psec_cnt ++;
	for(int j=0; j<256; j++){
	  raw_ped_data_array[i][j][k] = AC_RAW_DATA[psec_cnt][i%6*256+j];
	  raw_sum_array[i][j] += AC_RAW_DATA[psec_cnt][i%6*256+j];
	}
      }
      usleep(100);
    }
    //do a median ped generation:: allows for pulses in data during ped run
    int middle = int(num_ped_reads/2);
    for(int i = 0; i<AC_CHANNELS; i++){
      for(int j = 0; j<256; j++){
	//printf("organizing data %d %d\n", i, j);
	qsort(raw_ped_data_array[i][j], num_ped_reads, sizeof(unsigned short), compare);
	PED_DATA[targetAC][i][j] = (int)raw_ped_data_array[i][j][middle];
	raw_sum_array[i][j] /= num_ped_reads;
	//printf("%d\n", PED_DATA[i][j]);
      }
    }
    
    //calc std deviation for cell pedestals **diagnostic**
    for(int i = 0; i<AC_CHANNELS; i++){
      for(int j = 0; j<256; j++){
	temp = 0;
	for(int k = 0; k<num_ped_reads; k++){
	  //temp += (raw_ped_data_array[i][j][k]-raw_sum_array[i][j]) * (raw_ped_data_array[i][j][k]-raw_sum_array[i][j]);
	  temp += (raw_ped_data_array[i][j][k]-PED_DATA[targetAC][i][j]) * (raw_ped_data_array[i][j][k]-PED_DATA[targetAC][i][j]);
	}
	PED_STDEV[targetAC][i][j] = sqrt(temp);
      }
    }  

    if(ENABLE_FILESAVE != false){
      char ped_filename[500];
      sprintf(ped_filename, "calibrations/PED_DATA_%d.txt", targetAC);
      FILE* fped = fopen(ped_filename, "w");
      
      char ped_STDEV_filename[500];
      sprintf(ped_STDEV_filename, "calibrations/PED_DATA_STDEV_%d.txt", targetAC);
      FILE* fped_sd = fopen(ped_STDEV_filename, "w");      
      
      for(int i=0; i < 256; i++){
	//printf("%d\n", i);
	fprintf(fped, 
		"%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t\n",
		i,PED_DATA[targetAC][0][i],PED_DATA[targetAC][1][i],PED_DATA[targetAC][2][i],
		PED_DATA[targetAC][3][i],PED_DATA[targetAC][4][i],PED_DATA[targetAC][5][i],
		PED_DATA[targetAC][6][i],PED_DATA[targetAC][7][i],PED_DATA[targetAC][8][i],
		PED_DATA[targetAC][9][i],PED_DATA[targetAC][10][i],PED_DATA[targetAC][11][i],
		PED_DATA[targetAC][12][i],PED_DATA[targetAC][13][i],PED_DATA[targetAC][14][i],
		PED_DATA[targetAC][15][i],PED_DATA[targetAC][16][i],PED_DATA[targetAC][17][i],
		PED_DATA[targetAC][18][i],PED_DATA[targetAC][19][i],PED_DATA[targetAC][20][i],
		PED_DATA[targetAC][21][i],PED_DATA[targetAC][22][i],PED_DATA[targetAC][23][i],
		PED_DATA[targetAC][24][i],PED_DATA[targetAC][25][i],PED_DATA[targetAC][26][i],
		PED_DATA[targetAC][27][i],PED_DATA[targetAC][28][i],PED_DATA[targetAC][29][i]);
	fprintf(fped_sd, 
		"%i\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t\n",
		i,PED_STDEV[targetAC][0][i],PED_STDEV[targetAC][1][i],PED_STDEV[targetAC][2][i],
		PED_STDEV[targetAC][3][i],PED_STDEV[targetAC][4][i],PED_STDEV[targetAC][5][i],
		PED_STDEV[targetAC][6][i],PED_STDEV[targetAC][7][i],PED_STDEV[targetAC][8][i],
		PED_STDEV[targetAC][9][i],PED_STDEV[targetAC][10][i],PED_STDEV[targetAC][11][i],
		PED_STDEV[targetAC][12][i],PED_STDEV[targetAC][13][i],PED_STDEV[targetAC][14][i],
		PED_STDEV[targetAC][15][i],PED_STDEV[targetAC][16][i],PED_STDEV[targetAC][17][i],
		PED_STDEV[targetAC][18][i],PED_STDEV[targetAC][19][i],PED_STDEV[targetAC][20][i],
		PED_STDEV[targetAC][21][i],PED_STDEV[targetAC][22][i],PED_STDEV[targetAC][23][i],
		PED_STDEV[targetAC][24][i],PED_STDEV[targetAC][25][i],PED_STDEV[targetAC][26][i],
		PED_STDEV[targetAC][27][i],PED_STDEV[targetAC][28][i],PED_STDEV[targetAC][29][i]);

      }
      fclose(fped);
      fclose(fped_sd);
      printf("Pedestal values saved to file: %s\n", ped_filename);
      printf("*******\n");
    }
  }
  return 0;
    
}

int SuMo::load_ped(){
  
  for(int targetAC = 0; targetAC < 4; targetAC++){  
    char ped_filename[500];
    sprintf(ped_filename, "calibrations/PED_DATA_%d.txt", targetAC);
    
    FILE* fped_in = fopen(ped_filename, "r");

    if (fped_in == NULL){
      printf("No pedestal file found for board #%d\n", targetAC);
      continue;
    }
    int rows=0;
  
    while (!feof(fped_in)) {
      int i;
      char buf[100000];
      
      fgets(buf, 100000, fped_in);
      
      if (feof(fped_in))
	continue;
      
      if (count(buf, buf + strlen(buf), '\t') < AC_CHANNELS)
	continue;  // not 5-column or more
      
      sscanf(buf, "%d\t", &i);
      
      int tmp;
      sscanf(buf, 
	     "%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t\n",
	      &tmp,&PED_DATA[targetAC][0][i],&PED_DATA[targetAC][1][i],&PED_DATA[targetAC][2][i],
	      &PED_DATA[targetAC][3][i],&PED_DATA[targetAC][4][i],&PED_DATA[targetAC][5][i],
	      &PED_DATA[targetAC][6][i],&PED_DATA[targetAC][7][i],&PED_DATA[targetAC][8][i],
	      &PED_DATA[targetAC][9][i],&PED_DATA[targetAC][10][i],&PED_DATA[targetAC][11][i],
	      &PED_DATA[targetAC][12][i],&PED_DATA[targetAC][13][i],&PED_DATA[targetAC][14][i],
	      &PED_DATA[targetAC][15][i],&PED_DATA[targetAC][16][i],&PED_DATA[targetAC][17][i],
	      &PED_DATA[targetAC][18][i],&PED_DATA[targetAC][19][i],&PED_DATA[targetAC][20][i],
	      &PED_DATA[targetAC][21][i],&PED_DATA[targetAC][22][i],&PED_DATA[targetAC][23][i],
	      &PED_DATA[targetAC][24][i],&PED_DATA[targetAC][25][i],&PED_DATA[targetAC][26][i],
	      &PED_DATA[targetAC][27][i],&PED_DATA[targetAC][28][i],&PED_DATA[targetAC][29][i]);
        
        rows++;
    }
    fclose(fped_in);
  }
    return 0;
}

int SuMo::make_count_to_voltage(void){
     
  char LUT_filename[100], raw_scan_filename[100];
  ofstream ftemp[6];
  int dac_level;
  FILE* flut[6];
  int temp[AC_CHANNELS];

  for(int targetAC = 0; targetAC < 4; targetAC++){
    if(DC_ACTIVE[targetAC] == false){
      printf("no AC detected at address %d. \n", targetAC);
      continue;
    }
    
    sprintf(LUT_filename, "calibrations/LUT_%d.txt", targetAC);
    sprintf(raw_scan_filename, "calibrations/raw_lin_scan_%d.txt", targetAC);
  
    flut[targetAC] = fopen(LUT_filename, "w");
    ftemp[targetAC].open(raw_scan_filename, ios::trunc);
  
    memcpy(oldLUT[targetAC], LUT[targetAC], sizeof(LUT[targetAC]));
    for( int i = 0; i< AC_CHANNELS; i++){
      for( int j = 0; j<4096; j++){
	LUT[targetAC][j][i] = -1;
      }
    }

  }

  // looping thru DAC values:
  for( int i = 10; i < 4082; i+=80){

    dac_level = i;
    float voltage = i * 1.200/4096.0;

    set_pedestal_value(i);
    set_pedestal_value(i);
    printf("waiting....taking pedestal @ %f V\n", voltage);
    usleep(3000000); //allow DAC voltage to settle

    generate_ped(false);
    
    for(int targetAC = 0; targetAC < 4; targetAC++){
 
      if(DC_ACTIVE[targetAC] == true){
	
	for( int j = 0; j< AC_CHANNELS; j++)
	  temp[j] = 0;
	
	ftemp[targetAC] << i << "\t";

	  for(int m=0;m<AC_CHANNELS; m++){
	    for(int j=0;j<256; j++){
	      temp[m] += (int) PED_DATA[targetAC][m][j];
	    }
	    temp[m] /= 256;
	    ftemp[targetAC]<< temp[m] <<  "\t"; 
	    if (temp[m] > 0 && temp[m] < 4096)
	      LUT[targetAC][temp[m]][m] = voltage;  
                     
	  }
	
      ftemp[targetAC] << "\n";
      }
    }
	
    usleep(10000);
  }	
  
  printf("Now, computing linear interpolations of raw scan data...\n\n");

  for(int targetAC = 0; targetAC < 4; targetAC++){   
    if(DC_ACTIVE[targetAC] == true){
	ftemp[targetAC].close();


	// do linear interpolations
	int first, second, j;
	for(int i=0; i < AC_CHANNELS; i++) {  // for each channel
	  // find first value
	  for(j=0; LUT[targetAC][j][i] < 0; j++);  
	  // don't test equality with floating point #s
        
	  first = j;  // first is now the first LUT value
	  while (j < 4096) {
	    for(j++; LUT[targetAC][j][i] < 0 && j < 4096; j++);
            
	    if (j >= 4096) continue;
            
	    second = j;
            
	    for(int k=first+1; k < second; k++) {
	      LUT[targetAC][k][i] = LUT[targetAC][first][i] + 
		(float)(k-first)/(second-first) * 
		(LUT[targetAC][second][i] - LUT[targetAC][first][i] );
	    }

	    first = second;
	  }
	}
	for(int i=0; i < 4096; i++)
	  fprintf(flut[targetAC], 
		  "%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t\n", 
		  i,LUT[targetAC][i][0],LUT[targetAC][i][1],LUT[targetAC][i][2],
		  LUT[targetAC][i][3],LUT[targetAC][i][4],LUT[targetAC][i][5],
		  LUT[targetAC][i][6],LUT[targetAC][i][7],LUT[targetAC][i][8],
		  LUT[targetAC][i][9],LUT[targetAC][i][10],LUT[targetAC][i][11],
		  LUT[targetAC][i][12],LUT[targetAC][i][13],LUT[targetAC][i][14],
		  LUT[targetAC][i][15],LUT[targetAC][i][16],LUT[targetAC][i][17],
		  LUT[targetAC][i][18],LUT[targetAC][i][19],LUT[targetAC][i][20],
		  LUT[targetAC][i][21],LUT[targetAC][i][22],LUT[targetAC][i][23],
		  LUT[targetAC][i][24],LUT[targetAC][i][25],LUT[targetAC][i][26],
		  LUT[targetAC][i][27],LUT[targetAC][i][28],LUT[targetAC][i][29]);
	fclose(flut[targetAC]);
    }
  }
  printf("Done. Count to voltage LUTs saved to file\n");
  printf("*******\n");
  return 0;
}

/*
int SuMo::load_lut(){
    FILE* fin = fopen("calibrations/LUT.txt", "r");
    if (fin == NULL)
        return 1;
    
    int rows=0;
    
    while (!feof(fin)) {
        int i;
        char buf[1000];
        
        fgets(buf, 1000, fin);
        
        if (feof(fin))
            continue;
            
        if (count(buf, buf + strlen(buf), '\t') < AC_CHANNELS)
            continue;  // not 5-column or more
        
        sscanf(buf, "%d\t", &i);
        
        int tmp;
        sscanf(buf,  
"%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t\n", 
	    &tmp,&LUT[i][0],&LUT[i][1],&LUT[i][2],
	    &LUT[i][3],&LUT[i][4],&LUT[i][5],
	    &LUT[i][6],&LUT[i][7],&LUT[i][8],
	    &LUT[i][9],&LUT[i][10],&LUT[i][11],
	    &LUT[i][12],&LUT[i][13],&LUT[i][14],
	    &LUT[i][15],&LUT[i][16],&LUT[i][17],
	    &LUT[i][18],&LUT[i][19],&LUT[i][20],
	    &LUT[i][21],&LUT[i][22],&LUT[i][23],
	    &LUT[i][24],&LUT[i][25],&LUT[i][26],
	    &LUT[i][27],&LUT[i][28],&LUT[i][29]);
        
        rows++;
    }
    fclose(fin);
    return 0;
}
*/