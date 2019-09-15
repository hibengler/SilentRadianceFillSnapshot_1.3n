# - note - silentradiance (versus demos, and variants) uses a static libprojectm
# we will add more of this in the future to be able to shove silentradiance into
# debian

# apt-get install libpulse-dev
#apt-get install libasound2-dev
# apt-get install default-jdk
 
system: ../../other/libraries test_receiver  receiver_alsa  receiver decode_srd \
	 receiver_alsa6    receiver_no_sound simple_receiver_no_sound \
	 silentradiance 
	 touch system
	@echo done

demos: silentradiance_make_demo combine_raw_vids use_raw_vid_for_layer_background \
	silentradiance_make_mtv_like_demo 

clean:
	-rm -rf system
	-rm -rf *.o display/*.o
	

   

		
demos: silentradiance_make_demo combine_raw_vids use_raw_vid_for_layer_background \
	silentradiance_make_mtv_like_demo
	

simple_stuff: simple_receiver_no_sound simple_receiver

combine_raw_vids: combine_raw_vids.c
	gcc -O3 -o combine_raw_vids combine_raw_vids.c

use_raw_vid_for_layer_background:	use_raw_vid_for_layer_background.c
	gcc -g -o use_raw_vid_for_layer_background use_raw_vid_for_layer_background.c
	

../../other/libraries_resplandor:
	cd ../../other && make
	cd libpng && make
	cd libbpg && make


../../other/libraries:
	cd ../../other && make
	cd display && make
	cd libpng && make
	cd libbpg && make

test_receiver: test_receiver.c
	gcc -o test_receiver test_receiver.c

	


receiver_alsa: receiver_alsa.c music_decoder.c crypt.c
	gcc -O3 -o receiver_alsa receiver_alsa.c  crypt.c music_decoder.c  ../../other/libopus.a -lasound -lm

receiver_alsa6: receiver_alsa6.c music_decoder.c crypt.c
	gcc -O3 -o receiver_alsa6 receiver_alsa6.c  crypt.c music_decoder.c  ../../other/libopus.a -lasound -lm

decode_srd: decode_srd.c 
	gcc -g -o decode_srd decode_srd.c  ../../other/libopus.a -lasound -lm

receiver: receiver.c crypt.c network_receiver.c processor.c glue_alsa.c sound_alsa.c packet_summary.c credit_database.c analyzer.c \
	       file_receiver.c curl_web_stream.c seven_eight_codec.c
	gcc -pthread -g -o receiver -DNO_GRAPHICS=1 crypt.c curl_web_stream.c packet_summary.c receiver.c network_receiver.c processor.c glue_alsa.c  sound_alsa.c credit_database.c analyzer.c file_receiver.c \
	  seven_eight_codec.c \
	 ../../other/libopus.a -lasound -lm -lcurl

eereceiver: receiver.c crypt.c network_receiver.c processor.c glue_alsa.c sound_alsa.c packet_summary.c credit_database.c analyzer.c \
	       file_receiver.c curl_web_stream.c seven_eight_codec.c
	gcc -pthread -g -o receiver crypt.c curl_web_stream.c packet_summary.c receiver.c network_receiver.c processor.c glue_alsa.c  sound_alsa.c credit_database.c analyzer.c file_receiver.c \
	  seven_eight_codec.c \
	 ../../other/libopus.a -lasound -lm -lcurl

receiver_no_sound:  receiver_no_sound.c crypt.c network_receiver.c curl_web_stream.c processor.c glue_alsa.c  packet_summary.c credit_database.c analyzer.c file_receiver.c  \
		seven_eight_codec.c
	gcc -pthread -g -o receiver_no_sound -DNO_GRAPHICS=1 crypt.c packet_summary.c curl_web_stream.c receiver_no_sound.c network_receiver.c processor.c -DSR_NOSOUND=1 glue_alsa.c   credit_database.c analyzer.c file_receiver.c ../../other/libopus.a -lm \
	  seven_eight_codec.c \
	 -lcurl


simple_receiver_no_sound:  simple_receiver_no_sound.c simple_decrypt.c simple_curl_web_stream.c simple_processor.c     \
		simple_seven_eight_decode.c
	gcc -pthread -g -o simple_receiver_no_sound -DNO_GRAPHICS=1 simple_decrypt.c  simple_curl_web_stream.c simple_receiver_no_sound.c simple_processor.c -DSR_NOSOUND=1   ../../other/libopus.a -lm \
	  simple_seven_eight_decode.c \
	 -lcurl

simple_receiver:  simple_receiver.c simple_decrypt.c  simple_curl_web_stream.c simple_processor.c simple_sound_alsa.c    \
		simple_seven_eight_decode.c
	gcc -pthread -g -o simple_receiver -DNO_GRAPHICS=1 simple_decrypt.c  simple_curl_web_stream.c simple_receiver.c simple_processor.c simple_sound_alsa.c \
	 -lopus -lm \
	  simple_seven_eight_decode.c \
	 -lasound -lcurl

#	    ../../other/libopus.a -lm \
	 	 

receiver_roomforyourhead:  receiver_roomforyourhead.c crypt.c network_receiver.c curl_web_stream.c processor.c glue_alsa.c  packet_summary.c credit_database.c analyzer.c file_receiver.c \
		seven_eight_codec.c
	gcc -pthread -g -o receiver_roomforyourhead \
	 -DNO_GRAPHICS=1 crypt.c packet_summary.c receiver_roomforyourhead.c curl_web_stream.c \
	 network_receiver.c processor.c -DSR_NOSOUND=1 glue_alsa.c  file_receiver.c seven_eight_codec.c \
	 credit_database.c analyzer.c ../../other/libopus.a -lm  -lSDL -LGL -lGLU -lX11 -lcurl
	
crypt.o: crypt.c
	gcc -c -g -Bstatic  -I/usr/include/aubio crypt.c

	



silentradiance: silentradiance.c crypt.c network_receiver.c processor.c glue_alsa.c sound_alsa.c packet_summary.c credit_database.c analyzer.c \
    display_stuff.a png_stuff.a preferences.c file_receiver.c curl_web_stream.c  \
    seven_eight_codec.c store.c
	cd display;make
	cp /usr/local/lib/libprojectM.a .
	gcc -I display -pthread -DLINUX_CLIENT=1 -O3 \
	  -o silentradiance \
	  preferences.c crypt.c curl_web_stream.c seven_eight_codec.c  \
	  packet_summary.c silentradiance.c network_receiver.c \
	  processor.c glue_alsa.c file_receiver.c \
	  store.c \
	sound_alsa.c credit_database.c analyzer.c  -Bstatic ../../other/libopus.a  \
	display_stuff.a  png_stuff.a \
	libbpg/libbpg.a libprojectM.a display/libftgles2.a \
	 -Bdynamic -lasound -lm  -lGLESv2 -lglfw  \
	-lfreetype -lz -lcurl -lEGL -lstdc++  -ldl




resplandor: ../../other/libraries_resplandor silentradiance.c crypt.c network_receiver.c processor.c glue_alsa.c sound_alsa.c packet_summary.c credit_database.c analyzer.c \
    display_stuff.a png_stuff.a preferences.c \
    file_receiver.c curl_web_stream.c seven_eight_codec.c store.c 
	cd display;export SRTOPDEFS=" -DPROGRAM_LANGUAGE_VARIANT_IS_resplandor_pr_es" \
	   && make
	gcc -I display -pthread -DLINUX_CLIENT=1 \
	  -DPROGRAM_LANGUAGE_VARIANT_IS_resplandor_pr_es \
	 -O3 -o resplandor preferences.c crypt.c curl_web_stream.c seven_eight_codec.c packet_summary.c silentradiance.c network_receiver.c \
	  processor.c glue_alsa.c file_receiver.c \
	  store.c \
	sound_alsa.c credit_database.c analyzer.c ../../other/libopus.a  \
	display_stuff.a  png_stuff.a \
	libbpg/libbpg.a \
	-lasound -lm  -lGLESv2 -lglfw display/libftgles2.a \
	-lfreetype -lz -lcurl -lEGL -lstdc++ -lprojectM

reggaeton69: ../../other/libraries_resplandor silentradiance.c crypt.c network_receiver.c processor.c glue_alsa.c sound_alsa.c packet_summary.c credit_database.c analyzer.c \
    display_stuff.a png_stuff.a preferences.c \
    file_receiver.c curl_web_stream.c seven_eight_codec.c store.c 
	cd display;export SRTOPDEFS=" -DPROGRAM_LANGUAGE_VARIANT_IS_reggaeton69_pr_es" \
	   && make
	gcc -I display -pthread -DLINUX_CLIENT=1 \
	  -DPROGRAM_LANGUAGE_VARIANT_IS_reggaeton69_pr_es \
	 -O3 -o reggaeton69 preferences.c crypt.c curl_web_stream.c seven_eight_codec.c packet_summary.c silentradiance.c network_receiver.c \
	  processor.c glue_alsa.c file_receiver.c \
	  store.c \
	sound_alsa.c credit_database.c analyzer.c ../../other/libopus.a  \
	display_stuff.a  png_stuff.a \
	libbpg/libbpg.a \
	-lasound -lm  -lGLESv2 -lglfw display/libftgles2.a \
	-lfreetype -lz -lcurl -lEGL -lstdc++ -lprojectM

leleradiance: ../../other/libraries_resplandor silentradiance.c crypt.c network_receiver.c processor.c glue_alsa.c sound_alsa.c packet_summary.c credit_database.c analyzer.c \
    display_stuff.a png_stuff.a preferences.c \
    file_receiver.c curl_web_stream.c seven_eight_codec.c store.c 
	cd display;export SRTOPDEFS=" -DPROGRAM_LANGUAGE_VARIANT_IS_lele_radiance_en_us" \
	   && make
	gcc -I display -pthread -DLINUX_CLIENT=1 \
	  -DPROGRAM_LANGUAGE_VARIANT_IS_lele_radiance_en_us \
	 -O3 -o leleradiance preferences.c crypt.c curl_web_stream.c seven_eight_codec.c packet_summary.c silentradiance.c network_receiver.c \
	  processor.c glue_alsa.c file_receiver.c \
	  store.c \
	sound_alsa.c credit_database.c analyzer.c ../../other/libopus.a  \
	display_stuff.a  png_stuff.a \
	libbpg/libbpg.a \
	-lasound -lm  -lGLESv2 -lglfw display/libftgles2.a \
	-lfreetype -lz -lcurl -lEGL -lstdc++ -lprojectM


silentradiance_make_demo:  silentradiance_make_demo.c crypt.c network_receiver.c processor.c \
   glue_alsa.c sound_to_file.c packet_summary.c credit_database.c analyzer.c \
    display_stuff.a png_stuff.a preferences.c file_receiver.c \
   curl_web_stream.c seven_eight_codec.c store.c
	cd display;make
	gcc -g -I display -DLINUX_CLIENT=1 -pthread -o silentradiance_make_demo preferences.c \
	crypt.c curl_web_stream.c seven_eight_codec.c packet_summary.c \
	silentradiance_make_demo.c network_receiver.c \
	  processor.c glue_alsa.c file_receiver.c \
	  store.c \
	sound_to_file.c credit_database.c analyzer.c ../../other/libopus.a  \
	display_stuff.a  png_stuff.a \
	libbpg/libbpg.a \
	-lasound -lm  -lGLESv2 -lglfw display/libftgles2.a \
	-lfreetype -lz -lcurl -lEGL -lstdc++ -lprojectM



silentradiance_make_mtv_like_demo:  silentradiance_make_mtv_like_demo.c crypt.c network_receiver.c processor.c \
   glue_alsa.c sound_to_file.c packet_summary.c credit_database.c analyzer.c \
    display_stuff.a png_stuff.a preferences.c file_receiver.c curl_web_stream.c seven_eight_codec.c store.c
	cd display;make
	gcc -g -I display -DLINUX_CLIENT=1 -pthread -o \
	silentradiance_make_mtv_like_demo preferences.c \
	crypt.c curl_web_stream.c seven_eight_codec.c packet_summary.c \
	silentradiance_make_mtv_like_demo.c network_receiver.c \
	  processor.c glue_alsa.c file_receiver.c \
	  store.c \
	sound_to_file.c credit_database.c analyzer.c ../../other/libopus.a  \
	display_stuff.a  png_stuff.a \
	libbpg/libbpg.a \
	-lasound -lm  -lGLESv2 -lglfw display/libftgles2.a \
	-lfreetype -lz -lcurl -lEGL -lstdc++ -lprojectM
