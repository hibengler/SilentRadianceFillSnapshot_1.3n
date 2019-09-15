# apt-get install libpulse-dev
#apt-get install libasound2-dev
# apt-get install default-jdk
 
system: other/libraries sr_send_binary sr_turn_on_image   music_broadcast  src_to_wav src_to_numbers  \
	sslconnect  sr_command silent_radiance_pack.a music_broadcast_from_stdin  upload_to_server randomize build_cds_from_songs build_cds_from_songs.sbl \
	srplayer_to_sirad one_step_ahead playlist_into_video_demos playlist_into_mtv_like_video_demos redo_all_songs_dat get_lrc_timing  \
	srplayer srx_100_to_200 srx_200_to_100 srx_to_compress 
	
	@echo done

OUTS =filter_sr_playlist_log.sbl isrc_codes_sql_generator.sbl convert_playlist_log_to_sql.sbl convert_songs_dat_to_sql.sbl \
	  sr_send_binary sr_turn_on_image  music_broadcast src_to_wav src_to_numbers  \
	  playnow.sh sslconnect sr_command music_broadcast_from_stdin upload_to_server randomize one_step_ahead \
	  playlist_into_video_demos playlist_into_mtv_like_video_demos \
	  convert_incremental_playlist_log_to_sql.sbl srplayer_to_sirad \
	  start_follow count_fields.sno  guess_info.sno start_broadcast  build_cds_from_songs build_cds_from_songs.sbl \
	  html_to_tokens  html_to_tokens.sno  scrape_all_album_images  scrape_all_album_images.sbl  \
	  redo_all_songs_dat  redo_all_playlist_log follow_playlist_log follow_songs_dat get_lrc_timing srplayer  \
	   songs_to_consumables firefox_url gather_acuerdo_stats.sh gather_acuerdo_stats.sbl \
         make_radio_ramdisks.sh clear_old_sirad_files.sh order_fulfillment_on_verified_transaction.sh \
	   order_fulfillment_all_verified_paypal.sh generate_order_fulfillment_code.sbl \
	   always_verify_order_fulfillment.sh playnow_reggaeton.sh

install:
	chmod 755 $(OUTS)
	sudo cp $(OUTS)	/usr/local/bin


srx_200_to_100: srx_200_to_100.o srx_format.o

srx_100_to_200: srx_100_to_200.o srx_format.o

srx_to_compress: srx_to_compress.o srx_format.o

other/libraries:
	cd other && make




get_lrc_timing: get_lrc_timing.c aubio_utils.c 
	gcc -g -Bstatic -o get_lrc_timing -I/usr/include/aubio  \
	 get_lrc_timing.c aubio_utils.c aubio_jackio.c -lm  \
	  -laubio -lsamplerate -lavresample -lavcodec -lavformat -lavutil -lsndfile other/libopus.a -Bdynamic -ljack 


srplayer: srplayer.c aubio_utils.c shm_communication.o srx_format.o sr_handling_srx_common.o \
	srplayer_handling_sound_common.o
	gcc -g -Bstatic -o srplayer -I/usr/include/aubio  \
	 srplayer.c aubio_utils.c srplayer_handling_sound_common.o sr_handling_srx_common.o \
	 aubio_jackio.c shm_communication.o srx_format.o -lm  \
	  -laubio -lsamplerate -lavresample -lavcodec -lavformat -lavutil -lsndfile other/libopus.a -lasound  -Bdynamic -ljack 

srplayer_to_sirad: srplayer_to_sirad.c silent_radiance_pack.a c_aubio_utils.o srx_format.o sr_handling_srx_common.o
	gcc -g -Bstatic -o srplayer_to_sirad -I/usr/include/aubio  \
       srplayer_to_sirad.c c_aubio_utils.c sr_handling_srx_common.o srx_format.o silent_radiance_pack.a \
	  -laubio -lsamplerate -lavresample -lavcodec \
	  -lavformat -lavutil -lsndfile other/libopus.a -lasound  -Bdynamic -ljack -lm


randomize: randomize.c
	gcc -o randomize randomize.c

music_broadcast: music_broadcast_server.o music_broadcast_server.h \
        music_broadcast.o aubio_utils.o transmitter.o crypt.o shm_communication.o \
	aubio_jackio.o
	gcc -g -Bstatic -o music_broadcast  -I/usr/include/aubio  \
	music_broadcast.o  music_broadcast_server.o aubio_utils.o aubio_jackio.o transmitter.o  crypt.o \
	shm_communication.o  \
	 -lm  -laubio -lsamplerate -lavresample -lavutil -lavcodec -lavformat -lsndfile other/libopus.a -Bdynamic -ljack -lasound

