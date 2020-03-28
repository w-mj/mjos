//
// Created by wmj on 3/28/20.
//

#ifndef OS_PROCESS_HELPER_HPP
#define OS_PROCESS_HELPER_HPP

#ifdef __cplusplus
extern "C" {
#endif

// 复制打开的文件
void copy_file(ProcessDescriptor *to, ProcessDescriptor *from);

#ifdef __cplusplus
};
#endif
#endif //OS_PROCESS_HELPER_HPP
