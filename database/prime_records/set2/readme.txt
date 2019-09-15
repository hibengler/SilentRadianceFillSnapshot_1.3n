Set1 was a small set
set2 is everything

directories1.txt	- maintained to have new dir name, etc, build snobol table out of it

Grrew->ls -d */ >>/tmp/directories1  
ls -d */*/ >>/tmp/directories1  
Grrew->ls -d */*/*/ >>/tmp/directories1  
Grrew->ls -d */*/*/*/ >>/tmp/directories1  
cp /tmp/directories1  directories.txt

directories format:

dirname|cdnum|Style|new_dir_name|cd_artist|cd_producer|


!* questions for sexy:
A. Where is MerenRap 2 album?
B. Where is Funky Fever album?

C. What is PRIME CATALOGUE/Joey Tu mochila de amor/Michila Pistas   compared to PRIME CATALOGUE/Joey Tu mochila de amor/?

D. MC Reggae Non-Stop is awesome. Right?  Is this released?  It is Now!
E. MC Reggae Non-Stop was produced on a commodore Amiga computer? Dude!
F. We should charge more for these extended play tracks, right?  Whats the breakdown?
G. MC Reggae Non-Stop is a 32 bit float - which is higher quality than PCM.  Some of these songs are 96KHZ master quality- Much higher. What is the premium for Master quality?
For Master quality?
H. For master quality, you might be dealing with producers looking for samples. Can we sell a generic sample license?
I. Mixes - HAve DJ's use the musin in your catalog to make new stuff - and have special programming for this - live with facebook.
J. Los Mariachis (Maraichis Master) is cool, looks like you tried to cross promote. Dont see anything on the internet. can we use some of this?
K. Maraichis Master/Mariachis Intro/Dj Pangua - can we use some of this?
L. What about 5.1? Since this is a master with alternate tracks?
M. I want to have alternate tracks on some songs.
N. Merengue de PR DJ Edition - does teh DJ edition get a special price?
O. I heard Ma' Mai' by El Sexy Boy!
P. PRIME VIDEOS - I sal that Fransheska Menealo was not up. Lets smooth them up, put fractals on the edges: https://www.toolfarm.com/tutorial/upscale/
Q. Pistas de reggaeton - Again sample use, etc?
R. Could we use demBow to add additional drums to songs?
S. I could have links to youtube videos in your channel. then have the buy now card.
T. Where is the music for Reggaeton University?
U. Is TONO PISTAS any good?
V. is tono viejos any good?


!* Note Primeecords_CatalogueLinks.docx - itunes, amazon, spotify

!* TRL3 master - use these because they are 48000
!* Son a TOA Por Culpa de MElchor video to song merge with priep001



!* mv PRIME CATALOGUE/Joey Tu mochila de amor/Michila Pistas to  PRIME CATALOGUE/Joey Michila Pistas
!* mv LOS GLADIATORS FINAL/LOS GLADIATORS MP3/ to LOS GLADIATORS MP3/

/t/sr/set2 - this has the stuff we will rename and stuff



-----------------------------------

snobol4 d_to_d2.sno <directories.txt >directories2.txt


# test to see if mispellings
export PATH=~/opencvs/bar_database/bin:$PATH
fieldu 2 <directories.txt | wc -l
field 2 1 <directories.txt | sort

snobol4 d2_move.sno <directories2.txt
