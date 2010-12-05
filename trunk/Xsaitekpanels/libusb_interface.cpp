#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

/*
enum {
    LIB_USB_CLOSED,
    LIB_USB_OPENED,
    LIB_USB_DEVICE_OPENDED,
    LIB_USB_KERNEL_DETACHED,
    LIB_USB_INTERFACE_CLAIMED,
    LIB_USB_TRANSFERS_ALLOCATED
} libusb_state = LIB_USB_CLOSED;

*/

static libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
static libusb_context *context = NULL; //a libusb session
static libusb_context *ctx = NULL;     //a libusb session

static struct libusb_transfer *interrupt_transfer;

libusb_device_handle *radio_handle[4];

libusb_device_handle *multi_handle;
libusb_device_handle *switch_handle;



unsigned char radiowdata[4][20]; //data to write
unsigned char radiodata[4][3];   //data to read

int radiocnt = 0, multicnt = 0, switchcnt = 0;
int radionumb = 0;

int r; //for return values



bool find_saitek_panels()
{

   ssize_t cnt; //holding number of devices in list
   r = libusb_init(&context);  //initialize a library session
   libusb_device_descriptor desc;

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

            if(libusb_kernel_driver_active(radio_handle[radiocnt], 0) == 1) { //find out if kernel driver is attached
                    printf("Kernel Driver Active\n");
                    if(libusb_detach_kernel_driver(radio_handle[radiocnt], 0) == 0) //detach it
                            printf("Kernel Driver Detached!\n");
            }
            r = libusb_claim_interface(radio_handle[radiocnt], 0); //claim interface 0 (the first) of device
            if(r < 0) {
                    printf("Cannot Claim Interface\n");
                    return false;
            }




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
           //multicnt = 1;
           //if(libusb_kernel_driver_active(multi_handle, 0) == 1) { //find out if kernel driver is attached
           //        printf("multi Kernel Driver Active\n");
           //        if(libusb_detach_kernel_driver(multi_handle, 0) == 0) //detach it
           //                printf("multi Kernel Driver Detached!\n");
           //}
           //r = libusb_claim_interface(multi_handle, 0); //claim interface 0 (the first) of device
           //if(r < 0) {
           //        printf("Cannot Claim multi Interface\n");
           //        return false;
          //}

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

           //switchcnt = 1;
           //if(libusb_kernel_driver_active(switch_handle, 0) == 1) { //find out if kernel driver is attached
           //  printf("switch Kernel Driver Active\n");
           //  if(libusb_detach_kernel_driver(switch_handle, 0) == 0) //detach it
           //    printf("switch Kernel Driver Detached!\n");
           //}
           //r = libusb_claim_interface(switch_handle, 0); //claim interface 0 (the first) of device
           //if(r < 0) {
           //printf("Cannot Claim switch Interface\n");
           //  return false;
          //}

        }
     }

   }

   libusb_free_device_list(devs, 1); //free the list, unref the devices in it

   return true;
}


static void event_callback(struct libusb_transfer *transfer)
{
    printf("event_callback length=%d actual_length=%d: ",
           transfer->length, transfer->actual_length);
        if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
        int i;
        for (i=0;i<transfer->actual_length; i++) printf("0x%02x ", transfer->buffer[i]);
        printf("\n");

        //hci_dump_packet( HCI_EVENT_PACKET, 1, transfer->buffer, transfer->actual_length);
        //event_packet_handler(transfer->buffer, transfer->actual_length);
    }
        //int r = libusb_submit_transfer(transfer);
        //if (r) {
        //        printf("Error submitting interrupt transfer %d\n", r);
        //}
}

bool write_radio_panel(libusb_device_handle *radio_handle, unsigned char radiowdata[]){


    r = libusb_control_transfer(radio_handle, //handle
            LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT,  //request type
            0x09,                     //HID set_report request
            (3<< 8),                  //HID feature value
            0,                        //index
            radiowdata,    //data
            20,                       //length
            50);                    //timeout


    if(r > 0) //we wrote the 20 bytes successfully
            printf("Writing To Radio Successfuly!\n");
    else
            printf("Write To Radio Error\n");

    return true;
}


bool async_read_radio_panel(libusb_device_handle *radio_handle, unsigned char radiodata[]){

    interrupt_transfer = libusb_alloc_transfer(0); // 0 isochronous transfers Events

    // interrupt (= HCI event) handler
    libusb_fill_interrupt_transfer(interrupt_transfer, radio_handle, 0x81, radiodata, 3, event_callback, NULL, 3000) ;
        // interrupt_transfer->flags = LIBUSB_TRANSFER_SHORT_NOT_OK;
    r = libusb_submit_transfer(interrupt_transfer);
    if (r) {
                printf("Error submitting interrupt transfer %d\n", r);
    }
    printf("interrupt started\n");

    return true;

}

bool sync_read_radio_panel(libusb_device_handle *radio_handle, unsigned char radiodata[]){


    int transfered;
    r = libusb_interrupt_transfer(radio_handle,
                                  0x81,
                                  radiodata,
                                  3, //length
                                  &transfered,
                                  500);

    //cout<<"Read Data -> "<<radiodata[radcnt]<<  "<- -> ";
    // cout<<"transfered -> "<<transfered<<  " ";
    if(r == 0) //we read the 3 bytes successfully
            printf("Reading Successful!\n");
    else
            printf("Read Error\n");



    return true;

}




bool close_saitek_panels(){
   radionumb = radiocnt;
   while (radionumb > 0){
     r = libusb_release_interface(radio_handle[radionumb], 0); //release the claimed interface
     if(r!=0) {
       printf("Cannot Release Radio Panel %d Interface\n", radionumb );
       //return false;
     }
     printf("Released Radio Panel %d Interface\n", radionumb);
       if (libusb_attach_kernel_driver(radio_handle[radionumb], 0) == 0) //Attach Driver
         printf("Kernel Driver Atached!\n");

     libusb_close(radio_handle[radionumb]);
     printf("Closed Radio Panel # %d\n",radionumb);
     radionumb--;
   }


   r = libusb_release_interface(multi_handle, 0); //release the claimed interface
   if(r!=0) {
     printf("Cannot Release Multi Panel Interface\n");
     //return false;
   }
   if(r == 0){
     printf("Released Multi Panel Interface\n");
   }
   if (libusb_attach_kernel_driver(multi_handle, 0) == 0) //Attach Driver
       printf("Kernel Driver Atached!\n");

   libusb_close(multi_handle);


   r = libusb_release_interface(switch_handle, 0); //release the claimed interface
   if(r!=0) {
     printf("Cannot Release Switch Panel Interface\n");
     //return false;
   }
   if(r == 0){
     printf("Released Switch Panel Interface\n");
   }
   if (libusb_attach_kernel_driver(switch_handle, 0) == 0) //Attach Driver
       printf("Kernel Driver Atached!\n");



   libusb_close(switch_handle);

   libusb_exit(ctx); //close the session
   return true;
}
