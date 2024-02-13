/******************************************************************************
 * Programacao Concorrente
 * 
 * Francisco Apolinário		1103681
 * Inês Mesquita			1103645 
 *
 * Projecto - Parte1
 *                           serial-basic.c
 * 
 * Compilacao: 	gcc -Wall -pedantic serial-basic.c image-lib.c -g -o ap-paralelo-2 -pthread -lgd
 *           
 *****************************************************************************/

#include <stdio.h>
#include <pthread.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <gd.h>
#include <string.h>
#include "image-lib.h"

char dict1[50], dict2[50], dict3[50];

/* the directories wher output files will be placed */
#define RESIZE_DIR "/Resize-dir/"
#define THUMB_DIR "/Thumbnail-dir/"
#define WATER_DIR "/Watermark-dir/"

/* structure that is passed to thread */
typedef struct _Images
{
	int nn_files;	/* Number of files */
	char **files;	/* files directory */
	char **outputfiles;	/* files name */

} Images;

/******************************************************************************
 * watermark()
 *
 * Arguments: pict
 * Returns: The normal exit of program is zero return value, when it has errors return is non-zero
 * Side-Effects: creates watermarks
 *
 * Description: implementation of the function that creates watermarks 
 * 
 *****************************************************************************/

void * watermark (void* pict )
{
	int i;
	char verify[100];
	Images *pict_w = (Images*)pict;

	/* input images */
	gdImagePtr in_img,  watermark_img;
	/* output images */
	gdImagePtr out_watermark_img;

	
	/* file name of the image created and to be saved on disk	 */
	char out_file_name[100];

	/* open and read watermark */
	watermark_img = read_png_file("watermark.png");
	if(watermark_img == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		exit(0);
	}

	for (i = 0; i < pict_w -> nn_files; i++){
		strcpy (verify, dict1);
		strcat (verify, pict_w -> outputfiles[i]);

		/* check if the file already exists */
		if (access (verify, F_OK) == 0){
			continue;
		}	
		/* load of the input file */
		in_img = read_png_file(pict_w -> files[i]);
		if (in_img == NULL){
			fprintf(stderr, "Impossible to read %s image\n", pict_w -> outputfiles[i]);
			continue;
		}

		/* add watermark */
		out_watermark_img = add_watermark(in_img, watermark_img);
		if (out_watermark_img == NULL) {
			fprintf(stderr, "Impossible to creat thumbnail of %s image\n", pict_w -> outputfiles[i]);
		} else {
			/* save watermark */
			sprintf(out_file_name, "%s%s", dict1, pict_w -> outputfiles[i]);
			if(write_png_file(out_watermark_img, out_file_name) == 0) {
				fprintf(stderr, "Impossible to write %s image\n", out_file_name);
			}
			gdImageDestroy(out_watermark_img);
		}
		gdImageDestroy(in_img);
	}
	gdImageDestroy(watermark_img);
	return NULL;

}

/******************************************************************************
 * resize()
 *
 * Arguments: pict
 * Returns: NULL
 * Side-Effects: creates resize
 *
 * Description: implementation of the function that adds watermark and creates 
 * resizes
 * 
 *****************************************************************************/

