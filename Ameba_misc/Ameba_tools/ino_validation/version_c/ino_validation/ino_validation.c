﻿/*******************************************************************************************************************************************************************
*  README
*
*  COMPILATION
* Windows:          VS Build Solution
*                   MINGW64 git bash
*                       $gcc.exe -o ino_validation_windows.exe ino_validation.c cJSON.c ../../../../Ameba_Icon/ico-out.o
*                       $strip ino_validation_windows.exe
* Linux/macOS:      open terminal and execute "make clean;make" command
*
*******************************************************************************************************************************************************************/
#define _GNU_SOURCE

#ifdef _WIN32
#include <io.h>
#include "dirent.h"     // https://codeyarns.com/tech/2014-06-06-how-to-use-dirent-h-with-visual-studio.html#gsc.tab=0
#else // #elif __linux__
#include <inttypes.h>
#include <unistd.h>
#include <dirent.h>
#define __int64 int64_t
#define _close close
#define _read read
#define _lseek64 lseek64
#define _O_RDONLY O_RDONLY
#define _open open
#define _lseeki64 lseek64
#define _lseek lseek
#define _strdup strdup
#define stricmp strcasecmp
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "cJSON.h"
#include <locale.h>

#define PRINT_DEBUG         0
#define MAX_PATH_LENGTH     1024

// -------------------------------
//           Headers
// -------------------------------
// File operations
/* Declear function headers */
int dirExists(const char* directory_path);
/* Return folder names under current directory */
const char* dirName(const char* directory_path);
/* Collect all header files to be updte to txt file */
void convertToHeaderFiles(const char* input);
/* List down the directory that current example locating at */
void listExampleDir(char* basePath, char* examplePath);
/* Find the dir name of input source file path*/
void extractRootDirectory(char* source, char* result);

// String operation
/* Remove char c from string str */
void removeChar(char* str, char c);
/* Similar function as REGEX*/
void extractParam(char* line, char* param);
/* Extract header from quote symbols*/
void extractString(char* source, char* result);
/* Extract header from quote symbols*/
void extractString2(char* source, char* result);
/* Convert model input type to model name*/
const char* input2model(const char* input);
/* convert \\ in file path to \ */
const char* pathTidy(const char* input);
/* Returns temperorily created example file path in Temp folder */
const char* pathTempINO(const char* directory_path, const char* ext, const char* key);

// Others
/* Handler to print error message in the Arduino IDE */
void error_handler(const char* message);
/* Validate example in directory_path and returns example path */
const char* validateINO(const char* directory_path);

// JSON operations
/* Clear nn model json file*/
void resetJSON(const char* input);
/* Load JSON file from the directory and parse into cJSON data format */
cJSON* loadJSONFile(const char* file_path);
/* Returns example fpath inside the temp JSON file */
const char* pathTempJSON(const char* directory_path, const char* ext, const char* key);

// TXT operations
/* Clear all content in the ino_validation.txt file */
void resetTXT(char* directory_path);
/* Update content in the input to TXT file */
void updateTXT(const char* input);
/* Function checks whether the directory exisits */
int writeTXT(const char* path_example);

/* Declear global vairables */
const char* key_amb_NN = "modelSelect";
const char* key_amb_bypassNN1 = " .modelSelect";
const char* key_amb_bypassNN2 = " modelSelect";
const char* key_amb_VOE = "configVideoChannel";
const char* key_amb_bypassVOE1 = " .configVideoChannel";
const char* key_amb_bypassVOE2 = " configVideoChannel";
const char* key_amb_header = "#include";
const char* key_amb_customized = "CUSTOMIZED";
const char* key_json = "build";
const char* key_amb = "Arduino15";
const char* key_ino = ".ino";
const char* ext_json = ".json";
const char* ext_cpp = ".cpp";
const char* filename_txt = "ino_validation.txt";

/* Declear common file paths */
#ifdef _WIN32
char* path_arduino15_add = "\\AppData\\Local\\Arduino15\\";
char* path_ambpro2_add = "\\packages\\realtek\\hardware\\AmebaPro2\\";
char* path_model_add = "\\variants\\common_nn_models\\";
char* path_library_add = "\\libraries\\";
char* path_txtfile_add = "\\misc\\";
char* backspace = "\\";
#elif __linux__
char* path_arduino15_add = "/.arduino15/";
char* path_ambpro2_add = "/packages/realtek/hardware/AmebaPro2/";
char* path_model_add = "/variants/common_nn_models/";
char* path_library_add = "/libraries/";
char* path_txtfile_add = "/misc/";
char* backspace = "/";
#else
char* path_arduino15_add = "/Library/Arduino15/";
char* path_ambpro2_add = "/packages/realtek/hardware/AmebaPro2/";
char* path_model_add = "/variants/common_nn_models/";
char* path_library_add = "/libraries/";
char* path_txtfile_add = "/misc/";
char* backspace = "/";
#endif

const char* path_build_options_json = NULL;
const char* path_example_temp = NULL;
const char* path_example = NULL;
const char* name_example = NULL;

char path_root[MAX_PATH_LENGTH];
char path_arduino15[MAX_PATH_LENGTH];
char path_pro2[MAX_PATH_LENGTH];
char path_model[MAX_PATH_LENGTH];
char path_library[MAX_PATH_LENGTH];
char path_txtfile[MAX_PATH_LENGTH];
char folder_example[MAX_PATH_LENGTH];

