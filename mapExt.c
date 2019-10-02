#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "bSwap.h"

typedef struct
{
    uint8_t  code;
    uint8_t  dat1;
    uint32_t dat2;
}
header_t;

typedef struct
{
    uint32_t vBeg;
    uint32_t vEnd;
}
room_t;

int main(int argc, char** argv)
{
    header_t header;
    room_t* rooms;
    uint32_t i, j, start, end;
    uint32_t numRooms, size, offs;
    char tempStr[128];
    uint8_t* data;
    FILE* fout;
    FILE* fin;

    /* Check the arguments */
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s <title> <Starting offset> <End offset>\n", argv[0]);
        return(1);
    }

    /* Open fin, make sure it opened */
    fin = fopen("ZOOTDEC.z64", "rb");
    if(fin == NULL)
    {
        perror("ZOOTDEC.z64");
        return(1);
    }

    /* Get the starting offset of the scene */
    sscanf(argv[2], "%x", &start);
    sscanf(argv[3], "%x", &end);
    size = end - start;
    data = malloc(sizeof(uint8_t) * size);
    printf("Scene size: %d\n", size);
    
    /* Read and then write the scene */ 
    sprintf(tempStr, "%s.zscene", argv[1]);
    fout = fopen(tempStr, "wb");
    fseek(fin, start, SEEK_SET);
    fread(data, sizeof(uint8_t), size, fin);
    fwrite(data, sizeof(uint8_t), size, fout);
    fclose(fout);
    free(data);

    /* Read header, find room list (0x04) */
    fseek(fin, start, SEEK_SET);
    fread(&header, sizeof(header_t), 1, fin);
    while(header.code != 0x04)
        fread(&header, sizeof(header_t), 1, fin);
    
    /* Allocate space for the rooms */
    numRooms = header.dat1;
    rooms = malloc(sizeof(room_t) * numRooms);
    printf("%d rooms found\n", numRooms);

    /* Skip to start of rooms list */
    offs = bSwap_32(header.dat2 & 0xffffff00);
    fseek(fin, start + offs, SEEK_SET);

    /* Read in all rooms offsets */
    for(i = 0; i < numRooms; i++)
    {
        fread(&rooms[i], sizeof(room_t), 1, fin);
        rooms[i].vBeg = bSwap_32(rooms[i].vBeg);
        rooms[i].vEnd = bSwap_32(rooms[i].vEnd);
    }
    
    /* Read and write all of the rooms */
    for(i = 0; i < numRooms; i++)
    {
        /* Go to start of current room, read it in */
        fseek(fin, rooms[i].vBeg, SEEK_SET);
        size = rooms[i].vEnd - rooms[i].vBeg;
        data = malloc(sizeof(uint8_t) * size);
        fread(data, sizeof(uint8_t), size, fin);
        
        /* Make a filename, print the size, write the file */
        sprintf(tempStr, "%s-%d.zmap", argv[1], i);
        printf("Room %2d size: %d\n", i, size);
        fout = fopen(tempStr, "wb");
        fwrite(data, sizeof(uint8_t), size, fout);
        fclose(fout);
        free(data);
    }

    fclose(fin);
    free(rooms);
    return(0);
}
