//
//  main.c
//  KrcDecoder
//
//  Created by apple on 2020/4/15.
//  Copyright © 2020 apple. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniz.h"

const char k_xor_key[] = {64, 71, 97, 119, 94, 50, 116, 71, 81, 54, 49, 45, 206, 210, 110, 105};

int krc_decode_write(char *src_name)
{
    // 1.read
    FILE *read_file = fopen(src_name, "r");
    if (read_file == NULL) {
        printf("打开文件失败！");
        return -1;
    }
    fseek(read_file, 0, SEEK_END);
    long src_len = ftell(read_file);
    char *src_str = (char *)malloc(src_len + 1);
    rewind(read_file);
    fread(src_str, sizeof(char), src_len, read_file);
    fclose(read_file);
    src_str[src_len] = '\0';
    
    // 2.decode
    if (memcmp(src_str, "krc1", 4) != 0) {
        printf("文件格式错误！");
        free(src_str);
        return -2;
    }
    for (mz_ulong i = 4; i < src_len; i++) {
        src_str[i] = src_str[i] ^ k_xor_key[(i-4) % 16];
    }
    mz_ulong dst_len = src_len * 100;
    mz_ulong *dst_size = &dst_len;
    unsigned char *dst_str = (unsigned char *)malloc(dst_len);
    int result = mz_uncompress(dst_str, dst_size, (unsigned char *)(src_str + 4), src_len - 4);
    if (result != Z_OK) {
        printf("解码krc文件失败！");
        free(dst_str);
        free(src_str);
        return -3;
    }
    dst_str[dst_len] = '\0';
    
    // 3.write
    long dst_name_len = strlen(src_name) + 4;
    char *dst_name = (char *)malloc(dst_name_len);
    strcpy(dst_name, src_name);
    strcat(dst_name, ".txt");
    FILE *file_write = fopen(dst_name, "w+");
    if (file_write == NULL) {
        printf("写入文件失败！");
        free(dst_name);
        free(dst_str);
        free(src_str);
        return -4;
    }
    fwrite(dst_str, sizeof(char), dst_len, file_write);
    fclose(file_write);
    free(dst_name);
    free(dst_str);
    free(src_str);
    return 0;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    int result = 0;
    if (argc <= 1) {
        char src_name[1024];
        printf("请输入文件路径：");
        scanf("%s", src_name);
        result = krc_decode_write(src_name);
    } else {
        for (int i = 1; i < argc; i++) {
           char *src_name = (char *)argv[i];
           result += krc_decode_write(src_name);
        }
    }
    return result;
}
