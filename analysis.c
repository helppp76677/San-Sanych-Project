#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum {
    COLUMN_INT,
    COLUMN_DOUBLE
} ColumnType;


void comma_enter_to_zero(char *arr, const size_t length)
{
    for (size_t i = 0; i < length; i++) {
        if (arr[i] == ',' || arr[i]=='\n') {
            arr[i] = '\0';
        }
    }
}

int run_schema(const char *filename)
{
    typedef struct {
        char *name;
        ColumnType type;
    } Column;

    FILE *file = fopen(filename, "r");
    if (file == NULL){
        fprintf(stderr,"Не удалось открыть файл\n");
        return 1;
    }
    char *header = NULL;
    size_t header_capacity = 0;
    ssize_t header_length;

    header_length = getline(&header, &header_capacity, file);
    if (header_length == -1){
        fprintf(stderr,"Не удалось прочитать заголовок\n");
        free(header);
        fclose(file);
        return 1;
    }
    if (header_length>0 && header[header_length - 1] == '\n'){
        header[header_length - 1] = '\0';
        header_length--;
    }

    size_t column_count = 1;
    for (ssize_t i = 0; i < header_length; i++) {
        if (header[i] == ',') {
            column_count++;
        }
    }

    Column *columns = malloc(column_count * sizeof(Column));
    if (columns == NULL){
        fprintf(stderr,"Не удалось выделить память\n");
        free(header);
        fclose(file);
        return 1;
    }

    size_t column_index = 0;

    columns[column_index].name = header;
    columns[column_index].type = COLUMN_INT;

    for (ssize_t i = 0; i < header_length; i++) {
        if (header[i] == ',') {
            header[i] = '\0';

            column_index++;

            columns[column_index].name = &header[i + 1];
            columns[column_index].type = COLUMN_INT;
        }
    }

    size_t raws_counter = 0;
    char *line = NULL;
    size_t line_capacity = 0;
    ssize_t line_length;
    
    while ((line_length = getline(&line, &line_capacity, file)) != -1) {
        if (line[line_length - 1] == '\n') {
            line[line_length - 1] = '\0';
            line_length--;
        }
        
        comma_enter_to_zero(line,line_length);

        column_index = 0;
        for(int i = 1;i<line_length;i++){
            if (line[i-1] == '\0'){
                column_index+=1;
            }
            if (line[i] == '.'){
                columns[column_index].type = COLUMN_DOUBLE;
            }
        }
        raws_counter+=1;
    }

    if (!feof(file)){
        fprintf(stderr,"Не удалось дочитать файл\n");
        free(line);
        free(columns);
        free(header);
        fclose(file);
        return 1;
    }

    printf("rows %zu\n",raws_counter);
    printf("columns %zu\n",column_count);
    for(size_t i = 0;i<column_count;i++){
        printf("%zu \"%s\" ",i,columns[i].name);
        if (columns[i].type == COLUMN_INT){
            printf("INT\n");
        }
        else{
            printf("DOUBLE\n");
        }
    }

    free(line);
    free(columns);
    free(header);
    fclose(file);

    return 0;
}

