#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>


static libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
static libusb_context *context = NULL; //a libusb session
static libusb_context *ctx = NULL;     //a libusb session
libusb_device_handle *radio_handle[4];
libusb_device_handle *multi_handle;
libusb_device_handle *switch_handle;
unsigned char radiowdata[4][20]; //data to write


bool find_saitek_panels()
{
   int r; //for return values
   ssize_t cnt; //holding number of devices in list
   r = libusb_init(&context);  //initialize a library session
   libusb_device_descriptor desc;
   int radiocnt = 0;
   if(r != 0)              //there was an init error
     return false;
   libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
   cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
   if(cnt < 0) {
     return false; //there was an error
   }
   ssize_t i; //for iterating through the list
   printf("\n");
   for(i = 0; i < cnt; i++) {
     r = libusb_get_device_descriptor(devs[i], &desc);
     if (r < 0) {
      return false; //failed to get device descriptor
     }

     if (desc.idVendor == 0x06a3){ //looking for saitel panels

        if (desc.idProduct == 0x0d05){ //looking for radio panels
            radiocnt++;  //looking for more than one
            printf("Found Radio Panel # %d  ", radiocnt);
            r = libusb_open(devs[i], &radio_handle[radiocnt]); //Use devs[i] from libusb list to create handle
            if(radio_handle[radiocnt] < 0){
                    printf("Cannot open Radio Panel\n");
                    return false;
            }
                    else
                    printf("Radio Panel Opened\n");

        }

        if (desc.idProduct == 0x0d06){ //looking for multi panel
           printf("Found Multi Panel ");
           r = libusb_open(devs[i], &multi_handle); //Use devs[i] from libusb list to create handle
           if(multi_handle < 0){
                   printf("Cannot open Multi Panel\n");
                   return false;
           }
                   else
                   printf("Multi Panel Opened\n");


        }

        if (desc.idProduct == 0x0d67){ // looking for switch panel
           printf("Found Switch Panel ");
           r = libusb_open(devs[i], &switch_handle); //Use devs[i] from libusb list to create handle
           if(switch_handle < 0){
                   printf("Cannot open switch Panel\n");
                   return false;
           }
                   else
                   printf("Switch Panel Opened\n");

        }
     }

   }

   libusb_free_device_list(devs, 1); //free the list, unref the devices in it

   return true;
}

bool close_saitek_panels(){

   libusb_close(radio_handle[1]);
   libusb_close(radio_handle[2]);
   libusb_close(multi_handle);
   libusb_close(switch_handle);

   libusb_exit(ctx); //close the session
   return true;
}
