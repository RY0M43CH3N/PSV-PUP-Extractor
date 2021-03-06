#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/modulemgr.h>

#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>

#include <psp2/sysmodule.h>
#include <psp2/types.h>

#include "main.h"
#include "graphics.h"




uint32_t SfcGetAddressIsLittle(const void *buf, size_t int_byte, size_t read_byte) {

	const uint8_t *p;

	p = buf;

	for(int i=int_byte;i<(int_byte + read_byte);i++){

		printf("%02X", p[i]);

	}

	return 0;

}


uint32_t SfcGetAddressIsBig(const void *buf, size_t int_byte, size_t read_byte){

	const uint8_t *p;

	p = buf;

	int i;

	for(i=(int_byte + read_byte - 1);i>=int_byte;i--){

		printf("%02X", p[i]);

	}

	return 0;

}




int SfcPupExtractor(pup_path) {



	printf("PUP Path : %s\n\n\n",pup_path);

	printf("Start Extractor");

	for(int i=0;i<3;i++){
		sceKernelDelayThread(0.2*1000*1000);
		printf(".");
	}

	sceKernelDelayThread(0.1*1000*1000);
	printf("\n\n\n");


	SceUID file_check = sceIoOpen(pup_path, SCE_O_RDONLY, 0);
	if(file_check >= 0){


		char header_buffer[0x2000];//8KB

		const uint32_t *header_buffer_u32t;
		const uint16_t *header_buffer_u16t;


		int rd = sceIoRead(file_check, header_buffer, 0x2000);

		header_buffer_u32t = header_buffer;


		printf("header_buffer = 0x%X\n\n",rd);


		printf("PUP magic check ... ");


		if(header_buffer[0] == 0x53 &&
		   header_buffer[1] == 0x43 &&
		   header_buffer[2] == 0x45 &&
		   header_buffer[3] == 0x55 &&
		   header_buffer[4] == 0x46 &&
		   header_buffer[5] == 0x00 &&
		   header_buffer[6] == 0x00 &&
		   header_buffer[7] == 0x01){


			printf("ok, PUP file.\n\n");


			SfcGetPupInfo(header_buffer);


			printf("Please wait 3 seconds.\n\n\n");
			sceKernelDelayThread(3*1000*1000);



			char pup_dec_dir[0x100];
			sprintf(pup_dec_dir, "%s%s_%07X/", pup_dec_base_dir, pup_type, image_version);


			NotExistMkdir(pup_dec_dir);



			for(int i=0;i<file_count;i++){


				SfcExtractPupFiles(pup_dec_dir, file_check, i, file_count, header_buffer);


				printf("\n");
				sceKernelDelayThread(0.3*1000*1000);

			}


		}else{

			printf("Failed, Not PUP file.\n\n");

		}


	}else{
		printf("Error : File does not exist.\n\n");
		printf("res : 0x%08X\n\n",file_check);
		return file_check;
	}

	sceIoClose(file_check);
	printf("\n\n");
	
	return 0;
}



int NotExistMkdir(a1){

	int dfd = sceIoDopen(a1);
	if(dfd >= 0){

		sceIoDclose(dfd);

	}else{

		sceIoMkdir(a1, 0777);

	}

}


int main(int argc, char *argv[]) {

	psvDebugScreenInit();

	printf("*** Pup Extractor ***\n\n\n");


	NotExistMkdir(pup_dec_base_dir);

	NotExistMkdir(read_pup_dir);


	SceUID dfd2 = sceIoDopen(read_pup_dir);

	int res = 0;

	do {
		SceIoDirent dir;
		memset(&dir, 0, sizeof(SceIoDirent));

		res = sceIoDread(dfd2, &dir);
		if (res > 0) {

			char read_pup_dir_path[0x100];
			sprintf(read_pup_dir_path, "%s%s", read_pup_dir, dir.d_name);

			SfcPupExtractor(read_pup_dir_path);


		}
	} while (res > 0);

	sceIoDclose(dfd2);




	printf("*** ALL DONE ***\n\n");
	printf("This app will close in 3 seconds!\n");
	
	sceKernelDelayThread(3*1000*1000);

	sceKernelExitProcess(0);
	return 0;
}
