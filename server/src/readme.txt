Drivers that work:


Hi Hib,
  Based on the research, (by looking at the code) I have done so far, I believe the following drivers will work with what you are looking for 

1. brcmfmac - driver which supports the following USB devices 
   
USB devices
Name 	USB Device ID 	Notes 
BCM43235 (rev == 3) 	? 	
BCM43236 (rev == 3) 	0a5c:bd17 	
BCM43238 (rev == 3) 	? 	
BCM43143 	0a5c:bd1e 	Supported in 3.7+ 
BCM43242 	0a5c:bd1f 	Supported in 3.7+ 
BCM43566 	0a5c:bd27 	Supported in 3.17+ 
BCM43569 		

 2. atheros9k_htc driver which supports the following devices

    https://wireless.wiki.kernel.org/en/users/drivers/ath9k_htc/devices
 The above driver is well written and will be easier to patch if needed.

 Some of the other drivers that I looked at, like p54usb are legacy and not really recommended.

I have spent about 3 hours till now on this and can spend more if you would like me to look further.  

My paypal a/c - mksingh_tx@yahoo.com

We can discuss over phone if needed to discuss how to proceed further.

Thanks,
--Manish

