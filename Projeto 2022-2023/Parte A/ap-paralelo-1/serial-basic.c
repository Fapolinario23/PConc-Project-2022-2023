/******************************************************************************
 * Programacao Concorrente
 * 
 * Francisco Apolinário		1103681
 * Inês Mesquita			1103645 
 *
 * Projecto - Parte1
 *                           serial-basic.c
 * 
 * Compilacao: gcc -Wall -pedantic serial-basic.c image-lib.c -g -o ap-paralelo-1 -pthread -lgd
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

/* the directories where output files will be placed */
#define RESIZE_DIR "/Resize-dir/"
#define THUMB_DIR "/Thumbnail-dir/"
#define WATER_DIR "/Watermark-dir/"

/* structure that is passed to thread */
typedef struct _Images
{
	int nn_files;	/*	nº elements per thread	*/
	char **files;	/* files directory per thread */
	char **outputfiles;	/* files name per thread */

} Images;

/******************************************************************************
 * thread_function()
 *
 * Arguments: pict
 * Returns: NULL
 * Side-Effects: creates thumbnail, resized copy and watermarked copies
 *               of images
 *
 * Description: implementation of the function that creates thumbnail, resized 
 * copy and watermarked copies of images  
 * 
 *****************************************************************************/

void * thread_function (void* pict)
{
	int i;
	char verify[100], aux[100];
	Images *pict_f = (Images*)pict;

	/*	input images	*/
	gdImagePtr in_img,  watermark_img;
	/*	output images	*/
	gdImagePtr out_resized_img, out_thumb_img, out_watermark_img;

	/*	file name of the image created and to be saved on disk	*/
	char out_file_name[100];

	/* open and read watermark */
	watermark_img = read_png_file("watermark.png");
	if(watermark_img == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		exit(0);
	}

	for (i = 0; i < pict_f -> nn_files; i++){

		/*	load of the input file	*/
		in_img = read_png_file(pict_f -> files[i]);
		if (in_img == NULL){
			fprintf(stderr, "Impossible to read %s image\n", pict_f -> outputfiles[i]);
			exit (0);
		}

		strcpy (verify, dict1);
		strcat (verify, pict_f -> outputfiles[i]);

		/* check if the file already exists */
		if (access (verify, F_OK) == -1){

			/*	Add the watermarks	*/
			out_watermark_img = add_watermark (in_img, watermark_img);
			if (out_watermark_img == NULL) {
				fprintf(stderr, "Impossible to creat thumbnail of %s image\n", pict_f -> outputfiles[i]);
			} else {
				/*	save watermark	*/
				sprintf(out_file_name, "%s%s", dict1, pict_f -> outputfiles[i]);
				if(write_png_file(out_watermark_img, out_file_name) == 0){
					fprintf(stderr, "Impossible to write %s image\n", out_file_name);
				}
				gdImageDestroy(out_watermark_img);
			}
		}

		strcpy (verify, dict2);
		strcat (verify, pict_f -> outputfiles[i]);

		/* check if the file already exists */
		if (access (verify, F_OK) == -1){

			strcpy (aux, dict1);
			strcat (aux, pict_f -> outputfiles[i]);
			out_watermark_img = read_png_file(aux);
			
			/*	Resize images	*/
			out_resized_img = resize_image(out_watermark_img, 800);
			if (out_resized_img == NULL) {
				fprintf(stderr, "Impossible to resize %s image\n", pict_f -> outputfiles[i]);
			} else {
				/* save resized */
				sprintf(out_file_name, "%s%s", dict2, pict_f -> outputfiles[i]);
				if(write_png_file(out_resized_img, out_file_name) == 0){
					fprintf(stderr, "Impossible to write %s image\n", out_file_name);
				}
				gdImageDestroy(out_resized_img);
			}
			gdImageDestroy(out_watermark_img);
		}

		strcpy (verify, dict3);
		strcat (verify, pict_f -> outputfiles[i]);

		/* check if the file already exists */
		if (access (verify, F_OK) == -1){

			strcpy (aux, dict1);
			strcat (aux, pict_f -> outputfiles[i]);
			out_watermark_img = read_png_file(aux);

			/*	Create thumbnails	*/
			out_thumb_img = make_thumb(out_watermark_img, 150);
			if (out_thumb_img == NULL) {
				fprintf(stderr, "Impossible to creat thumbnail of %s image\n", pict_f -> outputfiles[i]);
			} else {
				/* save thumbnail image */
				sprintf(out_file_name, "%s%s", dict3, pict_f -> outputfiles[i]);
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
 * Returns: The normal exit of program is zero return value, when it has errors return is non-zero
 * Side-Effects: creates thumbnail, resized copy and watermarked copies
 *               of images
 *
 * Description: implementation of the complex serial version 
 *              This application only works for a fixed pre-defined set of files
 *
 *****************************************************************************/

int main(int argc, char **argv){

	if (argc != 3) {
        exit(0);
    }

	pthread_t * thread_id;
	Images **pict;
	FILE *ficheiro;
	char nome[50], im[30], foto[50];
	char * point, *directoria = argv[1];
	char **aux_files, **aux_out;
	int i = 0, j = 0, k = 0, h = 0,  max_name = 0;
	long int n_threads = 0;
	int elements_per_thread = 0, remaining_elements = 0, total_files = 0;

	/* converts a string to an int */
	n_threads = atoi(argv[2]);

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
				total_files++;
				if(strlen(im) > max_name){
					max_name = strlen(im);
				}
			}	
		}
	}

	rewind(ficheiro);

	/* memory allocation and verification */
	aux_files = (char**) malloc(total_files*sizeof(char*));
	aux_out = (char**) malloc(total_files*sizeof(char*));
	if (aux_files == NULL || aux_out == NULL){
		fprintf(stderr, "Impossible to allocate\n");
		exit(0);
	}
	for (i = 0; i < total_files; i++){
		aux_files[i] = (char*) malloc ((max_name + strlen(directoria) + 2)*sizeof(char));
		aux_out[i] = (char*) malloc((max_name + 1)*sizeof(char));
		if (aux_files[i] == NULL || aux_out[i] == NULL){
			fprintf(stderr, "Impossible to allocate\n");
			exit(0);
		}
		/* variables initialization */
		aux_files[i][0] = '\0';
		aux_out[i][0] = '\0';
	}

	/* struct verctor allocation */
	pict = (Images**) malloc(n_threads*sizeof(Images*));
	if (pict == NULL){
		fprintf(stderr, "Impossible to allocate\n");
		exit(0);
	}  
	/* structs allocation */  
	for (i = 0; i < n_threads; i++){
		pict[i] = (Images*) malloc(sizeof(Images));
		if (pict[i] == NULL){
			fprintf(stderr, "Impossible to allocate\n");
			exit(0);
		}    
	}

	/* calculation of the minimum number of photos per thread */
	elements_per_thread = total_files/n_threads;
	/* calculation of the number of left photos */
	remaining_elements = total_files%n_threads;
	
	/* stores the number of photos each thread has to handle */
	for(i = 0; i < n_threads; i++){
		pict[i] -> nn_files = elements_per_thread;
		if(remaining_elements != 0){
			pict[i]-> nn_files++;
			remaining_elements--;
		}
		

		/* Memory allocation and verification */
		pict[i] -> files = (char**) malloc(pict[i] -> nn_files*sizeof(char*));
		pict[i] -> outputfiles = (char**) malloc(pict[i] -> nn_files*sizeof(char*));
		if (pict[i] -> files == NULL || pict[i] -> outputfiles == NULL){
			fprintf(stderr, "Impossible to allocate\n");
			exit(0);
		}
		for (j = 0; j < pict[i] -> nn_files; j++){
			pict[i] -> files[j] = (char*) malloc ((max_name + strlen(directoria) + 2)*sizeof(char));
			pict[i] -> outputfiles[j] = (char*) malloc((max_name + 1)*sizeof(char));
			if (pict[i] -> files[j] == NULL || pict[i] -> outputfiles[j] == NULL){
				fprintf(stderr, "Impossible to allocate\n");
				exit(0);
			}
			/* variables initialization */
			pict[i] -> files[j][0] = '\0';
			pict[i] -> outputfiles[j][0] = '\0';
		}
	}

	/* new reading of the file to save the images */
	i = 0;
	while (fscanf(ficheiro, "%s", im) == 1){
		point = strrchr(nome, '.');
		if (point != NULL)
		{
			if (strcmp (point, ".png") != 0){
				strcpy (foto, directoria);
				strcat (foto, "/");
				strcat (foto, im);
				strcpy (aux_files[i], foto);
				strcpy (aux_out[i], im);
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

	/* threads allocation */
    thread_id = (pthread_t*) malloc(n_threads*sizeof(pthread_t));
	if (thread_id == NULL){
		exit(0);
	}

	/* distribution of images by structures */
	for (i = 0; i < n_threads; i++){
		h = pict[i] -> nn_files;
		for (j = k; j < total_files; j++){
			if (h <= 0){
				k = j;
				break;
			}
			strcpy (pict[i] -> files[(pict[i] -> nn_files) - h], aux_files[j]);
			strcpy (pict[i] -> outputfiles[(pict[i] -> nn_files) - h], aux_out[j]);
			h--;
		}
	}

	/*	Free Memory */
	for (i = 0; i < total_files; i++){
		free (aux_out[i]);
		free (aux_files[i]);
	}
	free (aux_out);
	free (aux_files);

	/*	Create threads and passes the respective struct	*/
	i = 0;
	while(i < n_threads){
        pthread_create(&thread_id[i], NULL, thread_function, (void*)pict[i]);
		i++;
	}

    i = 0;
	while(i < n_threads){
		pthread_join(thread_id[i], NULL);
		i++;
	}

	/* Free Memory */
	for (i = 0; i < n_threads; i++){
		for (j = 0; j < pict[i] -> nn_files; j++){
			free (pict[i] -> files[j]);
			free (pict[i] -> outputfiles[j]);
		}
		free (pict[i] -> files);
		free (pict[i] -> outputfiles);
		free (pict[i]);
	}
	free (pict);
	free (thread_id);

	exit(0);
}
