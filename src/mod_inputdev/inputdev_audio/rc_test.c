/*
 * CRRCsim - the Charles River Radio Control Club Flight Simulator Project
 *   Copyright (C) 2004 - Joel Lienard, Lionel Cailler (original author)
 *   Copyright (C) 2004 - Jan Reucker
 *   Copyright (C) 2008 - Jens Wilhelm Wulf
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */
/*
 *****************************************************************************
 * rc_test.c
 Title: audio signal acquisition from radio control transmitter
 Authors:
  Joel Lienard
  Lionel Cailler
 
 Purpose:  This is a test  programm to check audio rc signal  acquisition.  It
  displays channel acquisition values.
  This code is based on origianl  Joel Lienard source code modified to support a
  portable library to a wide list of oses : http://www.portaudio.com
  It is a cross-platform code that works on Windows, Macintosh, Unix running OSS,
  SGI, BeOS, and perhaps other platforms by the time you read this.
 
 It's free, it works, enjoy.  This  is an open source project, so if you  don't
 like the way something works, help us fix it!
 
 * CHANGE HISTORY:
   03nov2003 add recompile with port_audio_v18_1 and text display improvements.
   06aug2004 add display of quantity of detected chanels 
 *
 *****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include "audio_rc.h"
#include "portaudio.h"

void audio_rc_test()
{
  float values[11];
  int i,n;
  // do{
  if(n=get_data_from_audio_interface(values))
  {
    for(i=0;i<10;i++)
      printf(" %2d:%+5.3f ",i,values[i]);
    printf("< %d chanels detected>\n",n);
  }
  //          }while(1);
}

/*******************************************************************/
int main(void);
int main(void)
{
  //    PortAudioStream *stream;
  //    int inputDevice = Pa_GetDefaultInputDeviceID();
  //    int        i,numDevices;
  //    PaDeviceInfo pdi;


  //    printf("Channel data acquisition configuration test\n");
  //    printf("\n"); fflush(stdout);

  //    numDevices = Pa_CountDevices();
  //    printf(    "Number of total devices : Pa_CountDevices()\t: %d.\n", numDevices );
  //    for( i=0; i<numDevices; i++ ) {
  //        pdi = *Pa_GetDeviceInfo( i );
  //        printf("Device(%d) name  \t: %s.\n", i, pdi.name);
  //        printf("\tMax Inputs   = %d\n", pdi.maxInputChannels  );
  //        printf("\tMax Outputs  = %d\n", pdi.maxOutputChannels  );
  //    }
  //    printf("Default Devic id : %d.\n", Pa_GetDefaultInputDeviceID() );
  int i,numDevices;
  PaDeviceInfo pdi;
  PaError    err;

  err = Pa_Initialize();
  if( err != paNoError )
    goto error;

  numDevices = Pa_CountDevices();
  printf(    "Number of total devices : Pa_CountDevices()\t: %d.\n", numDevices );
  for( i=0; i<numDevices; i++ )
  {
    pdi = *Pa_GetDeviceInfo( i );
    printf("Device(%d) name  \t: %s.\n", i, pdi.name);
    printf("\tMax Inputs   = %d\n", pdi.maxInputChannels  );
    printf("\tMax Outputs  = %d\n", pdi.maxOutputChannels  );
  }
  printf("Default Devic id : %d.\n", Pa_GetDefaultInputDeviceID() );

  Pa_Terminate();
  printf("\n");


  audio_rc_open();

  printf("\nNow display channel acquisition 0 up to 9 during 10seconds!!\n");
  fflush(stdout);


  for( i=0; i<100; i++ )
  {
    Pa_Sleep(100);
    /* launch test */
    audio_rc_test();
    fflush(stdout);
  }

  audio_rc_close();




  return 0;

error:
  Pa_Terminate();
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );

  return -1;
}