// -------------------------------
//              Main
// -------------------------------
int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "en_US.UTF-8");
	// Check if the number of input arguments is correct 
	if (argc != 3) {
		if (PRINT_DEBUG) printf("[Error] [%d] Incorrect number of input parameters. Expected 2 parameters.\r\n", __LINE__);
		exit(1);
	}

	// Retrieve the input parameters 
	const char* path_build = argv[1];
	const char* path_tools = argv[2];

	// Retrive root path
#ifdef _WIN32
	strcpy(path_root, getenv("USERPROFILE"));
	strcpy(path_arduino15, getenv("USERPROFILE"));
#else
	strcpy(path_root, getenv("HOME"));
	strcpy(path_arduino15, getenv("HOME"));
#endif

	strcat(path_arduino15, path_arduino15_add);
	strcpy(path_pro2, path_arduino15);
	strcat(path_pro2, path_ambpro2_add);
	strcpy(path_model, path_pro2);
	strcat(path_model, dirName(path_pro2));
	strcpy(path_library, path_model);
	strcat(path_model, path_model_add);
	strcat(path_library, path_library_add);
	strcpy(path_txtfile, argv[2]);
	strcat(path_txtfile, path_txtfile_add);

#if PRINT_DEBUG
	// Print the input parameters 
	printf("Parameter 1      = %s\n", path_build);
	printf("Parameter 2      = %s\n", path_tools);
	//printf("USERPROFILE      = %s\n", getenv("USERPROFILE"));
	//printf("HOMEDRIVE        = %s\n", getenv("HOMEDRIVE"));
	//printf("HOMEPATH         = %s\n", getenv("HOMEPATH"));
	printf("path_root        = %s\n", path_root);
	printf("path_arduino15   = %s\n", path_arduino15);
	printf("path_pro2        = %s\n", path_pro2);
	printf("ver_pro2         = %s\n", dirName(path_pro2));
	printf("path_model       = %s\n", path_model);
	printf("path_library     = %s\n", path_library);
	printf("path_txtfile     = %s\n", path_txtfile);
#endif
	resetTXT(path_txtfile);
	resetJSON(path_model);

	path_build_options_json = pathTempJSON(path_build, ext_json, key_json);
	path_example = validateINO(path_build);
	writeTXT(path_example);
	if (PRINT_DEBUG) printf("[%s]----END----\n", __func__);

	return 0;
}

// -------------------------------
//           Functions
// -------------------------------
#ifndef __MINGW64__
extern int mkdir(char* filename);
#endif

int dirExists(const char* directory_path) {
	DIR* directory = opendir(directory_path);
	// check weather dir is valid
	if (directory) {
		closedir(directory);
		return 1;
	}
	else {
		printf("[%s][%d][Error] Failed to open directory.\n", __func__, __LINE__);
		return 0;
	}
}


const char* dirName(const char* directory_path) {
	int sdk_counter = 0;
	struct dirent* entry;
	DIR* directory = opendir(directory_path);
	const char* sdk_name = "";
	// check dir validation
	if (directory) {
		while ((entry = readdir(directory)) != NULL) {
#ifdef __APPLE__
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".DS_Store") == 0) {
#else
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
#endif
				continue;
			}
			else {
				sdk_counter++;
				sdk_name = entry->d_name;
			}
		}
		// non singular SDK validation
		if (sdk_counter > 1) {
			goto error_non_singular;
		}
		else {
			return sdk_name;
		}
	}
	else {
		printf("[%s][%d][Error]Failed to open directory.\n", __func__, __LINE__);
	}
	closedir(directory);

error_non_singular:
	error_handler("AmebaPro2 directory only allow 1 SDK!!! Please check again.");
}


void convertToHeaderFiles(const char* input) {
	const char* delimiter = ".h";

	if (input != "") {
#ifndef _WIN32
		char* inputCopy = input;
#else
		char* inputCopy = (char*)input;
#endif
		char* token = strtok(inputCopy, delimiter);
#if PRINT_DEBUG
		printf("[%s][%d][Info] token: %s\n", __func__, __LINE__, token);
#endif
		while (token != NULL) {
#if PRINT_DEBUG
			printf("[%s][Info]%s.h\n", __func__, token);
#endif
			updateTXT(token);
			token = strtok(NULL, delimiter);
		}
	}
}


void listExampleDir(char* basePath, char* exampleName)
{
	char path[1000];
	struct dirent* dp;
	DIR* dir = opendir(basePath);

	// Unable to open directory stream
	if (!dir)
		return;

	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			if (strstr(path, exampleName) != NULL && (!strcmp(&path[strlen(path) - strlen(exampleName)], exampleName))) {
				//printf("%s\n", path);  
				strcpy(folder_example, path);
			}
			// Construct new path from base path
			strcpy(path, basePath);
			strcat(path, backspace);
			strcat(path, dp->d_name);

			listExampleDir(path, exampleName);
		}
	}
	closedir(dir);
}


void extractRootDirectory(char* filepath, char* rootDir) {
#ifdef _WIN32
	char* lastSeparator = strrchr(filepath, '\\'); // find last occurance of backspace
#else
	char* lastSeparator = strrchr(filepath, '/');	// find last occurance of backspace
#endif

	if (lastSeparator == NULL) {
		strcpy(rootDir, ""); // set as empty string if not found
		return;
	}
	__int64 length = lastSeparator - filepath + 1;
	strncpy(rootDir, filepath, length);
	rootDir[length] = '\0'; // add ending param at EOL
}