void * resize (void* pict )
{
	int i;
	char verify[100];
	Images *pict_r = (Images*)pict;

	/* input images */
	gdImagePtr in_img,  watermark_img;
	/* output images */
	gdImagePtr out_resized_img, out_watermark_img;

	char out_file_name[100];

	/* open and read watermark */
	watermark_img = read_png_file("watermark.png");
	if(watermark_img == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		exit(0);
	}

	for (i = 0; i < pict_r -> nn_files; i++){	

		strcpy (verify, dict2);
		strcat (verify, pict_r -> outputfiles[i]);

		/* check if the file already exists */
		if (access (verify, F_OK) == 0){
			continue;
		}	
		/* load of the input file */
	    in_img = read_png_file(pict_r -> files[i]);
		if (in_img == NULL){
			fprintf(stderr, "Impossible to read %s image\n", pict_r -> outputfiles[i]);
			continue;
		}
		/* resizes of each image */
		out_watermark_img = add_watermark(in_img, watermark_img);
		if (out_watermark_img == NULL) {
			fprintf(stderr, "Impossible to add watermark to %s image\n", pict_r -> outputfiles[i]);
		} else {
			out_resized_img = resize_image(out_watermark_img, 800);
			if (out_resized_img == NULL) {
				fprintf(stderr, "Impossible to resize %s image\n", pict_r -> outputfiles[i]);
			} else {
				/* save resized */
				sprintf(out_file_name, "%s%s", dict2, pict_r -> outputfiles[i]);
				if(write_png_file(out_resized_img, out_file_name) == 0){
					fprintf(stderr, "Impossible to write %s image\n", out_file_name);
				}
				gdImageDestroy(out_resized_img);
			}
			gdImageDestroy(out_watermark_img);
		}
		gdImageDestroy(in_img);
	}
	gdImageDestroy(watermark_img);
	return NULL;

}

/******************************************************************************
 * thumbnail()
 *
 * Arguments: pict
 * Returns: NULL
 * Side-Effects: creates thumbnail
 *
 * Description: implementation of the function that adds watermark and creates 
 * thumbnails
 * 
 *****************************************************************************/

void * thumbnail(void* pict)
{
	int i;
	char verify[100];
	Images *pict_t = (Images*)pict;

	/* input images */
	gdImagePtr in_img,  watermark_img;
	/* output images */
	gdImagePtr out_thumb_img, out_watermark_img;

	char out_file_name[100];

	/* open and read watermark */
	watermark_img = read_png_file("watermark.png");
	if(watermark_img == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		exit(0);
	}

	for (i = 0; i < pict_t -> nn_files; i++){	
		strcpy (verify, dict3);
		strcat (verify, pict_t -> outputfiles[i]);

		/* check if the file already exists */
		if (access (verify, F_OK) == 0){
			continue;
		}	
		/* load of the input file */
	    in_img = read_png_file(pict_t -> files[i]);
		if (in_img == NULL){
			fprintf(stderr, "Impossible to read %s image\n", pict_t -> outputfiles[i]);
			continue;
		}

		/* creation of thumbnail image */
		out_watermark_img = add_watermark(in_img, watermark_img);
		if (out_watermark_img == NULL) {
			fprintf(stderr, "Impossible to creat thumbnail of %s image\n", pict_t -> outputfiles[i]);
		} else {
			out_thumb_img = make_thumb(out_watermark_img, 150);
			if (out_thumb_img == NULL) {
				fprintf(stderr, "Impossible to creat thumbnail of %s image\n", pict_t -> outputfiles[i]);
			} else {
				/* save thumbnail image */
				sprintf(out_file_name, "%s%s", dict3, pict_t -> outputfiles[i]);
				if(write_png_file(out_thumb_img, out_file_name) == 0){
					fprintf(stderr, "Impossible to write %s image\n", out_file_name);
				}
				gdImageDestroy(out_thumb_img);
			}
			gdImageDestroy(out_watermark_img);
		}
		gdImageDestroy(in_img);
	}
	gdImageDestroy(watermark_img);
	return NULL;
}

/******************************************************************************
 * main()
 *
 * Arguments: argc, argv
 * Returns: None
 * Side-Effects: creates thumbnail, resized copy and watermarked copies
 *               of images
 *
 * Description: implementation of the complex serial version 
 *              This application only works for a fixed pre-defined set of files
 *
 *****************************************************************************/

