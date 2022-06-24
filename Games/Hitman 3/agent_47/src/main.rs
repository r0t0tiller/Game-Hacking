use std;
use std::ffi::CStr;
use std::process::Command;

use windows::{
    Win32::Foundation::*, Win32::System::Diagnostics::Debug::*,
    Win32::System::Diagnostics::ToolHelp::*, Win32::System::Memory::*, Win32::System::Threading::*,
};

extern crate static_vcruntime;

const AMMO_OFFSET: u64 = 0x5A3577;

fn get_pid_by_name(target_process: String) -> u32 {
    let mut pid = 0;

    let snapshot = unsafe { CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };

    let snapshot_ret = snapshot.unwrap();

    if snapshot_ret == INVALID_HANDLE_VALUE {
        return 0;
    }

    let mut process_entry = PROCESSENTRY32 {
        dwSize: std::mem::size_of::<PROCESSENTRY32>() as u32,
        ..PROCESSENTRY32::default()
    };

    let mut res = unsafe { Process32First(snapshot_ret, &mut process_entry).as_bool() };

    while res {
        let process_name = unsafe {
            CStr::from_ptr(process_entry.szExeFile.as_ptr() as _)
                .to_string_lossy()
                .to_lowercase()
        };

        if process_name == target_process.to_string().to_lowercase() {
            pid = process_entry.th32ProcessID;

            break;
        }

        res = unsafe { Process32Next(snapshot_ret, &mut process_entry).as_bool() };
    }

    unsafe { CloseHandle(snapshot_ret) };

    return pid;
}

fn get_base_address(target_module_name: String, pid: u32) -> u64 {
    let mut base_address = 0;

    let snapshot =
        unsafe { CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid) };

    let snapshot_ret = snapshot.unwrap();

    if snapshot_ret == INVALID_HANDLE_VALUE {
        return 0;
    }

    let mut process_entry = MODULEENTRY32 {
        dwSize: std::mem::size_of::<MODULEENTRY32>() as u32,
        ..MODULEENTRY32::default()
    };

    unsafe {
        let mut res = Module32First(snapshot_ret, &mut process_entry).as_bool();

        while res {
            let module_name = CStr::from_ptr(process_entry.szModule.as_ptr() as _)
                .to_string_lossy()
                .to_lowercase();

            if module_name == target_module_name.to_lowercase() {
                base_address = process_entry.modBaseAddr as u64;
                break;
            }

            res = Module32Next(snapshot_ret, &mut process_entry).as_bool();
        }

        CloseHandle(snapshot_ret);
    }

    return base_address;
}

fn do_infinite_ammo(game_handle: HANDLE, base_address: u64) -> bool {
    let mut status = false;

    let ammo_addr = base_address + AMMO_OFFSET;

    let patch: Vec<u8> = vec![0x89,0x0C,0xA9]; // mov DWORD PTR [RCX+RBP*4], ecx

    if !write_patch(game_handle, ammo_addr, &patch) {
        println!("[*] Failed writing patch");
        return status;
    }

    status = true;

    return status;
}

fn write_patch(game_handle: HANDLE, address: u64, patch: &[u8]) -> bool {
    let mut status = false;

    unsafe {
        let mut old_protection: u32 = 0;
        let mut written: usize = 0;

        let protect = VirtualProtectEx(
            game_handle,
            address as *mut _,
            patch.len(),
            PAGE_EXECUTE_READWRITE,
            &mut old_protection as *mut _ as _,
        );

        if !protect.as_bool() {
            println!("[!] Failed setting memory protection");
            return status;
        }

        let write = WriteProcessMemory(
            game_handle,
            address as *mut _,
            patch as *const _ as *mut _,
            patch.len(),
            &mut written as *mut _,
        );

        if !write.as_bool() {
            println!("[!] Failed writing to process memory");
            return status;
        }

        let reset_protect = VirtualProtectEx(
            game_handle,
            address as *mut _,
            patch.len(),
            PAGE_PROTECTION_FLAGS(old_protection),
            &mut old_protection as *mut _ as _,
        );

        if !reset_protect.as_bool() {
            println!("[!] Failed resetting memory protection");
            return status;
        }

        status = true;
    }

    return status;
}

fn main() {
    println!("[*] Welcome Agent 47");

    println!("[*] Initializing mods");

    println!("[*] Finding game");

    let game_name = "HITMAN3.exe".to_string();

    let game_pid = get_pid_by_name(game_name.clone());

    if game_pid == 0 {
        println!("[!] Unable to find Hitman 3!");
        return;
    }

    println!("[*] Getting base address");

    let base_address = get_base_address(game_name.clone(), game_pid.clone());

    if base_address == 0 {
        println!("[!] Failed getting base address");
        return;
    }

    println!("[*] Base Address: {:X}", base_address);

    println!("[*] Opening handle to game");

    let game_handle = unsafe { OpenProcess(PROCESS_ALL_ACCESS, None, game_pid).unwrap() };

    if game_handle == HANDLE(0) {
        println!("[!] Failed opening handle to game");
    }

    println!("[*] Getting infinite ammo");

    if do_infinite_ammo(game_handle, base_address) {
        println!("[*] Infinite ammo mod complete, enjoy!");
    }

    println!("[*] Done");

    let _ = Command::new("cmd.exe")
    .arg("/c")
    .arg("pause")
    .status();
}