void removeChar(char* str, char c) {
	int i, j;
	size_t len = strlen(str);
	for (i = j = 0; i < len; i++) {
		if (str[i] != c) {
			str[j++] = str[i];
		}
	}
	str[j] = '\0';
}


void extractParam(char* line, char* param) {
	char* start = strchr(line, '(');
	char* end = strchr(line, ')');
	if (start != NULL && end != NULL && end > start) {
		size_t length = end - start - 1;
		strncpy(param, start + 1, length);
		param[length] = '\0';
	}
}


void extractString(char* source, char* result) {
	char* start = strchr(source, '\"'); // find the 1st "
	if (start == NULL) {
		strcpy(result, ""); // set as empty string if not found
		return;
	}

	start++; // skip the first "

	char* end = strchr(start, '\"'); // find the 2nd "
	if (end == NULL) {
		strcpy(result, ""); // set as empty string if not found
		return;
	}

	__int64 length = end - start;
	strncpy(result, start, length);
	result[length] = '\0'; // add ending param at EOL
}


void extractString2(char* source, char* result) {
	char* start = strchr(source, '<'); // find the 1st <
	if (start == NULL) {
		strcpy(result, ""); // set as empty string if not found
		return;
	}

	start++; // skip the first "

	char* end = strchr(start, '>'); // find the 1st >
	if (end == NULL) {
		strcpy(result, "");// set as empty string if not found
		return;
	}

	__int64 length = end - start;
	strncpy(result, start, length);
	result[length] = '\0'; // add ending param at EOL
}


const char* input2model(const char* input) {
	const char* model_mapping[][2] = {
		{"CUSTOMIZED_YOLOV3TINY",    "yolov3_tiny"},
		{"CUSTOMIZED_YOLOV4TINY",    "yolov4_tiny"},
		{"CUSTOMIZED_YOLOV7TINY",    "yolov7_tiny"},
		{"CUSTOMIZED_MOBILEFACENET", "mobilefacenet_i16"},
		{"CUSTOMIZED_SCRFD",         "scrfd640"},
		{"CUSTOMIZED_YAMNET",        "yamnet_fp16"},
		{"DEFAULT_YOLOV3TINY",       "yolov3_tiny"},
		{"DEFAULT_YOLOV4TINY",       "yolov4_tiny"},
		{"DEFAULT_YOLOV7TINY",       "yolov7_tiny"},
		{"DEFAULT_MOBILEFACENET",    "mobilefacenet_i8"},
		{"DEFAULT_SCRFD",            "scrfd320p"},
		{"DEFAULT_YAMNET",			 "yamnet_fp16"},
	};

	int mapping_size = sizeof(model_mapping) / sizeof(model_mapping[0]);

	for (int i = 0; i < mapping_size; i++) {
		if (strcmp(input, model_mapping[i][0]) == 0) {
			return model_mapping[i][1];
		}
	}

	return "NA";
}


const char* pathTidy(const char* input) {
	size_t input_length = strlen(input);
	size_t output_length = 0;
	for (size_t i = 0; i < input_length; i++) {
		if (input[i] == '\\') {
			output_length += 2;
		}
		output_length++; // for the character itself
	}
	char* output = (char*)malloc(output_length + 1); // +1 for null-terminator
	if (!output) {
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	size_t j = 0;
	for (size_t i = 0; i < input_length; i++) {
		if (input[i] == '\\') {
			output[j++] = '\\';
			output[j++] = '\\';
		}
		else {
			output[j++] = input[i];
		}
	}

	output[j] = '\0'; // Null-terminator

	return output;

}


const char* pathTempINO(const char* directory_path, const char* ext, const char* key) {
	DIR* dir;
	struct dirent* entry;

	// Redirect to Temp/sketch folder to read modified ino
	strcat(directory_path, backspace);
	strcat(directory_path, "sketch");
	strcat(directory_path, backspace);
#if PRINT_DEBUG
	printf("[%s][%d][INFO] Load ino in \"%s\"\n", __func__, __LINE__, directory_path);
#endif

	if ((dir = opendir(directory_path)) == NULL) {
#if PRINT_DEBUG
		perror("opendir() error");
		printf("[%s][%d][Error] Failed to open dir at: %s\n", __func__, __LINE__, directory_path);
		exit(1);
#endif
	}
	else {
		while ((entry = readdir(dir)) != NULL) {
			// filter the file with .cpp as extension
			char* dot = strrchr(entry->d_name, '.');
			if (dot && !strcmp(dot, ext_cpp)) {
				printf("[%s][%d][INFO] Ino: %s\n", __func__, __LINE__, entry->d_name);
				strcat(directory_path, entry->d_name);
				break;
			}
		}
		closedir(dir);
	}

	return directory_path;
}


void error_handler(const char* message) {
	fprintf(stderr, "[Error] %s\n", message);
	exit(1);
}


const char* validateINO(const char* directory_path) {
	// Open the JSON file and retrive the data
	cJSON* data = loadJSONFile(path_build_options_json);
	cJSON* path_example = cJSON_GetObjectItem(data, "sketchLocation");
#if PRINT_DEBUG
	printf("[%s][INFO] Current example path2: %s \n", __func__, pathTidy(path_example->valuestring));
#endif

	return pathTidy(path_example->valuestring);
}


void resetJSON(const char* input) {
	DIR* dir;
	struct dirent* entry;

	dir = opendir(input);
	if (dir == NULL) {
		perror("Error opening directory");
		return;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strstr(entry->d_name, ".json") != NULL) {
			char filepath[256];
			snprintf(filepath, sizeof(filepath), "%s/%s", input, entry->d_name);

			FILE* file = fopen(filepath, "r");
			if (file == NULL) {
				perror("Error opening file");
				continue;
			}

			fseek(file, 0, SEEK_END);
			long file_size = ftell(file);
			fseek(file, 0, SEEK_SET);

			char* file_contents = (char*)malloc(file_size + 1);
			fread(file_contents, file_size, 1, file);
			fclose(file);

			cJSON* root = cJSON_Parse(file_contents);
			if (root == NULL) {
				fprintf(stderr, "Error parsing JSON in file: %s\n", entry->d_name);
				free(file_contents);
				continue;
			}

			cJSON* fwfs = cJSON_GetObjectItemCaseSensitive(root, "FWFS");
			if (fwfs == NULL || !cJSON_IsObject(fwfs)) {
				cJSON_Delete(root);
				free(file_contents);
				fprintf(stderr, "Invalid JSON format in file: %s\n", entry->d_name);
				continue;
			}

			cJSON* files = cJSON_GetObjectItemCaseSensitive(fwfs, "files");
			if (files == NULL || !cJSON_IsArray(files)) {
				cJSON_Delete(root);
				free(file_contents);
				fprintf(stderr, "Invalid JSON format in file: %s\n", entry->d_name);
				continue;
			}

			cJSON_DeleteItemFromObject(fwfs, "files");

			cJSON* new_files = cJSON_CreateArray();
			cJSON_AddItemToObject(fwfs, "files", new_files);

			char* new_file_contents = cJSON_Print(root);
			cJSON_Delete(root);
			free(file_contents);

			FILE* new_file = fopen(filepath, "w");
			if (new_file == NULL) {
				perror("Error opening file for writing");
				free(new_file_contents);
				continue;
			}

			fwrite(new_file_contents, strlen(new_file_contents), 1, new_file);
			fclose(new_file);
			free(new_file_contents);
		}
	}

	closedir(dir);
}


