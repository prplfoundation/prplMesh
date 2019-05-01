/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../include/beerocks/bcl/beerocks_os_utils.h"
#include "../include/beerocks/bcl/beerocks_string_utils.h"

#include <sys/utsname.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <easylogging++.h>

using namespace beerocks;

std::string os_utils::get_process_path()
{
    char exe_path[PATH_MAX] = {0};
    if (-1 == readlink("/proc/self/exe", exe_path, sizeof(exe_path))) {
        LOG(ERROR) << "unable to determine execution path";
    }
    return std::string(exe_path);
}

std::string os_utils::get_process_dir()
{
    auto exe_path = get_process_path();
    auto dir_end = exe_path.find_last_of("/");
    return exe_path.substr(0, dir_end);
}

std::string os_utils::system_call(std::string cmd, int log_lvl, bool detached)
{
    std::string ret_str;

#ifdef IS_WINDOWS
    char buffer[512];
    FILE *popen_fd = _popen(cmd.c_str(), "r");
    if (popen_fd) {
        while (fgets(buffer, sizeof(buffer)-1, popen_fd)) {
            ret_str.append(buffer);
        }
        _pclose(popen_fd);
    }
    if (log_lvl == 2) LOG(INFO) << "system_call ret:\n" << ret_str << "\n";
#else
    cmd += " 2>&1";

    if(detached){
        cmd += " &";
    }

    if (log_lvl >= 1) LOG(INFO) << "system_call cmd: " << cmd ;

    // *** This code may block indefinitely ...
    // char buffer[1024];
    // FILE *popen_fd = popen(cmd.c_str(), "r");
    // if (popen_fd) {
    //     if(!detached){
    //         while (fgets(buffer, sizeof(buffer)-1, popen_fd)) {
    //             ret_str.append(buffer);
    //         }
    //     }
    //     pclose(popen_fd);
    // }

    if (system(cmd.c_str()) != 0) {
        LOG(ERROR) << "System returned non-zero";
    }

    //if (log_lvl == 2) LOG(INFO) << "system_call ret:\n" << ret_str << "\n";
#endif
    return ret_str;
}

void os_utils::kill_pid(std::string path, std::string file_name)
{
    int pid_out;
    if (is_pid_running(path, file_name, &pid_out)){
        std::cout << "kill_pid() kill pid=" <<  pid_out << std::endl;
        kill(pid_out, SIGTERM);
        sleep(2); //TODO: find alternative solution
        if(getpgid(pid_out) >= 0){
            kill(pid_out, SIGKILL);
        }
    }
}

bool os_utils::is_pid_running(std::string path, std::string file_name, int* pid_out)
{
    std::string pid_file_name = path + "pid/" + file_name;
    std::string pid_str;
    std::string cmdline;
    int pid;

    // get pid from file 
    std::ifstream pid_file(pid_file_name);
    if(pid_file.is_open()) {
        std::getline(pid_file, pid_str);
        pid_file.close();
        pid = beerocks::string_utils::stoi(pid_str);

        //check pid program name
        {
            std::string proc_file_path = "/proc/" + pid_str + "/cmdline";
            char buffer[1024] = {0};
            std::ifstream proc_file(proc_file_path);
            if (proc_file.is_open()) {
                proc_file.read(buffer, sizeof(buffer));
                buffer[sizeof(buffer) -1] = 0; // putting null terminator 
                cmdline.assign(buffer);
                proc_file.close();
            }
        }

        if(cmdline.length() > 0) {
            auto p1 = cmdline.rfind("/");
            if(p1 != std::string::npos){
                cmdline = cmdline.substr(p1 + 1);
            }

            // check if pid running 
            if((file_name.find(cmdline) != std::string::npos) && (getpgid(pid) >= 0)){
                if (pid_out){
                    *pid_out = pid;
                }
                return true;//pid is running
            }
        }
    }
    return false;//pid is not running
}

bool os_utils::write_pid_file(std::string path, std::string file_name)
{
    std::string pid_file_path = path + "pid";
    mkdir(pid_file_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    std::string pid_file_name = pid_file_path + "/" + file_name;
    int pid = getpid();
    std::ofstream pid_file;
    pid_file.open(pid_file_name.c_str(), std::fstream::out);
    if (pid_file.is_open()) {
        pid_file << pid << std::endl;
        pid_file.close();
        return true;
    }else{
        LOG(ERROR) << "can't save pid to file: " << pid_file_name;
        return false;
    }
}

bool os_utils::touch_pid_file(std::string file_path)
{
    if (utimensat(0, file_path.c_str(), nullptr, 0)) {
        LOG(ERROR) << "couldn't update file timestamp: " << file_path;
        return false;
    }
    
    return true;
}

int os_utils::redirect_console_std(std::string log_file_name)
{
    int fd_log_file_std = open( log_file_name.c_str(), O_CREAT | O_APPEND | O_RDWR, S_IWUSR);
    if(fd_log_file_std > 0){
        std::ostringstream msg;
        msg  << std::endl << "Start Log" << std::endl << std::endl;
        if (write(fd_log_file_std, msg.str().c_str(), msg.str().size()) < 0) {
            LOG(ERROR) << "Failed writing to file";
        }
        // dup2() - If the newfd was previously open,
        // it is silently closed before being reused
        dup2(fd_log_file_std, STDOUT_FILENO);
        dup2(fd_log_file_std, STDERR_FILENO);
    }
    return fd_log_file_std;
}

void os_utils::close_file(int fd)
{
    if(fd){
        close(fd);
    }
}