int main(int argc, char **argv)
{
	/* verify if exists the right number of arguments */
	if (argc != 2) {
        exit(0);
    }

	pthread_t * thread_id;
	Images *pict;
	FILE *ficheiro;
	char nome[50], im[30], foto[50];
	char *point, *directoria = argv[1];
	int i, max_name = 0;

	/* struct allocation */
	pict = (Images*) malloc(sizeof(Images));
	if (pict == NULL){
		fprintf(stderr, "Impossible to allocate\n");
		exit(0);
	}    

	/* length of the files array (number of files to be processed	 */   
	pict -> nn_files = 0;

	strcpy (nome, directoria);
	strcat (nome, "/image-list.txt");

	/* file opening */
	ficheiro = fopen(nome, "r");
	if (ficheiro == NULL){
		fprintf(stderr, "Impossible to open file %s\n", nome);
		exit(0);
	}

	/* file reading for word verification */
	while (fscanf(ficheiro, "%s", im) == 1){
		point = strrchr(nome, '.');
		if (point != NULL)
		{
			if (strcmp (point, ".png") != 0){
				pict -> nn_files++;
				if(strlen(im) > max_name){
					max_name = strlen(im);
				}
			}	
		}
	}

	rewind(ficheiro);

	/* memory allocation and verification */
	pict -> files = (char**) malloc(pict -> nn_files*sizeof(char*));
	pict -> outputfiles = (char**) malloc(pict -> nn_files*sizeof(char*));
	if (pict -> files == NULL || pict -> outputfiles == NULL){
		fprintf(stderr, "Impossible to allocate directory\n");
		exit(0);
	}
	for (i = 0; i < pict -> nn_files; i++){
		pict -> files[i] = (char*) malloc ((max_name + strlen(directoria) + 2)*sizeof(char));
		pict -> outputfiles[i] = (char*) malloc((max_name + 1)*sizeof(char));
		if (pict -> files[i] == NULL || pict -> outputfiles[i] == NULL){
			fprintf(stderr, "Impossible to allocate directory\n");
			exit(0);
		}
	}
	/* variables initialization */
	for (i = 0; i < pict -> nn_files; i++){
		pict -> files[i][0] = '\0';
		pict -> outputfiles[i][0] = '\0';
	}
	
	i = 0;
	/* new reading of the file to save the images */
	while (fscanf(ficheiro, "%s", im) == 1){
		point = strrchr(nome, '.');
		if (point != NULL)
		{
			if (strcmp (point, ".png") != 0){
				strcpy (foto, directoria);
				strcat (foto, "/");
				strcat (foto, im);
				strcpy (pict -> files[i], foto);
				strcpy (pict -> outputfiles[i], im);
			}
		}
		i++;
	}
	fclose (ficheiro);

	/* creation of output directories */
	strcpy (dict1, directoria);
	strcpy (dict2, directoria);
	strcpy (dict3, directoria);

	strcat (dict2, RESIZE_DIR);
	strcat (dict3, THUMB_DIR);
	strcat (dict1, WATER_DIR);

	if (create_directory(dict2) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", RESIZE_DIR);
		exit(0);
	}
	if (create_directory(dict3) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", THUMB_DIR);
		exit(0);
	}
	if (create_directory(dict1) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", WATER_DIR);
		exit(0);
	}

	thread_id = (pthread_t*) malloc(3*sizeof(pthread_t));
	if (thread_id == NULL){
		fprintf(stderr, "Impossible to allocate directory\n");
		exit(0);
	}

	/* create the threads */
	pthread_create(&thread_id[0], NULL, watermark, (void*)pict);
	pthread_create(&thread_id[1], NULL, resize, (void*)pict);
	pthread_create(&thread_id[2], NULL, thumbnail, (void*)pict);

    i = 0;
	while( i < 3){
		pthread_join(thread_id[i], NULL);
		i++;
	}

	/*	Free Memory */
	for (i = 0; i < pict -> nn_files; i++){
		free (pict -> files[i]);
		free (pict -> outputfiles[i]);
	}
	free(pict -> files);
	free (pict -> outputfiles);
	free (pict);
	free (thread_id);

	exit(0);
}