cJSON* loadJSONFile(const char* file_path) {
	FILE* file;
	cJSON* data = NULL;

	if ((file = fopen(file_path, "r")) == NULL) {
		perror("Error opening file");
		printf("[%s][%d][Error] Failed to open the file.\n", __func__, __LINE__);
		exit(1);
		return data;
	}

	// Get the file size
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	// Allocate memory to hold the JSON data
	char* json_data = (char*)malloc(file_size + 1);
	if (json_data == NULL) {
		printf("[%s][%d][Error] Failed to allocate memory.\n", __func__, __LINE__);
		fclose(file);
		return data;
	}

	// Read the JSON data from the file
	size_t read_size = fread(json_data, 1, file_size, file);
	if (read_size != file_size) {
		printf("[%s][%d][Error] Failed to read the file.\n", __func__, __LINE__);
		fclose(file);
		free(json_data);
		return data;
	}
	json_data[file_size] = '\0';  // Null-terminate the string

	// Close the file
	fclose(file);

	// Parse the JSON data
	data = cJSON_Parse(json_data);

	return data;
}


const char* pathTempJSON(const char* directory_path, const char* ext, const char* key) {
	DIR* dir;
	struct dirent* entry;

	if ((dir = opendir(directory_path)) == NULL) {
#if PRINT_DEBUG
		perror("opendir() error");
		printf("[%s][%d][Error] Failed to open dir at: %s\n", __func__, __LINE__, directory_path);
		exit(1);
#endif
	}
	else {
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_type == DT_REG && strstr(entry->d_name, ext_json) != NULL && strstr(entry->d_name, key_json) != NULL) {
#if PRINT_DEBUG
				printf("[%s][%d][INFO] File: %s\n", __func__, __LINE__, entry->d_name);
#endif

#ifndef _WIN32
				strcat(directory_path, backspace);
				strcat(directory_path, entry->d_name);
#else
				strcat((char*)directory_path, backspace);
				strcat((char*)directory_path, entry->d_name);
#endif
				break;
			}
		}
		closedir(dir);
	}

	return directory_path;
}


void resetTXT(char* directory_path) {
#ifndef _WIN32
	DIR* dir;
	dir = opendir(directory_path);

	struct stat st;

	// create directory if not exists
	if (stat(dir, &st) == -1) {
		mkdir(dir, 0700);
	}
	strcat(directory_path, filename_txt);
#else
	struct stat st;

	// create directory if not exists
	if (stat(directory_path, &st) == -1) {
		mkdir(directory_path);
	}
	strcat((char*)directory_path, filename_txt);
#endif

	// open txt file and clear everything
	FILE* file = fopen(directory_path, "w");
	if (file != NULL) {
	}
	if (PRINT_DEBUG) printf("[%s][%d][INFO] resetTXT done\n", __func__, __LINE__);
}


void updateTXT(const char* input) {
	FILE* file = fopen(path_txtfile, "a");

	if (file) {
		fprintf(file, "%s\n", input);
		fclose(file);
	}
	else {
#if PRINT_DEBUG
		printf("[%s][%d][Error] Failed to open the file.\n", __func__, __LINE__);
#endif
		perror(path_txtfile);
	}
}


