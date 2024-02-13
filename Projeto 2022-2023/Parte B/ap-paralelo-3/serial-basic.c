/******************************************************************************
 * Programacao Concorrente
 * 
 * Francisco Apolinário		1103681
 * Inês Mesquita			1103645 
 *
 * Projecto - ParteB
 *                           serial-basic.c
 * 
 * Compilacao: gcc -Wall -pedantic serial-basic.c image-lib.c -g -o ap-paralelo-3 -pthread -lgd
 *           
 *****************************************************************************/

#include <stdio.h>
#include <pthread.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <gd.h>
#include <string.h>
#include "image-lib.h"

int val[2], pipe_water[2];
char dict1[50], dict2[50], dict3[50];

/* the directories where output files will be placed */
#define RESIZE_DIR "/Resize-dir/"
#define THUMB_DIR "/Thumbnail-dir/"
#define WATER_DIR "/Watermark-dir/"

/* structure that is passed to thread */
typedef struct _Images
{
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
	int value;
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

	while(1){
		read (val[0], &value, sizeof(value));
		if (value == -1){
			write (pipe_water[1], &value, sizeof(value));
			break;
		}
		strcpy (verify, dict1);
		strcat (verify, pict_w -> outputfiles[value]);

		/* check if the file already exists */
		if (access (verify, F_OK) != 0){
			/* load of the input file */
			in_img = read_png_file(pict_w -> files[value]);
			if (in_img == NULL){
				fprintf(stderr, "Impossible to read %s image\n", pict_w -> outputfiles[value]);
				continue;
			}

			/* add watermark */
			out_watermark_img = add_watermark(in_img, watermark_img);
			if (out_watermark_img == NULL) {
				fprintf(stderr, "Impossible to creat thumbnail of %s image\n", pict_w -> outputfiles[value]);
			} else {
				/* save watermark */
				sprintf(out_file_name, "%s%s", dict1, pict_w -> outputfiles[value]);
				if(write_png_file(out_watermark_img, out_file_name) == 0) {
					fprintf(stderr, "Impossible to write %s image\n", out_file_name);
				}
				gdImageDestroy(out_watermark_img);
			}
			gdImageDestroy(in_img);
		}
		write (pipe_water[1], &value, sizeof(value));
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
	int value;
	char verify[100], aux[100];
	Images *pict_r = (Images*)pict;

	/* output images */
	gdImagePtr out_resized_img, out_watermark_img;

	char out_file_name[100];

	while(1){
		read (pipe_water[0], &value, sizeof(value));
		if (value == -1){
			break;
		}
		strcpy (verify, dict2);
		strcat (verify, pict_r -> outputfiles[value]);

		/* check if the file already exists */
		if (access (verify, F_OK) != 0){
			/* resizes of each image */
			strcpy (aux, dict1);
			strcat (aux, pict_r -> outputfiles[value]);
			/* reuse of previously created watermark */
			out_watermark_img = read_png_file(aux);
			if (out_watermark_img == NULL) {
				fprintf(stderr, "Impossible to add watermark to %s image\n", pict_r -> outputfiles[value]);
			} else {
				out_resized_img = resize_image(out_watermark_img, 800);
				if (out_resized_img == NULL) {
					fprintf(stderr, "Impossible to resize %s image\n", pict_r -> outputfiles[value]);
				} else {
					/* save resized */
					sprintf(out_file_name, "%s%s", dict2, pict_r -> outputfiles[value]);
					if(write_png_file(out_resized_img, out_file_name) == 0){
						fprintf(stderr, "Impossible to write %s image\n", out_file_name);
					}
					gdImageDestroy(out_resized_img);
				}
				gdImageDestroy(out_watermark_img);
			}
		}	
	}
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
	int value;
	char verify[100], aux[100];
	Images *pict_t = (Images*)pict;

	/* output images */
	gdImagePtr out_thumb_img, out_watermark_img;

	char out_file_name[100];

	while(1){
		read (pipe_water[0], &value, sizeof(value));
		if (value == -1){
			break;
		}	
		strcpy (verify, dict3);
		strcat (verify, pict_t -> outputfiles[value]);

		/* check if the file already exists */
		if (access (verify, F_OK) != 0){
			/* creation of thumbnail image */
			strcpy (aux, dict1);
			strcat (aux, pict_t -> outputfiles[value]);
			/* reuse of previously created watermark */
			out_watermark_img = read_png_file(aux);
			if (out_watermark_img == NULL) {
				fprintf(stderr, "Impossible to creat thumbnail of %s image\n", pict_t -> outputfiles[value]);
			} else {
				out_thumb_img = make_thumb(out_watermark_img, 150);
				if (out_thumb_img == NULL) {
					fprintf(stderr, "Impossible to creat thumbnail of %s image\n", pict_t -> outputfiles[value]);
				} else {
					/* save thumbnail image */
					sprintf(out_file_name, "%s%s", dict3, pict_t -> outputfiles[value]);
					if(write_png_file(out_thumb_img, out_file_name) == 0){
						fprintf(stderr, "Impossible to write %s image\n", out_file_name);
					}
					gdImageDestroy(out_thumb_img);
				}
				gdImageDestroy(out_watermark_img);
			}
		}	
	}
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

	pthread_t * thread_watermark_id, * thread_resize_id, * thread_thumbnail_id;
	Images *pict;
	FILE *ficheiro;
	char nome[50], im[30], foto[50];
	char *directoria = argv[1];
	int i = 0, max_name = 0;
	long int n_threads = 0;
	int total_files = 0, end = -1;

	if (pipe(val) != 0){
		printf ("ERROR CREATING THE THREAD!");
		exit (-1);
	}
	if (pipe(pipe_water) != 0){
		printf ("ERROR CREATING THE THREAD!");
		exit (-1);
	}

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
		total_files++;
		if(strlen(im) > max_name){
			max_name = strlen(im);
		}
	}

	if (n_threads > total_files){
		n_threads = total_files;
	}

	rewind(ficheiro);

	/* struct allocation */  
	pict = (Images*) malloc(sizeof(Images));
	if (pict == NULL){
		fprintf(stderr, "Impossible to allocate\n");
		exit(0);
	}    
	
	pict -> files = (char**) malloc(total_files*sizeof(char*));
	pict -> outputfiles = (char**) malloc(total_files*sizeof(char*));
	if (pict -> files == NULL || pict -> outputfiles == NULL){
			fprintf(stderr, "Impossible to allocate\n");
			exit(0);
	}

	/* Memory allocation and verification */
	for (i = 0; i < total_files; i++){
		pict -> files[i] = (char*) malloc((max_name + strlen(directoria) + 2)*sizeof(char));
		pict -> outputfiles[i] = (char*) malloc((max_name + 1)*sizeof(char));
		if (pict -> files[i] == NULL || pict -> outputfiles[i] == NULL){
			fprintf(stderr, "Impossible to allocate\n");
			exit(0);
		}
		/* variables initialization */
		pict -> files[i][0] = '\0';
		pict -> outputfiles[i][0] = '\0';
	}

	/* new reading of the file to save the images */
	i = 0;
	while (fscanf(ficheiro, "%s", im) == 1){
		strcpy (foto, directoria);
		strcat (foto, "/");
		strcat (foto, im);
		strcpy (pict -> files[i], foto);
		strcpy (pict -> outputfiles[i], im);
		i++;
	}

	fclose (ficheiro);

	/* creation of output directories */
	strcpy (dict1, directoria);
	strcpy (dict2, directoria);
	strcpy (dict3, directoria);

	strcat (dict1, WATER_DIR);
	strcat (dict2, RESIZE_DIR);
	strcat (dict3, THUMB_DIR);

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
    thread_watermark_id = (pthread_t*) malloc(n_threads*sizeof(pthread_t));
	if (thread_watermark_id == NULL){
		exit(0);
	}
	thread_resize_id = (pthread_t*) malloc(n_threads*sizeof(pthread_t));
	if (thread_resize_id == NULL){
		exit(0);
	}
	thread_thumbnail_id = (pthread_t*) malloc(n_threads*sizeof(pthread_t));
	if (thread_thumbnail_id == NULL){
		exit(0);
	}

	/*	Create threads and pass the struct	*/
	i = 0;
	while(i < n_threads){
        pthread_create(&thread_watermark_id[i], NULL, watermark, (void*)pict);
		pthread_create(&thread_resize_id[i], NULL, resize, (void*)pict);
		pthread_create(&thread_thumbnail_id[i], NULL, thumbnail, (void*)pict);
		i++;
	}

	for (i = 0; i < total_files; i++){
		write(val[1], &i, sizeof(i));
	}
	for (i = 0; i < n_threads; i++){
		write(val[1], &end, sizeof(end));
	}

    i = 0;
	while(i < n_threads){
		pthread_join(thread_watermark_id[i], NULL);
		i++;
	}
	i = 0;
	while(i < n_threads){
		pthread_join(thread_resize_id[i], NULL);
		i++;
	}
	i = 0;
	while(i < n_threads){
		pthread_join(thread_thumbnail_id[i], NULL);
		i++;
	}

	/* Free Memory */
	for (i = 0; i < total_files; i++){
		free (pict -> files[i]);
		free (pict -> outputfiles[i]);
	}
	free (pict -> files);
	free (pict -> outputfiles);
	free (pict);
	free (thread_watermark_id);
	free (thread_resize_id);
	free (thread_thumbnail_id);

	close(val[0]);
	close(val[1]);
	close(pipe_water[0]);
	close(pipe_water[1]);

	exit(0);
}