music_broadcast_from_stdin: music_broadcast_from_stdin.c silent_radiance_pack.a
	gcc -g -Bstatic -o music_broadcast_from_stdin -I/usr/include/aubio  music_broadcast_from_stdin.c  \
	  silent_radiance_pack.a \
	 -lm  -laubio -lsamplerate -lavresample -lavutil -lavcodec -lavformat -lsndfile other/libopus.a -Bdynamic -ljack -lasound

silent_radiance_pack.o: music_broadcast_server.c music_broadcast_server.h \
	music_broadcast.c aubio_utils.c transmitter.c crypt.c shm_communication.c
	gcc -g -Bstatic  -I/usr/include/aubio  \
	music_broadcast.c  music_broadcast_server.c aubio_utils.c aubio_jackio.c transmitter.c  crypt.c \
	shm_communication.c  -c -o silent_radiance_pack.o \
	 -lm  -laubio -lsamplerate -lavresample -lavutil -lavcodec -lavformat -lsndfile other/libopus.a -Bdynamic -ljack -lasound


one_step_ahead: one_step_ahead.c shm_communication.c music_dat_processing.o shm_communication.h
	gcc -g    one_step_ahead.c shm_communication.c music_dat_processing.o  -o one_step_ahead

playlist_into_video_demos: playlist_into_video_demos.c shm_communication.c music_dat_processing.o shm_communication.h
	gcc -g    playlist_into_video_demos.c shm_communication.c music_dat_processing.o  -o playlist_into_video_demos

playlist_into_mtv_like_video_demos: playlist_into_mtv_like_video_demos.c shm_communication.c music_dat_processing.o shm_communication.h
	gcc -g    playlist_into_mtv_like_video_demos.c shm_communication.c music_dat_processing.o  -o playlist_into_mtv_like_video_demos
	

src_to_wav: src_to_wav.c 
	gcc -g -o src_to_wav -Iother/wavtools-0.2.1 src_to_wav.c other/libwav.a other/libopus.a -lm

src_to_numbers: src_to_numbers.c 
	gcc -g -o src_to_numbers -Iother/wavtools-0.2.1 src_to_numbers.c  other/libopus.a -lm


sslconnect: sslconnect.c
	gcc -o sslconnect sslconnect.c -lssl -lcrypto 

sr_command: sr_command.c shm_communication.c shm_communication.h
	gcc -g -o sr_command sr_command.c shm_communication.c

sr_send_binary: sr_send_binary.c shm_communication.c shm_communication.h
	gcc -g -o sr_send_binary sr_send_binary.c shm_communication.c

sr_turn_on_image: sr_turn_on_image.c shm_communication.c shm_communication.h
	gcc -g -o sr_turn_on_image sr_turn_on_image.c shm_communication.c

music_broadcast_server.o: music_broadcast_server.c music_broadcast_server.h
	gcc -c -g -Bstatic  -I/usr/include/aubio music_broadcast_server.c

music_broadcast.o: music_broadcast.c
	gcc -c -g -Bstatic  -I/usr/include/aubio music_broadcast.c

aubio_utils.o: aubio_utils.c
	gcc -c -g -Bstatic  -I/usr/include/aubio aubio_utils.c

transmitter.o: transmitter.c
	gcc -c -g -Bstatic  -I/usr/include/aubio transmitter.c

crypt.o: crypt.c
	gcc -c -g -Bstatic  -I/usr/include/aubio crypt.c

aubio_jackio.o: aubio_jackio.c
	gcc -c -g -Bstatic  -I/usr/include/aubio aubio_jackio.c

shm_communication.o: shm_communication.c
	gcc -c -g -Bstatic  -I/usr/include/aubio shm_communication.c
	
silent_radiance_pack.a:  music_broadcast.o  music_broadcast_server.o aubio_utils.o aubio_jackio.o transmitter.o  crypt.o \
        shm_communication.o
	ar rcs silent_radiance_pack.a music_broadcast.o  music_broadcast_server.o \
	aubio_utils.o aubio_jackio.o transmitter.o  crypt.o \
	shm_communication.o 


upload_to_server: upload_to_server.c
	gcc -g  -o upload_to_server upload_to_server.c -lpthread