int writeTXT(const char* path) {
	const char buf[MAX_PATH_LENGTH] = "";
	const char backslash[] = "\\";
	char line[MAX_PATH_LENGTH] = { 0 };
	char voe_status[MAX_PATH_LENGTH] = "NA";
	char model_type[MAX_PATH_LENGTH] = "";
	char model_name_od[MAX_PATH_LENGTH] = "";
	char model_name_fd[MAX_PATH_LENGTH] = "";
	char model_name_fr[MAX_PATH_LENGTH] = "";
	char model_name_ac[MAX_PATH_LENGTH] = "";
	char header_od[MAX_PATH_LENGTH] = "NA";
	char header_fd[MAX_PATH_LENGTH] = "NA";
	char header_fr[MAX_PATH_LENGTH] = "NA";
	char header_ac[MAX_PATH_LENGTH] = "NA";
	char header_all[MAX_PATH_LENGTH] = "";
	char line_strip_header[MAX_PATH_LENGTH] = "NA";
	char line_strip_headerNN[MAX_PATH_LENGTH] = "NA";
	char dir_example[MAX_PATH_LENGTH] = "NA";

	char* file_path = NULL;
	const char* ino_extension = ".ino";

	if (PRINT_DEBUG) printf("[%s][%d][INFO] Load example: \"%s\"\n", __func__, __LINE__, path);

	// check IDE Version
	if (strstr(path, ".ino") == NULL) {
		// IDE2 creates temp dir
		if (PRINT_DEBUG) printf("[%d] IDE2\n", __LINE__);

		DIR* dir;
		struct dirent* entry;
		dir = opendir(path);

		if (dir == NULL) {
			printf("Unable to open directory: %s\n", path);
			return 0;
		}

		// find .ino file in IDE2 TEMP folder
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_type == DT_REG) {
				if (strstr(entry->d_name, ".ino") != NULL) {
					size_t path_len = strlen(path);
					size_t file_name_len = strlen(entry->d_name);

					file_path = malloc(path_len + file_name_len + 2);
					if (file_path == NULL) {
						printf("Memory allocation error.\n");
					}
					strcpy(file_path, path);
					strcat(file_path, pathTidy(backspace));
					strcat(file_path, entry->d_name);
					break;
				}
			}
		}
		closedir(dir);
	}
	else {
		// IDE1 creates temp file
		if (PRINT_DEBUG) printf("[%d] IDE1\n", __LINE__);
		// unmodified example
		size_t path_len = strlen(path);
		file_path = malloc(path_len + 2);
		file_path = (char*)path;
		if (PRINT_DEBUG) printf("[%d] file_path %s\n", __LINE__, file_path);

		if (file_path == NULL) {
			printf("Memory allocation error.\n");
		}

		// modified example
		if (strstr(path_example, "modified") != NULL) {
			char* example_name = strrchr(path_example, '\\');   // find the last "\"
			removeChar(example_name, '\\');
			listExampleDir(path_library, example_name);
			strcpy(file_path, folder_example);					// update example directory name
			if (PRINT_DEBUG) printf("[%d] file_path %s\n", __LINE__, file_path);
		}

	}

	path = (const char*)file_path;
	FILE* f_model = fopen(path, "r");
	char param[100];

	if (f_model) {
		char line[MAX_PATH_LENGTH];
		char* token = "";

		while (fgets(line, sizeof(line), f_model)) {
			// check whether keywordNN in file
			if (strstr(line, key_amb_NN) != NULL && strstr(line, "//") == NULL && strstr(line, key_amb_bypassNN1) == NULL && strstr(line, key_amb_bypassNN2) == NULL) {
				extractParam(line, param);
				if (PRINT_DEBUG) printf("[%d] Extracted parameter: %s\n", __LINE__, param);
				token = strtok(param, ", ");

				if (token != NULL) {
					strcpy(model_type, token);
					if (PRINT_DEBUG) printf("[%d] Model Type: %s\n", __LINE__, model_type);
				}
				else {
					goto error_syntax;
				}

				/* ------------------ object detection ------------------*/
				token = strtok(NULL, ", ");
				if (PRINT_DEBUG) printf("[%d]  Param 1: %s\n", __LINE__, token);
				if (token != NULL) {
					strcpy(model_name_od, token);
					// OBJECT_DETECTION example: check model combination rules
					if (strcmp(model_type, "OBJECT_DETECTION") == 0) {
						if (strcmp(token, "NA_MODEL") == 0 || strstr(token, "YOLO") == NULL) {
							goto error_combination;
						}

						// check customized od model
						if (strstr(token, key_amb_customized) != NULL) {
							if (PRINT_DEBUG) printf("[%d] od key_amb_customized\n", __LINE__);
							if (PRINT_DEBUG) printf("[%d] customized od: %s\n", __LINE__, input2model(token));
							if (PRINT_DEBUG) printf("[%d] path example %s\r\n", __LINE__, path_example);

							if (strcmp(path_example, "Temp") == 0) {
								// IDE1
								if (PRINT_DEBUG) printf("[%d] IDE1\r\n", __LINE__);
#ifndef _WIN32
								extractRootDirectory(path_example, dir_example);
#else
								extractRootDirectory((char*)path_example, dir_example);
#endif
							}
							else {
								// IDE2
								if (PRINT_DEBUG) printf("[%d] IDE2\r\n", __LINE__);
								char* example_name = strrchr(path_example, '\\');   // find the last "\"
								removeChar(example_name, '\\');
								listExampleDir(path_library, example_name);
								strcpy(dir_example, folder_example);                // update example directory name
							}

							DIR* dir;
							struct dirent* entry;
							int count = 0;
							int count_match = 0;

							// check weather dir is valid
							if ((dir = opendir(dir_example)) != NULL) {
								// print all the files and directories within directory 
								while ((entry = readdir(dir)) != NULL) {
									if (entry->d_type == DT_REG) {
										count++;
									}
									if (strstr(entry->d_name, ".nb") != NULL) {
										if (strstr(entry->d_name, input2model(token))) {
#if PRINT_DEBUG
											printf("[%d] %s\n", __LINE__, entry->d_name);
#endif
											count_match++;
										}
									}
								}
							}
							if (count <= 1) {
								goto error_customized_missing;
							}
							if (count_match == 0) {
								goto error_customized_mismatch;
							}
						}
					}
					strcpy(model_name_od, token);

					/* ----------------- face detection -----------------*/
					token = strtok(NULL, ", ");
					if (PRINT_DEBUG) printf("[%d]  Param 2: %s\n", __LINE__, token);
					if (token != NULL) {
						strcpy(model_name_fd, token);
						// FACE_DETECTION example: check model combination rules
						if (strcmp(model_type, "FACE_DETECTION") == 0) {
							if (strcmp(token, "NA_MODEL") == 0 || strstr(token, "SCRFD") == NULL) {
								goto error_combination;
							}
						}
						// check customized FD model
						if (strstr(token, key_amb_customized) != NULL) {
							if (PRINT_DEBUG) printf("[%d]fd key_amb_customized\n", __LINE__);
							if (PRINT_DEBUG) printf("[%d]customized fd: %s\n", __LINE__, input2model(token));
							if (PRINT_DEBUG) printf("[%d]path_example: %s\n", __LINE__, path_example);

							if (strcmp(path_example, "Temp") == 0) {
								// IDE1
								if (PRINT_DEBUG) printf("[%d] IDE1\r\n", __LINE__);
#ifndef _WIN32
								extractRootDirectory(path_example, dir_example);
#else
								extractRootDirectory((char*)path_example, dir_example);
#endif
							}
							else {
								// IDE2
								if (PRINT_DEBUG) printf("[%d] IDE2\r\n", __LINE__);
								char* example_name = strrchr(path_example, '\\');   // find the last "\"
								removeChar(example_name, '\\');
								listExampleDir(path_library, example_name);
								strcpy(dir_example, folder_example);                // update example directory name
							}


							DIR* dir;
							struct dirent* entry;
							int count = 0;
							int count_match = 0;

							// check weather dir is valid
							if ((dir = opendir(dir_example)) != NULL) {
								/* print all the files and directories within directory */
								while ((entry = readdir(dir)) != NULL) {
									if (entry->d_type == DT_REG) {
										count++;
									}
									if (strstr(entry->d_name, ".nb") != NULL) {
										if (strstr(entry->d_name, "scrfd")) {
											if (PRINT_DEBUG) printf("[%d] %s\n", __LINE__, entry->d_name);
											count_match++;
										}
									}
								}
							}
							if (count <= 1) {
								goto error_customized_missing;
							}
							if (count_match == 0) {
								goto error_customized_mismatch;
							}
						}

						strcpy(model_name_fd, token);

						/*-------------- face recognition --------------*/
						token = strtok(NULL, ", ");
						if (PRINT_DEBUG) printf("[%d]  Param 3: %s\n", __LINE__, token);
						if (token != NULL) {
							strcpy(model_name_fr, token);
							// FACE_RECOGNITION example: check model combination rules
							if (strcmp(model_type, "FACE_RECOGNITION") == 0) {
								if (strcmp(model_name_fd, "NA_MODEL") == 0 || strstr(model_name_fd, "SCRFD") == NULL || strcmp(token, "NA_MODEL") == 0 || strstr(token, "MOBILEFACENET") == NULL) {
									goto error_combination;
								}
							}

							// check customized FR model
							if (strstr(token, key_amb_customized) != NULL) {
								if (PRINT_DEBUG) printf("[%d]fr key_amb_customized\n", __LINE__);
								if (PRINT_DEBUG) printf("[%d]customized fr: %s\n", __LINE__, input2model(token));
								if (PRINT_DEBUG) printf("[%d]path_example: %s\n", __LINE__, path_example);

								if (strcmp(path_example, "Temp") == 0) {
									// IDE1
									if (PRINT_DEBUG) printf("[%d] IDE1\r\n", __LINE__);
#ifndef _WIN32
									extractRootDirectory(path_example, dir_example);
#else
									extractRootDirectory((char*)path_example, dir_example);
#endif
								}
								else {
									// IDE1
									if (PRINT_DEBUG) printf("[%d] IDE2\r\n", __LINE__);
									char* example_name = strrchr(path_example, '\\');   // find the last "\"
									removeChar(example_name, '\\');
									listExampleDir(path_library, example_name);
									strcpy(dir_example, folder_example);                // update example directory name
								}

								DIR* dir;
								struct dirent* entry;
								int count = 0;
								int count_match = 0;

								// check weather dir is valid
								if ((dir = opendir(dir_example)) != NULL) {
									/* print all the files and directories within directory */
									while ((entry = readdir(dir)) != NULL) {
										if (entry->d_type == DT_REG) {
											count++;
										}
										if (strstr(entry->d_name, ".nb") != NULL) {
											if (strstr(entry->d_name, "mobilefacenet")) {
												if (PRINT_DEBUG) printf("[%d] %s\n", __LINE__, entry->d_name);
												count_match++;
											}
										}
									}
								}
								if (count <= 1) {
									goto error_customized_missing;
								}
								if (count_match == 0) {
									goto error_customized_mismatch; // error_customized_exceed;
								}
							}
							if (token != NULL) {
								strcpy(model_name_fr, token);

								/*-------------- audio classification --------------*/
								token = strtok(NULL, ", ");
								if (PRINT_DEBUG) printf("[%d] Param 4: %s\n", __LINE__, token);
								if (token != NULL) {
									strcpy(model_name_ac, token);
									// AUDIO_CLASSIFICATION(AC) example: check model combination rules
									if (strcmp(model_type, "AUDIO_CLASSIFICATION") == 0) {
										if (strcmp(model_name_ac, "NA_MODEL") == 0) { // if selected AUDIO_CLASSIFICATION without giving corresponding model
											goto error_combination;
										}
										else {
											if ((strcmp(model_name_od, "NA_MODEL") || strcmp(model_name_fd, "NA_MODEL") || strcmp(model_name_fr, "NA_MODEL")) != 0) {
												goto error_exceed;
											}
										}
									}
									// non AUDIO_CLASSIFICATION(AC) example: model cannot exists with audio model
									else {
										if (strcmp(model_name_ac, "NA_MODEL") != 0) { // if non AUDIO_CLASSIFICATION(AC) example also contains audio model
											goto error_exceed;
										}
									}
									// check customized AC model
									if (strstr(token, key_amb_customized) != NULL) {
										if (PRINT_DEBUG) printf("[%d] ac key_amb_customized\n", __LINE__);
										if (PRINT_DEBUG) printf("[%d] customized ac: %s\n", __LINE__, input2model(token));
										if (PRINT_DEBUG) printf("[%d] path example %s\r\n", __LINE__, path_example);

										if (strcmp(path_example, "Temp") == 0) {
											// IDE1
											if (PRINT_DEBUG) printf("[%d] IDE1\r\n", __LINE__);
#ifndef _WIN32
											extractRootDirectory(path_example, dir_example);
#else
											extractRootDirectory((char*)path_example, dir_example);
#endif
										}
										else {
											// IDE2
											if (PRINT_DEBUG) printf("[%d] IDE2\r\n", __LINE__);
											char* example_name = strrchr(path_example, '\\');   // find the last "\"
											removeChar(example_name, '\\');
											listExampleDir(path_library, example_name);
											strcpy(dir_example, folder_example);                // update example directory name
										}

										DIR* dir;
										struct dirent* entry;
										int count = 0;
										int count_match = 0;

										// check weather dir is valid
										if ((dir = opendir(dir_example)) != NULL) {
											// print all the files and directories within directory 
											while ((entry = readdir(dir)) != NULL) {
												if (entry->d_type == DT_REG) {
													count++;
												}
												if (strstr(entry->d_name, ".nb") != NULL) {
													if (strstr(entry->d_name, input2model(token))) {
														count_match++;
													}
												}
											}
										}
										if (count <= 1) {
											goto error_customized_missing;
										}
										if (count_match == 0) {
											goto error_customized_mismatch;
										}
									}
								}
								else {
									goto error_combination; // missing parameter for Audio Classification
								}
								strcpy(model_name_ac, token);
							}
						}
					}
				}

				else {
					// provide default settings for all models if user never provide any selections
					if (strcmp(model_type, "OBJECT_DETECTION") == 0) {
						//if (strcmp(model_type, "OBJECT_DETECTION") == 0 && strcmp(input2model(model_name_od), "NA") == 0) {
						strcpy(model_name_od, "DEFAULT_YOLOV4TINY");
						strcpy(model_name_fd, "NA_MODEL");
						strcpy(model_name_fr, "NA_MODEL");
						strcpy(model_name_ac, "NA_MODEL");
					}
					if (strcmp(model_type, "FACE_DETECTION") == 0) {
						//if (strcmp(model_type, "FACE_DETECTION") == 0 && strcmp(input2model(model_name_fd), "NA") == 0) {
						strcpy(model_name_od, "NA_MODEL");
						strcpy(model_name_fd, "DEFAULT_SCRFD");
						strcpy(model_name_fr, "NA_MODEL");
						strcpy(model_name_ac, "NA_MODEL");
					}
					if (strcmp(model_type, "FACE_RECOGNITION") == 0) {
						//if (strcmp(model_type, "FACE_RECOGNITION") == 0 && strcmp(input2model(model_name_fr), "NA") == 0) {
						strcpy(model_name_od, "NA_MODEL");
						strcpy(model_name_fd, "DEFAULT_SCRFD");
						strcpy(model_name_fr, "DEFAULT_MOBILEFACENET");
						strcpy(model_name_ac, "NA_MODEL");
					}
					// TODO: add AUDIO_CLASSIFICATION
					if (strcmp(model_type, "AUDIO_CLASSIFICATION") == 0) {
						strcpy(model_name_od, "NA_MODEL");
						strcpy(model_name_fd, "NA_MODEL");
						strcpy(model_name_fr, "NA_MODEL");
						strcpy(model_name_ac, "DEFAULT_YAMNET");
					}
				}
			}
		}
		fclose(f_model);
	}
	else {
		printf("[%s][%d][Error] Failed to open f_model.\n", __func__, __LINE__);
		perror(file_path);
		fprintf(stderr, "Failed to open the f_model.\n");
		exit(1);
	}

	if (PRINT_DEBUG) printf("-------------------------------------\n");
	if (PRINT_DEBUG) printf("[%d] Model Name OD: %s\n", __LINE__, input2model(model_name_od));
	if (PRINT_DEBUG) printf("[%d] Model Name FD: %s\n", __LINE__, input2model(model_name_fd));
	if (PRINT_DEBUG) printf("[%d] Model Name FR: %s\n", __LINE__, input2model(model_name_fr));
	if (PRINT_DEBUG) printf("[%d] Model Name AC: %s\n", __LINE__, input2model(model_name_ac));
	if (PRINT_DEBUG) printf("-------------------------------------\n");


	// ------------------------- update TXT -------------------------
	updateTXT("----------------------------------");
	updateTXT("Current ino contains model(s):");

	if (strlen(model_name_od) == 0 && strlen(model_name_fd) == 0 && strlen(model_name_fr) == 0 && strlen(model_name_ac) == 0) {
		for (int i = 0; i < 4; i++) {
			updateTXT("NA");
		}
	}
	else {
		updateTXT(input2model(model_name_od));
		updateTXT(input2model(model_name_fd));
		updateTXT(input2model(model_name_fr));
		updateTXT(input2model(model_name_ac));
	}

	updateTXT("-----------------------------------");
	updateTXT("Current NN header file(s): ");

	FILE* f_headerNN = fopen(file_path, "r");  //FILE* file = fopen(path, "r, ccs=UTF-8");

	if (f_headerNN) {
		char line[1024];
		while (fgets(line, sizeof(line), f_headerNN)) {
			/* check whether keywordNN in file content */
			if (strstr(line, key_amb_header) != NULL && strstr(line, "NN") != NULL) {
				if (strstr(line, "Object") != NULL) {
					extractString(line, header_od);
				}
				if (strstr(line, "FaceDetection") != NULL) {
					extractString(line, header_fd);
				}
				if (strstr(line, "FaceRecognition") != NULL) {
					extractString(line, header_fr);
				}
				if (strstr(line, "Audio") != NULL) {
					extractString(line, header_ac);
				}
			}
		}
		if (PRINT_DEBUG) printf("[%d] Extracted OD header: %s\n", __LINE__, header_od);
		if (PRINT_DEBUG) printf("[%d] Extracted FD header: %s\n", __LINE__, header_fd);
		if (PRINT_DEBUG) printf("[%d] Extracted FR header: %s\n", __LINE__, header_fr);
		if (PRINT_DEBUG) printf("[%d] Extracted AC header: %s\n", __LINE__, header_ac);
		if (PRINT_DEBUG) printf("-------------------------------------\n");
		fclose(f_headerNN);
	}
	else {
		printf("[%s][Error] Failed to open the file.\n", __func__);
		perror(file_path);
		return EXIT_FAILURE;
	}

	updateTXT(header_od);
	updateTXT(header_fd);
	updateTXT(header_fr);
	updateTXT(header_ac);

	updateTXT("----------------------------------");
	updateTXT("Current ino video status:");

	FILE* f_VOE = fopen(file_path, "r");  //FILE* f_model = fopen(path, "r, ccs=UTF-8");
	if (f_VOE) {
		char line[1024];
		while (fgets(line, sizeof(line), f_VOE)) {
			/* check whether keywordVOE in file content */
			if (strstr(line, key_amb_VOE) != NULL && strstr(line, "//") == NULL && strstr(line, key_amb_bypassVOE1) == NULL && strstr(line, key_amb_bypassVOE2) == NULL) {
				strcpy(voe_status, "ON");
			}
		}
		fclose(f_VOE);      // update NA for video status
	}
	else {
		printf("[%s][Error] Failed to open the file.\n", __func__);
		perror(file_path);
		return EXIT_FAILURE;
	}
	updateTXT(voe_status);

	updateTXT("-------------------------------------");
	updateTXT("Current ino contains header file(s): ");

	FILE* f_header = fopen(file_path, "r");  //FILE* f_model = fopen(path, "r, ccs=UTF-8");
	if (f_header) {
		char line[1024];
		strcpy(header_all, "");
		while (fgets(line, sizeof(line), f_header)) {
			/* check whether keyword_header in file content */
			if (strstr(line, key_amb_header) != NULL) {
				extractString(line, line_strip_header);	// remove unnecessary part in the header
				if (strlen(line_strip_header) == 0) {
					extractString2(line, line_strip_header);
				}
				if (PRINT_DEBUG) printf("%s\n", line_strip_header);
				strcat(header_all, line_strip_header); // store headers into a string
			}
		}
		fclose(f_header);

		// update header_all to txt
		convertToHeaderFiles(header_all);
	}
	return 0;

error_combination:
	error_handler("Model combination mismatch. Please check modelSelect() again.");

error_customized_missing:
	error_handler("Model missing. Please check your sketch folder again.");

error_customized_exceed:
	error_handler("Too many models. Please remove unwanted models.");

error_customized_mismatch:
	error_handler("Customized model mismatch. Please rename your model.");

error_exceed:
	error_handler("Exceeds model size limitation. Please remove unwanted model(s).");

error_syntax:
	error_handler("Syntax Error! Please input a valid Neural Network task in modelSelect().");
}