int run_stats(const char *filename)
{
    typedef struct {
        char *name;
        ColumnType type;
        long double min;
        long double max;
        long double mean;
        size_t positive;
        size_t negative;
        size_t integer;
        size_t even;
    } Column;

    FILE *file = fopen(filename, "r");
    if (file == NULL){
        fprintf(stderr,"Не удалось открыть файл\n");
        return 1;
    }
    char *header = NULL;
    size_t header_capacity = 0;
    ssize_t header_length;

    header_length = getline(&header, &header_capacity, file);
    if (header_length == -1){
        fprintf(stderr,"Не удалось прочитать заголовок\n");
        free(header);
        fclose(file);
        return 1;
    }
    if (header_length>0 && header[header_length - 1] == '\n'){
        header[header_length - 1] = '\0';
        header_length--;
    }

    size_t column_count = 1;
    for (ssize_t i = 0; i < header_length; i++) {
        if (header[i] == ',') {
            column_count++;
        }
    }

    Column *columns = calloc(column_count, sizeof(Column));
    if (columns == NULL){
        fprintf(stderr,"Не удалось выделить память\n");
        free(header);
        fclose(file);
        return 1;
    }

    size_t column_index = 0;

    columns[column_index].name = header;
    columns[column_index].type = COLUMN_INT;

    for (ssize_t i = 0; i < header_length; i++) {
        if (header[i] == ',') {
            header[i] = '\0';

            column_index++;

            columns[column_index].name = &header[i + 1];
            columns[column_index].type = COLUMN_INT;
        }
    }

    size_t raws_counter = 0;
    char *line = NULL;
    size_t line_capacity = 0;
    ssize_t line_length;
    
    while ((line_length = getline(&line, &line_capacity, file)) != -1) {
        if (line[line_length - 1] == '\n') {
            line[line_length - 1] = '\0';
            line_length--;
        }
        
        comma_enter_to_zero(line,line_length);

        column_index = 0;
        size_t i = 0;
        while(i<(size_t)line_length){
            size_t field_length = 0;
            int is_double = 0;

            while(line[i+field_length] != '\0'){
                if (line[i+field_length] == '.'){
                    is_double = 1;
                }
                field_length+=1;
            }

            long double value;
            if (is_double){
                columns[column_index].type = COLUMN_DOUBLE;
                value = strtold(&line[i],NULL);
                if (value == truncl(value)){
                    columns[column_index].integer+=1;
                }
            }
            else{
                long long int int_value = strtoll(&line[i],NULL,10);
                value = int_value;
                columns[column_index].integer+=1;
                if (int_value%2 == 0){
                    columns[column_index].even+=1;
                }
            }

            if (raws_counter == 0 || value<columns[column_index].min){
                columns[column_index].min = value;
            }
            if (raws_counter == 0 || value>columns[column_index].max){
                columns[column_index].max = value;
            }
            if (value>0){
                columns[column_index].positive+=1;
            }
            if (value<0){
                columns[column_index].negative+=1;
            }
            columns[column_index].mean+=value;

            i+=field_length+1;
            column_index+=1;
        }
        raws_counter+=1;
    }

    if (!feof(file)){
        fprintf(stderr,"Не удалось дочитать файл\n");
        free(line);
        free(columns);
        free(header);
        fclose(file);
        return 1;
    }

    if (raws_counter>0){
        for(size_t i = 0;i<column_count;i++){
            columns[i].mean/=raws_counter;
        }
    }

    printf("rows %zu\n",raws_counter);
    printf("columns %zu\n",column_count);
    for(size_t i = 0;i<column_count;i++){
        if (i>0){
            printf("\n");
        }
        if (columns[i].min == 0){
            columns[i].min = 0;
        }
        if (columns[i].max == 0){
            columns[i].max = 0;
        }
        if (columns[i].mean == 0){
            columns[i].mean = 0;
        }

        printf("column %zu\n",i);
        printf("name \"%s\"\n",columns[i].name);
        if (columns[i].type == COLUMN_INT){
            printf("type INT\n");
        }
        else{
            printf("type DOUBLE\n");
        }
        printf("nulls 0\n");
        if (columns[i].type == COLUMN_INT){
            printf("min %.0Lf\n",columns[i].min);
            printf("max %.0Lf\n",columns[i].max);
        }
        else{
            printf("min %.6Lf\n",columns[i].min);
            printf("max %.6Lf\n",columns[i].max);
        }
        printf("mean %.6Lf\n",columns[i].mean);
        printf("positive %zu\n",columns[i].positive);
        printf("negative %zu\n",columns[i].negative);
        if (columns[i].type == COLUMN_INT){
            printf("even %zu\n",columns[i].even);
        }
        else{
            printf("integer %zu\n",columns[i].integer);
        }
    }

    free(line);
    free(columns);
    free(header);
    fclose(file);
    return 0;
}
